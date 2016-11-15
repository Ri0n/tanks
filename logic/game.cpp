#include <QTimer>
#include <QCoreApplication>
#include <QDebug>

#include "game.h"
#include "board.h"
#include "randommaploader.h"
#include "tank.h"
#include "ai.h"

namespace Tanks {

Game::Game(QObject *parent) : QObject(parent),
    _board(new Board(this)),
    _mapLoader(nullptr),
    _playersCount(1)
{
    _ai = new AI(this);
    _mapLoader = new RandomMapLoader();
    _clock = new QTimer(this);
    _clock->setInterval(50);
    connect(_clock, &QTimer::timeout, this, &Game::clockTick);
}

Game::~Game()
{
    delete _mapLoader;
}

void Game::setPlayersCount(int n)
{
    _playersCount = qMax(1, qMin(n, 20)); // it makes me think about network support :)
}

void Game::connectPlayerSignals(AbstractPlayer *player)
{
    connect(player, &AbstractPlayer::newTankAvailable, this, &Game::newTankAvailable);
    connect(player, &AbstractPlayer::tankDestroyed, this, &Game::tankDestroyed);
    connect(player, &AbstractPlayer::fired, this, &Game::tankFired);
    connect(player, &AbstractPlayer::moved, this, &Game::moveTank);
}

void Game::playerMoveRequested(int playerNum, int direction)
{
    auto player = _humans.value(playerNum % _humans.count());
    if (player) {
        player->move((Direction)direction);
    }
}

void Game::playerFireRequested(int playerNum)
{
    auto player = _humans.value(playerNum % _humans.count());
    if (player) {
        player->fire();
    }
}

void Game::playerStopMoveRequested(int playerNum, int direction)
{
    auto player = _humans.value(playerNum % _humans.count());
    if (player) {
        player->stop((Direction)direction);
    }
}

void Game::playerStopFireRequested(int playerNum)
{
    auto player = _humans.value(playerNum % _humans.count());
    if (player) {
        player->stopFire();
    }
}

void Game::start()
{
    if (!_board->loadMap(_mapLoader)) {
        qDebug("Failed to load  map");
        return;
    }
    _humans.clear();
    _robots.clear();
    QTimer::singleShot(0, this, &Game::mapReady);
}

void Game::mapReady()
{
    emit mapLoaded();

    for (int i = 0; i < _playersCount; i++) {
        auto human = new HumanPlayer(this,i);
        connectPlayerSignals(human);
        _humans.append(QSharedPointer<HumanPlayer>(human));
        human->start();
    }

    foreach (auto &p, _ai->players()) {
        connectPlayerSignals(p.data());
    }

    _ai->start();
    _clock->start();
}

void Game::newTankAvailable()
{
    qDebug() << "New tank!";
    AbstractPlayer *player = qobject_cast<AbstractPlayer *>(sender());
    auto tank = player->tank();
    if (tank->affinity() == Friendly) {
        const auto &posList = _board->friendlyStartPositions();
        int playerIndex = static_cast<HumanPlayer*>(player)->index();
        tank->setInitialPosition(posList[playerIndex % posList.count()]);
    }
    _board->addDynBlock(tank);

    //_pendingNewTanks.enqueue(tank);
    emit newTank(tank.data()); // let's believe it won't be destroyed and connection is direct

}

void Game::tankDestroyed()
{
    // todo
}

void Game::tankFired()
{
    AbstractPlayer *player = qobject_cast<AbstractPlayer *>(sender());
    auto bullet = player->takeBullet();
    _bullets.prepend(bullet);
    emit newBullet(bullet.data());
}

void Game::moveTank()
{
    AbstractPlayer *player = qobject_cast<AbstractPlayer *>(sender());
    auto tank = player->tank();
    emit tankMoved(tank.data());
}

void Game::clockTick()
{
    foreach (auto p, _humans) {
        p->clockTick();
    }
    _ai->clockTick();

    auto it = _bullets.begin();
    while (it != _bullets.end()) {
        auto bullet = *it;

        bool clashFound = false;
        if (bullet->affinity() == Alien) {
            foreach (auto p, _humans) {
                if (p->tank() && p->tank()->hasClash(*bullet)) {
                    p->catchBullet();
                    clashFound = true;
                    break;
                }
            }
        } else {
            foreach (auto p, _robots) {
                if (p->tank() && p->tank()->hasClash(*bullet)) {
                    p->catchBullet();
                    clashFound = true;
                    break;
                }
            }
        }
        if (clashFound) {
            it = _bullets.erase(it);
            emit bulletRemoved(bullet.data());
        } else {
            QRect fmr = bullet->forwardMoveRect();
            //qDebug() << "Bullet forward" << fmr;
            auto props = _board->rectProps(fmr);
            if (props & Board::BulletObstackle) {

                // resize damage area to four blocks
                if (fmr.width() > fmr.height()) {
                    fmr.setWidth(4);
                    fmr.translate(-1, 0);
                } else {
                    fmr.setHeight(4);
                    fmr.translate(0, -1);
                }
                for (int i = 0; i < fmr.width(); i++) {
                    for (int j = 0; j < fmr.height(); j++) {
                        QPoint p(fmr.left() + i, fmr.top() + j);
                        props = _board->blockProperties(p);
                        if (props & Board::Breakable) {
                            if (bullet->level() == Bullet::ArmorPiercing || !(props & Board::Sturdy)) {
                                QRect r(p, QSize(1,1));
                                _board->renderBlock(Nothing, r);
                                emit blockRemoved(r);
                            }
                        }
                    }
                }
                it = _bullets.erase(it);
                qDebug("Remove!!!");
                emit bulletRemoved(bullet.data());
            } else {
                if (bullet->canMove()) {
                    bullet->move();
                    if (!QRect(QPoint(0,0), _board->size()).contains(bullet->geometry())) {
                        it = _bullets.erase(it);
                        emit bulletRemoved(bullet.data());
                        continue;
                    }
                    emit bulletMoved(bullet.data());
                }

                bullet->clockTick();
                ++it;
            }
        }
    }
}

} // namespace Tanks

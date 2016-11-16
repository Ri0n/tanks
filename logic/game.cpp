#include <QTimer>
#include <QCoreApplication>
#include <QDebug>

#include "game.h"
#include "humanplayer.h"
#include "aiplayer.h"
#include "board.h"
#include "randommaploader.h"
#include "tank.h"
#include "ai.h"
#include "flag.h"

namespace Tanks {

class GamePrivate
{
public:
    GamePrivate(Game *game) :
        game(game),
        board(),
        mapLoader(nullptr),
        playersCount(1)
    {}

    Game *game;
    Board *board;
    AbstractMapLoader *mapLoader;
    QTimer *clock;
    quint8 playersCount;
    AI *ai;

    QList<QSharedPointer<HumanPlayer>> humans;
    QLinkedList<QSharedPointer<Bullet>> bullets;

    QSharedPointer<Flag> flag;
};


Game::Game(QObject *parent) : QObject(parent),
    _d(new GamePrivate(this))
{
    _d->board = new Board(this);
    _d->ai = new AI(this);
    _d->mapLoader = new RandomMapLoader();
    _d->flag = QSharedPointer<Flag>(new Flag);

    _d->clock = new QTimer(this);
    _d->clock->setInterval(50);
    connect(_d->clock, &QTimer::timeout, this, &Game::clockTick);
    connect(_d->ai, &AI::newPlayer, this, &Game::connectPlayerSignals);
}

Game::~Game()
{
    reset();
    delete _d->mapLoader;
    delete _d;
}

void Game::reset()
{
    _d->clock->stop();
    _d->humans.clear();
    _d->bullets.clear();
    _d->ai->reset();
    //_d->board->reset();
}

Board *Game::board() const
{
    return _d->board;
}

void Game::setPlayersCount(int n)
{
    _d->playersCount = qMax(1, qMin(n, 20)); // it makes me think about network support :)
}

QSharedPointer<Flag> &Game::flag() const
{
    return _d->flag;
}

void Game::connectPlayerSignals(AbstractPlayer *player)
{
    connect(player, &AbstractPlayer::newTankAvailable, this, &Game::newTankAvailable);
    connect(player, &AbstractPlayer::tankDestroyed, this, &Game::destroyTank);
    connect(player, &AbstractPlayer::fired, this, &Game::tankFired);
    connect(player, &AbstractPlayer::moved, this, &Game::moveTank);
}

void Game::playerMoveRequested(int playerNum, int direction)
{
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->move((Direction)direction);
    }
}

void Game::playerFireRequested(int playerNum)
{
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->fire();
    }
}

void Game::playerStopMoveRequested(int playerNum, int direction)
{
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->stop((Direction)direction);
    }
}

void Game::playerStopFireRequested(int playerNum)
{
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->stopFire();
    }
}

void Game::start()
{
    reset();
    if (!_d->board->loadMap(_d->mapLoader)) {
        qDebug("Failed to load  map");
        return;
    }
    QTimer::singleShot(0, this, &Game::mapReady);
}

void Game::mapReady()
{
    _d->flag->restore();
    _d->flag->setInitialPosition(_d->board->flagPosition());
    emit mapLoaded();

    for (int i = 0; i < _d->playersCount; i++) {
        auto human = new HumanPlayer(this,i);
        connectPlayerSignals(human);
        _d->humans.append(QSharedPointer<HumanPlayer>(human));
        human->start();
    }

    _d->ai->start();
    _d->clock->start();
}

void Game::newTankAvailable()
{
    qDebug() << "New tank!";
    AbstractPlayer *player = qobject_cast<AbstractPlayer *>(sender());
    auto tank = player->tank();
    emit newTank(tank.data()); // let's believe it won't be destroyed and connection is direct
}

void Game::destroyTank()
{
    AbstractPlayer *p = qobject_cast<AbstractPlayer *>(sender());
    if (p) {
        emit tankDestroyed(p->tank().data());
        return;
    }
}

void Game::tankFired()
{
    AbstractPlayer *player = qobject_cast<AbstractPlayer *>(sender());
    auto bullet = player->takeBullet();
    _d->bullets.prepend(bullet);
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
    foreach (auto p, _d->humans) {
        p->clockTick();
    }
    _d->ai->clockTick();

    for (int bMove = 0; bMove < 2; bMove++) {
        moveBullets();
    }
}

void Game::moveBullets()
{
    auto it = _d->bullets.begin();
    while (it != _d->bullets.end()) {
        auto bullet = *it;

        bool clashFound = false;
        Bullet::ExplosionType explType = Bullet::Explosion;
        if (bullet->affinity() == Alien) {
            foreach (auto p, _d->humans) {
                if (p->tank() && p->tank()->hasClash(*bullet)) {
                    p->catchBullet();
                    clashFound = true;
                    break;
                }
            }
        } else {
            QSharedPointer<AIPlayer> p = _d->ai->findClash(bullet.dynamicCast<Block>());
            if (p) {
                p->catchBullet();
                clashFound = true;
            }
        }
        if (!clashFound && _d->flag->hasClash(*bullet)) {
            clashFound = true;
            _d->flag->burn();
            explType = Bullet::BigExplosion;
            emit flagLost();
        }
        if (!clashFound) {
            // meet other bullets
            auto it2 = it + 1;
            Affinity invAff = bullet->affinity() == Alien? Friendly : Alien;
            while (it2 != _d->bullets.end()) {
                auto &b2 = **it2;
                if (b2.affinity() == invAff && b2.hasClash(*bullet)) {
                    explType = Bullet::BrickDestroyed;
                    b2.explode(explType); // let's imagine tank shoot with bricks / FIXME
                    _d->bullets.erase(it2);
                    clashFound = true;
                    break;
                }
                ++it2;
            }
        }
        if (clashFound) {
            it = _d->bullets.erase(it);
            bullet->explode(explType);
        } else {
            QRect fmr = bullet->forwardMoveRect();
            //qDebug() << "Bullet forward" << fmr;
            auto props = _d->board->rectProps(fmr);
            if (props & Board::BulletObstackle) {

                // resize damage area to four blocks
                if (fmr.width() > fmr.height()) {
                    fmr.setWidth(4);
                    fmr.translate(-1, 0);
                } else {
                    fmr.setHeight(4);
                    fmr.translate(0, -1);
                }
                bool brickDamage = false;
                for (int i = 0; i < fmr.width(); i++) {
                    for (int j = 0; j < fmr.height(); j++) {
                        QPoint p(fmr.left() + i, fmr.top() + j);
                        props = _d->board->blockProperties(p);
                        if (props & Board::Breakable) {
                            if (bullet->level() == Bullet::ArmorPiercing || !(props & Board::Sturdy)) {
                                QRect r(p, QSize(1,1));
                                _d->board->renderBlock(Nothing, r);
                                emit blockRemoved(r);
                                brickDamage = true;
                            }
                        }
                    }
                }
                it = _d->bullets.erase(it);
                //qDebug("Remove!!!");
                bullet->explode(brickDamage? Bullet::BrickDestroyed : Bullet::NoDamage);
            } else {
                if (bullet->canMove()) {
                    bullet->move();
                    if (!QRect(QPoint(0,0), _d->board->size()).contains(bullet->geometry())) {
                        it = _d->bullets.erase(it);
                        bullet->explode(Bullet::NoDamage);
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

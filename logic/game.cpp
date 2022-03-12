/*
 Copyright (c) 2016, Sergey Ilinykh
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL IL'INYKH SERGEY BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "game.h"
#include "ai.h"
#include "aiplayer.h"
#include "board.h"
#include "flag.h"
#include "humanplayer.h"
#include "randommaploader.h"
#include "tank.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

#include <list>

namespace Tanks {

class GamePrivate {
public:
    GamePrivate(Game *game) : game(game), board(), mapLoader(nullptr), playersCount(1) { }

    Game              *game;
    Board             *board;
    AbstractMapLoader *mapLoader;
    QTimer            *clock;
    quint8             playersCount;
    AI                *ai;

    QList<QSharedPointer<HumanPlayer>> humans;
    std::list<QSharedPointer<Bullet>>  bullets;

    QSharedPointer<Flag> flag;
};

Game::Game(QObject *parent) : QObject(parent), _d(new GamePrivate(this))
{
    _d->board     = new Board(this);
    _d->ai        = new AI(this);
    _d->mapLoader = new RandomMapLoader();
    _d->flag      = QSharedPointer<Flag>(new Flag);

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

Board *Game::board() const { return _d->board; }

void Game::setPlayersCount(int n)
{
    _d->playersCount = qMax(1, qMin(n, 20)); // it makes me think about network support :)
}

int Game::playersCount() { return _d->playersCount; }

int Game::aiLifes() { return _d->ai->lifesCount(); }

int Game::playerLifes(int playerId)
{
    if (!_d->humans.count()) {
        return 0;
    }
    auto player(_d->humans.value(playerId % _d->humans.count()));
    if (player) {
        return player->lifesCount();
    }
    return 0;
}

QSharedPointer<Flag> &Game::flag() const { return _d->flag; }

void Game::connectPlayerSignals(AbstractPlayer *player)
{
    connect(player, &AbstractPlayer::newTankAvailable, this, &Game::newTankAvailable);
    connect(player, &AbstractPlayer::lifeLost, this, &Game::statsChanged);
}

void Game::playerMoveRequested(int playerNum, int direction)
{
    if (!_d->humans.count()) {
        return;
    }
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->move((Direction)direction);
    }
}

void Game::playerFireRequested(int playerNum)
{
    if (!_d->humans.count()) {
        return;
    }
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->fire();
    }
}

void Game::playerStopMoveRequested(int playerNum, int direction)
{
    if (!_d->humans.count()) {
        return;
    }
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->stop((Direction)direction);
    }
}

void Game::playerStopFireRequested(int playerNum)
{
    if (!_d->humans.count()) {
        return;
    }
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->stopFire();
    }
}

void Game::start(int playersCount)
{
    reset();
    _d->playersCount = playersCount;
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
        auto human = new HumanPlayer(this, i);
        connectPlayerSignals(human);
        _d->humans.append(QSharedPointer<HumanPlayer>(human));
        human->start();
    }

    _d->ai->start();
    _d->clock->start();

    emit statsChanged();
}

void Game::newTankAvailable()
{
    // qDebug() << "New tank!";
    AbstractPlayer *player = qobject_cast<AbstractPlayer *>(sender());
    auto            tank   = player->tank();
    emit            newTank(tank.data()); // let's believe it won't be destroyed and connection is direct

    // the last signal
    connect(tank.data(), &Tank::fired, this, &Game::onTankFired);
}

void Game::onTankFired()
{
    Tank *tank   = qobject_cast<Tank *>(sender());
    auto  bullet = tank->takeBullet();
    _d->bullets.push_front(bullet);
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

        bool                  clashFound = false;
        Bullet::ExplosionType explType   = Bullet::Explosion;
        if (bullet->affinity() == Alien) {
            foreach (auto p, _d->humans) {
                if (p->tank() && p->tank()->hasClash(*bullet)) {
                    p->tank()->catchBullet();
                    clashFound = true;
                    break;
                }
            }
        } else {
            QSharedPointer<AIPlayer> p = _d->ai->findClash(bullet.dynamicCast<Block>());
            if (p) {
                p->tank()->catchBullet();
                clashFound = true;
            }
        }
        if (!clashFound && _d->flag->hasClash(*bullet)) {
            clashFound = true;
            _d->flag->burn();
            explType = Bullet::BigExplosion;
            emit flagLost();
            foreach (auto &human, _d->humans) {
                if (human->tank()) { // still alive
                    human->killAll();
                }
            }
        }
        if (!clashFound) {
            // meet other bullets
            auto it2 = it;
            ++it2;
            Affinity invAff = bullet->affinity() == Alien ? Friendly : Alien;
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
            // qDebug() << "Bullet forward" << fmr;
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
                                QRect r(p, QSize(1, 1));
                                _d->board->renderBlock(Nothing, r);
                                emit blockRemoved(r);
                                brickDamage = true;
                            }
                        }
                    }
                }
                it = _d->bullets.erase(it);
                // qDebug("Remove!!!");
                bullet->explode(brickDamage ? Bullet::BrickDestroyed : Bullet::NoDamage);
            } else {
                if (bullet->canMove()) {
                    bullet->move();
                    if (!QRect(QPoint(0, 0), _d->board->size()).contains(bullet->geometry())) {
                        it = _d->bullets.erase(it);
                        bullet->explode(Bullet::NoDamage);
                        continue;
                    }
                    // emit bulletMoved(bullet.data());
                }

                bullet->clockTick();
                ++it;
            }
        }
    }
}

} // namespace Tanks

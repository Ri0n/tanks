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

#include "humanplayer.h"
#include "board.h"
#include "game.h"

#include <QDebug>

namespace Tanks {

HumanPlayer::HumanPlayer(Game *game, int playerIndex) :
    _game(game), _playerIndex(playerIndex), _lifes(3), _shooting(false)
{
}

int HumanPlayer::lifesCount() const { return _lifes; }

void HumanPlayer::start()
{
    _tank         = QSharedPointer<Tank>(new Tank(Friendly));
    _oldDirection = _tank->direction();
    moveToStart();
    emit newTankAvailable();
    // our handler is the last
    connect(_tank.data(), &Tank::tankDestroyed, this, &HumanPlayer::onTankDestroyed);
}

void HumanPlayer::moveToStart()
{
    const auto &posList = _game->board()->friendlyStartPositions();
    _tank->setInitialPosition(posList[_playerIndex % posList.count()]);
}

void HumanPlayer::move(Direction dir)
{
    if (_tank) {
        Direction curDir = _tank->direction();
        _movingDir.erase(std::remove(_movingDir.begin(), _movingDir.end(), dir), _movingDir.end());
        _movingDir.push_front(dir);
        if (curDir != dir) {
            _oldDirection = curDir;
            _tank->setDirection(dir);
        }
    }
}

void HumanPlayer::fire() { _shooting = true; }

void HumanPlayer::stop(Direction dir)
{
    // we can stop pressing a key, but it does not mean other keys are not pressed
    // so we have to figure out what's left and continue moving
    _movingDir.erase(std::remove(_movingDir.begin(), _movingDir.end(), dir), _movingDir.end());
    if (!_movingDir.empty()) {
        auto dir = _movingDir.front();
        _movingDir.pop_front();
        move(dir);
    }
}

void HumanPlayer::stopFire() { _shooting = false; }

void HumanPlayer::clockTick()
{
    if (!_tank) {
        return; // nothing todo w/o tank
    }
    AbstractPlayer::clockTick();

    QRect             fmr;
    Board::BlockProps props;

    bool shouldMove  = !_movingDir.empty() && _tank->canMove();
    bool shouldShoot = (_shooting && _tank->canShoot());

    if (shouldMove || shouldShoot) {
        fmr   = _tank->forwardMoveRect(); // space before tank it's going to occupy
        props = _game->board()->rectProps(fmr);
    }

    if (shouldMove && !(props & Board::TankObstackle)) {
        _tank->move();
        _oldDirection = _tank->direction();
    }
    //    else if (_oldDirection != _tank->direction() && _tank->canMove()) {
    //        emit moved();
    //        _oldDirection = _tank->direction();
    //    }

    if (shouldShoot) {
        _tank->fire();
    }
}

void HumanPlayer::onTankDestroyed()
{
    if (!_lifes) {
        qDebug("Something went wrong");
        return;
    }
    _lifes--;
    if (_lifes) {
        start();
    } else {
        _tank.clear();
    }
    emit lifeLost();
}

void HumanPlayer::killAll()
{
    if (_tank) {
        _lifes = 1;
        _tank->selfDestroy();
    }
}

} // namespace Tanks

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

#include "aiplayer.h"
#include "ai.h"
#include "board.h"
#include "flag.h"
#include "game.h"

#include <QDebug>
#include <QRandomGenerator>

namespace Tanks {

AIPlayer::AIPlayer(AI *ai) : _ai(ai) { }

int AIPlayer::lifesCount() const { return _ai->pendingTanks(); }

void AIPlayer::start()
{
    _tank = QSharedPointer<Tank>(new Tank(Alien, _ai->takeTank()));
    _tank->setInitialPosition(_ai->initialPosition());
    emit newTankAvailable();
    connect(_tank.data(), &Tank::tankDestroyed, this, &AIPlayer::onTankDestroyed);
}

void AIPlayer::clockTick()
{
    AbstractPlayer::clockTick();

    if (!_tank) {
        return;
    }

    bool forceShoot = false;

    if (_tank->canMove()) {
        Board::BlockProps props          = _ai->game()->board()->rectProps(_tank->forwardMoveRect());
        bool              canMoveForward = !(props & Board::TankObstackle);

        int r = QRandomGenerator::global()->bounded(16);
        int d = QRandomGenerator::global()->bounded(16);

        bool moving     = r < 15;
        bool needNewDir = !canMoveForward || d > 13;

        // Direction oldDir = _tank->direction();
        if (needNewDir) {
            if (_ai->game()->flag()->isBroken()) {
                _tank->setDirection((Direction)(QRandomGenerator::global()->bounded(4)));
            } else {
                QPoint    tc = _tank->geometry().center();
                QPoint    fc = _ai->game()->flag()->geometry().center();
                Direction dirs[4]; // first directions are more probable (towards the flag)
                if (tc.x() < fc.x()) {
                    dirs[0] = East;
                    dirs[2] = West;
                } else {
                    dirs[0] = West;
                    dirs[2] = East;
                }
                if (tc.y() < fc.y()) {
                    dirs[1] = South;
                    dirs[3] = North;
                } else {
                    dirs[1] = North;
                    dirs[3] = South;
                }

                int toFlagInd
                    = QRandomGenerator::global()->generate() < (std::numeric_limits<quint32>::max() * 0.9) ? 0 : 2;
                Direction newDir = dirs[toFlagInd + (d & 1)];

                _tank->setDirection(newDir);
            }
            props          = _ai->game()->board()->rectProps(_tank->forwardMoveRect());
            canMoveForward = !(props & Board::TankObstackle);
        }

        if (canMoveForward && moving) {
            _tank->move();
            // oldDir = _tank->direction();
        } else if (props & Board::Breakable && !(props & Board::Sturdy)) {
            forceShoot = true;
        }
        //        if (oldDir != _tank->direction()) {
        //            emit moved(); // just turned, but anyway need to update
        //        }
        if (!moving) {
            _tank->setClockPhase(20);
        }
    }

    if (_tank->canShoot()) {
        if (forceShoot || QRandomGenerator::global()->generate() < std::numeric_limits<quint32>::max() / 100) {
            _tank->fire();
        }
    }
}

void AIPlayer::onTankDestroyed()
{
    _tank.clear();
    emit lifeLost();
}

} // namespace Tanks

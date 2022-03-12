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

#include "ai.h"
#include "aiplayer.h"
#include "board.h"
#include "game.h"

#include <QRandomGenerator>

namespace Tanks {

AI::AI(Game *game) : QObject(game), _game(game), _activateClock(0) { }

AI::~AI() { reset(); }

void AI::reset()
{
    _activePlayers.clear();
    _inactivePlayers.clear();
    _tanks.clear();
    _activateClock = 0;
}

void AI::start()
{
    _tanks = _game->board()->initialEnemyTanks();
    for (int i = 0; i < 8; i++) { // we want 4 tanks at once on the map
        auto robot = QSharedPointer<AIPlayer>(new AIPlayer(this));
        _inactivePlayers.push_back(robot);

        // we need this connection first to keep stats valid
        connect(robot.data(), &AIPlayer::lifeLost, this, &AI::deactivatePlayer);
        emit newPlayer(robot.data());
    }
}

QSharedPointer<AIPlayer> AI::findClash(const QSharedPointer<Block> &block)
{
    foreach (auto p, _activePlayers) {
        if (p->tank() && p->tank()->hasClash(*block)) {
            return p;
        }
    }
    return QSharedPointer<AIPlayer>();
}

QPoint AI::initialPosition() const
{
    auto &pos = _game->board()->enemyStartPositions();
    return pos.value(QRandomGenerator::global()->bounded(pos.count()));
}

void AI::clockTick()
{
    if (_activateClock) {
        _activateClock--;
    }
    if (!_activateClock && !_inactivePlayers.empty() && _tanks.count()) {
        auto player = _inactivePlayers.front();
        _inactivePlayers.pop_front();
        _activePlayers.push_back(player);
        player->start();
        _activateClock = 100;
    }

    for (auto &p : _activePlayers) {
        p->clockTick();
    }
}

void AI::deactivatePlayer()
{
    auto it = _activePlayers.begin();
    while (it != _activePlayers.end()) {
        if ((*it).data() == sender()) {
            _inactivePlayers.push_back(*it);
            _activePlayers.erase(it);
            break;
        }
        ++it;
    }
}

} // namespace Tanks

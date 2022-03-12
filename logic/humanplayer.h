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

#ifndef TANKS_HUMANPLAYER_H
#define TANKS_HUMANPLAYER_H

#include "abstractplayer.h"
#include "basics.h"

#include <list>

namespace Tanks {

class Game;

class HumanPlayer : public AbstractPlayer {
public:
    HumanPlayer(Game *game, int playerIndex);
    int lifesCount() const;

    void       start();
    void       move(Direction dir);
    void       fire();
    void       stop(Direction dir);
    void       stopFire();
    inline int index() const { return _playerIndex; }

    void clockTick();
    void killAll();

protected:
    void moveToStart();

private slots:
    void onTankDestroyed();

private:
    Game                *_game;
    int                  _playerIndex;
    int                  _lifes;
    bool                 _shooting;
    Direction            _oldDirection;
    std::list<Direction> _movingDir;
};

} // namespace Tanks

#endif // TANKS_HUMANPLAYER_H

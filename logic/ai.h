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

#ifndef TANKS_AI_H
#define TANKS_AI_H

#include <QObject>
#include <QLinkedList>

#include "aiplayer.h"

namespace Tanks {

class Game;

class AI : public QObject
{
    Q_OBJECT
public:
    explicit AI(Game *game = 0);
    ~AI();
    void reset();
    inline Game *game() const { return _game; }
    inline int pendingTanks() const { return _tanks.count(); }
    inline int lifesCount() const { return _tanks.count() + _activePlayers.count(); }
    inline quint8 takeTank() { return _tanks.takeFirst(); }
    void start();
    QSharedPointer<AIPlayer> findClash(const QSharedPointer<Block> &block);

    QPoint initialPosition() const;

    void clockTick();

signals:
    void newPlayer(AIPlayer*);

public slots:

private slots:
    void deactivatePlayer();
private:
    Game *_game;
    QList<quint8> _tanks;
    QLinkedList<QSharedPointer<AIPlayer>> _activePlayers;
    QLinkedList<QSharedPointer<AIPlayer>> _inactivePlayers;
    int _activateClock;
};

} // namespace Tanks

#endif // TANKS_AI_H

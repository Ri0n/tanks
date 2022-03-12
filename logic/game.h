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

#ifndef TANKS_GAME_H
#define TANKS_GAME_H

#include <QObject>
#include <QRect>
#include <QSharedPointer>

namespace Tanks {

class AbstractPlayer;
class Board;
class Flag;

class GamePrivate;
class Game : public QObject {
    Q_OBJECT
public:
    explicit Game(QObject *parent = 0);
    ~Game();
    Board                *board() const;
    QSharedPointer<Flag> &flag() const;

    void setPlayersCount(int n);
    int  playersCount();
    int  aiLifes();
    int  playerLifes(int playerId);

private:
    void moveBullets();
    void reset();

signals:
    void mapLoaded();
    void playerRestarted();
    void newTank(QObject *);
    void tankDestroyed(QObject *);
    void bulletRemoved(QObject *);
    void blockRemoved(QRect);
    void flagLost();
    void statsChanged();

public slots:
    void playerMoveRequested(int playerNum, int direction);
    void playerFireRequested(int playerNum);
    void start(int playersCount = 1);

    void playerStopMoveRequested(int playerNum, int direction);
    void playerStopFireRequested(int playerNum);
private slots:
    void connectPlayerSignals(Tanks::AbstractPlayer *player);
    void mapReady();
    void clockTick();

    void newTankAvailable();
    void onTankFired();

private:
    friend class GamePrivate;
    GamePrivate *_d;
};

} // namespace Tanks

#endif // TANKS_GAME_H

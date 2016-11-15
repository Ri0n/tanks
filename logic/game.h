#ifndef TANKS_GAME_H
#define TANKS_GAME_H

#include <QObject>
#include <QQueue>
#include <QLinkedList>

#include "humanplayer.h"
#include "aiplayer.h"

class QTimer;

namespace Tanks {

class Board;
class AbstractMapLoader;
class AI;

class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(QObject *parent = 0);
    ~Game();
    Board *board() const { return _board; }
    void setPlayersCount(int n);
    QSharedPointer<Tank> takePendingNewTank()
    { return _pendingNewTanks.isEmpty()? QSharedPointer<Tank>() : _pendingNewTanks.dequeue(); }

private:
    void connectPlayerSignals(AbstractPlayer *player);

signals:
    void mapLoaded();
    void playerRestarted();
    void newTank(QObject*);
    void newBullet(QObject*);
    void tankMoved(QObject*);
    void bulletRemoved(QObject*);
    void bulletMoved(QObject*);
    void blockRemoved(QRect);

public slots:
    void playerMoveRequested(int playerNum, int direction);
    void playerFireRequested(int playerNum);
    void start();

    void playerStopMoveRequested(int playerNum, int direction);
    void playerStopFireRequested(int playerNum);
private slots:
    void mapReady();
    void clockTick();

    void newTankAvailable();
    void tankDestroyed();
    void tankFired();
    void moveTank();
private:
    Board *_board;
    AbstractMapLoader *_mapLoader;
    QTimer *_clock;
    quint8 _playersCount;
    AI *_ai;

    QList<QSharedPointer<HumanPlayer>> _humans;
    QList<QSharedPointer<AIPlayer>> _robots;
    QLinkedList<QSharedPointer<Bullet>> _bullets;

    QQueue<QSharedPointer<Tank>> _pendingNewTanks;

};

} // namespace Tanks

#endif // TANKS_GAME_H

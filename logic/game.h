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
class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(QObject *parent = 0);
    ~Game();
    Board *board() const;
    void setPlayersCount(int n);
    QSharedPointer<Flag> &flag() const;

private:
    void moveBullets();
    void reset();

signals:
    void mapLoaded();
    void playerRestarted();
    void newTank(QObject*);
    void newBullet(QObject*);
    void tankMoved(QObject*);
    void tankDestroyed(QObject*);
    void bulletRemoved(QObject*);
    void bulletMoved(QObject*);
    void blockRemoved(QRect);
    void flagLost();

public slots:
    void playerMoveRequested(int playerNum, int direction);
    void playerFireRequested(int playerNum);
    void start();

    void playerStopMoveRequested(int playerNum, int direction);
    void playerStopFireRequested(int playerNum);
private slots:
    void connectPlayerSignals(AbstractPlayer *player);
    void mapReady();
    void clockTick();

    void newTankAvailable();
    void destroyTank();
    void tankFired();
    void moveTank();
private:
    friend class GamePrivate;
    GamePrivate *_d;
};

} // namespace Tanks

#endif // TANKS_GAME_H

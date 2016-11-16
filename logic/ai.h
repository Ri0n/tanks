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
    inline Game *game() const { return _game; }
    inline int lifesCount() const { return _tanks.count(); }
    inline quint8 takeTank() { return _tanks.takeFirst(); }
    void start();
    QList<QSharedPointer<AIPlayer>> players() const;
    QSharedPointer<AIPlayer> findClash(const QSharedPointer<Block> &block);

    QPoint initialPosition() const;

    void clockTick();

signals:

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

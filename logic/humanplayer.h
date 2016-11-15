#ifndef TANKS_HUMANPLAYER_H
#define TANKS_HUMANPLAYER_H

#include <QLinkedList>

#include "abstractplayer.h"
#include "basics.h"

namespace Tanks {

class Game;

class HumanPlayer : public AbstractPlayer
{
public:
    HumanPlayer(Game *game, int playerIndex);
    int lifesCount() const;

    void start();
    void move(Direction dir);
    void fire();
    void stop(Direction dir);
    void stopFire();
    inline int index() const { return _playerIndex; }

    void clockTick();

private:
    Game *_game;
    int _playerIndex;
    int _lifes;
    bool _shooting;
    Direction _oldDirection;
    QLinkedList<Direction> _movingDir;
};

} // namespace Tanks

#endif // TANKS_HUMANPLAYER_H

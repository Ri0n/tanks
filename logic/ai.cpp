#include "ai.h"
#include "game.h"
#include "board.h"

namespace Tanks {

AI::AI(Game *game) : QObject(game),
    _game(game)
{

}

void AI::resetTanks()
{
    _tanks = _game->board()->initialEnemyTanks();
}

void AI::start()
{
    // todo
}

void AI::clockTick()
{

}

} // namespace Tanks

#include "ai.h"
#include "game.h"
#include "board.h"
#include "aiplayer.h"

namespace Tanks {

AI::AI(Game *game) : QObject(game),
    _game(game),
    _activateClock(0)
{
    for (int i = 0; i < 4; i++) { // we want 4 tanks at once on the map
        auto robot = QSharedPointer<AIPlayer>(new AIPlayer(this));
        _inactivePlayers.append(robot);
        connect(robot.data(), &AIPlayer::tankDestroyed, this, &AI::deactivatePlayer);
    }
}

AI::~AI()
{
    _activePlayers.clear();
    _inactivePlayers.clear();
}

void AI::start()
{
    _tanks = _game->board()->initialEnemyTanks();
}

QList<QSharedPointer<AIPlayer> > AI::players() const
{
    QList<QSharedPointer<AIPlayer>> ret;
    for (auto &p: _activePlayers) {
        ret.append(p);
    }
    for (auto &p: _inactivePlayers) {
        ret.append(p);
    }
    return ret;
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
    return pos.value(qrand() % pos.count());
}

void AI::clockTick()
{
    if (_activateClock) {
        _activateClock--;
    }
    if (!_activateClock && _inactivePlayers.count() && _tanks.count()) {
        auto player = _inactivePlayers.takeLast();
        _activePlayers.append(player);
        player->start();
        _activateClock = 100;
    }

    for(auto &p: _activePlayers) {
        p->clockTick();
    }
}

void AI::deactivatePlayer()
{
    auto it = _activePlayers.begin();
    while (it != _activePlayers.end()) {
        if ((*it).data() == sender()) {
            _inactivePlayers.append(*it);
            _activePlayers.erase(it);
            break;
        }
        ++it;
    }
}

} // namespace Tanks

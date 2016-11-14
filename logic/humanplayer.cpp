#include "humanplayer.h"
#include "game.h"
#include "board.h"

namespace Tanks {

HumanPlayer::HumanPlayer(Game *game, int playerIndex) :
    _game(game),
    _playerIndex(playerIndex),
    _lifes(3),
    _moving(false),
    _shooting(false)
{

}

int HumanPlayer::lifesCount() const
{
    return _lifes;
}

void HumanPlayer::start()
{
    _tank = QSharedPointer<Tank>(new Tank(Friendly));
    emit newTankAvailable();
}

void HumanPlayer::move(Direction dir)
{
    if (_tank) {
        _moving = true;
        _tank->setDirection(dir);
    }
}

void HumanPlayer::fire()
{
    _shooting = true;
}

void HumanPlayer::stop()
{
    _moving = false;
}

void HumanPlayer::stopFire()
{
    _shooting = false;
}

void HumanPlayer::clockTick()
{
    if (!_tank) {
        return; // nothing todo w/o tank
    }

    QRect fmr;
    Board::BlockProps props;

    bool shouldMove = _moving && _tank->canMove();
    bool shouldShoot = _shooting && _tank->canShoot();
    if (shouldMove || shouldShoot) {
        fmr = _tank->forwardMoveRect(); // space before tank it's going to occupy
        props = _game->board()->rectProps(fmr);
    }

    if (shouldMove) {
        if (props & Board::TankObstackle) {
            return; // can't move
        }
        _tank->move();
        emit moved();
    }

    if (shouldShoot) {
        _bullet = _tank->makeBullet();
        emit fired();
    }
}

} // namespace Tanks

#include <QDebug>

#include "humanplayer.h"
#include "game.h"
#include "board.h"

namespace Tanks {

HumanPlayer::HumanPlayer(Game *game, int playerIndex) :
    _game(game),
    _playerIndex(playerIndex),
    _lifes(3),
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
	_oldDirection = _tank->direction();
	moveToStart();
    emit newTankAvailable();
}

void HumanPlayer::moveToStart()
{
	const auto &posList = _game->board()->friendlyStartPositions();
    _tank->setInitialPosition(posList[_playerIndex % posList.count()]);
}

void HumanPlayer::move(Direction dir)
{
    if (_tank) {
        Direction curDir = _tank->direction();
        _movingDir.removeAll(dir);
        _movingDir.prepend(dir);
        if (curDir != dir) {
            _oldDirection = curDir;
            _tank->setDirection(dir);
        }
    }
}

void HumanPlayer::fire()
{
    _shooting = true;
}

void HumanPlayer::stop(Direction dir)
{
    // we can stop pressing a key, but it does not mean other keys are not pressed
    // so we have to figure out what's left and continue moving
    _movingDir.removeAll(dir);
    if (!_movingDir.isEmpty()) {
        move(_movingDir.takeFirst());
    }
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
    AbstractPlayer::clockTick();

    QRect fmr;
    Board::BlockProps props;

    bool shouldMove = !_movingDir.isEmpty() && _tank->canMove();
    bool shouldShoot = (_shooting && _tank->canShoot());

    if (shouldMove || shouldShoot) {
        fmr = _tank->forwardMoveRect(); // space before tank it's going to occupy
        props = _game->board()->rectProps(fmr);
    }

    if (shouldMove && !(props & Board::TankObstackle)) {
        _tank->move();
        emit moved();
        _oldDirection = _tank->direction();
    } else if (_oldDirection != _tank->direction() && _tank->canMove()) {
        emit moved();
        _oldDirection = _tank->direction();
    }

    if (shouldShoot) {
        _bullet = _tank->fire();
        emit fired();
	}
}

void HumanPlayer::catchBullet()
{
	if (!_lifes) {
		qDebug("Something went wrong");
        return;
    }
    _lifes--;
    if (_lifes) {
		moveToStart();
		emit moved();
	} else {
        emit tankDestroyed();
        _tank.clear();
    }
    emit lifeLost();
}

void HumanPlayer::killAll()
{
    _lifes = 1;
    catchBullet();
}

} // namespace Tanks

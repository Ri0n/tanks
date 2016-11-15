#include <QDebug>

#include "aiplayer.h"
#include "ai.h"
#include "board.h"
#include "game.h"

namespace Tanks {

AIPlayer::AIPlayer(AI *ai) :
    _ai(ai)
{
}

int AIPlayer::lifesCount() const
{
    return _ai->lifesCount();
}

void AIPlayer::start()
{
    _tank = QSharedPointer<Tank>(new Tank(Alien, _ai->takeTank()));
    _tank->setInitialPosition(_ai->initialPosition());
    emit newTankAvailable();
}

void AIPlayer::clockTick()
{
    AbstractPlayer::clockTick();

    if (!_tank) {
        return;
    }

    bool forceShoot = false;

    if (_tank->canMove()) {
        Board::BlockProps props = _ai->game()->board()->rectProps(_tank->forwardMoveRect());
        bool canMoveForward = !(props & Board::TankObstackle);


        int r = qrand() % 16;
        int d = qrand() % 16;

        bool moving = r < 14;
        bool needNewDir = !canMoveForward || d > 12;


        if (needNewDir) {
            _tank->setDirection((Direction)(d % (East + 1)));
            props = _ai->game()->board()->rectProps(_tank->forwardMoveRect());
            canMoveForward = !(props & Board::TankObstackle);
        }

        if (canMoveForward && moving) {
            _tank->move();
            emit moved();
        } else if (props & Board::Breakable && !(props & Board::Sturdy)) {
            forceShoot = true;
        }
        if (!moving) {
            _tank->setClockPhase(20);
        }
    }

    if (_tank->canShoot()) {
        if (forceShoot || qrand() < RAND_MAX / 100 ) {
            _bullet = _tank->fire();
            emit fired();
        }
    }




}


} // namespace Tanks

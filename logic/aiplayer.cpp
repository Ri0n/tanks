#include <QDebug>

#include "aiplayer.h"
#include "ai.h"
#include "board.h"
#include "game.h"
#include "flag.h"

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

        bool moving = r < 15;
        bool needNewDir = !canMoveForward || d > 13;

        Direction oldDir = _tank->direction();
        if (needNewDir) {
            QPoint tc = _tank->geometry().center();
            QPoint fc = _ai->game()->flag()->geometry().center();
            Direction dirs[4]; // first directions are more probable (towards the flag)
            if (tc.x() < fc.x()) {
                dirs[0] = East;
                dirs[2] = West;
            } else {
                dirs[0] = West;
                dirs[2] = East;
            }
            if (tc.y() < fc.y()) {
                dirs[1] = South;
                dirs[3] = North;
            } else {
                dirs[1] = North;
                dirs[3] = South;
            }

            int toFlagInd = qrand() < (RAND_MAX * 0.85)? 0 : 2;
            Direction newDir = dirs[toFlagInd + (d & 1)];

            _tank->setDirection(newDir);
            props = _ai->game()->board()->rectProps(_tank->forwardMoveRect());
            canMoveForward = !(props & Board::TankObstackle);
        }

        if (canMoveForward && moving) {
            _tank->move();
            emit moved();
            oldDir = _tank->direction();
        } else if (props & Board::Breakable && !(props & Board::Sturdy)) {
            forceShoot = true;
        }
        if (oldDir != _tank->direction()) {
            emit moved(); // just turned, but anyway need to update
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

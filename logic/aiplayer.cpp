#include "aiplayer.h"
#include "ai.h"

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


} // namespace Tanks

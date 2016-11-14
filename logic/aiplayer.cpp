#include "aiplayer.h"
#include "ai.h"

namespace Tanks {

AIPlayer::AIPlayer(AI *ai) :
    _ai(ai)
{
    _tank = QSharedPointer<Tank>(new Tank(Alien));
}

int AIPlayer::lifesCount() const
{
    return _ai->lifesCount();
}

} // namespace Tanks

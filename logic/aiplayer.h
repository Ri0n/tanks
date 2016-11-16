#ifndef TANKS_AIPLAYER_H
#define TANKS_AIPLAYER_H

#include "abstractplayer.h"
#include "tank.h"

namespace Tanks {

class AI;

class AIPlayer : public AbstractPlayer
{
public:
    AIPlayer(AI *ai);
    int lifesCount() const;

    void start();
    void clockTick();
private:
    AI *_ai;
};

} // namespace Tanks

#endif // TANKS_AIPLAYER_H

#include "abstractplayer.h"

namespace Tanks {

AbstractPlayer::AbstractPlayer(QObject *parent) : QObject(parent)
{

}

void AbstractPlayer::clockTick()
{
    if (_tank) {
        _tank->clockTick();
    }
}

void AbstractPlayer::catchBullet()
{
     // todo
}

} // namespace Tanks

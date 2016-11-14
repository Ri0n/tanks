#include "bullet.h"

namespace Tanks {

Bullet::Bullet(Affinity affinity, Level level) :
    _affinity(affinity),
    _level(level)
{
    _geometry.setWidth(2);
    _geometry.setHeight(2);
}

DynamicBlock::OutBoardAction Bullet::outBoardAction() const
{
    return DynamicBlock::Disappear;
}

} // namespace Tanks

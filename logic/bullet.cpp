#include "bullet.h"

namespace Tanks {

Bullet::Bullet(Affinity affinity, Level level) :
    _affinity(affinity),
    _level(level)
{
    _geometry.setWidth(2);
    _geometry.setHeight(2);
}

void Bullet::explode(Bullet::ExplosionType et)
{
    _etype = et;
    emit detonated();
}

DynamicBlock::OutBoardAction Bullet::outBoardAction() const
{
    return DynamicBlock::Disappear;
}

} // namespace Tanks

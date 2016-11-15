#include "dynamicblock.h"

namespace Tanks {

DynamicBlock::DynamicBlock(quint8 speed, Direction direction) :
    _clockPhase(0),
    _direction(direction)
{
    setSpeed(speed);
}

void DynamicBlock::clockTick()
{
    if (_clockPhase) {
        _clockPhase--;
    }
}

bool DynamicBlock::canMove() const
{
    return _clockPhase == 0;
}

void DynamicBlock::move()
{
    int distance = 1;
    int dx = 0, dy = 0;
    switch (_direction) {
    case North:
        dy = -distance;
        break;
    case South:
        dy = distance;
        break;
    case West:
        dx = -distance;
        break;
    case East:
        dx = distance;
        break;
    }
    _geometry.translate(dx, dy);
    _clockPhase = _speed;
}

QRect DynamicBlock::forwardMoveRect(int distance) const
{
    switch (_direction) {
    case North:
        return QRect(_geometry.left(), _geometry.top() - distance, _geometry.width(), distance);
    case South:
        return QRect(_geometry.left(), _geometry.y()+_geometry.height(), _geometry.width(), distance);
    case West:
        return QRect(_geometry.left() - distance, _geometry.top(), distance, _geometry.height());
    case East:
        return QRect(_geometry.x() + _geometry.width(), _geometry.top(), distance, _geometry.height());
    }
    return QRect();
}

} // namespace Tanks

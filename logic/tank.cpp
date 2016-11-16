#include "tank.h"

namespace Tanks {

Tank::Tank(Affinity affinity, quint8 variant) :
    _affinity(affinity),
    _variant(variant),
    _shootTicks(0)
{
    setTankDefaults();
    _geometry.setWidth(4); // it's in board coords. so 4 instead of expected 2
    _geometry.setHeight(4);
}

void Tank::setTankDefaults()
{
    if (_affinity == Friendly) {
        _lives = 1;
        if (_variant == BurstFireTank) { // burst-fire tank
            _bulletCount = 2;
        }
    } else {
        if (_variant == SpeedyTank) { // speedy
            _speed = 2;
        }
        if (_variant == ArmoredTank) { // armoured
            _lives = 4;
        }
    }
}

QSharedPointer<Bullet> Tank::fire()
{
    auto b = new Bullet(_affinity,
                        isArmorPiercing()?Bullet::ArmorPiercing : Bullet::Regular);
    b->setSpeed(_affinity == Alien && _variant == FastBulletTank ? 3 : 2);

    QRect fmr = QRect(0,0,2,2);
    fmr.moveCenter(_geometry.center());
    int dx = 0, dy = 0;
    switch (_direction) {
    case North:
        dy = -1;
        break;
    case South:
        dy = 1;
        break;
    case West:
        dx = -1;
        break;
    case East:
        dx = 1;
        break;
    }
    fmr.translate(dx, dy);

    b->setInitialPosition(fmr.topLeft());
    b->setDirection(_direction);
    resetShootClock();

    return QSharedPointer<Bullet>(b);
}

void Tank::resetShootClock()
{
    _shootTicks = 10; // as example
    if (_affinity == Friendly && _variant == BurstFireTank) {
        _shootTicks = 5; // that's a little bit wrong for burst. will be fixed later
    }
}

DynamicBlock::OutBoardAction Tank::outBoardAction() const
{
    return DynamicBlock::StopMove;
}

void Tank::clockTick()
{
    if (_shootTicks) {
        _shootTicks--;
    }
    DynamicBlock::clockTick();
}

} // namespace Tanks

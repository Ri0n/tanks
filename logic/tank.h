#ifndef TANKS_TANK_H
#define TANKS_TANK_H

#include "dynamicblock.h"
#include "bullet.h"

namespace Tanks {

class Tank : public DynamicBlock
{
    Q_OBJECT
public:


    enum FriendlyVariant {
        SmallTank,
        SpeedFireTank,
        BurstFireTank,
        ArmorPiercingTank,
    };

    enum EnemyVariant {
        RegularTank,
        SpeedyTank,
        FastBulletTank,
        ArmoredTank,

        LastEnemyVariant
    };

    Tank(Affinity affinity, quint8 variant = 0);

    inline Affinity affinity() const { return _affinity; }
    inline quint8 variant() const { return _variant; }
    void setTankDefaults();

#if 0 // use less. just change clock count to freeze
    bool canMove() const; /* if we can move by timer and own will. */
    inline void setMoving(bool state) { _moving = state; } /* willing to move */
#endif
    inline bool canShoot() const { return _shootTicks == 0; }

    bool isArmorPiercing() const
    { return (_affinity == Friendly) && (_variant == ArmorPiercingTank); }

    QSharedPointer<Bullet> fire();
    void resetShootClock();

    OutBoardAction outBoardAction() const;

    void clockTick();

private:
    Affinity _affinity;
    quint8 _variant;
    quint8 _lives;
    quint8 _bulletCount;
    int _shootTicks;
};

} // namespace Tanks

#endif // TANKS_TANK_H

#ifndef TANKS_BULLET_H
#define TANKS_BULLET_H

#include "dynamicblock.h"

namespace Tanks {

class Bullet : public DynamicBlock
{
    Q_OBJECT
public:
    enum Level {
        Regular,
        ArmorPiercing
    };

    enum ExplosionType {
        NoDamage,
        BrickDestroyed,
        Explosion,
        BigExplosion
    };

    Bullet(Affinity affinity, Level level);

    inline Affinity affinity() const { return _affinity; }
    inline Level level() const { return _level; }
    void explode(ExplosionType);
    inline ExplosionType explosionType() const { return _etype; }

    OutBoardAction outBoardAction() const;

signals:
    void detonated();

private:
    Affinity _affinity;
    Level _level;
    ExplosionType _etype;
};

} // namespace Tanks

#endif // TANKS_BULLET_H

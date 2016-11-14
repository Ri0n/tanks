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

    Bullet(Affinity affinity, Level level);

    inline Affinity affinity() const { return _affinity; }
    inline Level level() const { return _level; }

    OutBoardAction outBoardAction() const;

private:
    Affinity _affinity;
    Level _level;
};

} // namespace Tanks

#endif // TANKS_BULLET_H

#ifndef TANKS_DYNAMICBLOCK_H
#define TANKS_DYNAMICBLOCK_H


#include <QtGlobal>

#include "basics.h"
#include "block.h"

namespace Tanks {

class DynamicBlock : public Block
{
    Q_OBJECT
public:


    enum ForwardHint {
        ForwardNothing,
        ForwardBreakable = 1,
        ForwardBlock = 2
    };
    Q_DECLARE_FLAGS(ForwardHints, ForwardHint)

    enum OutBoardAction {
        Disappear,
        StopMove
    };

    /* regarding speeds. we need next
     * 0 - slowest possible. regular
     * 1 - fast tank
     * 2 - bullet
     * 3 - fast bullet
     */

    DynamicBlock(quint8 speed = 1, Direction direction = North);
    inline void setClockPhase(quint16 phase) { _clockPhase = phase; } // useful for freeze bonus
    virtual void clockTick();
    virtual bool canMove() const;
    void move();
    virtual OutBoardAction outBoardAction() const = 0;
    QRect forwardMoveRect(int distance = 1) const;

    inline void setDirection(Direction dir) {
        _direction = dir;
    }
    inline Direction direction() const { return _direction; }

    inline void setSpeed(quint8 speed) {
        _speed = speed > 3? 3 : speed;
    }

protected:



protected:
    quint16 _clockPhase;
    quint8 _speed;
    Direction _direction;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DynamicBlock::ForwardHints)

} // namespace Tanks

#endif // TANKS_DYNAMICBLOCK_H

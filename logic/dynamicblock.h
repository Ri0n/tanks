/*
 Copyright (c) 2016, Sergey Ilinykh
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL IL'INYKH SERGEY BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef TANKS_DYNAMICBLOCK_H
#define TANKS_DYNAMICBLOCK_H

#include "basics.h"
#include "block.h"

#include <QtGlobal>

namespace Tanks {

class DynamicBlock : public Block {
    Q_OBJECT
public:
    enum ForwardHint { ForwardNothing, ForwardBreakable = 1, ForwardBlock = 2 };
    Q_DECLARE_FLAGS(ForwardHints, ForwardHint)

    enum OutBoardAction { Disappear, StopMove };

    /* regarding speeds. we need next
     * 0 - fast bullet
     * 1 - bullet
     * 2 - fast tank
     * 3 - slowest possible. regular
     */

    DynamicBlock(quint8 speed = 3, Direction direction = North);
    inline void            setClockPhase(quint16 phase) { _clockPhase = phase; } // useful for freeze bonus
    virtual void           clockTick();
    virtual bool           canMove() const;
    void                   move();
    virtual OutBoardAction outBoardAction() const = 0;
    QRect                  forwardMoveRect(int distance = 1) const;

    inline void setDirection(Direction dir)
    {
        _direction = dir;
        emit moved();
    }
    inline Direction direction() const { return _direction; }

    inline void setSpeed(quint8 speed) { _speed = speed > 3 ? 3 : speed; }

protected:
signals:
    void moved();

protected:
    quint16   _clockPhase;
    quint8    _speed;
    Direction _direction;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DynamicBlock::ForwardHints)

} // namespace Tanks

#endif // TANKS_DYNAMICBLOCK_H

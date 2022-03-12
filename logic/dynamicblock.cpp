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

#include "dynamicblock.h"

namespace Tanks {

DynamicBlock::DynamicBlock(quint8 speed, Direction direction) : _clockPhase(0), _direction(direction)
{
    setSpeed(speed);
}

void DynamicBlock::clockTick()
{
    if (_clockPhase) {
        _clockPhase--;
    }
}

bool DynamicBlock::canMove() const { return _clockPhase == 0; }

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

    emit moved();
}

QRect DynamicBlock::forwardMoveRect(int distance) const
{
    switch (_direction) {
    case North:
        return QRect(_geometry.left(), _geometry.top() - distance, _geometry.width(), distance);
    case South:
        return QRect(_geometry.left(), _geometry.y() + _geometry.height(), _geometry.width(), distance);
    case West:
        return QRect(_geometry.left() - distance, _geometry.top(), distance, _geometry.height());
    case East:
        return QRect(_geometry.x() + _geometry.width(), _geometry.top(), distance, _geometry.height());
    }
    return QRect();
}

} // namespace Tanks

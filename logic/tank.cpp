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

#include "tank.h"

namespace Tanks {

Tank::Tank(Affinity affinity, quint8 variant) : _affinity(affinity), _variant(variant), _shootTicks(0)
{
    setTankDefaults();
    _geometry.setWidth(4); // it's in board coords. so 4 instead of expected 2
    _geometry.setHeight(4);
}

void Tank::setTankDefaults()
{
    _armorLevel = 1;
    if (_affinity == Friendly) {
        _armorLevel = 1;
        if (_variant == BurstFireTank) { // burst-fire tank
            _bulletCount = 2;
        }
    } else {
        if (_variant == SpeedyTank) { // speedy
            _speed = 2;
        }
        if (_variant == ArmoredTank) { // armoured
            _armorLevel = 4;
        }
    }
}

void Tank::fire()
{
    auto b = new Bullet(_affinity, isArmorPiercing() ? Bullet::ArmorPiercing : Bullet::Regular);
    b->setSpeed(_affinity == Alien && _variant == FastBulletTank ? 3 : 2);

    QRect fmr = QRect(0, 0, 2, 2);
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

    _bullet = QSharedPointer<Bullet>(b);

    emit fired();
}

void Tank::resetShootClock()
{
    _shootTicks = 10; // as example
    if (_affinity == Friendly && _variant == BurstFireTank) {
        _shootTicks = 5; // that's a little bit wrong for burst. will be fixed later
    }
}

DynamicBlock::OutBoardAction Tank::outBoardAction() const { return DynamicBlock::StopMove; }

void Tank::clockTick()
{
    if (_shootTicks) {
        _shootTicks--;
    }
    DynamicBlock::clockTick();
}

void Tank::catchBullet()
{
    if (!_armorLevel) {
        qDebug("Something went wrong");
        return;
    }
    _armorLevel--;
    if (_armorLevel) {
        emit armourChanged();
    } else {
        emit tankDestroyed();
    }
}

void Tank::selfDestroy()
{
    _armorLevel = 1;
    catchBullet();
}

} // namespace Tanks

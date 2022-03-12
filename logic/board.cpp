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

#include "board.h"
#include "abstractmaploader.h"
#include "staticblock.h"
#include "tank.h"

#include <QTimer>

namespace Tanks {

// That's where we want to split out blocks even more.
// For example when just half a brick is breakable by bullet
// or tank moves just half a brick at a time
#define MAP_SCALE_FACTOR 2

Board::Board(QObject *parent) : QObject(parent) { }

bool Board::loadMap(AbstractMapLoader *loader)
{
    if (!loader->open()) {
        return false;
    }
    //_dynBlocks.clear();
    _size = loader->dimensions() * MAP_SCALE_FACTOR;
    _size = _size.boundedTo(QSize(1024, 1024));
    QRect boardRect(QPoint(0, 0), _size);
    _map.resize(_size.width() * _size.height());
    _map.fill(0);

    while (loader->hasNext()) {
        MapObject block = loader->next();
        QRect     cropped(block.geometry.topLeft() * MAP_SCALE_FACTOR, block.geometry.size() * MAP_SCALE_FACTOR);
        cropped &= boardRect;
        if (cropped.isEmpty()) {
            continue;
        }
        renderBlock(block.type, cropped);
    }

    _flagPosition = loader->flagPosition() * MAP_SCALE_FACTOR;
    renderBlock(Nothing, QRect(_flagPosition, QSize(4, 4)));
    renderFlagFrame(Brick);

    _initialEnemyTanks = loader->enemyTanks();

    foreach (const QPoint &p, loader->enemyStartPositions()) {
        QPoint sp = p * MAP_SCALE_FACTOR;
        _enemyStartPositions.append(sp);
        renderBlock(Nothing, QRect(sp, QSize(4, 4)));
    }

    foreach (const QPoint &p, loader->friendlyStartPositions()) {
        QPoint sp = p * MAP_SCALE_FACTOR;
        _friendlyStartPositions.append(sp);
        renderBlock(Nothing, QRect(sp, QSize(4, 4)));
    }

    return true;
}

void Board::renderBlock(MapObjectType type, const QRect &area)
{
    QRect cr = QRect(QPoint(0, 0), _size) & area;

    if (cr.isEmpty())
        return;

    int start = posToMapIndex(cr.topLeft());
    for (int r = 0; r < cr.height(); r++) {
        for (int c = 0; c < cr.width(); c++) {
            _map[start + c] = type;
        }
        start += _size.width();
    }
}

void Board::renderFlagFrame(MapObjectType type)
{
    QPoint tl = _flagPosition - QPoint(2, 2);
    renderBlock(type, QRect(tl, QSize(2, 8)));
    renderBlock(type, QRect(tl, QSize(8, 2)));
    renderBlock(type, QRect(tl + QPoint(0, 6), QSize(8, 2)));
    renderBlock(type, QRect(tl + QPoint(6, 0), QSize(2, 8)));
}

int Board::blockDivider() const { return MAP_SCALE_FACTOR; }

Board::BlockProps Board::rectProps(const QRect &rect)
{
    Board::BlockProps props;
    if (!QRect(QPoint(0, 0), _size).contains(rect)) {
        return TankObstackle;
    }
    for (int i = 0; i < rect.width(); i++) {
        for (int j = 0; j < rect.height(); j++) {
            props |= blockProperties(QPoint(rect.x() + i, rect.y() + j));
        }
    }
    return props;
}

Board::BlockProps Board::blockTypeProperties(MapObjectType type) const
{
    switch (type) {
    case Concrete:
        return TankObstackle | BulletObstackle | Breakable | Sturdy;
    case Brick:
        return TankObstackle | BulletObstackle | Breakable;
    case Nothing:
    case Bush:
    case LastMapObjectType: // make compiler happy
        return {};
    case Ice:
        return BadManoeuvre;
    case Water:
        return TankObstackle;
    }
    return {};
}

} // namespace Tanks

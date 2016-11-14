#include <QTimer>

#include "board.h"
#include "staticblock.h"
#include "abstractmaploader.h"
#include "tank.h"

namespace Tanks {

// That's where we want to split out blocks even more.
// For example when just half a brick is breakable by bullet
// or tank moves just half a brick at a time
#define MAP_SCALE_FACTOR 2

Board::Board(QObject *parent) : QObject(parent)
{

}

bool Board::loadMap(AbstractMapLoader *loader)
{
    if (!loader->open()) {
        return false;
    }
    _dynBlocks.clear();
    _size = loader->dimensions() * MAP_SCALE_FACTOR;
    _size.boundedTo(QSize(1024,1024));
    QRect boardRect(QPoint(0,0), _size);
    _map.resize(_size.width()*_size.height());
    _map.fill(0);

    while (loader->hasNext()) {
        MapObject block = loader->next();
        QRect cropped(block.geometry.topLeft() * MAP_SCALE_FACTOR,
                      block.geometry.size() * MAP_SCALE_FACTOR);
        cropped &= boardRect;
        if (cropped.isEmpty()) {
            continue;
        }
        renderBlock(block.type, cropped);
    }
    _initialEnemyTanks = loader->enemyTanks();

    foreach (const QPoint &p, loader->enemyStartPositions()) {
        _enemyStartPositions.append(p * MAP_SCALE_FACTOR);
    }

    foreach (const QPoint &p, loader->friendlyStartPositions()) {
        _friendlyStartPositions.append(p * MAP_SCALE_FACTOR);
    }

    return true;
}

void Board::renderBlock(MapObjectType type, const QRect &area)
{
    int start = posToMapIndex(area.topLeft());
    for (int r = 0; r < area.height(); r++) {
        for (int c = 0; c < area.width(); c++) {
            _map[start + c] = type;
        }
        start += _size.width();
    }
}

int Board::blockDivider() const
{
    return MAP_SCALE_FACTOR;
}

bool Board::addDynBlock(QSharedPointer<DynamicBlock> dblock)
{
    QRect board(QPoint(0,0), _size);
    if (!board.contains(dblock->geometry())) {
        return false; // it's initially possible only for bullets.
    }

    _dynBlocks.append(dblock);
    // Do we need to do aything else here?
    return true;
}

void Board::clockTick()
{
    auto it = _dynBlocks.begin();
    while (it != _dynBlocks.end()) {
        QSharedPointer<DynamicBlock> b = *it;
        if (b->canMove()) {
            QRect fmr = b->forwardMoveRect(); // space before tank it's going to occupy
            BlockProps props = rectProps(fmr);
            QSharedPointer<Tank> tank = b.dynamicCast<Tank>();
            if (tank && props & TankObstackle) {
                continue; // can't move
// next code should be used only for CompPlayer
//                DynamicBlock::ForwardHints hint = DynamicBlock::ForwardNothing;
//                if (props & TankObstackle) {
//                    bool canBreak = (props & Breakable) && ((props & Sturdy) || tank->isArmorPiercing());
//                    hint |= (canBreak? DynamicBlock::ForwardBreakable : DynamicBlock::ForwardBlock);
//                }
            }

            //tank->move();

            if (tank && (props & (Dangerous | Explosive))) {
                // These types of map block unsupported yet. may be mines in the future
                continue;
            }

            auto cmpIt = it + 1;
            while (cmpIt != _dynBlocks.end()) {
                if (b->hasClash(**cmpIt)) { // dered iterator and deref smart pointer

                }
                ++cmpIt;
            }

        }
        ++it;
    }
}

Board::BlockProps Board::rectProps(const QRect &rect)
{
    Board::BlockProps props;
    if (!QRect(QPoint(0,0), _size).contains(rect)) {
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
        return TankObstackle|BulletObstackle|Breakable|Sturdy;
    case Brick:
        return TankObstackle|BulletObstackle|Breakable;
    case Nothing:
    case Bush:
    case LastMapObjectType: // make compiler happy
        return 0;
    case Ice:
        return BadManoeuvre;
    case Water:
        return TankObstackle;
    }
    return 0;
}

} // namespace Tanks

#ifndef TANKS_ABSTRACTMAPLOADER_H
#define TANKS_ABSTRACTMAPLOADER_H

#include <QtGlobal>
#include <QRect>

#include "basics.h"

namespace Tanks {



class MapObject
{
public:
    QRect geometry;
    MapObjectType type;
};

class AbstractMapLoader
{
public:
    AbstractMapLoader();
    virtual ~AbstractMapLoader();

    virtual bool open() = 0;
    virtual QSize dimensions() const = 0;
    virtual bool hasNext() const = 0;
    virtual MapObject next() = 0;
    virtual QList<quint8> enemyTanks() const = 0;
    virtual QList<QPoint> enemyStartPositions() const = 0;
    virtual QList<QPoint> friendlyStartPositions() const = 0;
    virtual QPoint flagPosition() const = 0;
};

} // namespace Tanks

#endif // TANKS_ABSTRACTMAPLOADER_H

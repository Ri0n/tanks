#ifndef TANKS_RANDOMMAPLOADER_H
#define TANKS_RANDOMMAPLOADER_H

#include <QQueue>

#include "abstractmaploader.h"

namespace Tanks {

class RandomMapLoader : public AbstractMapLoader
{
    // abstract shape
    struct PendingShape {
        MapObjectType type;
        int minSize;
        int maxSize;
    };

public:
    RandomMapLoader();

    bool open();
    QSize dimensions() const;
    bool hasNext() const;
    MapObject next();
    QList<quint8> enemyTanks() const;
    QList<QPoint> enemyStartPositions() const;
    QList<QPoint> friendlyStartPositions() const;

private:
    void generateShape(const PendingShape &shape);

private:
    int boardWidth;
    int boardHeight;
    QQueue<PendingShape> shapesQueue;
    QQueue<MapObject> objectQueue;
};

} // namespace Tanks

#endif // TANKS_RANDOMMAPLOADER_H

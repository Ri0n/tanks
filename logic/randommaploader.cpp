#include <QDateTime>
#include <QPainter>
#include <QDebug>

#include "randommaploader.h"
#include "tank.h"

namespace Tanks {

RandomMapLoader::RandomMapLoader() :
    boardWidth(50),
    boardHeight(50)
{
    qsrand(QDateTime::currentDateTime().toTime_t());
}

bool RandomMapLoader::open()
{
    int i;
    for (i = 0; i < 40; i++) {
        // draw bricks
        shapesQueue.enqueue({Brick, 4, 20});
    }

    for (i = 0; i < 10; i++) {
        // draw concrete
        shapesQueue.enqueue({Concrete, 3, 8});
    }

    for (i = 0; i < 10; i++) {
        // draw water
        shapesQueue.enqueue({Water, 3, 8});
    }

    for (i = 0; i < 10; i++) {
        // draw water
        shapesQueue.enqueue({Ice, 3, 8});
    }

    for (i = 0; i < 20; i++) {
        // draw bush
        shapesQueue.enqueue({Bush, 3, 8});
    }
    return true;
}

void RandomMapLoader::generateShape(const PendingShape &shape)
{
    int rndWidth = qMax(shape.minSize, qrand()%(shape.maxSize+1));
    int rndHeight = qMax(shape.minSize, qrand()%(shape.maxSize+1));
    int rndLeft = qrand() % boardWidth - rndWidth / 2;
    int rndTop = qrand() % boardHeight - rndHeight / 2;

    int shapeVariant = qrand()%6; // with accent to ellipses
    switch (shapeVariant) {
    case 0:
        // vertical bar
        objectQueue.enqueue(MapObject{QRect(rndLeft, rndTop, rndWidth, 2), shape.type});
        return;
    case 1:
        // horizontal bar
        objectQueue.enqueue(MapObject{QRect(rndLeft, rndTop, 2, rndHeight), shape.type});
        return;
    case 2:
        if (shape.type != Brick) {
            objectQueue.enqueue(MapObject{QRect(rndLeft, rndTop, rndWidth, rndHeight), shape.type});
        } else {
            objectQueue.enqueue(MapObject{QRect(rndLeft, rndTop, 1, rndHeight), shape.type});
            objectQueue.enqueue(MapObject{QRect(rndLeft, rndTop, rndWidth, 1), shape.type});
            objectQueue.enqueue(MapObject{QRect(rndLeft+rndWidth-1, rndTop, 1, rndHeight), shape.type});
            objectQueue.enqueue(MapObject{QRect(rndLeft, rndTop+rndHeight-1, rndWidth, 1), shape.type});
        }
        return;
    }

    // comlex shapes left. have to scan them
    QSize shapeSize(rndWidth, rndHeight);
    QImage img(shapeSize, QImage::Format_ARGB32);
    img.fill(QColor(Qt::black));
    QPainter painter(&img);
    if (shape.type != Brick) {
        painter.setBrush(QColor(Qt::white));
    } else {
        QPen pen(QColor(Qt::white));
        int penWidth = 2;
        pen.setWidth(penWidth);
        painter.setPen(pen);
        shapeSize -= QSize(penWidth, penWidth);
    }
    QRect drawRect(QPoint(0,0), shapeSize);


    // only ellipse is supported
    painter.drawEllipse(drawRect);

    for (int y = 0; y < rndHeight; y++) {
        for (int x = 0; x < rndWidth; x++) {
            if (img.pixelColor(x, y) != Qt::black) {
                objectQueue.enqueue(MapObject{QRect(rndLeft+x, rndTop+y, 1, 1), shape.type});
            }
        }
    }
}

QSize RandomMapLoader::dimensions() const
{
    return QSize(boardWidth,boardHeight);
}

bool RandomMapLoader::hasNext() const
{
    return !shapesQueue.isEmpty() || !objectQueue.isEmpty();
}

MapObject RandomMapLoader::next()
{
    if (objectQueue.isEmpty()) {
        Q_ASSERT(!shapesQueue.isEmpty());
        generateShape(shapesQueue.dequeue());
    }
    return objectQueue.dequeue();
}

QList<quint8> RandomMapLoader::enemyTanks() const
{
    QList<quint8> ret;
    ret.reserve(10);
    for (int i = 0; i < 10; i++) {
        int val = qrand() % 12;
        if (val > 10) { // 11
            ret.append(Tank::ArmoredTank);
        } else if (val > 7) { // 8,9
            ret.append(Tank::FastBulletTank);
        } else if (val > 4) { // 5,6,7
            ret.append(Tank::SpeedyTank);
        } else { // 0,1,2,3,4
            ret.append(Tank::RegularTank);
        }
    }
    return ret;
}

QList<QPoint> RandomMapLoader::enemyStartPositions() const
{
    return QList<QPoint>() << QPoint(0,0) << QPoint(boardWidth - 2, 0)
                           << QPoint(boardWidth / 2, 0);
}

QList<QPoint> RandomMapLoader::friendlyStartPositions() const
{
    return QList<QPoint>() << QPoint(boardWidth / 2 - 5, boardHeight - 2)
                           << QPoint(boardWidth / 2 + 3, boardHeight - 2);
}

QPoint RandomMapLoader::flagPosition() const
{
    return QPoint(boardWidth / 2 - 2, boardHeight - 2);
}

} // namespace Tanks

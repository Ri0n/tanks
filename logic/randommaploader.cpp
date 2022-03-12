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

#include "randommaploader.h"
#include "tank.h"

#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <QRandomGenerator>

namespace Tanks {

RandomMapLoader::RandomMapLoader() : boardWidth(50), boardHeight(50) { }

bool RandomMapLoader::open()
{
    int i;

    shapesQueue.clear();
    objectQueue.clear();

    for (i = 0; i < 20; i++) {
        // draw bricks
        shapesQueue.enqueue({ Brick, 4, 20 });
    }

    for (i = 0; i < 10; i++) {
        // draw concrete
        shapesQueue.enqueue({ Concrete, 3, 8 });
    }

    for (i = 0; i < 10; i++) {
        // draw water
        shapesQueue.enqueue({ Water, 3, 8 });
    }

    for (i = 0; i < 10; i++) {
        // draw water
        shapesQueue.enqueue({ Ice, 3, 8 });
    }

    for (i = 0; i < 20; i++) {
        // draw bush
        shapesQueue.enqueue({ Bush, 3, 8 });
    }
    return true;
}

void RandomMapLoader::generateShape(const PendingShape &shape)
{
    int rndWidth  = qMax(shape.minSize, QRandomGenerator::global()->bounded(shape.maxSize + 1));
    int rndHeight = qMax(shape.minSize, QRandomGenerator::global()->bounded(shape.maxSize + 1));
    int rndLeft   = QRandomGenerator::global()->bounded(boardWidth) - rndWidth / 2;
    int rndTop    = QRandomGenerator::global()->bounded(boardHeight) - rndHeight / 2;

    int shapeVariant = QRandomGenerator::global()->bounded(6); // with accent to ellipses
    switch (shapeVariant) {
    case 0:
        // vertical bar
        objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop, rndWidth, 2), shape.type });
        return;
    case 1:
        // horizontal bar
        objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop, 2, rndHeight), shape.type });
        return;
    case 2:
        if (shape.type != Brick) {
            objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop, rndWidth, rndHeight), shape.type });
        } else {
            objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop, 1, rndHeight), shape.type });
            objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop, rndWidth, 1), shape.type });
            objectQueue.enqueue(MapObject { QRect(rndLeft + rndWidth - 1, rndTop, 1, rndHeight), shape.type });
            objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop + rndHeight - 1, rndWidth, 1), shape.type });
        }
        return;
    }

    // comlex shapes left. have to scan them
    QSize  shapeSize(rndWidth, rndHeight);
    QImage img(shapeSize, QImage::Format_ARGB32);
    img.fill(QColor(Qt::black));
    QPainter painter(&img);
    if (shape.type != Brick) {
        painter.setBrush(QColor(Qt::white));
    } else {
        QPen pen(Qt::white);
        int  penWidth = 2;
        pen.setWidth(penWidth);
        painter.setPen(pen);
        shapeSize -= QSize(penWidth, penWidth);
    }
    QRect drawRect(QPoint(0, 0), shapeSize);

    // only ellipse is supported
    painter.drawEllipse(drawRect);

    for (int y = 0; y < rndHeight; y++) {
        for (int x = 0; x < rndWidth; x++) {
            if (img.pixelColor(x, y) != Qt::black) {
                objectQueue.enqueue(MapObject { QRect(rndLeft + x, rndTop + y, 1, 1), shape.type });
            }
        }
    }
}

QSize RandomMapLoader::dimensions() const { return QSize(boardWidth, boardHeight); }

bool RandomMapLoader::hasNext() const { return !shapesQueue.isEmpty() || !objectQueue.isEmpty(); }

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
    for (int i = 0; i < 20; i++) {
        int val = QRandomGenerator::global()->bounded(12);
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
    return QList<QPoint>() << QPoint(0, 0) << QPoint(boardWidth - 2, 0) << QPoint(boardWidth / 2, 0);
}

QList<QPoint> RandomMapLoader::friendlyStartPositions() const
{
    return QList<QPoint>() << QPoint(boardWidth / 2 - 5, boardHeight - 2)
                           << QPoint(boardWidth / 2 + 1, boardHeight - 2);
}

QPoint RandomMapLoader::flagPosition() const { return QPoint(boardWidth / 2 - 2, boardHeight - 2); }

} // namespace Tanks

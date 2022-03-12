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

#include "qmltankimageprovider.h"
#include "board.h"
#include "game.h"
#include "qmlbridge.h"

namespace Tanks {

QMLTankImageProvider::QMLTankImageProvider() : QQuickImageProvider(QQmlImageProviderBase::Image)
{
    alltanks.load(":/img/tanks");
}

QImage QMLTankImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QSize s(16, 16); // size of tank in px
    if (size) {
        *size = s;
    }

    QStringList idParts   = id.split('/');
    Affinity    affinity  = (Affinity)idParts[0].toInt();
    int         tankLevel = idParts[1].toInt();
    Direction   dir       = (Direction)(idParts[2].toInt() % (East + 1));
    int         animFrame = idParts[3].toInt();
    QPoint      pos(animFrame * s.width(), tankLevel * s.height());
    if (affinity == Alien) {
        pos += QPoint(0, 64);
    }

    QImage img = alltanks.copy(QRect(pos, s));

    int degrees[] = { 0, 180, 270, 90 };

    QTransform rot;
    rot.rotate(degrees[dir]);
    img = img.transformed(rot);

    if (requestedSize.isValid()) {
        if (requestedSize.width()) {
            if (requestedSize.height()) {
                return img.scaled(requestedSize, Qt::KeepAspectRatio);
            } else {
                return img.scaledToWidth(requestedSize.width());
            }
        } else if (requestedSize.height()) {
            return img.scaledToHeight(requestedSize.height());
        }
    }
    return img;
}

QMLBulletImageProvider::QMLBulletImageProvider() : QQuickImageProvider(QQmlImageProviderBase::Image) { }

QImage QMLBulletImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QSize s(8, 8); // size of bullet in px
    if (size) {
        *size = s;
    }

    QImage    img(":/img/bullet");
    Direction dir = (Direction)(id.toInt() % (East + 1));

    int degrees[] = { 0, 180, 270, 90 };

    QTransform rot;
    rot.rotate(degrees[dir]);
    img = img.transformed(rot);

    if (requestedSize.isValid()) {
        if (requestedSize.width()) {
            if (requestedSize.height()) {
                return img.scaled(requestedSize, Qt::KeepAspectRatio);
            } else {
                return img.scaledToWidth(requestedSize.width());
            }
        } else if (requestedSize.height()) {
            return img.scaledToHeight(requestedSize.height());
        }
    }
    return img;
}

} // namespace Tanks

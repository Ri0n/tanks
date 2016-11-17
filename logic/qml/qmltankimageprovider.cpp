#include "qmltankimageprovider.h"
#include "game.h"
#include "board.h"
#include "qmlbridge.h"

namespace Tanks {

QMLTankImageProvider::QMLTankImageProvider() :
    QQuickImageProvider(QQmlImageProviderBase::Image)
{
    alltanks.load(":/img/tanks");
}

QImage QMLTankImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QSize s(16,16); // size of tank in px
    if (size) {
        *size = s;
    }

    QStringList idParts = id.split('/');
    Affinity affinity = (Affinity)idParts[0].toInt();
    int tankLevel = idParts[1].toInt();
    Direction dir = (Direction)(idParts[2].toInt() % (East+1));
    int animFrame = idParts[3].toInt();
    QPoint pos(animFrame * s.width(), tankLevel * s.height());
    if (affinity == Alien) {
        pos += QPoint(0, 64);
    }

    QImage img = alltanks.copy(QRect(pos, s));

    int degrees[] = {0, 180, 270, 90};

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


QMLBulletImageProvider::QMLBulletImageProvider() :
    QQuickImageProvider(QQmlImageProviderBase::Image)
{
}

QImage QMLBulletImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QSize s(8,8); // size of bullet in px
    if (size) {
        *size = s;
    }

    QImage img(":/img/bullet");
    Direction dir = (Direction)(id.toInt() % (East+1));

    int degrees[] = {0, 180, 270, 90};

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

#include "qmlmapimageprovider.h"
#include "qmlbridge.h"

namespace Tanks {

QMLMapImageProvider::QMLMapImageProvider() :
    QQuickImageProvider(QQmlImageProviderBase::Image)
{

}

void QMLMapImageProvider::registerBridge(QMLBridge *bridge)
{
    int i = 0;
    QString id;
    do {
        id = QString("b%1").arg(i++);
    } while (_bridges.contains(id));

    bridge->setBridgeId(id);
    _bridges.insert(id, bridge);
}

QImage QMLMapImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QSize s(16,16); // size of tank in px
    if (size) {
        *size = s;
    }

    QStringList idParts = id.split('/');
    QString bridgeId = idParts[0];
    QString layerId = idParts[1];

    QMLBridge *bridge = 0;
    QMutableMapIterator<QString,QPointer<QMLBridge>> it(_bridges);
    while (it.hasNext()) {
        auto n = it.next();
        if (!n.value()) { // dangled pointer
            it.remove();
            continue;
        }
        if (n.key() == bridgeId) {
            bridge = n.value();
        }
    }
    Q_ASSERT(bridge);

    QImage img;
    if (layerId == QLatin1Literal("map")) {
        img = bridge->lowerMapImage();
    } else if (layerId == QLatin1Literal("bush")) {
        img = bridge->bushImage();
    }

    if (!img.isNull() && requestedSize.isValid()) {
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

QMap<QString,QPointer<QMLBridge>> QMLMapImageProvider::_bridges;

} // namespace Tanks

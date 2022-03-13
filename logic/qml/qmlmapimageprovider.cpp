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

#include "qmlmapimageprovider.h"
#include "qmlbridge.h"

namespace Tanks {

QMLMapImageProvider::QMLMapImageProvider() : QQuickImageProvider(QQmlImageProviderBase::Image) { }

void QMLMapImageProvider::registerBridge(QMLBridge *bridge)
{
    int     i = 0;
    QString id;
    do {
        id = QString("b%1").arg(i++);
    } while (_bridges.contains(id));

    bridge->setBridgeId(id);
    _bridges.insert(id, bridge);
}

QImage QMLMapImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QSize s(16, 16); // size of tank in px
    if (size) {
        *size = s;
    }

    QStringList idParts  = id.split('/');
    QString     bridgeId = idParts[0];
    QString     layerId  = idParts[1];

    QMLBridge                                        *bridge = 0;
    QMutableMapIterator<QString, QPointer<QMLBridge>> it(_bridges);
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
    if (layerId == QLatin1String("map")) {
        img = bridge->lowerMapImage();
    } else if (layerId == QLatin1String("bush")) {
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

QMap<QString, QPointer<QMLBridge>> QMLMapImageProvider::_bridges;

} // namespace Tanks

#ifndef TANKS_QMLMAPIMAGEPROVIDER_H
#define TANKS_QMLMAPIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QPointer>

namespace Tanks {

class QMLBridge;

class QMLMapImageProvider : public QQuickImageProvider
{
public:
    QMLMapImageProvider();

    static void registerBridge(QMLBridge *);

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    static QMap<QString,QPointer<QMLBridge>> _bridges;
};

} // namespace Tanks

#endif // TANKS_QMLMAPIMAGEPROVIDER_H

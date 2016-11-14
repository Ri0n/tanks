#ifndef TANKS_QMLTANKIMAGEPROVIDER_H
#define TANKS_QMLTANKIMAGEPROVIDER_H

#include <QQuickImageProvider>

namespace Tanks {

class QMLBridge;

class QMLTankImageProvider : public QQuickImageProvider
{
public:
    QMLTankImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    QImage alltanks;
};

class QMLBulletImageProvider : public QQuickImageProvider
{
public:
    QMLBulletImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

} // namespace Tanks

#endif // TANKS_QMLTANKIMAGEPROVIDER_H

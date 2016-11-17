#ifndef QMLBRIDGE_H
#define QMLBRIDGE_H

#include <QObject>
#include <QImage>
#include <QTemporaryDir>
#include <QVariant>

#include "block.h"

namespace Tanks {

class Game;
class Tank;

class QMLBridge : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString bridgeId READ bridgeId)
    Q_PROPERTY(QSize boardImageSize READ boardImageSize NOTIFY mapRendered)
    Q_PROPERTY(QRect flagGeometry READ flagGeometry NOTIFY mapRendered)
    Q_PROPERTY(QString flagFile READ flagFile NOTIFY flagChanged)

    Q_PROPERTY(QString lifesStat READ lifesStat NOTIFY statsChanged)

public:
    explicit QMLBridge(QObject *parent = 0);
    QImage lowerMapImage() const;
    QImage bushImage() const;

    QSize boardImageSize() const;
    QRect flagGeometry() const;
    QString flagFile() const;
    QString lifesStat() const;

    inline QString bridgeId() const { return _bridgeId; }
    void setBridgeId(const QString &id);

private:
    QVariant tank2variant(Tank *tank);

signals:
    void mapRendered();
    void statsChanged();
    void blockRemoved(QRect block);

    void newTank(QVariant tank);
    void tankUpdated(QVariant tank);
    void tankDestroyed(QString id);

    void newBullet(QVariant bullet);
    void bulletMoved(QString id, QPoint pos);
    void bulletDetonated(QString id, int reason);

    void flagChanged();

    void qmlTankAction(int player, int key);
    void qmlTankActionStop(int player, int key);

public slots:
    void restart();

private slots:
    void humanTankAction(int player, int key);
    void humanTankActionStop(int player, int key);

    void mapLoaded();

    void newTankAvailable(QObject *obj);
    void newBulletAvailable(QObject *obj);
    void moveTank(QObject *obj);
    void removeBlock(const QRect &r);
    void moveBullet(QObject *obj);
    void destroyTank(QObject *obj);
    void detonateBullet();
private:
    QString _bridgeId;
    QTemporaryDir _tmpDir;
    Game *_game;

    QImage _lowerMapImage;
    QImage _bushImage;

    int _qmlId;
    //QHash<QString, QWeakPointer<Block>> _activeBlocks;
};

} // namespace Tanks

#endif // QMLBRIDGE_H

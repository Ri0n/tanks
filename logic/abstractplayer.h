#ifndef TANKS_ABSTRACTPLAYER_H
#define TANKS_ABSTRACTPLAYER_H

#include <QObject>

#include "tank.h"
#include "bullet.h"

namespace Tanks {


class AbstractPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AbstractPlayer(QObject *parent = 0);
    virtual int lifesCount() const = 0;
    QSharedPointer<Tank> tank() const { return _tank; }
    QSharedPointer<Bullet> takeBullet()
    { QSharedPointer<Bullet> ret; _bullet.swap(ret); return ret; }
    virtual void catchBullet();
    virtual void clockTick();



signals:
    void newTankAvailable();
    void tankDestroyed();
    void tankMoved();
    void moved();
    void fired();

public slots:

protected:
    QSharedPointer<Tank> _tank;
    QSharedPointer<Bullet> _bullet;
};

} // namespace Tanks

#endif // TANKS_ABSTRACTPLAYER_H

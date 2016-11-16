#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QStandardPaths>

#include "qmlbridge.h"
#include "game.h"
#include "board.h"
#include "abstractmaploader.h"
#include "tank.h"
#include "flag.h"


namespace Tanks {

static int minBlockSize = 8; // 4px. minimal breakable part or minimal move

QMLBridge::QMLBridge(QObject *parent) : QObject(parent),
  _tmpDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation)+"/tanksXXXXXX"),
  _qmlId(0)
{
    _tmpDir.setAutoRemove(true);
    QDir d(_tmpDir.path());
    _lowerFilename = d.filePath("lower.png");
    _bushFilename = d.filePath("bush.png");
    qDebug() << "Lower map filename: " << _lowerFilename;

    _game = new Game(this);
    connect(_game, &Game::mapLoaded, this, &QMLBridge::mapLoaded);
    connect(_game, &Game::newTank, this, &QMLBridge::newTankAvailable);
    connect(_game, &Game::newBullet, this, &QMLBridge::newBulletAvailable);
    connect(_game, &Game::tankMoved, this, &QMLBridge::moveTank);
    connect(_game, &Game::tankDestroyed, this, &QMLBridge::destroyTank);
    connect(_game, &Game::blockRemoved, this, &QMLBridge::removeBlock);
    connect(_game, &Game::bulletMoved, this, &QMLBridge::moveBullet);
    connect(_game, &Game::flagLost, this, &QMLBridge::flagChanged);
    //connect(_game, &Game::playerRestarted, this, &QMLBridge::playerRestarted)

    connect(this, SIGNAL(qmlTankAction(int,int)), SLOT(humanTankAction(int,int)));
    connect(this, SIGNAL(qmlTankActionStop(int,int)), SLOT(humanTankActionStop(int,int)));
    _game->start();
}

QString QMLBridge::lowerFilename() const
{
    return _lowerFilename;
}

QString QMLBridge::bushFilename() const
{
    return _bushFilename;
}

QSize QMLBridge::boardImageSize() const
{
    return _game->board()->size() * minBlockSize;
}

QRect QMLBridge::flagGeometry() const
{
    QRect g = _game->flag()->geometry();
    return QRect(g.topLeft() * minBlockSize, g.size() * minBlockSize);
}

QString QMLBridge::flagFile() const
{
    return _game->flag()->isBroken()? "img/flag_broken" : "img/flag";
}

void QMLBridge::mapLoaded()
{
    qDebug() << "Map loaded!";

    //_activeBlocks.clear();

    static const char *textures[LastMapObjectType] = {
        0,
        ":/img/concrete",
        ":/img/brick",
        ":/img/bush",
        ":/img/ice",
        ":/img/water",
    };
    static QVector<QImage> probes; // keeps full scaled textures.
    // into this size we should draw our texture to fully fit.
    // In short, Baord spliited blocks from MapLoader into smaller pieces and
    // now we need to bring back original size to render textures properly.
    QSize scaledTextureSize(minBlockSize * _game->board()->blockDivider(),
                            minBlockSize * _game->board()->blockDivider());
    if (probes.isEmpty()) {
        probes.resize(LastMapObjectType);
        for (int i = 0; i < LastMapObjectType; i++) {
            probes[i] = QImage(scaledTextureSize, QImage::Format_ARGB32);
            QPainter p(&probes[i]);
            if (textures[i]) {
                p.fillRect(probes[i].rect(), QBrush(QImage(textures[i]).scaled(scaledTextureSize)));
            } else {
                probes[i].fill(0);
            }
        }
    }

    QImage lowerLayer(_game->board()->size() * minBlockSize, QImage::Format_ARGB32);
    lowerLayer.fill(0);
    QPainter lowerPainter(&lowerLayer);

    QImage bushLayer(_game->board()->size() * minBlockSize, QImage::Format_ARGB32);
    bushLayer.fill(0);
    QPainter bushPainter(&bushLayer);

    Board::Iterator it = _game->board()->iterate();
    while (it.isValid()) {
        QPainter *painter = *it == Bush? &bushPainter : &lowerPainter;
        QImage &probe = probes[*it];
        QPoint pos = it.pos() * minBlockSize;
        int probeX = pos.x() % (_game->board()->blockDivider() * minBlockSize);
        int probeY = pos.y() % (_game->board()->blockDivider() * minBlockSize);
        painter->drawImage(pos, probe,
                           QRect(probeX, probeY, minBlockSize, minBlockSize));
        ++it;
    }

    // that's the most easy way. QQuickImageProvider is just a holy crap (I'm sorry)
    // and QSG* is probably not expected by interviewers and it's anyway even
    // more complicated even if idealogically more correct.
    lowerLayer.save(_lowerFilename);
    bushLayer.save(_bushFilename);

    emit mapRendered();
}

void QMLBridge::newTankAvailable(QObject *obj)
{
    Tank *tank = qobject_cast<Tank*>(obj);

    QString id = QString("tank") + QString::number(_qmlId++);
    //_activeBlocks.insert(id, tank.dynamicCast<Block>());
    tank->setProperty("qmlid", id);
    emit newTank(tank2variant(tank));
}

void QMLBridge::newBulletAvailable(QObject *obj)
{
    Bullet *bullet = qobject_cast<Bullet*>(obj);

    QString id = QString("bullet") + QString::number(_qmlId++);
    //_activeBlocks.insert(id, tank.dynamicCast<Block>());
    bullet->setProperty("qmlid", id);

    QVariantMap vb;
    vb["id"] = id;
    //vb["affinity"] = bullet->affinity();
    vb["direction"] = (int)bullet->direction();
    QRect geom = bullet->geometry();
    vb["geometry"] = QRect(geom.topLeft() * minBlockSize,
                              geom.size() * minBlockSize);

    connect(bullet, &Bullet::detonated, this, &QMLBridge::detonateBullet);
    emit newBullet(vb);
}

void QMLBridge::moveTank(QObject *obj)
{
    auto tank = qobject_cast<Tank*>(obj);
    emit tankUpdated(tank2variant(tank));
}

void QMLBridge::destroyTank(QObject *obj)
{
    emit tankDestroyed(obj->property("qmlid").toString());
}

void QMLBridge::moveBullet(QObject *obj)
{
    auto bullet = qobject_cast<Bullet*>(obj);
    //qDebug() << "New position: " << bullet->geometry().topLeft() * minBlockSize;
    emit bulletMoved(bullet->property("qmlid").toString(),
                    bullet->geometry().topLeft() * minBlockSize);
}

void QMLBridge::detonateBullet()
{
    auto bullet = qobject_cast<Bullet*>(sender());
    emit bulletDetonated(bullet->property("qmlid").toString(), (int)bullet->explosionType());
}

QVariant QMLBridge::tank2variant(Tank *tank)
{
    QVariantMap vtank;
    vtank["id"] = tank->property("qmlid").toString();
    vtank["affinity"] = (int)tank->affinity();
    vtank["variant"] = tank->variant();
    vtank["direction"] = (int)tank->direction();
    QRect tankGeom = tank->geometry();
    vtank["geometry"] = QRect(tankGeom.topLeft() * minBlockSize,
                              tankGeom.size() * minBlockSize);
    return vtank;
}

void QMLBridge::restart()
{
    _game->start();
}

void QMLBridge::removeBlock(const QRect &r)
{
    emit blockRemoved(QRect(r.topLeft()*minBlockSize, r.size()*minBlockSize));
}

void QMLBridge::humanTankAction(int player, int key)
{
    //qDebug() << "Catched start!";
    if (key < 4) {
        _game->playerMoveRequested(player, key);
    } else {
        _game->playerFireRequested(player);
    }
}

void QMLBridge::humanTankActionStop(int player, int key)
{
    //qDebug() << "Catched stop!";
    if (key < 4) {
        _game->playerStopMoveRequested(player, key);
    } else {
        _game->playerStopFireRequested(player);
    }
}

} //namespace Tanks

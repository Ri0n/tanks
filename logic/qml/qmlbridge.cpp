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

#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QStandardPaths>

#include "abstractmaploader.h"
#include "board.h"
#include "flag.h"
#include "game.h"
#include "qmlbridge.h"
#include "qmlmapimageprovider.h"
#include "tank.h"

namespace Tanks {

static int minBlockSize = 8; // 4px. minimal breakable part or minimal move

QMLBridge::QMLBridge(QObject *parent) : QObject(parent), _qmlId(0)
{
    QMLMapImageProvider::registerBridge(this);

    _game = new Game(this);
    connect(_game, &Game::mapLoaded, this, &QMLBridge::mapLoaded);
    connect(_game, &Game::newTank, this, &QMLBridge::newTankAvailable);

    connect(_game, &Game::blockRemoved, this, &QMLBridge::removeBlock);
    connect(_game, &Game::flagLost, this, &QMLBridge::flagChanged);
    connect(_game, &Game::statsChanged, this, &QMLBridge::statsChanged);
    // connect(_game, &Game::playerRestarted, this, &QMLBridge::playerRestarted)

    connect(this, SIGNAL(qmlTankAction(int, int)), SLOT(humanTankAction(int, int)));
    connect(this, SIGNAL(qmlTankActionStop(int, int)), SLOT(humanTankActionStop(int, int)));
    _game->start();
}

QImage QMLBridge::lowerMapImage() const { return _lowerMapImage; }

QImage QMLBridge::bushImage() const { return _bushImage; }

QSize QMLBridge::boardImageSize() const { return _game->board()->size() * minBlockSize; }

QRect QMLBridge::flagGeometry() const
{
    QRect g = _game->flag()->geometry();
    return QRect(g.topLeft() * minBlockSize, g.size() * minBlockSize);
}

QString QMLBridge::flagFile() const { return _game->flag()->isBroken() ? "img/flag_broken" : "img/flag"; }

QString QMLBridge::lifesStat() const
{
    QString lifes = QString("<b>AI: %1</b><br>").arg(_game->aiLifes());
    for (int i = 0; i < _game->playersCount(); i++) {
        lifes += QString("<b>P%1: %2</b><br>").arg(QString::number(i + 1), QString::number(_game->playerLifes(i)));
    }
    return lifes;
}

void QMLBridge::setBridgeId(const QString &id) { _bridgeId = id; }

void QMLBridge::mapLoaded()
{
    qDebug() << "Map loaded!";

    //_activeBlocks.clear();

    static const char *textures[LastMapObjectType] = {
        0, ":/img/concrete", ":/img/brick", ":/img/bush", ":/img/ice", ":/img/water",
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

    _lowerMapImage = QImage(_game->board()->size() * minBlockSize, QImage::Format_ARGB32);
    _lowerMapImage.fill(0);
    QPainter lowerPainter(&_lowerMapImage);

    _bushImage = QImage(_game->board()->size() * minBlockSize, QImage::Format_ARGB32);
    _bushImage.fill(0);
    QPainter bushPainter(&_bushImage);

    Board::Iterator it = _game->board()->iterate();
    while (it.isValid()) {
        QPainter *painter = *it == Bush ? &bushPainter : &lowerPainter;
        QImage   &probe   = probes[*it];
        QPoint    pos     = it.pos() * minBlockSize;
        int       probeX  = pos.x() % (_game->board()->blockDivider() * minBlockSize);
        int       probeY  = pos.y() % (_game->board()->blockDivider() * minBlockSize);
        painter->drawImage(pos, probe, QRect(probeX, probeY, minBlockSize, minBlockSize));
        ++it;
    }

    // that's the most easy way. QQuickImageProvider is just a holy crap (I'm sorry)
    // and QSG* is probably not expected by interviewers and it's anyway even
    // more complicated even if idealogically more correct.
    // lowerLayer.save(_lowerFilename);
    // bushLayer.save(_bushFilename);

    emit mapRendered();
}

void QMLBridge::newTankAvailable(QObject *obj)
{
    Tank *tank = qobject_cast<Tank *>(obj);

    connect(tank, &Tank::fired, this, &QMLBridge::newBulletAvailable);
    connect(tank, &Tank::moved, this, &QMLBridge::moveTank);
    connect(tank, &Tank::tankDestroyed, this, &QMLBridge::destroyTank);

    QString id = QString("tank") + QString::number(_qmlId++);
    //_activeBlocks.insert(id, tank.dynamicCast<Block>());
    tank->setProperty("qmlid", id);
    emit newTank(tank2variant(tank));
}

void QMLBridge::newBulletAvailable()
{
    Bullet *bullet = qobject_cast<Tank *>(sender())->bullet().data();

    connect(bullet, &DynamicBlock::moved, this, &QMLBridge::moveBullet);

    QString id = QString("bullet") + QString::number(_qmlId++);
    //_activeBlocks.insert(id, tank.dynamicCast<Block>());
    bullet->setProperty("qmlid", id);

    QVariantMap vb;
    vb["id"] = id;
    // vb["affinity"] = bullet->affinity();
    vb["direction"] = (int)bullet->direction();
    QRect geom      = bullet->geometry();
    vb["geometry"]  = QRect(geom.topLeft() * minBlockSize, geom.size() * minBlockSize);

    connect(bullet, &Bullet::detonated, this, &QMLBridge::detonateBullet);
    emit newBullet(vb);
}

void QMLBridge::moveTank()
{
    auto tank = qobject_cast<Tank *>(sender());
    emit tankUpdated(tank2variant(tank));
}

void QMLBridge::destroyTank() { emit tankDestroyed(sender()->property("qmlid").toString()); }

void QMLBridge::moveBullet()
{
    auto bullet = qobject_cast<Bullet *>(sender());
    // qDebug() << "New position: " << bullet->geometry().topLeft() * minBlockSize;
    emit bulletMoved(bullet->property("qmlid").toString(), bullet->geometry().topLeft() * minBlockSize);
}

void QMLBridge::detonateBullet()
{
    auto bullet = qobject_cast<Bullet *>(sender());
    emit bulletDetonated(bullet->property("qmlid").toString(), (int)bullet->explosionType());
}

QVariant QMLBridge::tank2variant(Tank *tank)
{
    QVariantMap vtank;
    vtank["id"]        = tank->property("qmlid").toString();
    vtank["affinity"]  = (int)tank->affinity();
    vtank["variant"]   = tank->variant();
    vtank["direction"] = (int)tank->direction();
    QRect tankGeom     = tank->geometry();
    vtank["geometry"]  = QRect(tankGeom.topLeft() * minBlockSize, tankGeom.size() * minBlockSize);
    return vtank;
}

void QMLBridge::restart(int playersCount) { _game->start(playersCount); }

void QMLBridge::removeBlock(const QRect &r)
{
    emit blockRemoved(QRect(r.topLeft() * minBlockSize, r.size() * minBlockSize));
}

void QMLBridge::humanTankAction(int player, int key)
{
    // qDebug() << "Catched start!";
    if (key < 4) {
        _game->playerMoveRequested(player, key);
    } else {
        _game->playerFireRequested(player);
    }
}

void QMLBridge::humanTankActionStop(int player, int key)
{
    // qDebug() << "Catched stop!";
    if (key < 4) {
        _game->playerStopMoveRequested(player, key);
    } else {
        _game->playerStopFireRequested(player);
    }
}

} // namespace Tanks

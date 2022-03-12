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

#ifndef QMLBRIDGE_H
#define QMLBRIDGE_H

#include <QImage>
#include <QObject>
#include <QTemporaryDir>
#include <QVariant>

#include "block.h"

namespace Tanks {

class Game;
class Tank;

class QMLBridge : public QObject {
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

    QSize   boardImageSize() const;
    QRect   flagGeometry() const;
    QString flagFile() const;
    QString lifesStat() const;

    inline QString bridgeId() const { return _bridgeId; }
    void           setBridgeId(const QString &id);

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
    void restart(int playersCount);

private slots:
    void humanTankAction(int player, int key);
    void humanTankActionStop(int player, int key);

    void mapLoaded();

    void newTankAvailable(QObject *obj);
    void newBulletAvailable();
    void moveTank();
    void removeBlock(const QRect &r);
    void moveBullet();
    void destroyTank();
    void detonateBullet();

private:
    QString       _bridgeId;
    QTemporaryDir _tmpDir;
    Game         *_game;

    QImage _lowerMapImage;
    QImage _bushImage;

    int _qmlId;
    // QHash<QString, QWeakPointer<Block>> _activeBlocks;
};

} // namespace Tanks

#endif // QMLBRIDGE_H

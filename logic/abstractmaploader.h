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

#ifndef TANKS_ABSTRACTMAPLOADER_H
#define TANKS_ABSTRACTMAPLOADER_H

#include "basics.h"

#include <QRect>
#include <QtGlobal>

namespace Tanks {

class MapObject {
public:
    QRect         geometry;
    MapObjectType type;
};

class AbstractMapLoader {
public:
    AbstractMapLoader();
    virtual ~AbstractMapLoader();

    virtual bool          open()                         = 0;
    virtual QSize         dimensions() const             = 0;
    virtual bool          hasNext() const                = 0;
    virtual MapObject     next()                         = 0;
    virtual QList<quint8> enemyTanks() const             = 0;
    virtual QList<QPoint> enemyStartPositions() const    = 0;
    virtual QList<QPoint> friendlyStartPositions() const = 0;
    virtual QPoint        flagPosition() const           = 0;
};

} // namespace Tanks

#endif // TANKS_ABSTRACTMAPLOADER_H

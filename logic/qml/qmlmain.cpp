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

#include <QQmlApplicationEngine>
#include <QtQml>

#include "qmlbridge.h"
#include "qmlmain.h"
#include "qmlmapimageprovider.h"
#include "qmltankimageprovider.h"

namespace Tanks {

QMLMain::QMLMain()
{

    qmlRegisterType<Tanks::QMLBridge>("com.rsoft.tanks", 1, 0, "Tanks");
    _engine = new QQmlApplicationEngine();

    _engine->addImageProvider(QLatin1String("tankprovider"), new Tanks::QMLTankImageProvider());
    _engine->addImageProvider(QLatin1String("bulletprovider"), new Tanks::QMLBulletImageProvider());
    _engine->addImageProvider(QLatin1String("mapprovider"), new Tanks::QMLMapImageProvider());

    _engine->load(QUrl(QStringLiteral("qrc:/main.qml")));
}

QMLMain::~QMLMain() { delete _engine; }

} // namespace Tanks

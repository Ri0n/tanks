#include <QQmlApplicationEngine>
#include <QtQml>

#include "qmlmain.h"
#include "qmlbridge.h"
#include "qmltankimageprovider.h"
#include "qmlmapimageprovider.h"

namespace Tanks {

QMLMain::QMLMain()
{

    qmlRegisterType<Tanks::QMLBridge>("com.rsoft.tanks", 1, 0, "Tanks");
    _engine = new QQmlApplicationEngine();

    _engine->addImageProvider(QLatin1String("tankprovider"),
                            new Tanks::QMLTankImageProvider());
    _engine->addImageProvider(QLatin1String("bulletprovider"),
                            new Tanks::QMLBulletImageProvider());
    _engine->addImageProvider(QLatin1String("mapprovider"),
                            new Tanks::QMLMapImageProvider());

    _engine->load(QUrl(QStringLiteral("qrc:/main.qml")));
}

QMLMain::~QMLMain()
{
    delete _engine;
}

} // namespace Tanks

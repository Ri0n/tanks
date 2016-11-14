#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "qmlbridge.h"
#include "qmltankimageprovider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Tanks::QMLBridge>("com.rsoft.tanks", 1, 0, "Tanks");

    QQmlApplicationEngine engine;

    engine.addImageProvider(QLatin1String("tankprovider"),
                            new Tanks::QMLTankImageProvider());
    engine.addImageProvider(QLatin1String("bulletprovider"),
                            new Tanks::QMLBulletImageProvider());

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

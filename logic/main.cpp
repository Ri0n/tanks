#include <QGuiApplication>

#include "qmlmain.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Tanks::QMLMain q;

    return app.exec();
}

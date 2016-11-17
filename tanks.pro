TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += logic/main.cpp \
    logic/board.cpp \
    logic/block.cpp \
    logic/dynamicblock.cpp \
    logic/staticblock.cpp \
    logic/bonus.cpp \
    logic/abstractmaploader.cpp \
    logic/randommaploader.cpp \
    logic/qml/qmlbridge.cpp \
    logic/game.cpp \
    logic/qml/qmltankimageprovider.cpp \
    logic/tank.cpp \
    logic/bullet.cpp \
    logic/abstractplayer.cpp \
    logic/humanplayer.cpp \
    logic/aiplayer.cpp \
    logic/ai.cpp \
    logic/flag.cpp \
    logic/qml/qmlmapimageprovider.cpp \
    logic/qml/qmlmain.cpp

RESOURCES += render/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    logic/board.h \
    logic/block.h \
    logic/dynamicblock.h \
    logic/staticblock.h \
    logic/bonus.h \
    logic/abstractmaploader.h \
    logic/randommaploader.h \
    logic/qml/qmlbridge.h \
    logic/game.h \
    logic/qml/qmltankimageprovider.h \
    logic/tank.h \
    logic/bullet.h \
    logic/abstractplayer.h \
    logic/humanplayer.h \
    logic/aiplayer.h \
    logic/ai.h \
    logic/basics.h \
    logic/flag.h \
    logic/qml/qmlmapimageprovider.h \
    logic/qml/qmlmain.h

INCLUDEPATH += $$PWD/logic $$PWD/logic/qml

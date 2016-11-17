#ifndef TANKS_QMLMAIN_H
#define TANKS_QMLMAIN_H

class QQmlApplicationEngine;

namespace Tanks {

class QMLMain
{
public:
    QMLMain();
    ~QMLMain();

private:
    QQmlApplicationEngine *_engine;
};

} // namespace Tanks

#endif // TANKS_QMLMAIN_H

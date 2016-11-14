#ifndef TANKS_AI_H
#define TANKS_AI_H

#include <QObject>

namespace Tanks {

class Game;

class AI : public QObject
{
    Q_OBJECT
public:
    explicit AI(Game *game = 0);
    void resetTanks();
    inline int lifesCount() const { return _tanks.count(); }
    inline quint8 takeTank() { return _tanks.takeFirst(); }
    void start();

    void clockTick();

signals:

public slots:

private:
    Game *_game;
    QList<quint8> _tanks;
};

} // namespace Tanks

#endif // TANKS_AI_H

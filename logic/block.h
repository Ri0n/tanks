#ifndef TANKS_BLOCK_H
#define TANKS_BLOCK_H

#include <QSharedPointer>
#include <QRect>
#include <QObject>

namespace Tanks {

class Block : public QObject
{
    Q_OBJECT
public:
    Block();

    inline void setInitialPosition(const QPoint &p) { _geometry.moveTopLeft(p); }

    inline bool hasClash(const Block &b) const
    { return _geometry.intersects(b._geometry); }

    inline const QRect &geometry() const { return _geometry; }
protected:
    QRect _geometry;
};

} // namespace Tanks

#endif // TANKS_BLOCK_H

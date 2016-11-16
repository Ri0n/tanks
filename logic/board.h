#ifndef TANKS_BOARD_H
#define TANKS_BOARD_H

#include <QObject>
#include <QSize>
#include <QVector>
#include <QLinkedList>

#include "abstractmaploader.h"
#include "dynamicblock.h"

namespace Tanks {

class AbstractMapLoader;

class Board : public QObject
{
    Q_OBJECT
public:
    typedef quint8 MapItem;

    enum BlockProp {
        TankObstackle = 1,
        BulletObstackle = 2,
        Sturdy = 4,
        Breakable = 8,
        BadManoeuvre = 0x10,
        Collectable = 0x20,
        Dangerous = 0x40, // Any. clash of two dangerous object destroys both
        Explosive = 0x80, // bullet. together with Dangerous gives armor-piercing
    };
    Q_DECLARE_FLAGS(BlockProps, BlockProp)

    /**
     * @brief The Iterator class
     * Iterates over all blocks line by line
     */
    class Iterator
    {
        const QVector<MapItem> _map; // copy-on-write and we are protected from board deletion;
        QPoint _pos;
        int _width;
        int _index;
    public:
        Iterator(const Board &board) :
            _map(board._map),
            _width(board._size.width()),
            _index(0)
        {}

        inline bool isValid() const { return _index < _map.size(); }
        inline Iterator &operator++()
        {
            _index++;
            if (++_pos.rx() >= _width) {
                _pos.setX(0);
                _pos.ry()++;
            }
            return *this;
        }
        inline MapItem operator*() const { return isValid()? _map.value(_index) : 0; }
        inline const QPoint pos() const { return _pos; }
    };
    friend class Iterator;
    inline Iterator iterate() const { return Iterator(*this); }

    explicit Board(QObject *parent = 0);
    bool loadMap(AbstractMapLoader *loader);

    inline int posToMapIndex(const QPoint &pos) const
    { return pos.y() * _size.width() + pos.x(); }

    inline MapObjectType blockType(const QPoint &pos) const
    { return (MapObjectType)_map.value(posToMapIndex(pos)); }

    inline BlockProps blockProperties(const QPoint &pos) const
    { return blockTypeProperties(blockType(pos)); }

    BlockProps blockTypeProperties(MapObjectType type) const;
    BlockProps rectProps(const QRect &rect);

    void renderBlock(MapObjectType type, const QRect &area);

    inline const QSize &size() const { return _size; }
    int blockDivider() const;

    //bool addDynBlock(QSharedPointer<DynamicBlock> dblock);
    void clockTick();

    inline const QList<quint8> &initialEnemyTanks() const
    { return _initialEnemyTanks; }

    inline const QList<QPoint> &enemyStartPositions() const
    { return _enemyStartPositions; }

    inline const QList<QPoint> &friendlyStartPositions() const
    { return _friendlyStartPositions; }

    inline const QPoint &flagPosition() const { return _flagPosition; }

    void renderFlagFrame(MapObjectType type);
signals:

public slots:
private:
    QSize _size;
    QVector<MapItem> _map;
    //QLinkedList<QSharedPointer<DynamicBlock>> _dynBlocks;
    QList<quint8> _initialEnemyTanks;
    QList<QPoint> _enemyStartPositions;
    QList<QPoint> _friendlyStartPositions;
    QPoint _flagPosition;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Board::BlockProps)

} // namespace Tanks

#endif // TANKS_BOARD_H

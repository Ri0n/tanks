#ifndef BASICS_H
#define BASICS_H

namespace Tanks {

enum MapObjectType
{
    Nothing,
    Concrete,
    Brick,
    Bush,
    Ice,
    Water,
    LastMapObjectType
};

enum Direction {
    North,
    South,
    West,
    East
};

enum Affinity {
    Friendly,
    Alien
};

} // namespace Tanks

#endif // BASICS_H

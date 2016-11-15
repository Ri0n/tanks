#ifndef TANKS_FLAG_H
#define TANKS_FLAG_H

#include "staticblock.h"

namespace Tanks {

class Flag : public StaticBlock
{
    Q_OBJECT
public:
    Flag();
    void restore();
    void burn();
    inline bool isBroken() const { return _broken; }

signals:
    void changed();

private:
    bool _broken;
};

} // namespace Tanks

#endif // TANKS_FLAG_H

#ifndef TANKS_STATICBLOCK_H
#define TANKS_STATICBLOCK_H

#include "block.h"

namespace Tanks {

class StaticBlock : public Block
{
    Q_OBJECT
public:    
    typedef QSharedPointer<StaticBlock> Ptr;

    StaticBlock();
};

} // namespace Tanks

#endif // TANKS_STATICBLOCK_H

#include "flag.h"

namespace Tanks {

Flag::Flag() :
    _broken(false)
{
    _geometry.setSize(QSize(4,4));
}

void Flag::restore()
{
    _broken = false;
    emit changed();
}

void Flag::burn()
{
    _broken = true;
    emit changed();
}

} // namespace Tanks

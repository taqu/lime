/**
@file Pack.cpp
@author t-sakai
@date 2012/03/20 create

*/
#include "Pack.h"

namespace pack
{
    bool isLittleEndian()
    {
        s32 x = 1;
        return (*(s8*)&x)? true : false;
    }
}

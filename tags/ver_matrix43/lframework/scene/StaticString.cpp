/**
@file StaticString.cpp
@author t-sakai
@date 2010/05/02 create
*/
#include "StaticString.h"
namespace lgraphics
{
namespace format
{
#define LGRAPHICS_STATIC_STRING_IMPL(name, value)\
    const char name[] = value;\
    const int name##Length = sizeof(name);

    LGRAPHICS_STATIC_STRING_IMPL(ModelFileExt, "lo")

#undef LGRAPHICS_STATIC_STRING_IMPL
}
}

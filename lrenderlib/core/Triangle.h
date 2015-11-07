#ifndef INC_LRENDER_TRIANGLE_H__
#define INC_LRENDER_TRIANGLE_H__
/**
@file Triangle.h
@author t-sakai
@date 2015/09/11 create
*/
#include "../lrender.h"
#include "Buffer.h"

namespace lrender
{
    struct Triangle
    {
        s32 indices_[3];
    };

    typedef Buffer<Triangle> BufferTriangle;
}
#endif //INC_LRENDER_TRIANGLE_H__

#ifndef INC_LRENDER_BUFFER_H__
#define INC_LRENDER_BUFFER_H__
/**
@file Buffer.h
@author t-sakai
@date 2015/09/11 create
*/
#include "../lrender.h"
#include <lcore/Buffer.h>

namespace lrender
{
    template<class T>
    bool create(lcore::Buffer<T>& buffer, s32 num)
    {
        lcore::Buffer<T> tmp(num);
        buffer.swap(tmp);
        return true;
    }

    typedef lcore::Buffer<lmath::Vector2> BufferVector2;
    typedef lcore::Buffer<lmath::Vector3> BufferVector3;
    typedef lcore::Buffer<lmath::Vector4> BufferVector4;
}
#endif //INC_LRENDER_BUFFER_H__

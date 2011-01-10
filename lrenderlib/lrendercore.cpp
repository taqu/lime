/**
@file lrendercore.cpp
@author t-sakai
@date 2009/05/11
*/
#include <limits>
#include <lgraphics/lgraphicsEnum.h>
#include "lrendercore.h"

namespace lrender
{
    const f32 RAY_EPSILON = 1.0e-5f;

    const f32 FLOAT_INITINITY = std::numeric_limits<float>::infinity();

    s32 toGraphicsFormat(BufferFormat format)
    {
        switch(format)
        {
        case Buffer_D32:
            return lgraphics::Buffer_D32;

        case Buffer_A32B32G32R32F:
            return lgraphics::Buffer_A32B32G32R32F;

        case Buffer_R8G8B8:
            return lgraphics::Buffer_R8G8B8;

        case Buffer_A8R8G8B8:
            return lgraphics::Buffer_A8R8G8B8;

        default:
            LASSERT(false && "unknown buffer format");
        };
        return -1;
    }
}

/**
@file VertexBufferRef.cpp
@author t-sakai
@date 2012/07/15 create
*/
#include "VertexBufferRef.h"

#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgraphics
{
    //------------------------------------------------------------
    //---
    //--- VertexBufferRef
    //---
    //------------------------------------------------------------
    void VertexBufferRef::attach(u32 start, u32 stride, u32 offset)
    {
        lgraphics::Graphics::getDevice().setVertexBuffers(
            start,
            1,
            &buffer_,
            &stride,
            &offset);
    }
}

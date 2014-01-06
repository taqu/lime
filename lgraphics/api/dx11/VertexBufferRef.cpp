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
    void VertexBufferRef::attach(u32 startSlot, u32 stride, u32 offsetInBytes)
    {
        lgraphics::Graphics::getDevice().setVertexBuffers(
            startSlot,
            1,
            &buffer_,
            &stride,
            &offsetInBytes);
    }

    void VertexBufferRef::attachStreamOut(u32 offsetInBytes)
    {
        lgraphics::Graphics::getDevice().setStreamOutTargets(
            1,
            &buffer_,
            &offsetInBytes);
    }
}

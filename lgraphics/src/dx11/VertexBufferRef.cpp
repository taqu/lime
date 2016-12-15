/**
@file VertexBufferRef.cpp
@author t-sakai
@date 2012/07/15 create
*/
#include "VertexBufferRef.h"

#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgfx
{
    //------------------------------------------------------------
    //---
    //--- VertexBufferRef
    //---
    //------------------------------------------------------------
    void VertexBufferRef::attach(ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes)
    {
        context.setVertexBuffers(
            startSlot,
            1,
            &buffer_,
            &stride,
            &offsetInBytes);
    }

    void VertexBufferRef::attachStreamOut(ContextRef& context, u32 offsetInBytes)
    {
        context.setStreamOutTargets(
            1,
            &buffer_,
            &offsetInBytes);
    }
}

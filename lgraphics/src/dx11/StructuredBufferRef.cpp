/**
@file StructuredBufferRef.cpp
@author t-sakai
@date 2015/12/22 create
*/
#include "StructuredBufferRef.h"
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgfx
{
    //------------------------------------------------------------
    //---
    //--- StructuredBufferRef
    //---
    //------------------------------------------------------------
    void StructuredBufferRef::attach(ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes)
    {
        context.setVertexBuffers(
            startSlot,
            1,
            &buffer_,
            &stride,
            &offsetInBytes);
    }

    void StructuredBufferRef::attachStreamOut(ContextRef& context, u32 offsetInBytes)
    {
        context.setStreamOutTargets(
            1,
            &buffer_,
            &offsetInBytes);
    }
}

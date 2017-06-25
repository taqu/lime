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

    StructuredBufferRef& StructuredBufferRef::operator=(const StructuredBufferRef& rhs)
    {
        StructuredBufferRef(rhs).swap(*this);
        return *this;
    }

    StructuredBufferRef& StructuredBufferRef::operator=(StructuredBufferRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            buffer_ = rhs.buffer_;
            rhs.buffer_ = NULL;
        }
        return *this;
    }

    void StructuredBufferRef::swap(StructuredBufferRef& rhs)
    {
        BufferRefBase::swap(rhs);
    }
}

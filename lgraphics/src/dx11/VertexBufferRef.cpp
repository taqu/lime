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

    VertexBufferRef& VertexBufferRef::operator=(const VertexBufferRef& rhs)
    {
        VertexBufferRef(rhs).swap(*this);
        return *this;
    }

    VertexBufferRef& VertexBufferRef::operator=(VertexBufferRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            buffer_ = rhs.buffer_;
            rhs.buffer_ = NULL;
        }
        return *this;
    }

    void VertexBufferRef::swap(VertexBufferRef& rhs)
    {
        BufferRefBase::swap(rhs);
    }
}

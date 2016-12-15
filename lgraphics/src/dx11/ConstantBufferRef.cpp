/**
@file ConstantBufferRef.cpp
@author t-sakai
@date 2012/07/22 create
*/
#include "ConstantBufferRef.h"
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgfx
{
    void ConstantBufferRef::attachVS(ContextRef& context, u32 start)
    {
        context.setVSConstantBuffers(start, 1, &buffer_);
    }

    void ConstantBufferRef::attachGS(ContextRef& context, u32 start)
    {
        context.setGSConstantBuffers(start, 1, &buffer_);
    }

    void ConstantBufferRef::attachPS(ContextRef& context, u32 start)
    {
        context.setPSConstantBuffers(start, 1, &buffer_);
    }

    void ConstantBufferRef::attachCS(ContextRef& context, u32 start)
    {
        context.setCSConstantBuffers(start, 1, &buffer_);
    }

    void ConstantBufferRef::update(
        ContextRef& context,
        u32 dstSubresource,
        const Box* dstBox,
        const void* srcData,
        u32 srcRowPitch,
        u32 srcDepthPitch)
    {
        context.updateSubresource(
            buffer_,
            dstSubresource,
            dstBox,
            srcData,
            srcRowPitch,
            srcDepthPitch);
    }
}

/**
@file ConstantBufferRef.cpp
@author t-sakai
@date 2012/07/22 create
*/
#include "ConstantBufferRef.h"
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgraphics
{
    void ConstantBufferRef::setVS(u32 start)
    {
        Graphics::getDevice().setVSConstantBuffers(start, 1, &buffer_);
    }

    void ConstantBufferRef::setGS(u32 start)
    {
        Graphics::getDevice().setGSConstantBuffers(start, 1, &buffer_);
    }

    void ConstantBufferRef::setPS(u32 start)
    {
        Graphics::getDevice().setPSConstantBuffers(start, 1, &buffer_);
    }

    void ConstantBufferRef::setCS(u32 start)
    {
        Graphics::getDevice().setCSConstantBuffers(start, 1, &buffer_);
    }

    void ConstantBufferRef::update(
        u32 dstSubresource,
        const Box* dstBox,
        const void* srcData,
        u32 srcRowPitch,
        u32 srcDepthPitch)
    {
        Graphics::getDevice().updateSubresource(
            buffer_,
            dstSubresource,
            dstBox,
            srcData,
            srcRowPitch,
            srcDepthPitch);
    }
}

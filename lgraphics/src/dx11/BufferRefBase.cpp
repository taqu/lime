/**
@file BufferRefBase.cpp
@author t-sakai
@date 2012/07/15 create
*/
#include "BufferRefBase.h"
#include "../../Graphics.h"

namespace lgfx
{
    //------------------------------------------------------------
    //---
    //--- BufferRefBase
    //---
    //------------------------------------------------------------
    BufferRefBase::BufferRefBase(const BufferRefBase& rhs)
        :buffer_(rhs.buffer_)
    {
        if(buffer_ != NULL){
            buffer_->AddRef();
        }
    }

    void BufferRefBase::destroy()
    {
        LDXSAFE_RELEASE(buffer_);
    }

    bool BufferRefBase::map(ContextRef& context, u32 subresource, MapType type, MappedSubresource& mapped)
    {
        return context.map(buffer_, subresource, type, mapped);
    }

    void BufferRefBase::unmap(ContextRef& context, u32 subresource)
    {
        context.unmap(buffer_, subresource);
    }

    void BufferRefBase::updateSubresource(ContextRef& context, u32 index, const Box* box, const void* data, u32 rowPitch, u32 depthPitch)
    {
        context.updateSubresource(buffer_, index, box, data, rowPitch, depthPitch);
    }

    void BufferRefBase::copy(ContextRef& context, BufferRefBase& src)
    {
        context.copyResource(buffer_, src.buffer_);
    }

    //------------------------------------------------------------
    //---
    //--- BufferBase
    //---
    //------------------------------------------------------------
    ID3D11Buffer* BufferBase::create(
        u32 size,
        Usage usage,
        u32 bindFlags,
        CPUAccessFlag accessFlags,
        ResourceMisc miscFlags,
        u32 structureByteStride,
        const void* data,
        u32 pitch,
        u32 slicePitch)
    {
        D3D11_BUFFER_DESC desc;
        desc.ByteWidth = size;
        desc.Usage = static_cast<D3D11_USAGE>(usage);
        desc.BindFlags = bindFlags;
        desc.CPUAccessFlags = accessFlags;
        desc.MiscFlags = miscFlags;
        desc.StructureByteStride = structureByteStride;

        D3D11_SUBRESOURCE_DATA res;

        D3D11_SUBRESOURCE_DATA* pRes;
        if(NULL != data){
            res.pSysMem = data;
            res.SysMemPitch = pitch;
            res.SysMemSlicePitch = slicePitch;
            pRes = &res;
        }else{
            pRes = NULL;
        }

        ID3D11Buffer* buffer = NULL;
        ID3D11Device* device = getDevice().getD3DDevice();
        device->CreateBuffer(
            &desc,
            pRes,
            &buffer);
        return buffer;
    }
}

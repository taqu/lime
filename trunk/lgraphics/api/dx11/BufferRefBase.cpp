/**
@file BufferRefBase.cpp
@author t-sakai
@date 2012/07/15 create
*/
#include "BufferRefBase.h"
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgraphics
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
        SAFE_RELEASE(buffer_);
    }

    bool BufferRefBase::map(u32 subresource, MapType type, MappedSubresource& mapped)
    {
        HRESULT hr = lgraphics::Graphics::getDevice().getContext()->Map(
            buffer_,
            subresource,
            (D3D11_MAP)type,
            0,
            reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mapped));
        return SUCCEEDED(hr);
    }

    void BufferRefBase::unmap(u32 subresource)
    {
        lgraphics::Graphics::getDevice().getContext()->Unmap(buffer_, subresource);
    }

    void BufferRefBase::updateSubresource(u32 index, const Box* box, const void* data, u32 rowPitch, u32 depthPitch)
    {
        lgraphics::Graphics::getDevice().updateSubresource(buffer_, index, box, data, rowPitch, depthPitch);
    }

    //------------------------------------------------------------
    //---
    //--- BufferBase
    //---
    //------------------------------------------------------------
    ID3D11Buffer* BufferBase::create(
        u32 size,
        Usage usage,
        BindFlag bindFlags,
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
        ID3D11Device* device = Graphics::getDevice().getD3DDevice();
        device->CreateBuffer(
            &desc,
            pRes,
            &buffer);
        return buffer;
    }
}

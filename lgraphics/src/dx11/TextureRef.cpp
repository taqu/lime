/**
@file TextureRef.cpp
@author t-sakai
@date 2012/07/25 create
*/
#include "TextureRef.h"
#include "../../Graphics.h"

namespace lgfx
{
    ResourceRef& ResourceRef::operator=(const ResourceRef& rhs)
    {
        ResourceRef(rhs).swap(*this);
        return *this;
    }

    ResourceRef& ResourceRef::operator=(ResourceRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            resource_ = rhs.resource_;
            rhs.resource_ = NULL;
        }
        return *this;
    }

    ResourceRef& ResourceRef::operator=(Texture1DRef&& rhs)
    {
        if(resource_ != rhs.resource_){
            destroy();
            resource_ = rhs.resource_;
            rhs.resource_ = NULL;
        }
        return *this;
    }

    ResourceRef& ResourceRef::operator=(Texture2DRef&& rhs)
    {
        if(resource_ != rhs.resource_){
            destroy();
            resource_ = rhs.resource_;
            rhs.resource_ = NULL;
        }
        return *this;
    }

    ResourceRef& ResourceRef::operator=(Texture3DRef&& rhs)
    {
        if(resource_ != rhs.resource_){
            destroy();
            resource_ = rhs.resource_;
            rhs.resource_ = NULL;
        }
        return *this;
    }

    ResourceRef& ResourceRef::operator=(BufferRef&& rhs)
    {
        if(resource_ != rhs.resource_){
            destroy();
            resource_ = rhs.resource_;
            rhs.resource_ = NULL;
        }
        return *this;
    }

    Texture1DRef& Texture1DRef::operator=(const Texture1DRef& rhs)
    {
        Texture1DRef(rhs).swap(*this);
        return *this;
    }

    Texture1DRef& Texture1DRef::operator=(Texture1DRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            resource_ = rhs.resource_;
            rhs.resource_ = NULL;
        }
        return *this;
    }

    Texture2DRef& Texture2DRef::operator=(const Texture2DRef& rhs)
    {
        Texture2DRef(rhs).swap(*this);
        return *this;
    }

    Texture2DRef& Texture2DRef::operator=(Texture2DRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            resource_ = rhs.resource_;
            rhs.resource_ = NULL;
        }
        return *this;
    }

    Texture3DRef& Texture3DRef::operator=(const Texture3DRef& rhs)
    {
        Texture3DRef(rhs).swap(*this);
        return *this;
    }

    Texture3DRef& Texture3DRef::operator=(Texture3DRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            resource_ = rhs.resource_;
            rhs.resource_ = NULL;
        }
        return *this;
    }

    BufferRef& BufferRef::operator=(const BufferRef& rhs)
    {
        BufferRef(rhs).swap(*this);
        return *this;
    }

    BufferRef& BufferRef::operator=(BufferRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            resource_ = rhs.resource_;
            rhs.resource_ = NULL;
        }
        return *this;
    }

    //--------------------------------------------------------
    //---
    //--- Texture
    //---
    //--------------------------------------------------------
    Texture1DRef Texture::create1D(
        u32 width,
        u32 mipLevels,
        u32 arraySize,
        DataFormat format,
        Usage usage,
        u32 bind,
        CPUAccessFlag access,
        ResourceMisc misc,
        const SubResourceData* initData)
    {

        D3D11_TEXTURE1D_DESC desc;

        desc.Width = width;
        desc.MipLevels = mipLevels;
        desc.ArraySize = arraySize;
        desc.Format = static_cast<DXGI_FORMAT>(format);
        desc.Usage = static_cast<D3D11_USAGE>(usage);
        desc.BindFlags = bind;
        desc.CPUAccessFlags = access;
        desc.MiscFlags = misc;

        ID3D11Device* device = getDevice().getD3DDevice();

        ID3D11Texture1D* texture = NULL;
        HRESULT hr = device->CreateTexture1D(
            &desc,
            reinterpret_cast<const D3D11_SUBRESOURCE_DATA*>(initData),
            &texture);
        return SUCCEEDED(hr)? Texture1DRef(texture) : Texture1DRef();
    }

    Texture2DRef Texture::create2D(
            u32 width,
            u32 height,
            u32 mipLevels,
            u32 arraySize,
            DataFormat format,
            Usage usage,
            u32 bind,
            CPUAccessFlag access,
            ResourceMisc misc,
            const SubResourceData* initData)
    {
        return Texture::create2D(
            width,
            height,
            mipLevels,
            arraySize,
            format,
            1, //MSAAサンプル数
            0, //MSAA品質
            usage,
            bind,
            access,
            misc,
            initData);
    }

    Texture2DRef Texture::create2D(
        u32 width,
        u32 height,
        u32 mipLevels,
        u32 arraySize,
        DataFormat format,
        u32 samleCount,
        u32 sampleQuality,
        Usage usage,
        u32 bind,
        CPUAccessFlag access,
        ResourceMisc misc,
        const SubResourceData* initData)
    {
        D3D11_TEXTURE2D_DESC desc;

        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = mipLevels;
        desc.ArraySize = arraySize;
        desc.Format = static_cast<DXGI_FORMAT>(format);
        desc.SampleDesc.Count = samleCount;
        desc.SampleDesc.Quality = sampleQuality;
        desc.Usage = static_cast<D3D11_USAGE>(usage);
        desc.BindFlags = bind;
        desc.CPUAccessFlags = access;
        desc.MiscFlags = misc;

        ID3D11Device* device = getDevice().getD3DDevice();

        ID3D11Texture2D* texture = NULL;
        HRESULT hr = device->CreateTexture2D(
            &desc,
            reinterpret_cast<const D3D11_SUBRESOURCE_DATA*>(initData),
            &texture);

        return SUCCEEDED(hr)? Texture2DRef(texture) : Texture2DRef();
    }

    Texture3DRef Texture::create3D(
        u32 width,
        u32 height,
        u32 depth,
        u32 mipLevels,
        DataFormat format,
        Usage usage,
        u32 bind,
        CPUAccessFlag access,
        ResourceMisc misc,
        const SubResourceData* initData)
    {
        D3D11_TEXTURE3D_DESC desc;

        desc.Width = width;
        desc.Height = height;
        desc.Depth = depth;
        desc.MipLevels = mipLevels;
        desc.Format = static_cast<DXGI_FORMAT>(format);
        desc.Usage = static_cast<D3D11_USAGE>(usage);
        desc.BindFlags = bind;
        desc.CPUAccessFlags = access;
        desc.MiscFlags = misc;

        ID3D11Device* device = getDevice().getD3DDevice();

        ID3D11Texture3D* texture = NULL;
        HRESULT hr = device->CreateTexture3D(
            &desc,
            reinterpret_cast<const D3D11_SUBRESOURCE_DATA*>(initData),
            &texture);

        return SUCCEEDED(hr)? Texture3DRef(texture) : Texture3DRef();
    }


    BufferRef Texture::createBuffer(
        u32 size,
        Usage usage,
        u32 bind,
        CPUAccessFlag access,
        ResourceMisc misc,
        u32 structureByteStride,
        const SubResourceData* initData)
    {
        D3D11_BUFFER_DESC desc;

        desc.ByteWidth = size;
        desc.Usage = static_cast<D3D11_USAGE>(usage);
        desc.BindFlags = bind;
        desc.CPUAccessFlags = access;
        desc.MiscFlags = misc;
        desc.StructureByteStride = structureByteStride;

        ID3D11Device* device = getDevice().getD3DDevice();

        ID3D11Buffer* buffer = NULL;
        HRESULT hr = device->CreateBuffer(
            &desc,
            reinterpret_cast<const D3D11_SUBRESOURCE_DATA*>(initData),
            &buffer);

        return SUCCEEDED(hr)? BufferRef(buffer) : BufferRef();
    }


    void setRenderTargetsAndUAV(
        u32 numViews,
        RenderTargetViewRef* views,
        DepthStencilViewRef* depthStencilView,
        u32 UAVStart,
        u32 numUAVs,
        UnorderedAccessViewRef* uavs,
        const u32* UAVInitCounts)
    {
        ID3D11RenderTargetView* tmpTargets[LGFX_MAX_RENDER_TARGETS];
        ID3D11UnorderedAccessView* tmpUavs[LGFX_MAX_RENDER_TARGETS];

        for(u32 i=0; i<numViews; ++i){
            tmpTargets[i] = views[i].get();
        }

        for(u32 i=0; i<numUAVs; ++i){
            tmpUavs[i] = uavs[i].get();
        }
        getDevice().setRenderTargetsAndUAV(
            numViews,
            tmpTargets,
            (NULL == depthStencilView)? NULL : depthStencilView->get(),
            UAVStart,
            numUAVs,
            tmpUavs,
            UAVInitCounts);
    }

    void setCSUnorderedAccessViews(
        u32 UAVStart,
        u32 numUAVs,
        UnorderedAccessViewRef* uavs,
        const u32* UAVInitCounts)
    {
        ID3D11UnorderedAccessView* tmpUavs[LGFX_MAX_RENDER_TARGETS];

        for(u32 i=0; i<numUAVs; ++i){
            tmpUavs[i] = uavs[i].get();
        }
        getDevice().setCSUnorderedAccessViews(
            UAVStart,
            numUAVs,
            tmpUavs,
            UAVInitCounts);
    }
}

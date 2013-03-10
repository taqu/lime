/**
@file TextureRef.cpp
@author t-sakai
@date 2012/07/25 create
*/
#include "TextureRef.h"

#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgraphics
{
    //--------------------------------------------------------
    //---
    //--- RenderTargetViewRef
    //---
    //--------------------------------------------------------
    RenderTargetViewRef::RenderTargetViewRef(const RenderTargetViewRef& rhs)
        :view_(rhs.view_)
    {
        if(view_){
            view_->AddRef();
        }
    }

    void RenderTargetViewRef::destroy()
    {
        SAFE_RELEASE(view_);
    }

    //--------------------------------------------------------
    //---
    //--- DepthStencilViewRef
    //---
    //--------------------------------------------------------
    DepthStencilViewRef::DepthStencilViewRef(const DepthStencilViewRef& rhs)
        :view_(rhs.view_)
    {
        if(view_){
            view_->AddRef();
        }
    }

    void DepthStencilViewRef::destroy()
    {
        SAFE_RELEASE(view_);
    }

    //--------------------------------------------------------
    //---
    //--- Texture1DRef
    //---
    //--------------------------------------------------------
    Texture1DRef::Texture1DRef(const Texture1DRef& rhs)
        :parent_type(rhs)
    {
    }

    void Texture1DRef::destroy()
    {
        parent_type::destroy();
    }

    //--------------------------------------------------------
    //---
    //--- Texture2DRef
    //---
    //--------------------------------------------------------
    Texture2DRef::Texture2DRef(const Texture2DRef& rhs)
        :parent_type(rhs)
    {
    }

    void Texture2DRef::destroy()
    {
        parent_type::destroy();
    }

    //--------------------------------------------------------
    //---
    //--- Texture3DRef
    //---
    //--------------------------------------------------------
    Texture3DRef::Texture3DRef(const Texture3DRef& rhs)
        :parent_type(rhs)
    {
    }

    void Texture3DRef::destroy()
    {
        parent_type::destroy();
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
        TextureFilterType filter,
        TextureAddress adress,
        const SubResourceData* initData,
        const ResourceViewDesc* resourceViewDesc)
    {
        SamplerStateRef sampler = SamplerState::create(
            filter,
            adress,
            adress,
            adress);
        if(false == sampler.valid()){
            return Texture1DRef();
        }

        D3D11_TEXTURE1D_DESC desc;

        desc.Width = width;
        desc.MipLevels = mipLevels;
        desc.ArraySize = arraySize;
        desc.Format = static_cast<DXGI_FORMAT>(format);
        desc.Usage = static_cast<D3D11_USAGE>(usage);
        desc.BindFlags = bind;
        desc.CPUAccessFlags = access;
        desc.MiscFlags = misc;

        ID3D11Device* device = Graphics::getDevice().getD3DDevice();

        ID3D11Texture1D* texture = NULL;
        HRESULT hr = device->CreateTexture1D(
            &desc,
            reinterpret_cast<const D3D11_SUBRESOURCE_DATA*>(initData),
            &texture);

        if(FAILED(hr)){
            return Texture1DRef();
        }

        if(NULL == resourceViewDesc){
            return Texture1DRef(sampler, NULL, texture);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
        viewDesc.Format = static_cast<DXGI_FORMAT>(resourceViewDesc->format_);
        if(arraySize>1){
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
            viewDesc.Texture1DArray.MostDetailedMip = resourceViewDesc->tex1DArray_.mostDetailedMip_;
            viewDesc.Texture1DArray.MipLevels = resourceViewDesc->tex1DArray_.mipLevels_;
            viewDesc.Texture1DArray.FirstArraySlice = resourceViewDesc->tex1DArray_.firstArraySlice_;
            viewDesc.Texture1DArray.ArraySize = resourceViewDesc->tex1DArray_.arraySize_;

        }else{
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            viewDesc.Texture1D.MostDetailedMip = resourceViewDesc->tex1D_.mostDetailedMip_;
            viewDesc.Texture1D.MipLevels = resourceViewDesc->tex1D_.mipLevels_;
        }

        ID3D11ShaderResourceView* view = NULL;
        hr = device->CreateShaderResourceView(
            texture,
            &viewDesc,
            &view);

        if(SUCCEEDED(hr)){
            return Texture1DRef(sampler, view, texture);
        }else{
            texture->Release();
            return Texture1DRef();
        }
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
        TextureFilterType filter,
        TextureAddress adress,
        f32 borderColor,
        const SubResourceData* initData,
        const ResourceViewDesc* resourceViewDesc)
    {
        SamplerStateRef sampler = SamplerState::create(
            filter,
            adress,
            adress,
            adress,
            borderColor);
        if(false == sampler.valid()){
            return Texture2DRef();
        }

        D3D11_TEXTURE2D_DESC desc;

        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = mipLevels;
        desc.ArraySize = arraySize;
        desc.Format = static_cast<DXGI_FORMAT>(format);
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = static_cast<D3D11_USAGE>(usage);
        desc.BindFlags = bind;
        desc.CPUAccessFlags = access;
        desc.MiscFlags = misc;

        ID3D11Device* device = Graphics::getDevice().getD3DDevice();

        ID3D11Texture2D* texture = NULL;
        HRESULT hr = device->CreateTexture2D(
            &desc,
            reinterpret_cast<const D3D11_SUBRESOURCE_DATA*>(initData),
            &texture);

        if(FAILED(hr)){
            return Texture2DRef();
        }

        if(NULL == resourceViewDesc){
            return Texture2DRef(sampler, NULL, texture);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
        viewDesc.Format = static_cast<DXGI_FORMAT>(resourceViewDesc->format_);
        if(arraySize>1){
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            viewDesc.Texture2DArray.MostDetailedMip = resourceViewDesc->tex2DArray_.mostDetailedMip_;
            viewDesc.Texture2DArray.MipLevels = resourceViewDesc->tex2DArray_.mipLevels_;
            viewDesc.Texture2DArray.FirstArraySlice = resourceViewDesc->tex2DArray_.firstArraySlice_;
            viewDesc.Texture2DArray.ArraySize = resourceViewDesc->tex2DArray_.arraySize_;

        }else{
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            viewDesc.Texture2D.MostDetailedMip = resourceViewDesc->tex2D_.mostDetailedMip_;
            viewDesc.Texture2D.MipLevels = resourceViewDesc->tex2D_.mipLevels_;
        }

        ID3D11ShaderResourceView* view = NULL;
        hr = device->CreateShaderResourceView(
            texture,
            &viewDesc,
            &view);

        if(SUCCEEDED(hr)){
            return Texture2DRef(sampler, view, texture);
        }else{
            texture->Release();
            return Texture2DRef();
        }
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
        TextureFilterType filter,
        TextureAddress adress,
        const SubResourceData* initData,
        const ResourceViewDesc* resourceViewDesc)
    {
        SamplerStateRef sampler = SamplerState::create(
            filter,
            adress,
            adress,
            adress);
        if(false == sampler.valid()){
            return Texture3DRef();
        }

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

        ID3D11Device* device = Graphics::getDevice().getD3DDevice();

        ID3D11Texture3D* texture = NULL;
        HRESULT hr = device->CreateTexture3D(
            &desc,
            reinterpret_cast<const D3D11_SUBRESOURCE_DATA*>(initData),
            &texture);

        if(FAILED(hr)){
            return Texture3DRef();
        }

        if(NULL == resourceViewDesc){
            return Texture3DRef(sampler, NULL, texture);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
        viewDesc.Format = static_cast<DXGI_FORMAT>(resourceViewDesc->format_);
        viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        viewDesc.Texture2D.MostDetailedMip = resourceViewDesc->tex3D_.mostDetailedMip_;
        viewDesc.Texture2D.MipLevels = resourceViewDesc->tex3D_.mipLevels_;

        ID3D11ShaderResourceView* view = NULL;
        hr = device->CreateShaderResourceView(
            texture,
            &viewDesc,
            &view);

        if(SUCCEEDED(hr)){
            return Texture3DRef(sampler, view, texture);
        }else{
            texture->Release();
            return Texture3DRef();
        }
    }
}

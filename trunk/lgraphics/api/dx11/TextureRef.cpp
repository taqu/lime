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
    bool SRVDesc::copy(D3D11_SHADER_RESOURCE_VIEW_DESC& viewDesc, const SRVDesc& desc)
    {
        viewDesc.Format = static_cast<DXGI_FORMAT>(desc.format_);

        switch(desc.dimension_)
        {
        case ViewSRVDimension_Buffer:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            viewDesc.Buffer.FirstElement = desc.buffer_.firstElement_;
            viewDesc.Buffer.NumElements = desc.buffer_.numElements_;
            break;

        case ViewSRVDimension_Texture1D:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            viewDesc.Texture1D.MostDetailedMip = desc.tex1D_.mostDetailedMip_;
            viewDesc.Texture1D.MipLevels = desc.tex1D_.mipLevels_;
            break;

        case ViewSRVDimension_Texture1DArray:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
            viewDesc.Texture1DArray.MostDetailedMip = desc.tex1DArray_.mostDetailedMip_;
            viewDesc.Texture1DArray.MipLevels = desc.tex1DArray_.mipLevels_;
            viewDesc.Texture1DArray.FirstArraySlice = desc.tex1DArray_.firstArraySlice_;
            viewDesc.Texture1DArray.ArraySize = desc.tex1DArray_.arraySize_;
            break;

        case ViewSRVDimension_Texture2D:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            viewDesc.Texture2D.MostDetailedMip = desc.tex2D_.mostDetailedMip_;
            viewDesc.Texture2D.MipLevels = desc.tex2D_.mipLevels_;
            break;

        case ViewSRVDimension_Texture2DArray:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            viewDesc.Texture2DArray.MostDetailedMip = desc.tex2DArray_.mostDetailedMip_;
            viewDesc.Texture2DArray.MipLevels = desc.tex2DArray_.mipLevels_;
            viewDesc.Texture2DArray.FirstArraySlice = desc.tex2DArray_.firstArraySlice_;
            viewDesc.Texture2DArray.ArraySize = desc.tex2DArray_.arraySize_;
            break;

        case ViewSRVDimension_Texture3D:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
            viewDesc.Texture2D.MostDetailedMip = desc.tex3D_.mostDetailedMip_;
            viewDesc.Texture2D.MipLevels = desc.tex3D_.mipLevels_;
            break;

        case ViewSRVDimension_Cube:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            viewDesc.TextureCube.MostDetailedMip = desc.texCube_.mostDetailedMip_;
            viewDesc.TextureCube.MipLevels = desc.texCube_.mipLevels_;
            break;

        case ViewSRVDimension_CubeArray:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
            viewDesc.TextureCubeArray.MostDetailedMip = desc.texCubeArray_.mostDetailedMip_;
            viewDesc.TextureCubeArray.MipLevels = desc.texCubeArray_.mipLevels_;
            viewDesc.TextureCubeArray.First2DArrayFace = desc.texCubeArray_.first2DArraySlice_;
            viewDesc.TextureCubeArray.NumCubes = desc.texCubeArray_.numCubes_;
            break;

        default:
            return false;
        };
        return true;
    }

    //--------------------------------------------------------
    //---
    //--- ShaderResourceViewRef
    //---
    //--------------------------------------------------------
    ShaderResourceViewRef::ShaderResourceViewRef(const ShaderResourceViewRef& rhs)
        :view_(rhs.view_)
    {
        if(view_){
            view_->AddRef();
        }
    }

    void ShaderResourceViewRef::destroy()
    {
        SAFE_RELEASE(view_);
    }

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

    ShaderResourceViewRef RenderTargetViewRef::createSRView(const SRVDesc& desc)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

        if(SRVDesc::copy(viewDesc, desc)){
            ID3D11Device* device = Graphics::getDevice().getD3DDevice();

            ID3D11Resource* resource = NULL;
            view_->GetResource(&resource);
            if(NULL != resource){
                ID3D11ShaderResourceView* view = NULL;
                HRESULT hr = device->CreateShaderResourceView(
                    resource,
                    &viewDesc,
                    &view);

                resource->Release();

                if(SUCCEEDED(hr)){
                    return ShaderResourceViewRef(view);
                }
            }
        }
        return ShaderResourceViewRef();
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
    //--- UnorderedAccessViewRef
    //---
    //--------------------------------------------------------
    UnorderedAccessViewRef::UnorderedAccessViewRef(const UnorderedAccessViewRef& rhs)
        :view_(rhs.view_)
    {
        if(view_){
            view_->AddRef();
        }
    }

    void UnorderedAccessViewRef::destroy()
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
    //--- BufferRef
    //---
    //--------------------------------------------------------
    BufferRef::BufferRef(const BufferRef& rhs)
        :parent_type(rhs)
    {
    }

    void BufferRef::destroy()
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
        CmpFunc compFunc,
        f32 borderColor,
        const SubResourceData* initData,
        const SRVDesc* resourceViewDesc)
    {
        SamplerStateRef sampler = SamplerState::create(
            filter,
            adress,
            adress,
            adress,
            compFunc,
            borderColor);
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

        switch(resourceViewDesc->dimension_)
        {
        case ViewSRVDimension_Texture1D:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            viewDesc.Texture1D.MostDetailedMip = resourceViewDesc->tex1D_.mostDetailedMip_;
            viewDesc.Texture1D.MipLevels = resourceViewDesc->tex1D_.mipLevels_;
            break;

        case ViewSRVDimension_Texture1DArray:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
            viewDesc.Texture1DArray.MostDetailedMip = resourceViewDesc->tex1DArray_.mostDetailedMip_;
            viewDesc.Texture1DArray.MipLevels = resourceViewDesc->tex1DArray_.mipLevels_;
            viewDesc.Texture1DArray.FirstArraySlice = resourceViewDesc->tex1DArray_.firstArraySlice_;
            viewDesc.Texture1DArray.ArraySize = resourceViewDesc->tex1DArray_.arraySize_;
            break;

        default:
            return Texture1DRef(sampler, NULL, texture);
        };

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
            CmpFunc compFunc,
            f32 borderColor,
            const SubResourceData* initData,
            const SRVDesc* resourceViewDesc)
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
            filter,
            adress,
            compFunc,
            borderColor,
            initData,
            resourceViewDesc);
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
        TextureFilterType filter,
        TextureAddress adress,
        CmpFunc compFunc,
        f32 borderColor,
        const SubResourceData* initData,
        const SRVDesc* resourceViewDesc)
    {
        SamplerStateRef sampler = SamplerState::create(
            filter,
            adress,
            adress,
            adress,
            compFunc,
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
        desc.SampleDesc.Count = samleCount;
        desc.SampleDesc.Quality = sampleQuality;
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

        switch(resourceViewDesc->dimension_)
        {
        case ViewSRVDimension_Texture2D:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            viewDesc.Texture2D.MostDetailedMip = resourceViewDesc->tex2D_.mostDetailedMip_;
            viewDesc.Texture2D.MipLevels = resourceViewDesc->tex2D_.mipLevels_;
            break;

        case ViewSRVDimension_Texture2DArray:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            viewDesc.Texture2DArray.MostDetailedMip = resourceViewDesc->tex2DArray_.mostDetailedMip_;
            viewDesc.Texture2DArray.MipLevels = resourceViewDesc->tex2DArray_.mipLevels_;
            viewDesc.Texture2DArray.FirstArraySlice = resourceViewDesc->tex2DArray_.firstArraySlice_;
            viewDesc.Texture2DArray.ArraySize = resourceViewDesc->tex2DArray_.arraySize_;
            break;

        case ViewSRVDimension_Cube:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            viewDesc.TextureCube.MostDetailedMip = resourceViewDesc->texCube_.mostDetailedMip_;
            viewDesc.TextureCube.MipLevels = resourceViewDesc->texCube_.mipLevels_;
            break;

        case ViewSRVDimension_CubeArray:
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
            viewDesc.TextureCubeArray.MostDetailedMip = resourceViewDesc->texCubeArray_.mostDetailedMip_;
            viewDesc.TextureCubeArray.MipLevels = resourceViewDesc->texCubeArray_.mipLevels_;
            viewDesc.TextureCubeArray.First2DArrayFace = resourceViewDesc->texCubeArray_.first2DArraySlice_;
            viewDesc.TextureCubeArray.NumCubes = resourceViewDesc->texCubeArray_.numCubes_;
            break;

        default:
            return Texture2DRef(sampler, NULL, texture);
        };

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
        CmpFunc compFunc,
        f32 borderColor,
        const SubResourceData* initData,
        const SRVDesc* resourceViewDesc)
    {
        SamplerStateRef sampler = SamplerState::create(
            filter,
            adress,
            adress,
            adress,
            compFunc,
            borderColor);
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


    BufferRef Texture::createBuffer(
        u32 size,
        Usage usage,
        u32 bind,
        CPUAccessFlag access,
        ResourceMisc misc,
        u32 structureByteStride,
        const SubResourceData* initData,
        const SRVDesc* resourceViewDesc)
    {
        SamplerStateRef sampler = SamplerState::create(
            TextureFilterType::TexFilter_MinMagMipPoint,
            TextureAddress::TexAddress_Clamp,
            TextureAddress::TexAddress_Clamp,
            TextureAddress::TexAddress_Clamp,
            Cmp_Never);
        if(false == sampler.valid()){
            return BufferRef();
        }

        D3D11_BUFFER_DESC desc;

        desc.ByteWidth = size;
        desc.Usage = static_cast<D3D11_USAGE>(usage);
        desc.BindFlags = bind;
        desc.CPUAccessFlags = access;
        desc.MiscFlags = misc;
        desc.StructureByteStride = structureByteStride;

        ID3D11Device* device = Graphics::getDevice().getD3DDevice();

        ID3D11Buffer* buffer = NULL;
        HRESULT hr = device->CreateBuffer(
            &desc,
            reinterpret_cast<const D3D11_SUBRESOURCE_DATA*>(initData),
            &buffer);

        if(FAILED(hr)){
            return BufferRef();
        }

        if(NULL == resourceViewDesc){
            return BufferRef(sampler, NULL, buffer);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
        viewDesc.Format = static_cast<DXGI_FORMAT>(resourceViewDesc->format_);
        viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        viewDesc.Buffer.FirstElement = resourceViewDesc->buffer_.firstElement_;
        viewDesc.Buffer.NumElements = resourceViewDesc->buffer_.numElements_;

        ID3D11ShaderResourceView* view = NULL;
        hr = device->CreateShaderResourceView(
            buffer,
            &viewDesc,
            &view);

        if(SUCCEEDED(hr)){
            return BufferRef(sampler, view, buffer);
        }else{
            buffer->Release();
            return BufferRef();
        }
    }
}

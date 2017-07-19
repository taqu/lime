/**
@file ViewRef.cpp
@author t-sakai
@date 2014/10/01 create
*/
#include "ViewRef.h"
#include "../../Graphics.h"

namespace lgfx
{
    bool RTVDesc::copy(D3D11_RENDER_TARGET_VIEW_DESC& viewDesc, const RTVDesc& desc)
    {
        viewDesc.Format = static_cast<DXGI_FORMAT>(desc.format_);
        viewDesc.ViewDimension = static_cast<D3D11_RTV_DIMENSION>(desc.dimension_);
        switch(desc.dimension_)
        {
        case RTVDimension_Texture1D:
            viewDesc.Texture1D.MipSlice = desc.tex1D_.mipSlice_;
            break;

        case RTVDimension_Texture1DArray:
            viewDesc.Texture1DArray.MipSlice        = desc.tex1DArray_.mipSlice_;
            viewDesc.Texture1DArray.FirstArraySlice = desc.tex1DArray_.firstArraySlice_;
            viewDesc.Texture1DArray.ArraySize       = desc.tex1DArray_.arraySize_;
            break;

        case RTVDimension_Texture2D:
            viewDesc.Texture2D.MipSlice = desc.tex2D_.mipSlice_;
            break;

        case RTVDimension_Texture2DArray:
            viewDesc.Texture2DArray.MipSlice        = desc.tex2DArray_.mipSlice_;
            viewDesc.Texture2DArray.FirstArraySlice = desc.tex2DArray_.firstArraySlice_;
            viewDesc.Texture2DArray.ArraySize       = desc.tex2DArray_.arraySize_;
            break;

        case RTVDimension_Texture3D:
            viewDesc.Texture3D.MipSlice    = desc.tex3D_.mipSlice_;
            viewDesc.Texture3D.FirstWSlice = desc.tex3D_.firstWSlice_;
            viewDesc.Texture3D.WSize       = desc.tex3D_.wSize_;
            break;
        default:
            return false;
        };
        return true;
    }

    bool RTVDesc::copy(RTVDesc& desc, const D3D11_RENDER_TARGET_VIEW_DESC& viewDesc)
    {
        desc.format_ = static_cast<DataFormat>(viewDesc.Format);
        desc.dimension_ = static_cast<RTVDimension>(viewDesc.ViewDimension);
        switch(desc.dimension_)
        {
        case RTVDimension_Texture1D:
            desc.tex1D_.mipSlice_ = viewDesc.Texture1D.MipSlice;
            break;

        case RTVDimension_Texture1DArray:
            desc.tex1DArray_.mipSlice_        = viewDesc.Texture1DArray.MipSlice;
            desc.tex1DArray_.firstArraySlice_ = viewDesc.Texture1DArray.FirstArraySlice;
            desc.tex1DArray_.arraySize_       = viewDesc.Texture1DArray.ArraySize;
            break;

        case RTVDimension_Texture2D:
            desc.tex2D_.mipSlice_ = viewDesc.Texture2D.MipSlice;
            break;

        case RTVDimension_Texture2DArray:
            desc.tex2DArray_.mipSlice_        = viewDesc.Texture2DArray.MipSlice;
            desc.tex2DArray_.firstArraySlice_ = viewDesc.Texture2DArray.FirstArraySlice;
            desc.tex2DArray_.arraySize_       = viewDesc.Texture2DArray.ArraySize;
            break;

        case RTVDimension_Texture3D:
            desc.tex3D_.mipSlice_    = viewDesc.Texture3D.MipSlice;
            desc.tex3D_.firstWSlice_ = viewDesc.Texture3D.FirstWSlice;
            desc.tex3D_.wSize_       = viewDesc.Texture3D.WSize;
            break;
        default:
            return false;
        };
        return true;
    }

    bool DSVDesc::copy(D3D11_DEPTH_STENCIL_VIEW_DESC& viewDesc, const DSVDesc& desc)
    {
        viewDesc.Format = static_cast<DXGI_FORMAT>(desc.format_);
        viewDesc.ViewDimension = static_cast<D3D11_DSV_DIMENSION>(desc.dimension_);
        viewDesc.Flags = 0;

        switch(desc.dimension_)
        {
        case DSVDimension_Texture1D:
            viewDesc.Texture1D.MipSlice = desc.tex1D_.mipSlice_;
            break;

        case DSVDimension_Texture1DArray:
            viewDesc.Texture1DArray.MipSlice        = desc.tex1DArray_.mipSlice_;
            viewDesc.Texture1DArray.FirstArraySlice = desc.tex1DArray_.firstArraySlice_;
            viewDesc.Texture1DArray.ArraySize       = desc.tex1DArray_.arraySize_;
            break;

        case DSVDimension_Texture2D:
            viewDesc.Texture2D.MipSlice = desc.tex2D_.mipSlice_;
            break;

        case DSVDimension_Texture2DArray:
            viewDesc.Texture2DArray.MipSlice        = desc.tex2DArray_.mipSlice_;
            viewDesc.Texture2DArray.FirstArraySlice = desc.tex2DArray_.firstArraySlice_;
            viewDesc.Texture2DArray.ArraySize       = desc.tex2DArray_.arraySize_;
            break;
        default:
            return false;
        };
        return true;
    }

    bool DSVDesc::copy(DSVDesc& desc, const D3D11_DEPTH_STENCIL_VIEW_DESC& viewDesc)
    {
        desc.format_ = static_cast<DataFormat>(viewDesc.Format);
        desc.dimension_ = static_cast<DSVDimension>(viewDesc.ViewDimension);

        switch(desc.dimension_)
        {
        case DSVDimension_Texture1D:
            desc.tex1D_.mipSlice_ = viewDesc.Texture1D.MipSlice;
            break;

        case DSVDimension_Texture1DArray:
            desc.tex1DArray_.mipSlice_ = viewDesc.Texture1DArray.MipSlice;
            desc.tex1DArray_.firstArraySlice_ = viewDesc.Texture1DArray.FirstArraySlice;
            desc.tex1DArray_.arraySize_ = viewDesc.Texture1DArray.ArraySize;
            break;

        case DSVDimension_Texture2D:
            desc.tex2D_.mipSlice_ = viewDesc.Texture2D.MipSlice;
            break;

        case DSVDimension_Texture2DArray:
            desc.tex2DArray_.mipSlice_ = viewDesc.Texture2DArray.MipSlice;
            desc.tex2DArray_.firstArraySlice_ = viewDesc.Texture2DArray.FirstArraySlice;
            desc.tex2DArray_.arraySize_ = viewDesc.Texture2DArray.ArraySize;
            break;
        default:
            return false;
        };
        return true;
    }

    bool SRVDesc::copy(D3D11_SHADER_RESOURCE_VIEW_DESC& viewDesc, const SRVDesc& desc)
    {
        viewDesc.Format = static_cast<DXGI_FORMAT>(desc.format_);
        viewDesc.ViewDimension = static_cast<D3D11_SRV_DIMENSION>(desc.dimension_);

        switch(desc.dimension_)
        {
        case SRVDimension_Buffer:
            viewDesc.Buffer.FirstElement = desc.buffer_.firstElement_;
            viewDesc.Buffer.NumElements = desc.buffer_.numElements_;
            break;

        case SRVDimension_Texture1D:
            viewDesc.Texture1D.MostDetailedMip = desc.tex1D_.mostDetailedMip_;
            viewDesc.Texture1D.MipLevels = desc.tex1D_.mipLevels_;
            break;

        case SRVDimension_Texture1DArray:
            viewDesc.Texture1DArray.MostDetailedMip = desc.tex1DArray_.mostDetailedMip_;
            viewDesc.Texture1DArray.MipLevels = desc.tex1DArray_.mipLevels_;
            viewDesc.Texture1DArray.FirstArraySlice = desc.tex1DArray_.firstArraySlice_;
            viewDesc.Texture1DArray.ArraySize = desc.tex1DArray_.arraySize_;
            break;

        case SRVDimension_Texture2D:
            viewDesc.Texture2D.MostDetailedMip = desc.tex2D_.mostDetailedMip_;
            viewDesc.Texture2D.MipLevels = desc.tex2D_.mipLevels_;
            break;

        case SRVDimension_Texture2DArray:
            viewDesc.Texture2DArray.MostDetailedMip = desc.tex2DArray_.mostDetailedMip_;
            viewDesc.Texture2DArray.MipLevels = desc.tex2DArray_.mipLevels_;
            viewDesc.Texture2DArray.FirstArraySlice = desc.tex2DArray_.firstArraySlice_;
            viewDesc.Texture2DArray.ArraySize = desc.tex2DArray_.arraySize_;
            break;

        case SRVDimension_Texture3D:
            viewDesc.Texture2D.MostDetailedMip = desc.tex3D_.mostDetailedMip_;
            viewDesc.Texture2D.MipLevels = desc.tex3D_.mipLevels_;
            break;

        case SRVDimension_Cube:
            viewDesc.TextureCube.MostDetailedMip = desc.texCube_.mostDetailedMip_;
            viewDesc.TextureCube.MipLevels = desc.texCube_.mipLevels_;
            break;

        case SRVDimension_CubeArray:
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

    bool SRVDesc::copy(SRVDesc& desc, const D3D11_SHADER_RESOURCE_VIEW_DESC& viewDesc)
    {
        desc.format_ = static_cast<DataFormat>(viewDesc.Format);
        desc.dimension_ = static_cast<SRVDimension>(viewDesc.ViewDimension);

        switch(desc.dimension_)
        {
        case SRVDimension_Buffer:
            desc.buffer_.firstElement_ = viewDesc.Buffer.FirstElement;
            desc.buffer_.numElements_ = viewDesc.Buffer.NumElements;
            break;

        case SRVDimension_Texture1D:
            desc.tex1D_.mostDetailedMip_ = viewDesc.Texture1D.MostDetailedMip;
            desc.tex1D_.mipLevels_ = viewDesc.Texture1D.MipLevels;
            break;

        case SRVDimension_Texture1DArray:
            desc.tex1DArray_.mostDetailedMip_ = viewDesc.Texture1DArray.MostDetailedMip;
            desc.tex1DArray_.mipLevels_ = viewDesc.Texture1DArray.MipLevels;
            desc.tex1DArray_.firstArraySlice_ = viewDesc.Texture1DArray.FirstArraySlice;
            desc.tex1DArray_.arraySize_ = viewDesc.Texture1DArray.ArraySize;
            break;

        case SRVDimension_Texture2D:
            desc.tex2D_.mostDetailedMip_ = viewDesc.Texture2D.MostDetailedMip;
            desc.tex2D_.mipLevels_ = viewDesc.Texture2D.MipLevels;
            break;

        case SRVDimension_Texture2DArray:
            desc.tex2DArray_.mostDetailedMip_ = viewDesc.Texture2DArray.MostDetailedMip;
            desc.tex2DArray_.mipLevels_ = viewDesc.Texture2DArray.MipLevels;
            desc.tex2DArray_.firstArraySlice_ = viewDesc.Texture2DArray.FirstArraySlice;
            desc.tex2DArray_.arraySize_ = viewDesc.Texture2DArray.ArraySize;
            break;

        case SRVDimension_Texture3D:
            desc.tex3D_.mostDetailedMip_ = viewDesc.Texture2D.MostDetailedMip;
            desc.tex3D_.mipLevels_ = viewDesc.Texture2D.MipLevels;
            break;

        case SRVDimension_Cube:
            desc.texCube_.mostDetailedMip_ = viewDesc.TextureCube.MostDetailedMip;
            desc.texCube_.mipLevels_ = viewDesc.TextureCube.MipLevels;
            break;

        case SRVDimension_CubeArray:
            desc.texCubeArray_.mostDetailedMip_ = viewDesc.TextureCubeArray.MostDetailedMip;
            desc.texCubeArray_.mipLevels_ = viewDesc.TextureCubeArray.MipLevels;
            desc.texCubeArray_.first2DArraySlice_ = viewDesc.TextureCubeArray.First2DArrayFace;
            desc.texCubeArray_.numCubes_ = viewDesc.TextureCubeArray.NumCubes;
            break;

        default:
            return false;
        };
        return true;
    }

    bool UAVDesc::copy(D3D11_UNORDERED_ACCESS_VIEW_DESC& viewDesc, const UAVDesc& desc)
    {
        viewDesc.Format = static_cast<DXGI_FORMAT>(desc.format_);
        viewDesc.ViewDimension = static_cast<D3D11_UAV_DIMENSION>(desc.dimension_);

        switch(desc.dimension_)
        {
        case UAVDimension_Buffer:
            viewDesc.Buffer.FirstElement = desc.buffer_.firstElement_;
            viewDesc.Buffer.NumElements = desc.buffer_.numElements_;
            viewDesc.Buffer.Flags = desc.buffer_.flags_;
            break;

        case UAVDimension_Texture1D:
            viewDesc.Texture1D.MipSlice = desc.tex1D_.mipSlice_;
            break;

        case UAVDimension_Texture1DArray:
            viewDesc.Texture1DArray.MipSlice        = desc.tex1DArray_.mipSlice_;
            viewDesc.Texture1DArray.FirstArraySlice = desc.tex1DArray_.firstArraySlice_;
            viewDesc.Texture1DArray.ArraySize       = desc.tex1DArray_.arraySize_;
            break;

        case UAVDimension_Texture2D:
            viewDesc.Texture2D.MipSlice = desc.tex2D_.mipSlice_;
            break;

        case UAVDimension_Texture2DArray:
            viewDesc.Texture2DArray.MipSlice        = desc.tex2DArray_.mipSlice_;
            viewDesc.Texture2DArray.FirstArraySlice = desc.tex2DArray_.firstArraySlice_;
            viewDesc.Texture2DArray.ArraySize       = desc.tex2DArray_.arraySize_;
            break;

        case UAVDimension_Texture3D:
            viewDesc.Texture3D.MipSlice        = desc.tex3D_.mipSlice_;
            viewDesc.Texture3D.FirstWSlice = desc.tex3D_.firstWSlice_;
            viewDesc.Texture3D.WSize       = desc.tex3D_.wsize_;
            break;

        default:
            return false;
        };
        return true;
    }

    bool UAVDesc::copy(UAVDesc& desc, const D3D11_UNORDERED_ACCESS_VIEW_DESC& viewDesc)
    {
        desc.format_ = static_cast<DataFormat>(viewDesc.Format);
        desc.dimension_ = static_cast<UAVDimension>(viewDesc.ViewDimension);

        switch(desc.dimension_)
        {
        case UAVDimension_Buffer:
            desc.buffer_.firstElement_ = viewDesc.Buffer.FirstElement;
            desc.buffer_.numElements_ = viewDesc.Buffer.NumElements;
            desc.buffer_.flags_ = viewDesc.Buffer.Flags;
            break;

        case UAVDimension_Texture1D:
            desc.tex1D_.mipSlice_ = viewDesc.Texture1D.MipSlice;
            break;

        case UAVDimension_Texture1DArray:
            desc.tex1DArray_.mipSlice_ = viewDesc.Texture1DArray.MipSlice;
            desc.tex1DArray_.firstArraySlice_ = viewDesc.Texture1DArray.FirstArraySlice;
            desc.tex1DArray_.arraySize_ = viewDesc.Texture1DArray.ArraySize;
            break;

        case UAVDimension_Texture2D:
            desc.tex2D_.mipSlice_ = viewDesc.Texture2D.MipSlice;
            break;

        case UAVDimension_Texture2DArray:
            desc.tex2DArray_.mipSlice_ = viewDesc.Texture2DArray.MipSlice;
            desc.tex2DArray_.firstArraySlice_ = viewDesc.Texture2DArray.FirstArraySlice;
            desc.tex2DArray_.arraySize_ = viewDesc.Texture2DArray.ArraySize;
            break;

        case UAVDimension_Texture3D:
            desc.tex3D_.mipSlice_ = viewDesc.Texture3D.MipSlice;
            desc.tex3D_.firstWSlice_ = viewDesc.Texture3D.FirstWSlice;
            desc.tex3D_.wsize_ = viewDesc.Texture3D.WSize;
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
    ViewRef::ViewRef(const ShaderResourceViewRef& rhs)
        :parent_type(rhs.view_)
    {
        if(NULL != rhs.view_){
            rhs.view_->AddRef();
        }
    }

    ViewRef::ViewRef(const RenderTargetViewRef& rhs)
        :parent_type(rhs.view_)
    {
        if(NULL != rhs.view_){
            rhs.view_->AddRef();
        }
    }

    ViewRef::ViewRef(const DepthStencilViewRef& rhs)
        :parent_type(rhs.view_)
    {
        if(NULL != rhs.view_){
            rhs.view_->AddRef();
        }
    }
    ViewRef::ViewRef(const UnorderedAccessViewRef& rhs)
        :parent_type(rhs.view_)
    {
        if(NULL != rhs.view_){
            rhs.view_->AddRef();
        }
    }

    ViewRef& ViewRef::operator=(const ViewRef& rhs)
    {
        ViewRef(rhs).swap(*this);
        return *this;
    }

    ViewRef& ViewRef::operator=(ViewRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            view_ = rhs.view_;
            rhs.view_ = NULL;
        }
        return *this;
    }

    ViewRef& ViewRef::operator=(ShaderResourceViewRef&& rhs)
    {
        if(view_ != rhs.view_){
            destroy();
            view_ = rhs.view_;
            rhs.view_ = NULL;
        }
        return *this;

    }

    ViewRef& ViewRef::operator=(RenderTargetViewRef&& rhs)
    {
        if(view_ != rhs.view_){
            destroy();
            view_ = rhs.view_;
            rhs.view_ = NULL;
        }
        return *this;
    }

    ViewRef& ViewRef::operator=(DepthStencilViewRef&& rhs)
    {
        if(view_ != rhs.view_){
            destroy();
            view_ = rhs.view_;
            rhs.view_ = NULL;
        }
        return *this;
    }

    ViewRef& ViewRef::operator=(UnorderedAccessViewRef&& rhs)
    {
        if(view_ != rhs.view_){
            destroy();
            view_ = rhs.view_;
            rhs.view_ = NULL;
        }
        return *this;
    }

    //--------------------------------------------------------
    //---
    //--- ShaderResourceViewRef
    //---
    //--------------------------------------------------------
    ShaderResourceViewRef& ShaderResourceViewRef::operator=(const ShaderResourceViewRef& rhs)
    {
        ShaderResourceViewRef(rhs).swap(*this);
        return *this;
    }

    ShaderResourceViewRef& ShaderResourceViewRef::operator=(ShaderResourceViewRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            view_ = rhs.view_;
            rhs.view_ = NULL;
        }
        return *this;
    }

    void ShaderResourceViewRef::attachVS(lgfx::ContextRef& context, u32 index)
    {
        context.setVSResources(index, 1, &view_);
    }

    void ShaderResourceViewRef::attachGS(lgfx::ContextRef& context, u32 index)
    {
        context.setGSResources(index, 1, &view_);
    }

    void ShaderResourceViewRef::attachPS(lgfx::ContextRef& context, u32 index)
    {
        context.setPSResources(index, 1, &view_);
    }

    void ShaderResourceViewRef::attachCS(lgfx::ContextRef& context, u32 index)
    {
        context.setCSResources(index, 1, &view_);
    }

    void ShaderResourceViewRef::attachHS(lgfx::ContextRef& context, u32 index)
    {
        context.setHSResources(index, 1, &view_);
    }

    void ShaderResourceViewRef::attachDS(lgfx::ContextRef& context, u32 index)
    {
        context.setDSResources(index, 1, &view_);
    }

    //--------------------------------------------------------
    //---
    //--- RenderTargetViewRef
    //---
    //--------------------------------------------------------
    RenderTargetViewRef& RenderTargetViewRef::operator=(const RenderTargetViewRef& rhs)
    {
        RenderTargetViewRef(rhs).swap(*this);
        return *this;
    }

    RenderTargetViewRef& RenderTargetViewRef::operator=(RenderTargetViewRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            view_ = rhs.view_;
            rhs.view_ = NULL;
        }
        return *this;
    }

    //--------------------------------------------------------
    //---
    //--- DepthStencilViewRef
    //---
    //--------------------------------------------------------
    DepthStencilViewRef& DepthStencilViewRef::operator=(const DepthStencilViewRef& rhs)
    {
        DepthStencilViewRef(rhs).swap(*this);
        return *this;
    }

    DepthStencilViewRef& DepthStencilViewRef::operator=(DepthStencilViewRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            view_ = rhs.view_;
            rhs.view_ = NULL;
        }
        return *this;
    }

    //--------------------------------------------------------
    //---
    //--- UnorderedAccessViewRef
    //---
    //--------------------------------------------------------
    UnorderedAccessViewRef& UnorderedAccessViewRef::operator=(const UnorderedAccessViewRef& rhs)
    {
        UnorderedAccessViewRef(rhs).swap(*this);
        return *this;
    }

    UnorderedAccessViewRef& UnorderedAccessViewRef::operator=(UnorderedAccessViewRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            view_ = rhs.view_;
            rhs.view_ = NULL;
        }
        return *this;
    }

    //--------------------------------------------------------
    RenderTargetViewRef View::createRTView(const RTVDesc& desc, ID3D11Resource* resource)
    {
        D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
        ID3D11RenderTargetView* view = NULL;
        HRESULT hr = E_FAIL;
        if(RTVDesc::copy(viewDesc, desc)){
            ID3D11Device* device = getDevice().getD3DDevice();

            hr = device->CreateRenderTargetView(
                resource,
                &viewDesc,
                &view);
        }
        return (SUCCEEDED(hr))? RenderTargetViewRef(view) : RenderTargetViewRef();
    }

    //--------------------------------------------------------
    DepthStencilViewRef View::createDSView(const DSVDesc& desc, ID3D11Resource* resource)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
        ID3D11DepthStencilView* view = NULL;
        HRESULT hr = E_FAIL;
        if(DSVDesc::copy(viewDesc, desc)){
            ID3D11Device* device = getDevice().getD3DDevice();

            hr = device->CreateDepthStencilView(
                resource,
                &viewDesc,
                &view);
        }
        return (SUCCEEDED(hr))? DepthStencilViewRef(view) : DepthStencilViewRef();
    }

    //--------------------------------------------------------
    ShaderResourceViewRef View::createSRView(const SRVDesc& desc, ID3D11Resource* resource)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
        ID3D11ShaderResourceView* view = NULL;
        HRESULT hr = E_FAIL;
        if(SRVDesc::copy(viewDesc, desc)){
            ID3D11Device* device = getDevice().getD3DDevice();

            hr = device->CreateShaderResourceView(
                resource,
                &viewDesc,
                &view);
        }
        return (SUCCEEDED(hr))? ShaderResourceViewRef(view) : ShaderResourceViewRef();
    }

    //--------------------------------------------------------
    UnorderedAccessViewRef View::createUAView(const UAVDesc& desc, ID3D11Resource* resource)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;
        ID3D11UnorderedAccessView* view = NULL;
        HRESULT hr = E_FAIL;
        if(UAVDesc::copy(viewDesc, desc)){
            ID3D11Device* device = getDevice().getD3DDevice();

            hr = device->CreateUnorderedAccessView(
                resource,
                &viewDesc,
                &view);
        }
        return (SUCCEEDED(hr))? UnorderedAccessViewRef(view) : UnorderedAccessViewRef();
    }
}

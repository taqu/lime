/**
@file ViewRef.cpp
@author t-sakai
@date 2014/10/01 create
*/
#include "ViewRef.h"
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

    void ShaderResourceViewRef::attachVS(lgraphics::ContextRef& context, u32 index)
    {
        context.setVSResources(index, 1, &view_);
    }

    void ShaderResourceViewRef::attachGS(lgraphics::ContextRef& context, u32 index)
    {
        context.setGSResources(index, 1, &view_);
    }

    void ShaderResourceViewRef::attachPS(lgraphics::ContextRef& context, u32 index)
    {
        context.setPSResources(index, 1, &view_);
    }

    void ShaderResourceViewRef::attachCS(lgraphics::ContextRef& context, u32 index)
    {
        context.setCSResources(index, 1, &view_);
    }

    void ShaderResourceViewRef::attachHS(lgraphics::ContextRef& context, u32 index)
    {
        context.setHSResources(index, 1, &view_);
    }

    void ShaderResourceViewRef::attachDS(lgraphics::ContextRef& context, u32 index)
    {
        context.setDSResources(index, 1, &view_);
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
    UnorderedAccessViewRef View::createUAView(const UAVDesc& desc, ID3D11Resource* resource)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;
        viewDesc.Format = static_cast<DXGI_FORMAT>(desc.format_);
        viewDesc.ViewDimension = static_cast<D3D11_UAV_DIMENSION>(desc.dimension_);

        switch(desc.dimension_)
        {
        case UAVDimension_Unknown:
            break;

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
        };

        ID3D11Device* device = Graphics::getDevice().getD3DDevice();

        ID3D11UnorderedAccessView* view = NULL;
        HRESULT hr = device->CreateUnorderedAccessView(
            resource,
            &viewDesc,
            &view);

        return (SUCCEEDED(hr))? UnorderedAccessViewRef(view) : UnorderedAccessViewRef();
    }
}

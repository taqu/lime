/**
@file SamplerStateRef.cpp
@author t-sakai
@date 2012/07/23 create
*/
#include "SamplerStateRef.h"
#include "../../Graphics.h"

namespace lgfx
{
    //------------------------------------------------------------
    //---
    //--- SamplerStateRef
    //---
    //------------------------------------------------------------
    SamplerStateRef::SamplerStateRef(const SamplerStateRef& rhs)
        :state_(rhs.state_)
    {
        if(state_ != NULL){
            state_->AddRef();
        }
    }

    SamplerStateRef::SamplerStateRef(SamplerStateRef&& rhs)
        :state_(rhs.state_)
    {
        rhs.state_ = NULL;
    }

    void SamplerStateRef::destroy()
    {
        LDXSAFE_RELEASE(state_);
    }

    void SamplerStateRef::attachVS(ContextRef& context, u32 start)
    {
        context.setVSSamplers(start, 1, &state_);
    }

    void SamplerStateRef::attachHS(ContextRef& context, u32 start)
    {
        context.setHSSamplers(start, 1, &state_);
    }

    void SamplerStateRef::attachDS(ContextRef& context, u32 start)
    {
        context.setDSSamplers(start, 1, &state_);
    }

    void SamplerStateRef::attachGS(ContextRef& context, u32 start)
    {
        context.setGSSamplers(start, 1, &state_);
    }

    void SamplerStateRef::attachPS(ContextRef& context, u32 start)
    {
        context.setPSSamplers(start, 1, &state_);
    }

    void SamplerStateRef::attachCS(ContextRef& context, u32 start)
    {
        context.setCSSamplers(start, 1, &state_);
    }

    SamplerStateRef& SamplerStateRef::operator=(const SamplerStateRef& rhs)
    {
        SamplerStateRef(rhs).swap(*this);
        return *this;
    }

    SamplerStateRef& SamplerStateRef::operator=(SamplerStateRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            state_ = rhs.state_;
            rhs.state_ = NULL;
        }
        return *this;
    }

    void SamplerStateRef::swap(SamplerStateRef& rhs)
    {
        lcore::swap(state_, rhs.state_);
    }

    //------------------------------------------------------------
    //---
    //--- SamplerState
    //---
    //------------------------------------------------------------
    SamplerStateRef SamplerState::create(
        TextureFilterType filter,
        TextureAddress addressU,
        TextureAddress addressV,
        TextureAddress addressW,
        CmpFunc compFunc,
        f32 borderColor)
    {
        D3D11_SAMPLER_DESC desc;
        desc.Filter = static_cast<D3D11_FILTER>(filter);
        desc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(addressU);
        desc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(addressV);
        desc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(addressW);
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(compFunc);
        desc.BorderColor[0] = borderColor;
        desc.BorderColor[1] = borderColor;
        desc.BorderColor[2] = borderColor;
        desc.BorderColor[3] = borderColor;
        desc.MinLOD = -D3D11_FLOAT32_MAX;
        desc.MaxLOD = D3D11_FLOAT32_MAX;

        ID3D11SamplerState* state = NULL;
        HRESULT hr = getDevice().getD3DDevice()->CreateSamplerState(
            &desc,
            &state);

        return (SUCCEEDED(hr))? SamplerStateRef(state) : SamplerStateRef();
    }

    SamplerStateRef SamplerState::create(
        TextureFilterType filter,
        TextureAddress address,
        CmpFunc compFunc,
        f32 borderColor)
    {
        D3D11_SAMPLER_DESC desc;
        desc.Filter = static_cast<D3D11_FILTER>(filter);
        desc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(address);
        desc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(address);
        desc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(address);
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(compFunc);
        desc.BorderColor[0] = borderColor;
        desc.BorderColor[1] = borderColor;
        desc.BorderColor[2] = borderColor;
        desc.BorderColor[3] = borderColor;
        desc.MinLOD = -D3D11_FLOAT32_MAX;
        desc.MaxLOD = D3D11_FLOAT32_MAX;

        ID3D11SamplerState* state = NULL;
        HRESULT hr = getDevice().getD3DDevice()->CreateSamplerState(
            &desc,
            &state);

        return (SUCCEEDED(hr))? SamplerStateRef(state) : SamplerStateRef();
    }
}

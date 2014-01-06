/**
@file SamplerStateRef.cpp
@author t-sakai
@date 2012/07/23 create
*/
#include "SamplerStateRef.h"
#include "../../lgraphics.h"

namespace lgraphics
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

    void SamplerStateRef::destroy()
    {
        SAFE_RELEASE(state_);
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
        HRESULT hr = Graphics::getDevice().getD3DDevice()->CreateSamplerState(
            &desc,
            &state);

        return (SUCCEEDED(hr))? SamplerStateRef(state) : SamplerStateRef();
    }
}

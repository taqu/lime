/**
@file RasterizerStateRef.cpp
@author t-sakai
@date 2017/01/15 create
*/
#include "RasterizerStateRef.h"
#include "Graphics.h"

namespace lgfx
{
    //--------------------------------------
    //---
    //--- RasterizerStateRef
    //---
    //--------------------------------------
    RasterizerStateRef::RasterizerStateRef(const RasterizerStateRef& rhs)
        :state_(rhs.state_)
    {
        if(state_ != NULL){
            state_->AddRef();
        }
    }

    RasterizerStateRef::RasterizerStateRef(RasterizerStateRef&& rhs)
        :state_(rhs.state_)
    {
        rhs.state_ = NULL;
    }

    void RasterizerStateRef::destroy()
    {
        LDXSAFE_RELEASE(state_);
    }

    RasterizerStateRef::RasterizerStateRef(pointer_type state)
        :state_(state)
    {}

    RasterizerStateRef& RasterizerStateRef::operator=(const RasterizerStateRef& rhs)
    {
        RasterizerStateRef(rhs).swap(*this);
        return *this;
    }

    RasterizerStateRef& RasterizerStateRef::operator=(RasterizerStateRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            state_ = rhs.state_;
            rhs.state_ = NULL;
        }
        return *this;
    }

    void RasterizerStateRef::swap(RasterizerStateRef& rhs)
    {
        lcore::swap(state_, rhs.state_);
    }


    //---------------------------------------------------------
    RasterizerStateRef RasterizerState::create(
        FillMode fillMode,
        CullMode cullMode,
        bool frontCounterClockwise,
        s32 depthBias,
        f32 depthBiasClamp,
        f32 slopeScaledDepthBias,
        bool depthClipEnable,
        bool scissorEnable,
        bool multisampleEnable,
        bool antialiasedLineEnable)
    {
        D3D11_RASTERIZER_DESC desc;
        desc.FillMode = static_cast<D3D11_FILL_MODE>(fillMode);
        desc.CullMode = static_cast<D3D11_CULL_MODE>(cullMode);
        desc.FrontCounterClockwise = frontCounterClockwise? TRUE : FALSE;
        desc.DepthBias = depthBias;
        desc.DepthBiasClamp = depthBiasClamp;
        desc.SlopeScaledDepthBias = slopeScaledDepthBias;
        desc.DepthClipEnable = depthClipEnable? TRUE : FALSE;
        desc.ScissorEnable = scissorEnable? TRUE : FALSE;
        desc.MultisampleEnable = multisampleEnable? TRUE : FALSE;
        desc.AntialiasedLineEnable = antialiasedLineEnable? TRUE : FALSE;

        ID3D11RasterizerState* state = NULL;
        lgfx::getDevice().getD3DDevice()->CreateRasterizerState(&desc, &state);

        return RasterizerStateRef(state);
    }
}

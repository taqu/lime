/**
@file DepthStencilStateRef.cpp
@author t-sakai
@date 2017/01/15 create
*/
#include "DepthStencilStateRef.h"
#include "Graphics.h"

namespace lgfx
{
    //--------------------------------------
    //---
    //--- DepthStencilStateRef
    //---
    //--------------------------------------
    DepthStencilStateRef::DepthStencilStateRef()
        :state_(NULL)
    {}

    DepthStencilStateRef::DepthStencilStateRef(const DepthStencilStateRef& rhs)
        :state_(rhs.state_)
    {
        if(state_ != NULL){
            state_->AddRef();
        }
    }

    DepthStencilStateRef::DepthStencilStateRef(DepthStencilStateRef&& rhs)
        :state_(rhs.state_)
    {
        rhs.state_ = NULL;
    }

    DepthStencilStateRef::~DepthStencilStateRef()
    {
        destroy();
    }

    void DepthStencilStateRef::destroy()
    {
        LDXSAFE_RELEASE(state_);
    }

    DepthStencilStateRef::DepthStencilStateRef(pointer_type state)
        :state_(state)
    {}

    bool DepthStencilStateRef::valid() const
    {
        return (NULL != state_);
    }

    DepthStencilStateRef& DepthStencilStateRef::operator=(const DepthStencilStateRef& rhs)
    {
        DepthStencilStateRef(rhs).swap(*this);
        return *this;
    }

    DepthStencilStateRef& DepthStencilStateRef::operator=(DepthStencilStateRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            state_ = rhs.state_;
            rhs.state_ = NULL;
        }
        return *this;
    }

    void DepthStencilStateRef::swap(DepthStencilStateRef& rhs)
    {
        lcore::swap(state_, rhs.state_);
    }


    //---------------------------------------------------------
    DepthStencilStateRef DepthStencilState::create(
        bool depthEnable,
        DepthWriteMask depthWriteMask,
        CmpFunc depthFunc,
        bool stencilEnable,
        u8 stencilReadMask,
        u8 stencilWriteMask,
        const StencilOPDesc& frontFace,
        const StencilOPDesc& backFace)
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        desc.DepthEnable = (depthEnable)? TRUE : FALSE;
        desc.DepthWriteMask = (D3D11_DEPTH_WRITE_MASK)depthWriteMask;
        desc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(depthFunc);
        desc.StencilEnable = (stencilEnable)? TRUE : FALSE;
        desc.StencilReadMask = stencilReadMask;
        desc.StencilWriteMask = stencilWriteMask;

        desc.FrontFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(frontFace.failOp_);
        desc.FrontFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(frontFace.depthFailOp_);
        desc.FrontFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(frontFace.passOp_);
        desc.FrontFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(frontFace.cmpFunc_);

        desc.BackFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(backFace.failOp_);
        desc.BackFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(backFace.depthFailOp_);
        desc.BackFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(backFace.passOp_);
        desc.BackFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(backFace.cmpFunc_);

        ID3D11DepthStencilState* state = NULL;
        lgfx::getDevice().getD3DDevice()->CreateDepthStencilState(&desc, &state);

        return DepthStencilStateRef(state);
    }
}

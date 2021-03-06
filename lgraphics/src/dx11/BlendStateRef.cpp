﻿/**
@file BlendStateRef.cpp
@author t-sakai
@date 2012/07/24 create
*/
#include "BlendStateRef.h"
#include "../../Graphics.h"

namespace lgfx
{
    //------------------------------------------------------------
    //---
    //--- BlendStateRef
    //---
    //------------------------------------------------------------
    BlendStateRef::BlendStateRef(const BlendStateRef& rhs)
        :state_(rhs.state_)
    {
        if(state_ != NULL){
            state_->AddRef();
        }
    }

    BlendStateRef::BlendStateRef(BlendStateRef&& rhs)
        :state_(rhs.state_)
    {
        rhs.state_ = NULL;
    }

    void BlendStateRef::destroy()
    {
        LDXSAFE_RELEASE(state_);
    }

    void BlendStateRef::attach(ContextRef& context)
    {
        context.setBlendState(state_);
    }

    BlendStateRef& BlendStateRef::operator=(const BlendStateRef& rhs)
    {
        BlendStateRef(rhs).swap(*this);
        return *this;
    }

    BlendStateRef& BlendStateRef::operator=(BlendStateRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            state_ = rhs.state_;
            rhs.state_ = NULL;
        }
        return *this;
    }

    bool BlendStateRef::getDesc(BlendStateDesc& desc)
    {
        if(NULL != state_){
            state_->GetDesc(&desc);
            return true;
        }
        return false;
    }

    //------------------------------------------------------------
    //---
    //--- BlendState
    //---
    //------------------------------------------------------------
    ID3D11BlendState* BlendState::createRaw(
        s32 alphaToCoverage,
        s32 blendEnable,
        BlendType src,
        BlendType dst,
        BlendOp op,
        BlendType srcAlpha,
        BlendType dstAlpha,
        BlendOp opAlpha,
        u8 writeMask)
    {
        D3D11_BLEND_DESC desc;
        desc.AlphaToCoverageEnable = alphaToCoverage;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable = blendEnable;
        desc.RenderTarget[0].SrcBlend = static_cast<D3D11_BLEND>(src);
        desc.RenderTarget[0].DestBlend = static_cast<D3D11_BLEND>(dst);
        desc.RenderTarget[0].BlendOp = static_cast<D3D11_BLEND_OP>(op);
        desc.RenderTarget[0].SrcBlendAlpha = static_cast<D3D11_BLEND>(srcAlpha);
        desc.RenderTarget[0].DestBlendAlpha = static_cast<D3D11_BLEND>(dstAlpha);
        desc.RenderTarget[0].BlendOpAlpha = static_cast<D3D11_BLEND_OP>(opAlpha);
        desc.RenderTarget[0].RenderTargetWriteMask = writeMask;

        ID3D11BlendState* state = NULL;
        getDevice().getD3DDevice()->CreateBlendState(
            &desc,
            &state);
        return state;
    }


    BlendStateRef BlendState::createIndipendent(
            s32 alphaToCoverage,
            const s32* blendEnable,
            const BlendType* src,
            const BlendType* dst,
            const BlendOp* op,
            const BlendType* srcAlpha,
            const BlendType* dstAlpha,
            const BlendOp* opAlpha,
            const u8* writeMask)
    {
        D3D11_BLEND_DESC desc;
        desc.AlphaToCoverageEnable = alphaToCoverage;
        desc.IndependentBlendEnable = TRUE;

        for(s32 i=0; i<LGFX_MAX_RENDER_TARGETS; ++i){
            desc.RenderTarget[i].BlendEnable = blendEnable[i];
            desc.RenderTarget[i].SrcBlend = static_cast<D3D11_BLEND>(src[i]);
            desc.RenderTarget[i].DestBlend = static_cast<D3D11_BLEND>(dst[i]);
            desc.RenderTarget[i].BlendOp = static_cast<D3D11_BLEND_OP>(op[i]);
            desc.RenderTarget[i].SrcBlendAlpha = static_cast<D3D11_BLEND>(srcAlpha[i]);
            desc.RenderTarget[i].DestBlendAlpha = static_cast<D3D11_BLEND>(dstAlpha[i]);
            desc.RenderTarget[i].BlendOpAlpha = static_cast<D3D11_BLEND_OP>(opAlpha[i]);
            desc.RenderTarget[i].RenderTargetWriteMask = writeMask[i];
        }

        ID3D11BlendState* state = NULL;
        HRESULT hr = getDevice().getD3DDevice()->CreateBlendState(
            &desc,
            &state);

        return (SUCCEEDED(hr))? BlendStateRef(state) : BlendStateRef();
    }
}

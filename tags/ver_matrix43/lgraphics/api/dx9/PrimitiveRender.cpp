/**
@file PrimitiveRender.cpp
@author t-sakai
@date 2009/05/15
*/
#include <lmath/lmath.h>
#include "../lgraphics.h"
#include "GraphicsDevice.h"

#include "RenderState.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "RenderState.h"
#include "PrimitiveRender.h"

namespace lgraphics
{
    const u32 PRIMITIVE_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

    PrimitiveRender::PrimitiveRender(VertexBuffer* vb, RenderState* state, u32 maxTriangle, Texture* texture)
        :vb_(vb),
        texture_(texture),
        state_(state),
        maxTriangle_(maxTriangle),
        numTriangle_(0),
        lockedBuffer_(NULL)
    {
    }

    PrimitiveRender::~PrimitiveRender()
    {
        LIME_DELETE(texture_);
    }

    PrimitiveRender* PrimitiveRender::create(u32 maxTriangle, Texture* texture)
    {
        lcore::ScopedPtr<VertexBuffer> vbPtr( VertexBuffer::create(sizeof(PrimitiveVertex), maxTriangle*3, Pool_Default, Usage_Dynamic) );
        if(!RenderState::begin()){
            return NULL;
        }

        PrimitiveRender::setState(vbPtr.get(), texture);

        lcore::ScopedPtr<RenderState> statePtr( RenderState::end() );

        if(vbPtr && statePtr){
            PrimitiveRender *prim = LIME_NEW PrimitiveRender(vbPtr.release(), statePtr.release(), maxTriangle, texture);
            LASSERT(prim != NULL);
            return prim;
        };
        return NULL;
    }

    void PrimitiveRender::draw(const lmath::Matrix44& view, const lmath::Matrix44& proj)
    {
        LASSERT(lockedBuffer_ == NULL);

        state_->apply();

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        d3ddevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&view);
        d3ddevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&proj);

        d3ddevice->DrawPrimitive((D3DPRIMITIVETYPE )Primitive_TriangleList, 0, numTriangle_);
    }

    void PrimitiveRender::drawNoStateChange()
    {
        LASSERT(lockedBuffer_ == NULL);
        vb_->attach();

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        d3ddevice->DrawPrimitive((D3DPRIMITIVETYPE )Primitive_TriangleList, 0, numTriangle_);
    }

    bool PrimitiveRender::add(const PrimitiveVertex& v1, const PrimitiveVertex& v2, const PrimitiveVertex& v3)
    {
        LASSERT(lockedBuffer_ != NULL);
        if(numTriangle_ >= maxTriangle_){
            return false;
        }

        lockedBuffer_[numTriangle_ * 3 + 0] = v1;
        lockedBuffer_[numTriangle_ * 3 + 1] = v2;
        lockedBuffer_[numTriangle_ * 3 + 2] = v3;

        ++numTriangle_;
        return true;
    }

    void PrimitiveRender::clear()
    {
        numTriangle_ = 0;
    }

    void PrimitiveRender::lock()
    {
        bool isLock = vb_->lock((void**)&lockedBuffer_, Lock_Discard);
        if(false == isLock){
            return;
        }

        clear();
    }

    void PrimitiveRender::unlock()
    {
        vb_->unlock();
        lockedBuffer_ = NULL;
    }

    void PrimitiveRender::setState(VertexBuffer* vb, Texture* texture)
    {
        LASSERT(vb != NULL);

        RenderState::setZEnable(true);
        RenderState::setZWriteEnable(true);
        RenderState::setAlphaTest(true);

        RenderState::setAlphaBlendEnable(false);

        //RenderState::setAlphaBlendSrc(Blend_SrcAlpha);
        //RenderState::setAlphaBlendDst(Blend_InvSrcAlpha);

        //Disable Lighting
        RenderState::setLighting(false);

        RenderState::setCullMode(lgraphics::CullMode_None);


        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();

        d3ddevice->SetVertexShader(NULL);
        d3ddevice->SetPixelShader(NULL);
        d3ddevice->SetFVF(PRIMITIVE_FVF);

        if(texture != NULL){
            d3ddevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            d3ddevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            d3ddevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

            d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

            d3ddevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            d3ddevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

            texture->attach(0);
        }else{
            d3ddevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
            d3ddevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

            d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        }

        vb->attach();

        //lmath::Matrix44 mat;
        //mat.identity();
        //d3ddevice->SetTransform(D3DTS_WORLDMATRIX(0), (D3DMATRIX*)&mat);
    }
}

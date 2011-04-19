/**
@file RenderStateRef.cpp
@author t-sakai
@date 2010/06/16 create
@date 2010/12/12 modify for gles2
*/
#include "../../lgraphicsAPIInclude.h"
#include "../../lgraphics.h"
#include "RenderStateRef.h"


using namespace lcore;

namespace lgraphics
{
    //----------------------------------------------------------
    //---
    //--- RenderState
    //---
    //----------------------------------------------------------
    void RenderState::setAlphaTest(bool enable)
    {
        Graphics::getDevice().setAlphaTest(enable);
    }

    bool RenderState::getAlphaTest()
    {
        return Graphics::getDevice().getAlphaTest();
    }

    void RenderState::setAlphaTestFunc(CmpFunc func)
    {
        Graphics::getDevice().setAlphaTestFunc(func);
    }

    CmpFunc RenderState::getAlphaTestFunc()
    {
        return Graphics::getDevice().getAlphaTestFunc();
    }

    void RenderState::setAlphaTestRef(s32 refValue)
    {
        Graphics::getDevice().setAlphaTestRef(refValue);
    }

    s32 RenderState::getAlphaTestRef()
    {
        return Graphics::getDevice().getAlphaTestRef();
    }


    void RenderState::setCullMode(CullMode mode)
    {
        Graphics::getDevice().setCullMode(mode);
    }

    CullMode RenderState::getCullMode()
    {
        return Graphics::getDevice().getCullMode();
    }

    void RenderState::setMultiSampleAlias(bool enable)
    {
        Graphics::getDevice().setMultiSampleAlias(enable);
    }

    bool RenderState::getMultiSampleAlias()
    {
        return Graphics::getDevice().getMultiSampleAlias();
    }

    void RenderState::setZEnable(bool enable)
    {
        Graphics::getDevice().setZEnable(enable);
    }

    bool RenderState::getZEnable()
    {
        return Graphics::getDevice().getZEnable();
    }

    void RenderState::setZWriteEnable(bool enable)
    {
        Graphics::getDevice().setZWriteEnable(enable);
    }

    bool RenderState::getZWriteEnable()
    {
        return Graphics::getDevice().getZWriteEnable();
    }

    void RenderState::setAlphaBlendEnable(bool enable)
    {
        Graphics::getDevice().setAlphaBlendEnable(enable);
    }

    bool RenderState::getAlphaBlendEnable()
    {
        return Graphics::getDevice().getAlphaBlendEnable();
    }

    void RenderState::setAlphaBlend(BlendType src, BlendType dst)
    {
        Graphics::getDevice().setAlphaBlend(src, dst);
    }

    BlendType RenderState::getAlphaBlendSrc()
    {
        return Graphics::getDevice().getAlphaBlendSrc();
    }

    BlendType RenderState::getAlphaBlendDst()
    {
        return Graphics::getDevice().getAlphaBlendDst();
    }

    void RenderState::setViewPort(s32 x, s32 y, s32 width, s32 height, f32 minz, f32 maxz)
    {
        glViewport(x, y, width, height);
        glDepthRangef(minz, maxz);
    }

    void RenderState::getViewPort(s32& x, s32& y, s32& width, s32& height, f32& minz, f32& maxz)
    {
        GLint values[4];
        glGetIntegerv(GL_VIEWPORT, values);
        x = values[0];
        y = values[1];
        width = values[2];
        height = values[3];

        GLfloat range[2];
        glGetFloatv(GL_DEPTH_RANGE, range);
        minz = range[0];
        maxz = range[1];
    }

    //inline void RenderStateRef::StateBlock::apply()
    //{
    //    (check(Bit_ZEnable))? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

    //    if(check(Bit_CullingEnable)){
    //        glEnable(GL_CULL_FACE);
    //        glFrontFace(cullMode_);
    //    }else{
    //        glDisable(GL_CULL_FACE);
    //    }

    //    //アルファブレンド有効、無効の設定はレンダリングパスで行う
    //    if(check(Bit_AlphaBlendEnable)){
    //        //glEnable(GL_BLEND);
    //        glBlendFuncSeparate(alphaBlendSrc_, alphaBlendDst_, Blend_SrcAlpha, Blend_DestAlpha);
    //    }else{
    //        //glDisable(GL_BLEND);
    //    }

    //    GLboolean flag = (check(Bit_ZWriteEnable))? GL_TRUE : GL_FALSE;
    //    glDepthMask( flag );
    //}

    //----------------------------------------------------------
    //---
    //--- RenderStateRef
    //---
    //----------------------------------------------------------
    RenderStateRef::RenderStateRef()
        :flag_(0)
        ,alphaTestFunc_(Cmp_Less)
        ,alphaTestRef_(128)
        ,cullMode_(CullMode_CCW)
        ,alphaBlendSrc_(Blend_SrcAlpha)
        ,alphaBlendDst_(Blend_InvSrcAlpha)
    {
    }

    RenderStateRef::RenderStateRef(const RenderStateRef& rhs)
        :flag_(rhs.flag_)
        ,alphaTestFunc_(rhs.alphaTestFunc_)
        ,alphaTestRef_(rhs.alphaTestRef_)
        ,cullMode_(rhs.cullMode_)
        ,alphaBlendSrc_(rhs.alphaBlendSrc_)
        ,alphaBlendDst_(rhs.alphaBlendDst_)
    {
    }

    RenderStateRef::~RenderStateRef()
    {
    }

    RenderStateRef& RenderStateRef::operator=(const RenderStateRef& rhs)
    {
        RenderStateRef tmp(rhs);
        tmp.swap(*this);
        return *this;
    }

    void RenderStateRef::apply()
    {
        Graphics::getDevice().setRenderState(*this);
    }

    void RenderStateRef::swap(RenderStateRef& rhs)
    {
        lcore::swap(flag_, rhs.flag_);
        lcore::swap(alphaTestFunc_, rhs.alphaTestFunc_);
        lcore::swap(alphaTestRef_, rhs.alphaTestRef_);
        lcore::swap(cullMode_, rhs.cullMode_);
        lcore::swap(alphaBlendSrc_, rhs.alphaBlendSrc_);
        lcore::swap(alphaBlendDst_, rhs.alphaBlendDst_);
    }
}

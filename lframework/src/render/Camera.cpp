/**
@file Camera.cpp
@author t-sakai
@date 2016/11/18 create
*/
#include "render/Camera.h"
#include <lgraphics/GraphicsDeviceRef.h>
#include "resource/Resources.h"
#include "resource/ShaderManager.h"

namespace lfw
{
    Camera::Camera()
        :znear_(0.1f)
        ,zfar_(1000.0f)
        ,width_(1.0f)
        ,height_(1.0f)
        ,aspect_(1.0f)
        ,fovy_(45.0f/180.0f*PI)
        ,isJitter_(0)
        ,sortLayer_(0)
        ,jitterWidth_(0.0f)
        ,jitterHeight_(0.0f)
        ,sampleIndex_(0)
        ,layerMask_(Layer_Default)
        ,clearType_(ClearType_Color)
        ,clearStencil_(0)
        ,clearDepth_(0.0f)
        ,numRenderTargets_(0)
        ,useCameraRenderTargets_(0)
        ,renderType_(RenderType_Deferred)
    {
        viewMatrix_.identity();
        invViewMatrix_.identity();
        projMatrix_.identity();
        invProjMatrix_.identity();
        viewProjMatrix_.identity();
        prevVewProjMatrix_.identity();
        eyePosition_.zero();
        clearColor_.zero();

        lcore::memset(samples_, 0, sizeof(Sample2D)*NumJitterSamples);

        viewport_ = lgfx::getDevice().getDefaultViewport();
    }

    Camera::~Camera()
    {
    }

    void Camera::getRTSize(s32& width, s32& height)
    {
        if(0 != useCameraRenderTargets_){
            lgfx::Texture2DDesc desc;
            rtTextures_[0].getDesc(desc);
            width = static_cast<s32>(desc.Width);
            height = static_cast<s32>(desc.Height);
        } else{
            const lgfx::Viewport& viewport = lgfx::getDevice().getDefaultViewport();
            width = static_cast<s32>(viewport.Width);
            height = static_cast<s32>(viewport.Height);
        }
    }

    void Camera::getRTSizeF32(f32& width, f32& height)
    {
        if(0 != useCameraRenderTargets_){
            lgfx::Texture2DDesc desc;
            rtTextures_[0].getDesc(desc);
            width = static_cast<f32>(desc.Width);
            height = static_cast<f32>(desc.Height);
        } else{
            const lgfx::Viewport& viewport = lgfx::getDevice().getDefaultViewport();
            width = static_cast<f32>(viewport.Width);
            height = static_cast<f32>(viewport.Height);
        }
    }

    const lgfx::Viewport& Camera::getViewport() const
    {
        return viewport_;
    }

    void Camera::setViewport(f32 x, f32 y, f32 width, f32 height)
    {
        viewport_.TopLeftX = x;
        viewport_.TopLeftY = y;
        viewport_.Width = width;
        viewport_.Height = height;
        viewport_.MinDepth = 0.0f;
        viewport_.MaxDepth = 1.0f;
    }

    void Camera::setRenderType(RenderType type)
    {
        renderType_ = static_cast<u8>(type);
        switch(type)
        {
        case RenderType_Forward:
            deferredDepthStencilState_.destroy();
            setClearDepth(0.0f);
            break;
        case RenderType_Deferred:
        {
            lgfx::StencilOPDesc frontDesc = {lgfx::StencilOp_Keep, lgfx::StencilOp_Keep, lgfx::StencilOp_Replace, lgfx::Cmp_Always};
            lgfx::StencilOPDesc backDesc = {lgfx::StencilOp_Keep, lgfx::StencilOp_Keep, lgfx::StencilOp_Replace, lgfx::Cmp_Always};
            deferredDepthStencilState_ = lgfx::DepthStencilState::create(
                true,
                lgfx::DepthWrite_All,
                lgfx::Cmp_Greater,
                true,
                0xFFU,
                0xFFU,
                frontDesc,
                backDesc);

            frontDesc = {lgfx::StencilOp_Keep, lgfx::StencilOp_Keep, lgfx::StencilOp_Keep, lgfx::Cmp_Less};
            backDesc = {lgfx::StencilOp_Keep, lgfx::StencilOp_Keep, lgfx::StencilOp_Keep, lgfx::Cmp_Less};
            deferredLightingDepthStencilState_ = lgfx::DepthStencilState::create(
                false,
                lgfx::DepthWrite_Zero,
                lgfx::Cmp_Always,
                true,
                0xFFU,
                0x00U,
                frontDesc,
                backDesc);

            setClearDepth(0.0f);
        }
            break;
        }
    }

    void Camera::setViewMatrix(const lmath::Matrix44& view)
    {
        viewMatrix_ = view;
        viewMatrix_.getInvert(invViewMatrix_);
        getEyePosition(eyePosition_);
    }

    void Camera::setProjMatrix(const lmath::Matrix44& proj)
    {
        projMatrix_ = proj;
        projMatrix_.getInvert(invProjMatrix_);

#if 0
        if(proj.m_[3][3] < LMATH_F32_EPSILON){
            //透視投影
            znear_ = -proj.m_[2][3]/proj.m_[2][2];
            zfar_ = (proj.m_[2][2]*znear_)/(proj.m_[2][2]-1.0f);
            width_ = 2.0f*znear_/proj.m_[0][0];
            height_ = 2.0f*znear_/proj.m_[1][1];

        }else{
            //平行投影
            znear_ = -proj.m_[2][3]/proj.m_[2][2];
            zfar_ = (proj.m_[2][2]*znear_+1.0f)/proj.m_[2][2];
            width_ = 2.0f/proj.m_[0][0];
            height_ = 2.0f/proj.m_[1][1];
        }
#else
        //ReverseZ
        if(proj.m_[3][3] < LMATH_F32_EPSILON){
            //透視投影
            zfar_ = -proj.m_[2][3]/proj.m_[2][2];
            znear_ = (proj.m_[2][2]*zfar_)/(proj.m_[2][2]+1.0f);
            width_ = 2.0f*znear_/proj.m_[0][0];
            height_ = 2.0f*znear_/proj.m_[1][1];

        }else{
            //平行投影
            zfar_ = -proj.m_[2][3]/proj.m_[2][2];
            znear_ = (proj.m_[2][2]*zfar_+1.0f)/proj.m_[2][2];
            width_ = 2.0f/proj.m_[0][0];
            height_ = 2.0f/proj.m_[1][1];
        }
#endif
        aspect_ = width_/height_;
        fovy_ = lmath::calcFOVY(height_, znear_);
    }

    // 透視投影
    void Camera::perspective(f32 width, f32 height, f32 znear, f32 zfar)
    {
        znear_ = znear;
        zfar_ = zfar;
        width_ = width;
        height_ = height;
        aspect_ = width/height;
        fovy_ = lmath::calcFOVY(height, znear);
        perspectiveReverseZ(projMatrix_, invProjMatrix_, width, height, znear, zfar);
    }

    // 透視投影
    void Camera::perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar)
    {
        znear_ = znear;
        zfar_ = zfar;
        height_ = 2.0f*znear*lmath::tan(0.5f*fovy);
        width_ = height_*aspect;
        aspect_ = aspect;
        fovy_ = fovy;
        perspectiveFovReverseZ(projMatrix_, invProjMatrix_, fovy, aspect, znear, zfar);
    }

    // 透視投影
    void Camera::perspectiveLens(f32 focalLength, f32 frameHeight, f32 aspect, f32 znear, f32 zfar)
    {
        if(frameHeight<1.0e-5f){
            frameHeight = 0.024f;
        }
        if(focalLength<1.0e-5f){
            focalLength = 0.05f;
        }

        f32 fovy = 2.0f * lmath::atan(frameHeight/(2.0f*focalLength));

        znear_ = znear;
        zfar_ = zfar;
        height_ = 2.0f*znear*lmath::tan(0.5f*fovy);
        width_ = height_*aspect;
        aspect_ = aspect;
        fovy_ = fovy;
        perspectiveFovReverseZ(projMatrix_, invProjMatrix_, fovy, aspect, znear, zfar);
    }

    // 平行投影
    void Camera::ortho(f32 width, f32 height, f32 znear, f32 zfar)
    {
        znear_ = znear;
        zfar_ = zfar;
        width_ = width;
        height_ = height;
        aspect_ = width/height;
        fovy_ = 0.0f;
        orthoReverseZ(projMatrix_, invProjMatrix_, width, height, znear, zfar);
    }

    void Camera::lookAt(const lmath::Vector4& eye, const lmath::Vector4& at, const lmath::Vector4& up)
    {
        eyePosition_ = eye;
        lmath::lookAt(viewMatrix_, invViewMatrix_, eye, at, up);
    }

    void Camera::lookAt(const lmath::Vector3& eye, const lmath::Vector3& at, const lmath::Vector3& up)
    {
        eyePosition_ = lmath::Vector4::construct(eye);
        lmath::lookAt(viewMatrix_, invViewMatrix_, eye, at, up);
    }

    void Camera::lookAt(const lmath::Vector4& eye, const lmath::Quaternion& rotation)
    {
        eyePosition_ = eye;
        lmath::lookAt(viewMatrix_, invViewMatrix_, eye, rotation);
    }

    void Camera::lookAt(const lmath::Vector3& eye, const lmath::Quaternion& rotation)
    {
        eyePosition_ = lmath::Vector4::construct(eye);
        lmath::lookAt(viewMatrix_, invViewMatrix_, eye, rotation);
    }

    void Camera::updateMatrix()
    {
        if(isJitter()){
            pushMatrix();
            viewProjMatrix_ = projMatrix_;
            sampleIndex_ = (sampleIndex_+1) & (NumJitterSamples-1);

            viewProjMatrix_.m_[0][3] += samples_[sampleIndex_].x_;
            viewProjMatrix_.m_[1][3] += samples_[sampleIndex_].y_;
        } else{
            viewProjMatrix_ = projMatrix_;
        }
        viewProjMatrix_ *= viewMatrix_;
    }

    //void Camera::beginForward(lgfx::ContextRef& context)
    //{
    //    if(0 != useCameraRenderTargets_){
    //        context.setRenderTargets(numRenderTargets_, rtView_pointers_, depthStencil_.depthStencilView_.get());

    //        switch(clearType_)
    //        {
    //        case ClearType_None:
    //            break;
    //        case ClearType_Depth:
    //            context.clearDepthStencilView(depthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
    //            break;
    //        case ClearType_Color:
    //            context.clearDepthStencilView(depthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
    //            for(s8 i=0; i<numRenderTargets_; ++i){
    //                context.clearRenderTargetView(rtViews_[i].get(), &clearColor_[0]);
    //            }
    //            break;
    //        };
    //        context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWEnable);

    //    }else{
    //        context.restoreDefaultRenderTargets();
    //        switch(clearType_)
    //        {
    //        case ClearType_None:
    //            break;
    //        case ClearType_Depth:
    //            context.clearDepthStencilView(context.getDepthStencilView(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
    //            break;
    //        case ClearType_Color:
    //            context.clearDepthStencilView(context.getDepthStencilView(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
    //            context.clearRenderTargetView(context.getBackBufferRTView(), &clearColor_[0]);
    //            break;
    //        };

    //    }
    //    context.setViewport(viewport_);
    //}

    void Camera::beginDeferred(lgfx::ContextRef& context)
    {
        context.setRenderTargets(numRenderTargets_, rtView_pointers_, depthStencil_.depthStencilView_.get());

        switch(clearType_)
        {
        case ClearType_None:
            break;
        case ClearType_Depth:
            context.clearDepthStencilView(depthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
            break;
        case ClearType_Color:
            context.clearDepthStencilView(depthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
            for(s8 i=0; i<DeferredRT_Num; ++i){
                context.clearRenderTargetView(rtViews_[i].get(), &clearColor_[0]);
            }
            break;
        };
        context.setDepthStencilState(deferredDepthStencilState_, 1);
        context.setBlendState(lgfx::ContextRef::BlendState_NoAlpha);
        context.setViewport(0.0f, 0.0f, viewport_.Width, viewport_.Height);
    }

    void Camera::endDeferred(lgfx::ContextRef& context)
    {
        context.clearRenderTargets(numRenderTargets_);
    }

    void Camera::setRenderTargets(const lgfx::Viewport& viewport, s8 numTargets, const RenderTarget* targets, const DepthStencil* depthStencil)
    {
        LASSERT(0<=numTargets && numTargets<lgfx::LGFX_MAX_RENDER_TARGETS);
        viewport_ = viewport;
        numRenderTargets_ = numTargets;
        for(s8 i=0; i<numRenderTargets_; ++i){
            rtTextures_[i] = targets[i].texture_;
            rtViews_[i] = targets[i].renderTargetView_;
            srViews_[i] = targets[i].shaderResourceView_;

            rtView_pointers_[i] = rtViews_[i];
            srView_pointers_[i] = srViews_[i];
        }
        for(s8 i=numRenderTargets_; i<lgfx::LGFX_MAX_RENDER_TARGETS; ++i){
            rtTextures_[i].destroy();
            rtViews_[i].destroy();
            srViews_[i].destroy();
            rtView_pointers_[i] = NULL;
            srView_pointers_[i] = NULL;
        }
        if(NULL != depthStencil){
            depthStencil_ = *depthStencil;
        }else{
            depthStencil_.shaderResourceViewDepth_.destroy();
            depthStencil_.shaderResourceViewStencil_.destroy();
            depthStencil_.depthStencilView_.destroy();
            depthStencil_.texture_.destroy();
        }
        useCameraRenderTargets_ = 1;
    }

    void Camera::clearRenderTargets()
    {
        numRenderTargets_ = 0;
        for(s16 i=0; i<lgfx::LGFX_MAX_RENDER_TARGETS; ++i){
            rtTextures_[i].destroy();
            rtViews_[i].destroy();
            srViews_[i].destroy();
            rtView_pointers_[i] = NULL;
            srView_pointers_[i] = NULL;
        }
        depthStencil_.shaderResourceViewDepth_.destroy();
        depthStencil_.shaderResourceViewStencil_.destroy();
        depthStencil_.depthStencilView_.destroy();
        depthStencil_.texture_.destroy();
        useCameraRenderTargets_ = 0;
    }

    void Camera::setDeferred()
    {
        ShaderManager& shaderManager = Resources::getInstance().getShaderManager();

        lgfx::RTVDesc rtvDesc;
        lgfx::SRVDesc srvDesc;

        // Shadow Accumuration
        //------------------------------------------------
        shadowAccumulatingTarget_.texture_ = lgfx::Texture::create2D(
            static_cast<s32>(viewport_.Width),
            static_cast<s32>(viewport_.Height),
            1,1,
            lgfx::Data_R8_UNorm,
            lgfx::Usage_Default,
            lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            NULL);

        rtvDesc.format_ = lgfx::Data_R8_UNorm;
        rtvDesc.dimension_ = lgfx::ViewRTVDimension_Texture2D;
        rtvDesc.tex2D_.mipSlice_ = 0;
        shadowAccumulatingTarget_.renderTargetView_ = shadowAccumulatingTarget_.texture_.createRTView(rtvDesc);

        srvDesc.format_ = lgfx::Data_R8_UNorm;
        srvDesc.dimension_ = lgfx::ViewSRVDimension_Texture2D;
        srvDesc.tex2D_.mipLevels_ = 1;
        srvDesc.tex2D_.mostDetailedMip_ = 0;
        shadowAccumulatingTarget_.shaderResourceView_ = shadowAccumulatingTarget_.texture_.createSRView(srvDesc);
        deferredShadowAccumulatingPS_ = shaderManager.getPS(ShaderPS_DeferredShadowAccumulating);

        // Deferred Lighting Target
        //------------------------------------------------
        deferredSamplerSet_.create(
            0,
            lgfx::TexFilter_MinMagMipPoint,
            lgfx::TexAddress_Clamp,
            lgfx::TexAddress_Clamp,
            lgfx::TexAddress_Clamp,
            lgfx::Cmp_Always,
            0.0f);
        deferredSamplerSet_.create(
            1,
            lgfx::TexFilter_MinMagMipLinear,
            lgfx::TexAddress_Clamp,
            lgfx::TexAddress_Clamp,
            lgfx::TexAddress_Clamp,
            lgfx::Cmp_Always,
            0.0f);
        deferredSamplerSet_.create(
            2,
            lgfx::TexFilter_CompMinMagMipLinear,
            lgfx::TexAddress_Clamp,
            lgfx::TexAddress_Clamp,
            lgfx::TexAddress_Clamp,
            lgfx::Cmp_Less,
            0.0f);

        fullQuadVS_ = shaderManager.getVS(ShaderVS_FullQuad);
        deferredLightingPS_ = shaderManager.getPS(ShaderPS_DeferredLighting);

        deferredLightingRenderTarget_.texture_ = lgfx::Texture::create2D(
            static_cast<s32>(viewport_.Width),
            static_cast<s32>(viewport_.Height),
            1,1,
            lgfx::Data_R16G16B16A16_Float,
            lgfx::Usage_Default,
            lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            NULL);

        rtvDesc.format_ = lgfx::Data_R16G16B16A16_Float;
        rtvDesc.dimension_ = lgfx::ViewRTVDimension_Texture2D;
        rtvDesc.tex2D_.mipSlice_ = 0;
        deferredLightingRenderTarget_.renderTargetView_ = deferredLightingRenderTarget_.texture_.createRTView(rtvDesc);

        srvDesc.format_ = lgfx::Data_R16G16B16A16_Float;
        srvDesc.dimension_ = lgfx::ViewSRVDimension_Texture2D;
        srvDesc.tex2D_.mipLevels_ = 1;
        srvDesc.tex2D_.mostDetailedMip_ = 0;
        deferredLightingRenderTarget_.shaderResourceView_ = deferredLightingRenderTarget_.texture_.createSRView(srvDesc);
    }

    void Camera::getEyePosition(lmath::Vector4& eye) const
    {
        const lmath::Matrix44& view = viewMatrix_;

        eye.x_ = -(view.m_[0][0] * view.m_[0][3] + view.m_[1][0] * view.m_[1][3] + view.m_[2][0] * view.m_[2][3]);
        eye.y_ = -(view.m_[0][1] * view.m_[0][3] + view.m_[1][1] * view.m_[1][3] + view.m_[2][1] * view.m_[2][3]);
        eye.z_ = -(view.m_[0][2] * view.m_[0][3] + view.m_[1][2] * view.m_[1][3] + view.m_[2][2] * view.m_[2][3]);
        eye.w_ = 0.0f;
    }

    void Camera::generateJitterSamples()
    {
        for(s32 i=0; i<NumJitterSamples; ++i){
            samples_[i].x_ = (2.0f*lcore::halton(i, JitterPrime0) - 1.0f) * jitterWidth_;
            samples_[i].y_ = (2.0f*lcore::halton(i, JitterPrime1) - 1.0f) * jitterHeight_;
        }
    }
}

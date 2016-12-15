/**
@file Camera.cpp
@author t-sakai
@date 2016/11/18 create
*/
#include "render/Camera.h"
#include <lgraphics/GraphicsDeviceRef.h>

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
        ,shadowType_(ShadowType_ShadowMap)
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
            lgfx::ContextRef& context = lgfx::getDevice();
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
            lgfx::ContextRef& context = lgfx::getDevice();
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
            break;
        case RenderType_Deferred:
        {
            lgfx::StencilOPDesc frontDesc = {lgfx::StencilOp_Keep, lgfx::StencilOp_Keep, lgfx::StencilOp_Replace, lgfx::Cmp_Always};
            lgfx::StencilOPDesc backDesc = {lgfx::StencilOp_Keep, lgfx::StencilOp_Keep, lgfx::StencilOp_Replace, lgfx::Cmp_Always};
            deferredDepthStencilState_ = lgfx::getDevice().createDepthStencilState(
                true,
                lgfx::DepthWrite_All,
                lgfx::Cmp_Greater,
                true,
                0xFFU,
                0xFFU,
                frontDesc,
                backDesc);
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
        eyePosition_ = eye;
        lmath::lookAt(viewMatrix_, invViewMatrix_, eye, at, up);
    }

    void Camera::lookAt(const lmath::Vector4& eye, const lmath::Quaternion& rotation)
    {
        eyePosition_ = eye;
        lmath::lookAt(viewMatrix_, invViewMatrix_, eye, rotation);
    }

    void Camera::lookAt(const lmath::Vector3& eye, const lmath::Quaternion& rotation)
    {
        eyePosition_ = eye;
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

    void Camera::beginForward(lgfx::ContextRef& context)
    {
        if(0 != useCameraRenderTargets_){
            ID3D11RenderTargetView* rts[lgfx::LGFX_MAX_RENDER_TARGETS];
            for(s8 i=0; i<numRenderTargets_; ++i){
                rts[i] = rtViews_[i].get();
            }
            context.setRenderTargets(numRenderTargets_, rts, depthStencil_.depthStencilView_.get());

            switch(clearType_)
            {
            case ClearType_None:
                break;
            case ClearType_Depth:
                context.clearDepthStencilView(depthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                break;
            case ClearType_Color:
                context.clearDepthStencilView(depthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                for(s8 i=0; i<numRenderTargets_; ++i){
                    context.clearRenderTargetView(rtViews_[i].get(), &clearColor_[0]);
                }
                break;
            };
            switch(renderType_)
            {
            case RenderType_Forward:
                context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWEnable);
                break;
            case RenderType_Deferred:
                context.setDepthStencilState(deferredDepthStencilState_, 1);
                break;
            };

        }else{
            context.restoreDefaultRenderTargets();
            switch(clearType_)
            {
            case ClearType_None:
                break;
            case ClearType_Depth:
                context.clearDepthStencilView(context.getDepthStencilView(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                break;
            case ClearType_Color:
                context.clearDepthStencilView(context.getDepthStencilView(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                context.clearRenderTargetView(context.getBackBufferRTView(), &clearColor_[0]);
                break;
            };

        }
        context.setViewport(viewport_);
    }

    void Camera::beginDeferred(lgfx::ContextRef& context)
    {
        if(0 != useCameraRenderTargets_){
            ID3D11RenderTargetView* rts[lgfx::LGFX_MAX_RENDER_TARGETS];
            for(s8 i=0; i<numRenderTargets_; ++i){
                rts[i] = rtViews_[i].get();
            }
            context.setRenderTargets(numRenderTargets_, rts, depthStencil_.depthStencilView_.get());

            switch(clearType_)
            {
            case ClearType_None:
                break;
            case ClearType_Depth:
                context.clearDepthStencilView(depthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                break;
            case ClearType_Color:
                context.clearDepthStencilView(depthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                for(s8 i=0; i<numRenderTargets_; ++i){
                    context.clearRenderTargetView(rtViews_[i].get(), &clearColor_[0]);
                }
                break;
            };
            switch(renderType_)
            {
            case RenderType_Forward:
                context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWEnable);
                break;
            case RenderType_Deferred:
                context.setDepthStencilState(deferredDepthStencilState_, 1);
                break;
            };

        }else{
            context.restoreDefaultRenderTargets();
            switch(clearType_)
            {
            case ClearType_None:
                break;
            case ClearType_Depth:
                context.clearDepthStencilView(context.getDepthStencilView(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                break;
            case ClearType_Color:
                context.clearDepthStencilView(context.getDepthStencilView(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                context.clearRenderTargetView(context.getBackBufferRTView(), &clearColor_[0]);
                break;
            };

        }
        context.setViewport(viewport_);
    }

    void Camera::beginDeferredLighting(lgfx::ContextRef& context)
    {
        if(0 != useCameraRenderTargets_){
            ID3D11RenderTargetView* rts[lgfx::LGFX_MAX_RENDER_TARGETS];
            for(s8 i=0; i<numRenderTargets_; ++i){
                rts[i] = rtViews_[i].get();
            }
            context.setRenderTargets(numRenderTargets_, rts, depthStencil_.depthStencilView_.get());

            switch(clearType_)
            {
            case ClearType_None:
                break;
            case ClearType_Depth:
                context.clearDepthStencilView(depthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                break;
            case ClearType_Color:
                context.clearDepthStencilView(depthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                for(s8 i=0; i<numRenderTargets_; ++i){
                    context.clearRenderTargetView(rtViews_[i].get(), &clearColor_[0]);
                }
                break;
            };
            switch(renderType_)
            {
            case RenderType_Forward:
                context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWEnable);
                break;
            case RenderType_Deferred:
                context.setDepthStencilState(deferredDepthStencilState_, 1);
                break;
            };

        }else{
            context.restoreDefaultRenderTargets();
            switch(clearType_)
            {
            case ClearType_None:
                break;
            case ClearType_Depth:
                context.clearDepthStencilView(context.getDepthStencilView(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                break;
            case ClearType_Color:
                context.clearDepthStencilView(context.getDepthStencilView(), lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, clearDepth_, clearStencil_);
                context.clearRenderTargetView(context.getBackBufferRTView(), &clearColor_[0]);
                break;
            };

        }
        context.setViewport(viewport_);
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
            uaViews_[i] = targets[i].unorderedAccessView_;
        }
        for(s8 i=numRenderTargets_; i<lgfx::LGFX_MAX_RENDER_TARGETS; ++i){
            rtTextures_[i].destroy();
            rtViews_[i].destroy();
            srViews_[i].destroy();
            uaViews_[i].destroy();
        }
        if(NULL != depthStencil){
            depthStencil_ = *depthStencil;
        }else{
            depthStencil_.unorderedAccessView_.destroy();
            depthStencil_.shaderResourceView_.destroy();
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
            uaViews_[i].destroy();
        }
        depthStencil_.unorderedAccessView_.destroy();
        depthStencil_.shaderResourceView_.destroy();
        depthStencil_.depthStencilView_.destroy();
        depthStencil_.texture_.destroy();
        useCameraRenderTargets_ = 0;
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

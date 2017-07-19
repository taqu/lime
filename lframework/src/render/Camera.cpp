/**
@file Camera.cpp
@author t-sakai
@date 2016/11/18 create
*/
#include "render/Camera.h"
#include <lgraphics/Graphics.h>
#include "resource/Resources.h"
#include "resource/ShaderManager.h"
#include "render/graph/RenderPass.h"

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
        ,layerMask_(Layer_Default|Layer_Default2D)
        ,clearType_(ClearType_Color)
        ,clearStencil_(0)
        ,clearDepth_(0.0f)
        ,renderPasses_(4)
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
        linearZParameter_ = lmath::Vector4::construct(0.0f, -1.0f, 0.0f, 0.0f);
    }

    Camera::Camera(Camera&& rhs)
        :viewMatrix_(rhs.viewMatrix_)
        ,invViewMatrix_(rhs.invViewMatrix_)
        ,projMatrix_(rhs.projMatrix_)
        ,invProjMatrix_(rhs.invProjMatrix_)
        ,viewProjMatrix_(rhs.viewProjMatrix_)
        ,prevVewProjMatrix_(rhs.prevVewProjMatrix_)
        ,eyePosition_(rhs.eyePosition_)

        ,viewport_(rhs.viewport_)
        ,znear_(rhs.znear_)
        ,zfar_(rhs.zfar_)
        ,width_(rhs.width_)
        ,height_(rhs.height_)

        ,aspect_(rhs.aspect_)
        ,fovy_(rhs.fovy_)
        ,isJitter_(rhs.isJitter_)
        ,sortLayer_(rhs.sortLayer_)

        ,jitterWidth_(rhs.jitterWidth_)
        ,jitterHeight_(rhs.jitterHeight_)
        ,sampleIndex_(rhs.sampleIndex_)
        ,layerMask_(rhs.layerMask_)
        ,clearColor_(rhs.clearColor_)
        ,clearType_(rhs.clearType_)
        ,clearStencil_(rhs.clearStencil_)
        ,clearDepth_(rhs.clearDepth_)

        ,linearZParameter_(rhs.linearZParameter_)
        ,linearZDispatchX_(rhs.linearZDispatchX_)
        ,linearZDispatchY_(rhs.linearZDispatchY_)

        ,renderPasses_(lcore::move(rhs.renderPasses_))
    {
        for(s32 i=0; i<NumJitterSamples; ++i){
            samples_[i] = rhs.samples_[i];
        }
    }

    Camera::~Camera()
    {
        clearRenderPasses();
    }

    const lgfx::Viewport& Camera::getViewport() const
    {
        return viewport_;
    }

    void Camera::setViewport(s32 x, s32 y, s32 width, s32 height)
    {
        viewport_.x_ = x;
        viewport_.y_ = y;
        viewport_.width_ = width;
        viewport_.height_ = height;
        viewport_.minDepth_ = 0.0f;
        viewport_.maxDepth_ = 1.0f;
    }

#if 0
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
            setDeferred();
        }
            break;
        }
    }
#endif

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
        linearZParameter_ = lmath::getLinearZParameterReverseZ(znear_, zfar_);
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
        linearZParameter_ = lmath::getLinearZParameterReverseZ(znear_, zfar_);
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
        linearZParameter_ = lmath::getLinearZParameterReverseZ(znear_, zfar_);
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
        linearZParameter_ = lmath::getLinearZParameterReverseZ(znear_, zfar_);
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
        linearZParameter_ = lmath::Vector4::construct(0.0f, -1.0f, 0.0f, 0.0f);
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

    void Camera::clearRenderPasses()
    {
        for(s32 i=0; i<renderPasses_.size(); ++i){
            renderPasses_[i]->destroy();
            LDELETE(renderPasses_[i]);
        }
        renderPasses_.clear();
    }

    void Camera::addRenderPass(graph::RenderPass* renderPass)
    {
        LASSERT(NULL != renderPass);
        LASSERT(findRenderPass(renderPass)<0);
        renderPasses_.push_back(renderPass);
        renderPass->create(*this);
    }

    void Camera::removeRenderPass(graph::RenderPass* renderPass)
    {
        LASSERT(NULL != renderPass);
        for(s32 i=0; i<renderPasses_.size(); ++i){
            if(renderPass == renderPasses_[i]){
                renderPasses_[i]->destroy();
                LDELETE(renderPasses_[i]);
                renderPasses_.removeAt(i);
                return;
            }
        }
    }

    s32 Camera::findRenderPass(graph::RenderPass* renderPass) const
    {
        for(s32 i=0; i<renderPasses_.size(); ++i){
            if(renderPass == renderPasses_[i]){
                return i;
            }
        }
        return -1;
    }

    Camera& Camera::operator=(Camera&& rhs)
    {
        viewMatrix_ = rhs.viewMatrix_;
        invViewMatrix_ = rhs.invViewMatrix_;
        projMatrix_ = rhs.projMatrix_;
        invProjMatrix_ = rhs.invProjMatrix_;
        viewProjMatrix_ = rhs.viewProjMatrix_;
        prevVewProjMatrix_ = rhs.prevVewProjMatrix_;
        eyePosition_ = rhs.eyePosition_;

        viewport_ = rhs.viewport_;
        znear_ = rhs.znear_;
        zfar_ = rhs.zfar_;
        width_ = rhs.width_;
        height_ = rhs.height_;

        aspect_ = rhs.aspect_;
        fovy_ = rhs.fovy_;
        isJitter_ = rhs.isJitter_;
        sortLayer_ = rhs.sortLayer_;

        jitterWidth_ = rhs.jitterWidth_;
        jitterHeight_ = rhs.jitterHeight_;
        sampleIndex_ = rhs.sampleIndex_;
        layerMask_ = rhs.layerMask_;
        clearColor_ = rhs.clearColor_;
        clearType_ = rhs.clearType_;
        clearStencil_ = rhs.clearStencil_;
        clearDepth_ = rhs.clearDepth_;

        linearZParameter_ = rhs.linearZParameter_;
        linearZDispatchX_ = rhs.linearZDispatchX_;
        linearZDispatchY_ = rhs.linearZDispatchY_;

        renderPasses_ = lcore::move(rhs.renderPasses_);

        for(s32 i=0; i<NumJitterSamples; ++i){
            samples_[i] = rhs.samples_[i];
        }
        return *this;
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

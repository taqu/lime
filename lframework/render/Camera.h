#ifndef INC_LFRAMEWORK_CAMERA_H__
#define INC_LFRAMEWORK_CAMERA_H__
/**
@file Camera.h
@author t-sakai
@date 2016/11/18 create
*/
#include <lmath/lmath.h>
#include <lmath/Matrix44.h>
#include <lgraphics/lgraphics.h>
#include <lgraphics/GraphicsDeviceRef.h>

#include "../lframework.h"
#include "RenderTarget.h"
#include "DepthStencil.h"

namespace lgfx
{
    class ContextRef;
}

namespace lfw
{
    class Camera
    {
    public:
        Camera();
        ~Camera();

        void getRTSize(s32& width, s32& height);
        void getRTSizeF32(f32& width, f32& height);

        const lgfx::Viewport& getViewport() const;
        void setViewport(f32 x, f32 y, f32 width, f32 height);

        inline bool isUseCameraRenderTarget() const;
        inline RenderType getRenderType() const;
        void setRenderType(RenderType type);
        inline ShadowType getShadowType() const;
        inline void setShadowType(ShadowType type);

        inline s32 getSortLayer() const;
        inline void setSortLayer(s32 layer);
        inline s32 getLayerMask() const;
        inline void setLayerMask(s32 mask);

        inline ClearType getClearType() const;
        inline void setClearType(ClearType type);
        inline const lmath::Vector4& getClearColor() const;
        inline void setClearColor(const lmath::Vector4& color);
        inline f32 getClearDepth() const;
        inline void setClearDepth(f32 depth);
        inline u8 getClearStencil() const;
        inline void setClearStencil(u8 stencil);

        inline f32 getWidth() const{ return width_;}
        inline f32 getHeight() const{ return height_;}
        inline f32 getZNear() const{ return znear_;}
        inline f32 getZFar() const{ return zfar_;}
        inline f32 getAspect() const{ return aspect_;}
        inline f32 getFovy() const{ return fovy_;}

        inline const lmath::Matrix44& getViewMatrix() const;
        inline lmath::Matrix44& getViewMatrix();
        inline const lmath::Matrix44& getInvViewMatrix() const;
        inline lmath::Matrix44& getInvViewMatrix();

        void setViewMatrix(const lmath::Matrix44& view);

        inline const lmath::Matrix44& getProjMatrix() const;
        inline lmath::Matrix44& getProjMatrix();
        inline const lmath::Matrix44& getInvProjMatrix() const;

        void setProjMatrix(const lmath::Matrix44& proj);

        inline const lmath::Matrix44& getViewProjMatrix() const;

        /**
        @brief 透視投影
        */
        void perspective(f32 width, f32 height, f32 znear, f32 zfar);

        /**
        @brief 透視投影
        */
        void perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar);

        /**
        @brief 透視投影
        */
        void perspectiveLens(f32 focalLength, f32 frameHeight, f32 aspect, f32 znear, f32 zfar);

        /**
        @brief 平行投影
        */
        void ortho(f32 width, f32 height, f32 znear, f32 zfar);

        void lookAt(const lmath::Vector4& eye, const lmath::Vector4& at, const lmath::Vector4& up);
        void lookAt(const lmath::Vector3& eye, const lmath::Vector3& at, const lmath::Vector3& up);

        void lookAt(const lmath::Vector4& eye, const lmath::Quaternion& rotation);
        void lookAt(const lmath::Vector3& eye, const lmath::Quaternion& rotation);

        void updateMatrix();
        void beginForward(lgfx::ContextRef& context);
        void beginDeferred(lgfx::ContextRef& context);
        void beginDeferredLighting(lgfx::ContextRef& context);

        const lmath::Matrix44& getPrevViewProjMatrix() const
        {
            return prevVewProjMatrix_;
        }

        const lmath::Vector4& getEyePosition() const
        {
            return eyePosition_;
        }

        lmath::Vector4& getEyePosition()
        {
            return eyePosition_;
        }

        void pushMatrix()
        {
            prevVewProjMatrix_ = viewProjMatrix_;
        }

        inline void setJitter(bool enable);
        inline bool isJitter() const;

        inline void setJitterSize(f32 width, f32 height);

        inline s8 getNumRenderTargets() const;
        void setRenderTargets(const lgfx::Viewport& viewport, s8 numTargets, const RenderTarget* targets, const DepthStencil* depthStencil);
        void clearRenderTargets();

        inline const lgfx::Texture2DRef& getRTTexture(s32 index) const;
        inline const lgfx::RenderTargetViewRef& getRTView(s32 index) const;
        inline const lgfx::ShaderResourceViewRef& getSRView(s32 index) const;
        inline const lgfx::UnorderedAccessViewRef& getUAView(s32 index) const;
        inline const DepthStencil& getDepthStencil() const;
    private:
        static const s32 JitterPrime0 = 2;
        static const s32 JitterPrime1 = 3;
        static const s32 NumJitterSamples = 64;

        struct Sample2D
        {
            f32 x_;
            f32 y_;
        };

        void getEyePosition(lmath::Vector4& eye) const;
        void generateJitterSamples();

        lmath::Matrix44 viewMatrix_;
        lmath::Matrix44 invViewMatrix_;
        lmath::Matrix44 projMatrix_;
        lmath::Matrix44 invProjMatrix_;
        lmath::Matrix44 viewProjMatrix_;
        lmath::Matrix44 prevVewProjMatrix_;
        lmath::Vector4 eyePosition_;

        lgfx::Viewport viewport_;

        f32 znear_;
        f32 zfar_;
        f32 width_;
        f32 height_;

        f32 aspect_;
        f32 fovy_;
        s32 isJitter_;
        s32 sortLayer_;

        f32 jitterWidth_;
        f32 jitterHeight_;
        s32 sampleIndex_;
        s32 layerMask_;

        Sample2D samples_[NumJitterSamples];

        lmath::Vector4 clearColor_;
        ClearType clearType_;
        u8 clearStencil_;
        f32 clearDepth_;
        s8 numRenderTargets_;
        u8 useCameraRenderTargets_;
        u8 renderType_;
        u8 shadowType_;
        lgfx::DepthStencilStateRef deferredDepthStencilState_;
        lgfx::Texture2DRef rtTextures_[lgfx::LGFX_MAX_RENDER_TARGETS];
        lgfx::RenderTargetViewRef rtViews_[lgfx::LGFX_MAX_RENDER_TARGETS];
        lgfx::ShaderResourceViewRef srViews_[lgfx::LGFX_MAX_RENDER_TARGETS];
        lgfx::UnorderedAccessViewRef uaViews_[lgfx::LGFX_MAX_RENDER_TARGETS];
        DepthStencil depthStencil_;
    };

    inline bool Camera::isUseCameraRenderTarget() const
    {
        return 0 != useCameraRenderTargets_;
    }

    inline RenderType Camera::getRenderType() const
    {
        return static_cast<RenderType>(renderType_);
    }

    inline ShadowType Camera::getShadowType() const
    {
        return static_cast<ShadowType>(shadowType_);
    }

    inline void Camera::setShadowType(ShadowType type)
    {
        shadowType_ = static_cast<u8>(type);
    }

    inline s32 Camera::getSortLayer() const
    {
        return sortLayer_;
    }

    inline void Camera::setSortLayer(s32 layer)
    {
        sortLayer_ = layer;
    }

    inline s32 Camera::getLayerMask() const
    {
        return layerMask_;
    }

    inline void Camera::setLayerMask(s32 mask)
    {
        layerMask_ = mask;
    }

    inline ClearType Camera::getClearType() const
    {
        return clearType_;
    }

    inline void Camera::setClearType(ClearType type)
    {
        clearType_ = type;
    }

    inline const lmath::Vector4& Camera::getClearColor() const
    {
        return clearColor_;
    }

    inline void Camera::setClearColor(const lmath::Vector4& color)
    {
        clearColor_ = color;
    }

    inline f32 Camera::getClearDepth() const
    {
        return clearDepth_;
    }

    inline void Camera::setClearDepth(f32 depth)
    {
        clearDepth_ = depth;
    }

    inline u8 Camera::getClearStencil() const
    {
        return clearStencil_;
    }

    inline void Camera::setClearStencil(u8 stencil)
    {
        clearStencil_ = stencil;
    }
    
    inline const lmath::Matrix44& Camera::getViewMatrix() const
    {
        return viewMatrix_;
    }

    inline lmath::Matrix44& Camera::getViewMatrix()
    {
        return viewMatrix_;
    }

    inline const lmath::Matrix44& Camera::getInvViewMatrix() const
    {
        return invViewMatrix_;
    }

    inline lmath::Matrix44& Camera::getInvViewMatrix()
    {
        return invViewMatrix_;
    }

    inline const lmath::Matrix44& Camera::getProjMatrix() const
    {
        return projMatrix_;
    }

    inline lmath::Matrix44& Camera::getProjMatrix()
    {
        return projMatrix_;
    }

    inline const lmath::Matrix44& Camera::getInvProjMatrix() const
    {
        return invProjMatrix_;
    }

    inline const lmath::Matrix44& Camera::getViewProjMatrix() const
    {
        return viewProjMatrix_;
    }

    inline void Camera::setJitter(bool enable)
    {
        isJitter_ = (enable)? 1 : 0;
    }

    inline bool Camera::isJitter() const
    {
        return 0 != isJitter_;
    }

    inline void Camera::setJitterSize(f32 width, f32 height)
    {
        jitterWidth_ = width;
        jitterHeight_ = height;
        generateJitterSamples();
    }

    inline s8 Camera::getNumRenderTargets() const
    {
        return numRenderTargets_;
    }

    inline const lgfx::Texture2DRef& Camera::getRTTexture(s32 index) const
    {
        LASSERT(0<=index && index<numRenderTargets_);
        return rtTextures_[index];
    }

    inline const lgfx::RenderTargetViewRef& Camera::getRTView(s32 index) const
    {
        LASSERT(0<=index && index<numRenderTargets_);
        return rtViews_[index];
    }

    inline const lgfx::ShaderResourceViewRef& Camera::getSRView(s32 index) const
    {
        LASSERT(0<=index && index<numRenderTargets_);
        return srViews_[index];
    }

    inline const lgfx::UnorderedAccessViewRef& Camera::getUAView(s32 index) const
    {
        LASSERT(0<=index && index<numRenderTargets_);
        return uaViews_[index];
    }

    inline const DepthStencil& Camera::getDepthStencil() const
    {
        return depthStencil_;
    }

    inline bool lessCamera(const Camera& c0, const Camera& c1)
    {
        return c0.getSortLayer()<c1.getSortLayer();
    }
}

#endif //INC_LFRAMEWORK_CAMERA_H__

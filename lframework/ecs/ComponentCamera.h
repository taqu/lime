#ifndef INC_LFRAMEWORK_COMPONENTCAMERA_H__
#define INC_LFRAMEWORK_COMPONENTCAMERA_H__
/**
@file ComponentCamera.h
@author t-sakai
@date 2016/11/29 create
*/
#include "Component.h"
#include "ComponentSceneElementManager.h"

namespace lmath
{
    class Vector3;
    class Vector4;
    class Matrix44;
}

namespace lgfx
{
    struct Viewport;
    class Texture2DRef;
    class RenderTargetViewRef;
    class ShaderResourceViewRef;
    class UnorderedAccessViewRef;
}

namespace lfw
{
    class Entity;
    class RenderContext;
    class Camera;
    struct RenderTarget;
    struct DepthStencil;
    struct DeferredResource;
    struct RenderTask;

    class ComponentCamera : public Behavior
    {
    public:
        static u8 category(){ return ECSCategory_SceneElement;}
        static u32 type(){ return ECSType_Camera;}

        ComponentCamera();
        virtual ~ComponentCamera();

        Entity& getEntity();
        const Entity& getEntity() const;
        virtual u32 getType() const;

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();

        void initialize(
            f32 fovyDegree,
            f32 znear,
            f32 zfar,
            s32 width,
            s32 height,
            bool jitter);

        virtual void resetRenderPasses();
        virtual void render(RenderContext& renderContext);

        const lgfx::Viewport& getViewport() const;
        void setViewport(s32 x, s32 y, s32 width, s32 height);

        s32 getSortLayer() const;
        void setSortLayer(s32 layer);
        s32 getTargetLayerMask() const;
        void setTargetLayerMask(s32 layerMask);

        f32 getZNear() const;
        f32 getZFar() const;
        f32 getAspect() const;
        f32 getFovy() const;

        const lmath::Matrix44& getProjMatrix() const;

        /**
        @brief “§Ž‹“Š‰e
        */
        void perspective(f32 width, f32 height, f32 znear, f32 zfar);

        /**
        @brief “§Ž‹“Š‰e
        */
        void perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar);

        /**
        @brief “§Ž‹“Š‰e
        */
        void perspectiveLens(f32 focalLength, f32 frameHeight, f32 aspect, f32 znear, f32 zfar);

        /**
        @brief •½s“Š‰e
        */
        void ortho(f32 width, f32 height, f32 znear, f32 zfar);

        void lookAt(const lmath::Vector3& eye, const lmath::Vector3& at);
        void lookAt(const lmath::Vector4& eye, const lmath::Vector4& at);

        void setJitter(bool enable);
        bool isJitter() const;

        void setJitterSize(f32 width, f32 height);
    protected:
        ComponentCamera(const ComponentCamera&) = delete;
        ComponentCamera& operator=(const ComponentCamera&) = delete;
    };
}
#endif //INC_LFRAMEWORK_COMPONENTCAMERA_H__

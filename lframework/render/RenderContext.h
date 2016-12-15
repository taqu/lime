#ifndef INC_LFRAMEWORK_RENDERCONTEXT_H__
#define INC_LFRAMEWORK_RENDERCONTEXT_H__
/**
@file RenderContext.h
@author t-sakai
@date 2016/11/15 create
*/
#include "../lframework.h"
#include <lcore/Array.h>
#include "Frustum.h"

namespace lmath
{
    class Matrix34;
    class Matrix44;
}

namespace lgfx
{
    class ContextRef;
    class ConstantBufferRef;
    class BlendStateRef;
    class VertexShaderRef;
    class GeometryShaderRef;

    class ConstantBufferTableSet;
}

namespace lfw
{
    struct SceneConstantPS;
    struct LightClusterConstantPS;

    class Camera;
    class Light;

    class ShadowMap;

    class ComponentRenderer;

    class RenderContext
    {
    public:
        enum Shader
        {
            Shader_VS =0,
            Shader_HS,
            Shader_DS,
            Shader_GS,
            Shader_PS,
            Shader_CS,
            Shader_Num,
        };

        enum DepthShader
        {
            DepthShader_Normal =0,
            DepthShader_Skinning,
            DepthShader_Num,
        };

        RenderContext();
        ~RenderContext();

        void initialize(lgfx::ContextRef* context);
        void terminate();

        void setPerFrameConstants();
        void setPerCameraConstants(Camera& camera);
        void setPerShadowMapConstants();

        inline s32 getRenderPath() const;
        void setRenderPath(s32 path);

        inline const Camera& getCamera() const;
        inline void setCamera(const Camera* camera);
        inline const ShadowMap& getShadowMap();
        inline void setShadowMap(const ShadowMap* shadowMap);
        inline lgfx::ContextRef& getContext();
        inline void setContext(lgfx::ContextRef* context);

        inline lgfx::ConstantBufferTableSet& getConsantBuffer();
        inline void setConstantBuffer(lgfx::ConstantBufferTableSet* constantBufferTableSet);

        //void setSceneConstantVS(const Scene& scene, const ShadowMap& shadowMap);
        //void setSceneConstantDS(const Scene& scene);
        //void setSceneConstantPS(const SceneConstantPS& src, const Scene& scene, const LightClusterConstantPS& lightCluster);

        inline lgfx::ConstantBufferRef* getSceneConstantVS();
        inline lgfx::ConstantBufferRef* getSceneConstantPS();

        lgfx::ConstantBufferRef* createConstantBuffer(u32 size);
        lgfx::ConstantBufferRef* createConstantBuffer(u32 size, const void* data);

        bool setConstant(Shader shader, s32 index, u32 size, const void* data);
        bool setConstantBuffer(Shader shader, s32 index, lgfx::ConstantBufferRef* constant);
        bool setConstantAligned16(Shader shader, s32 index, u32 size, const void* data);
        bool setConstantMatricesAligned16(Shader shader, s32 index, s32 num, const lmath::Matrix34* matrices);

        inline const Frustum& getWorldFrustum() const;
        void updateWorldFrustum(const Camera& camera);

        void attachDepthShader(DepthShader type);
        inline void setDepthShaderVS(DepthShader type, lgfx::VertexShaderRef& vs);
        inline void setDepshShaderGS(lgfx::GeometryShaderRef& gs);
    private:
        RenderContext(const RenderContext&);
        RenderContext& operator=(const RenderContext&);

        s32 renderPath_;
        const Camera* camera_;
        const ShadowMap* shadowMap_;
        lgfx::ContextRef* context_;
        lgfx::ConstantBufferTableSet* constantBufferTableSet_;

        //lgfx::ConstantBufferRef* sceneConstantBufferVS_;
        //lgfx::ConstantBufferRef* sceneConstantBufferDS_;
        //lgfx::ConstantBufferRef* sceneConstantBufferPS_;
        //lgfx::ConstantBufferRef* lightClusterConstantBufferPS_;

        Frustum cameraFrustum_;
        Frustum worldFrustum_;
        lgfx::VertexShaderRef* depthShaderVS_[DepthShader_Num];
        lgfx::GeometryShaderRef* depthShaderGS_;
    };

    inline s32 RenderContext::getRenderPath() const
    {
        return renderPath_;
    }

    inline const Camera& RenderContext::getCamera() const
    {
        return *camera_;
    }

    inline void RenderContext::setCamera(const Camera* camera)
    {
        camera_ = camera;
    }

    inline const ShadowMap& RenderContext::getShadowMap()
    {
        return *shadowMap_;
    }

    inline void RenderContext::setShadowMap(const ShadowMap* shadowMap)
    {
        shadowMap_ = shadowMap;
    }

    inline lgfx::ContextRef& RenderContext::getContext()
    {
        return *context_;
    }

    inline void RenderContext::setContext(lgfx::ContextRef* context)
    {
        context_ = context;
    }

    inline lgfx::ConstantBufferTableSet& RenderContext::getConsantBuffer()
    {
        return *constantBufferTableSet_;
    }

    inline void RenderContext::setConstantBuffer(lgfx::ConstantBufferTableSet* constantBuffer)
    {
        constantBufferTableSet_ = constantBuffer;
    }

    //inline lgfx::ConstantBufferRef* RenderContext::getSceneConstantVS()
    //{
    //    return sceneConstantBufferVS_;
    //}

    //inline lgfx::ConstantBufferRef* RenderContext::getSceneConstantPS()
    //{
    //    return sceneConstantBufferPS_;
    //}


    inline const Frustum& RenderContext::getWorldFrustum() const
    {
        return worldFrustum_;
    }

    inline void RenderContext::setDepthShaderVS(DepthShader type, lgfx::VertexShaderRef& vs)
    {
        depthShaderVS_[type] = &vs;
    }

    inline void RenderContext::setDepshShaderGS(lgfx::GeometryShaderRef& gs)
    {
        depthShaderGS_ = &gs;
    }
}
#endif //INC_LFRAMEWORK_RENDERCONTEXT_H__

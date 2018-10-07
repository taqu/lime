#ifndef INC_LFRAMEWORK_RENDERCONTEXT_H_
#define INC_LFRAMEWORK_RENDERCONTEXT_H_
/**
@file RenderContext.h
@author t-sakai
@date 2016/11/15 create
*/
#include "../lframework.h"
#include <lcore/Array.h>
#include "SamplerSet.h"
#include "../render/ShaderConstant.h"
#include "../render/ShadowMap.h"
#include "../ecs/ComponentSceneElementManager.h"

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

    class Geometry;

    class ComponentRenderer;
    class RenderQueue;

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
        void setPerLightConstants(Light& light);
        void setPerShadowMapConstants();

        inline s32 getRenderPath() const;
        void beginRenderPath(s32 path);
        void endRenderPath(s32 path);

        inline RenderQueue& getRenderQueue();
        inline void setRenderQueue(RenderQueue* renderQueue);

        //void beginDeferredLighting(const Light& light);
        //void endDeferredLighting(const Light& light);

        inline const LightArray& getLights() const;
        inline LightArray& getLights();
        inline void setLights(LightArray&& lights);

        inline lgfx::ContextRef& getContext();
        inline void setContext(lgfx::ContextRef* context);

        inline lgfx::ConstantBufferTableSet& getConsantBuffer();
        inline void setConstantBuffer(lgfx::ConstantBufferTableSet* constantBufferTableSet);

        lgfx::ConstantBufferRef* createConstantBuffer(u32 size);
        lgfx::ConstantBufferRef* createConstantBuffer(u32 size, const void* data);

        bool setConstant(Shader shader, s32 index, u32 size, const void* data);
        bool setConstant(Shader shader, s32 index, u32 size, s32 num, s32 maxNum, const void* data);
        bool setConstantBuffer(Shader shader, s32 index, lgfx::ConstantBufferRef* constant);
        bool setConstantAligned16(Shader shader, s32 index, u32 size, const void* data);
        bool setConstantAligned16(Shader shader, s32 index, u32 size, s32 num, s32 maxNum, const void* data);
        bool setConstantMatricesAligned16(Shader shader, s32 index, s32 num, s32 maxNum, const lmath::Matrix34* matrices);

        void attachDepthShader(DepthShader type);
        inline void setDepthShaderVS(DepthShader type, lgfx::VertexShaderRef& vs);
        inline void setDepshShaderGS(lgfx::GeometryShaderRef& gs);
        void setDefaultSampler(Shader shader);
        void clearDefaultSampler(Shader shader);

        inline ShadowMap& getShadowMap();
        inline Geometry& getLightSphere();
    private:
        RenderContext(const RenderContext&) = delete;
        RenderContext& operator=(const RenderContext&) = delete;

        s32 renderPath_;
        LightArray lights_;
        ShadowMap shadowMap_;

        Geometry* geomLightSphere_;

        lgfx::ContextRef* context_;
        lgfx::ConstantBufferTableSet* constantBufferTableSet_;

        lgfx::VertexShaderRef* depthShaderVS_[DepthShader_Num];
        lgfx::GeometryShaderRef* depthShaderGS_;
        SamplerSet<3> samplerSet_;
        RenderQueue* renderQueue_;
    };

    inline s32 RenderContext::getRenderPath() const
    {
        return renderPath_;
    }

    inline RenderQueue& RenderContext::getRenderQueue()
    {
        return *renderQueue_;
    }

    inline void RenderContext::setRenderQueue(RenderQueue* renderQueue)
    {
        renderQueue_ = renderQueue;
    }

    inline const LightArray& RenderContext::getLights() const
    {
        return lights_;
    }

    inline LightArray& RenderContext::getLights()
    {
        return lights_;
    }

    inline void RenderContext::setLights(LightArray&& lights)
    {
        lights_ = lights;
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

    inline void RenderContext::setDepthShaderVS(DepthShader type, lgfx::VertexShaderRef& vs)
    {
        depthShaderVS_[type] = &vs;
    }

    inline void RenderContext::setDepshShaderGS(lgfx::GeometryShaderRef& gs)
    {
        depthShaderGS_ = &gs;
    }

    inline ShadowMap& RenderContext::getShadowMap()
    {
        return shadowMap_;
    }

    inline Geometry& RenderContext::getLightSphere()
    {
        return *geomLightSphere_;
    }
}
#endif //INC_LFRAMEWORK_RENDERCONTEXT_H_

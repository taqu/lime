#ifndef INC_LFRAMEWORK_RENDERCONTEXT_H__
#define INC_LFRAMEWORK_RENDERCONTEXT_H__
/**
@file RenderContext.h
@author t-sakai
@date 2016/11/15 create
*/
#include "../lframework.h"
#include <lcore/Array.h>
#include "SamplerSet.h"
#include "DepthStencil.h"

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

        void beginDeferredLighting(const Light& light);
        void endDeferredLighting(const Light& light);

        inline const Camera& getCamera() const;
        inline Camera& getCamera();
        inline void setCamera(Camera* camera);
        inline const ShadowMap& getShadowMap();
        inline void setShadowMap(const ShadowMap* shadowMap);
        inline DepthStencil& getShadowMapDepthStencil();

        inline lgfx::ContextRef& getContext();
        inline void setContext(lgfx::ContextRef* context);

        inline lgfx::ConstantBufferTableSet& getConsantBuffer();
        inline void setConstantBuffer(lgfx::ConstantBufferTableSet* constantBufferTableSet);

        lgfx::ConstantBufferRef* createConstantBuffer(u32 size);
        lgfx::ConstantBufferRef* createConstantBuffer(u32 size, const void* data);

        bool setConstant(Shader shader, s32 index, u32 size, const void* data);
        bool setConstantBuffer(Shader shader, s32 index, lgfx::ConstantBufferRef* constant);
        bool setConstantAligned16(Shader shader, s32 index, u32 size, const void* data);
        bool setConstantMatricesAligned16(Shader shader, s32 index, s32 num, const lmath::Matrix34* matrices);

        void attachDepthShader(DepthShader type);
        inline void setDepthShaderVS(DepthShader type, lgfx::VertexShaderRef& vs);
        inline void setDepshShaderGS(lgfx::GeometryShaderRef& gs);
        void resetDefaultSamplerSet();
    private:
        RenderContext(const RenderContext&);
        RenderContext& operator=(const RenderContext&);

        s32 renderPath_;
        Camera* camera_;
        const ShadowMap* shadowMap_;
        DepthStencil shadowMapDepthStencil_;

        lgfx::ContextRef* context_;
        lgfx::ConstantBufferTableSet* constantBufferTableSet_;

        lgfx::VertexShaderRef* depthShaderVS_[DepthShader_Num];
        lgfx::GeometryShaderRef* depthShaderGS_;
        SamplerSet<3> samplerSet_;
    };

    inline s32 RenderContext::getRenderPath() const
    {
        return renderPath_;
    }

    inline const Camera& RenderContext::getCamera() const
    {
        return *camera_;
    }

    inline Camera& RenderContext::getCamera()
    {
        return *camera_;
    }

    inline void RenderContext::setCamera(Camera* camera)
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

    inline DepthStencil& RenderContext::getShadowMapDepthStencil()
    {
        return shadowMapDepthStencil_;
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
}
#endif //INC_LFRAMEWORK_RENDERCONTEXT_H__

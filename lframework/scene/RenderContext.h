#ifndef INC_LSCENE_RENDERCONTEXT_H__
#define INC_LSCENE_RENDERCONTEXT_H__
/**
@file RenderContext.h
@author t-sakai
@date 2014/10/30 create
*/
#include "lscene.h"
#include "Frustum.h"
#include "RenderQueue.h"

namespace lmath
{
    class Matrix34;
    class Matrix44;
}

namespace lgraphics
{
    class ContextRef;
    class ConstantBufferRef;
    class BlendStateRef;
    class VertexShaderRef;
}

namespace lscene
{
    struct SceneConstantPS;

    class Scene;
    class ShadowMap;
    class ConstantBuffer;

    struct LightClusterConstantPS;

    class RenderContext
    {
    public:
        static const s32 MaxPathConstants = 4;

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

        void initialize(lgraphics::ContextRef* context, s32 numPaths);
        void terminate();

        inline s32 getPath() const;
        inline void setPath(s32 path);
        inline Scene& getScene();
        inline const Scene& getScene() const;
        inline void setScene(Scene* scene);
        inline const ShadowMap& getShadowMap();
        inline void setShadowMap(const ShadowMap* shadowMap);
        inline lgraphics::ContextRef& getContext();
        inline void setContext(lgraphics::ContextRef* context);

        inline ConstantBuffer& getConsantBuffer();
        inline void setConstantBuffer(ConstantBuffer* constantBuffer);

        void setSceneConstantVS(const Scene& scene, const ShadowMap& shadowMap);
        void setSceneConstantDS(const Scene& scene);
        void setSceneConstantPS(const SceneConstantPS& src, const Scene& scene, const LightClusterConstantPS& lightCluster);

        inline lgraphics::ConstantBufferRef* getSceneConstantVS();
        inline lgraphics::ConstantBufferRef* getSceneConstantPS();

        lgraphics::ConstantBufferRef* createConstantBuffer(u32 size);
        lgraphics::ConstantBufferRef* createConstantBuffer(u32 size, const void* data);
        bool setSystemConstantBuffer(Shader shader, s32 index, lgraphics::ConstantBufferRef* constant);
        bool setSystemConstant(Shader shader, s32 index, u32 size, const void* data);
        bool setSystemConstantAligned16(Shader shader, s32 index, u32 size, const void* data);

        bool setConstant(Shader shader, s32 userIndex, u32 size, const void* data);
        bool setConstantBuffer(Shader shader, s32 userIndex, lgraphics::ConstantBufferRef* constant);
        bool setConstantAligned16(Shader shader, s32 userIndex, u32 size, const void* data);
        bool setConstantMatricesAligned16(Shader shader, s32 userIndex, s32 num, const lmath::Matrix34* matrices);

        inline s32 getNumPathConstants(Shader shader) const;
        inline void setNumPathConstants(Shader shader, s32 numConstants);
        inline void setNumPathConstants(const s32* numConstants);
        void setDefaultNumPathConstants(s32 path);

        void setBlendState(s32 blendState);
        void setBlendState(lgraphics::BlendStateRef& blendState);

        inline RenderQueue& getRenderQueue();
        inline const RenderQueue& getRenderQueue() const;
        inline void clearRenderQueue();

        inline const Frustum& getWorldFrustum() const;
        void updateWorldFrustum();

        void attachDepthShader(DepthShader type);
        inline void setDepthShaderVS(DepthShader type, lgraphics::VertexShaderRef* vs);
    private:
        RenderContext(const RenderContext&);
        RenderContext& operator=(const RenderContext&);

        s32 path_;
        Scene* scene_;
        const ShadowMap* shadowMap_;
        lgraphics::ContextRef* context_;
        ConstantBuffer* constantBuffer_;

        lgraphics::ConstantBufferRef* sceneConstantBufferVS_;
        lgraphics::ConstantBufferRef* sceneConstantBufferDS_;
        lgraphics::ConstantBufferRef* sceneConstantBufferPS_;
        lgraphics::ConstantBufferRef* lightClusterConstantBufferPS_;

        s32 numPathConstants_[Shader_Num];
        s32 blendState_;

        RenderQueue renderQueue_;
        Frustum worldFrustum_;
        lgraphics::VertexShaderRef* depthShaderVS_[DepthShader_Num];
    };

    inline s32 RenderContext::getPath() const
    {
        return path_;
    }

    inline void RenderContext::setPath(s32 path)
    {
        path_ = path;
    }

    inline Scene& RenderContext::getScene()
    {
        return *scene_;
    }

    inline const Scene& RenderContext::getScene() const
    {
        return *scene_;
    }

    inline void RenderContext::setScene(Scene* scene)
    {
        scene_ = scene;
    }
    
    inline const ShadowMap& RenderContext::getShadowMap()
    {
        return *shadowMap_;
    }

    inline void RenderContext::setShadowMap(const ShadowMap* shadowMap)
    {
        shadowMap_ = shadowMap;
    }

    inline lgraphics::ContextRef& RenderContext::getContext()
    {
        return *context_;
    }

    inline void RenderContext::setContext(lgraphics::ContextRef* context)
    {
        context_ = context;
    }

    inline ConstantBuffer& RenderContext::getConsantBuffer()
    {
        return *constantBuffer_;
    }

    inline void RenderContext::setConstantBuffer(ConstantBuffer* constantBuffer)
    {
        constantBuffer_ = constantBuffer;
    }

    inline lgraphics::ConstantBufferRef* RenderContext::getSceneConstantVS()
    {
        return sceneConstantBufferVS_;
    }

    inline lgraphics::ConstantBufferRef* RenderContext::getSceneConstantPS()
    {
        return sceneConstantBufferPS_;
    }

    inline s32 RenderContext::getNumPathConstants(Shader shader) const
    {
        return numPathConstants_[shader];
    }

    inline void RenderContext::setNumPathConstants(Shader shader, s32 numConstants)
    {
        LASSERT(0<=numConstants && numConstants<=MaxPathConstants);
        numPathConstants_[shader] = numConstants;
    }

    inline void RenderContext::setNumPathConstants(const s32* numConstants)
    {
        for(s32 i=0; i<Shader_Num; ++i){
            numPathConstants_[i] = numConstants[i];
        }
    }

    inline RenderQueue& RenderContext::getRenderQueue()
    {
        return renderQueue_;
    }

    inline const RenderQueue& RenderContext::getRenderQueue() const
    {
        return renderQueue_;
    }

    inline void RenderContext::clearRenderQueue()
    {
        renderQueue_.clear();
    }

    inline const Frustum& RenderContext::getWorldFrustum() const
    {
        return worldFrustum_;
    }

    inline void RenderContext::setDepthShaderVS(DepthShader type, lgraphics::VertexShaderRef* vs)
    {
        depthShaderVS_[type] = vs;
    }
}
#endif //INC_LSCENE_RENDERCONTEXT_H__

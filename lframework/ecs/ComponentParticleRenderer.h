#ifndef INC_LFRAMEWORK_COMPONENTPARTICLERENDERER_H__
#define INC_LFRAMEWORK_COMPONENTPARTICLERENDERER_H__
/**
@file ComponentParticleRenderer.h
@author t-sakai
@date 2016/12/22 create
*/
#include "ComponentRenderer.h"
#include <lmath/Vector3.h>
#include <lmath/Vector4.h>
#include <lgraphics/DynamicBuffer.h>
#include <lgraphics/InputLayoutRef.h>
#include <lgraphics/BlendStateRef.h>
#include <lgraphics/SamplerStateRef.h>
#include <lgraphics/ShaderRef.h>
#include "../Application.h"
#include "PairArray.h"
#include "ComponentRendererManager.h"
#include "ComponentGeometric.h"
#include "../render/Renderer.h"
#include "../render/RenderQueue.h"
#include "../render/RenderContext.h"
#include "../render/Camera.h"

namespace lfw
{
    struct ParticleVertex
    {
        lmath::Vector3 position_;
        u16 texcoord_[4]; //f16
        u32 abgr_; //ABGR8888
        u16 size_[2]; //f16
        s16 rotation_[2]; //normalized s16 (sin, cos)
    };

    //----------------------------------------------------------------------------------------------
    //---
    //---
    //---
    //----------------------------------------------------------------------------------------------
    template<class U>
    class ComponentParticleRenderer : public ComponentRenderer
    {
    public:
        static const s32 DefaultMaxParticles = 64;
        static const s32 MaxFrames = 4;
        typedef lgfx::DynamicBuffer<lgfx::VertexBufferRef, lgfx::VertexBufferCreator> DynamicVertexBuffer;

        typedef PairArray<ParticleVertex, U> VertexArray;

        ComponentParticleRenderer();
        virtual ~ComponentParticleRenderer();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void postUpdate();
        virtual void onDestroy();
        virtual void addQueue(RenderQueue& queue);
        virtual void drawDepth(RenderContext& renderContext);
        virtual void drawOpaque(RenderContext& renderContext);
        virtual void drawTransparent(RenderContext& renderContext);

        void setTexture(lgfx::Texture2DRef& texture, lgfx::ShaderResourceViewRef& srv);
        inline s32 capacity() const;
        inline s32 size() const;
        inline void clear();

        inline void add(const ParticleVertex& vertex0, const U& vertex1);
        inline void removeAt(s32 index);

        inline const ParticleVertex& getFirst(s32 index) const;
        inline ParticleVertex& getFirst(s32 index);
        inline const U& getSecond(s32 index) const;
        inline U& getSecond(s32 index);

        void resize(s32 capacity);

        inline void setSoftParticleDepthDelta(f32 delta);

    protected:
        ComponentParticleRenderer(const ComponentParticleRenderer&);
        ComponentParticleRenderer& operator=(const ComponentParticleRenderer&);

        inline ComponentRendererManager* getManager()
        {
            return ECSManager::getInstance().getComponentManager<ComponentRendererManager>();
        }

        VertexArray vertices_;
        DynamicVertexBuffer vertexBuffer_;
        f32 particleConstant_[4];

        lgfx::InputLayoutRef inputLayout_;
        lgfx::BlendStateRef blendState_;
        lgfx::SamplerStateRef samLinear_;
        lgfx::VertexShaderRef vs_;
        lgfx::GeometryShaderRef gs_;
        lgfx::PixelShaderRef ps_;
        lgfx::Texture2DRef texture_;
        lgfx::ShaderResourceViewRef srv_;
    };

    template<class U>
    ComponentParticleRenderer<U>::ComponentParticleRenderer()
    {
        particleConstant_[0] = 1.0e-3f;
        particleConstant_[1] = particleConstant_[2] = particleConstant_[3] = 0.0f;
    }

    template<class U>
    ComponentParticleRenderer<U>::~ComponentParticleRenderer()
    {
        vertexBuffer_.terminate();
    }

    template<class U>
    void ComponentParticleRenderer<U>::onCreate()
    {
        Resources& resources = Resources::getInstance();
        inputLayout_ = resources.getInputLayoutFactory().get(InputLayout_Particle);
        blendState_ = lgfx::BlendState::create(
            FALSE,
            TRUE,
            lgfx::Blend_SrcAlpha,
            lgfx::Blend_InvSrcAlpha,
            lgfx::BlendOp_Add,
            lgfx::Blend_SrcAlpha,
            lgfx::Blend_DestAlpha,
            lgfx::BlendOp_Max,
            lgfx::ColorWrite_All);


        samLinear_ = lgfx::SamplerState::create(
            lgfx::TexFilter_MinMagMipLinear,
            lgfx::TexAddress_Clamp,
            lgfx::Cmp_Always,
            0.0f);

        vs_ = resources.getShaderManager().getVS(ShaderVS_Particle);
        gs_ = resources.getShaderManager().getGS(ShaderGS_Particle);
        ps_ = resources.getShaderManager().getPS(ShaderPS_Particle);
    }

    template<class U>
    void ComponentParticleRenderer<U>::onStart()
    {
    }

    template<class U>
    void ComponentParticleRenderer<U>::update()
    {
    }

    template<class U>
    void ComponentParticleRenderer<U>::postUpdate()
    {

    }

    template<class U>
    void ComponentParticleRenderer<U>::onDestroy()
    {
    }

    template<class U>
    void ComponentParticleRenderer<U>::addQueue(RenderQueue& queue)
    {
        if(vertices_.size()<=0){
            return;
        }
        const ComponentGeometric* geometric = getEntity().getGeometric();
        f32 depth = lmath::manhattanDistance3(queue.getCamera().getEyePosition(), geometric->getPosition());
        queue.add(RenderPath_Transparent, depth, this);
    }

    template<class U>
    void ComponentParticleRenderer<U>::drawDepth(RenderContext& /*renderContext*/)
    {
    }

    template<class U>
    void ComponentParticleRenderer<U>::drawOpaque(RenderContext& /*renderContext*/)
    {
    }

    template<class U>
    void ComponentParticleRenderer<U>::drawTransparent(RenderContext& renderContext)
    {
        vertexBuffer_.begin();
        lgfx::ContextRef& context = renderContext.getContext();

        s32 size = sizeof(ParticleVertex)*vertices_.size();
        lgfx::MappedSubresourceTransientBuffer mapped;
        if(!vertexBuffer_.map(context, 0, size, mapped)){
            vertexBuffer_.end();
            return;
        }
        f32* dst = mapped.getData<f32>();
        const f32* src = reinterpret_cast<const f32*>(&vertices_.getFirst(0));
        for(s32 i=0; i<vertices_.size(); ++i){
            _mm_store_ps(dst, _mm_load_ps(src));
            _mm_store_ps(dst+4, _mm_load_ps(src+4));
            dst += 8;
            src += 8;
        }
        vertexBuffer_.unmap(context, 0);

        context.setRasterizerState(lgfx::ContextRef::Rasterizer_FillSolidNoCull);
        context.setInputLayout(inputLayout_);
        context.setBlendState(blendState_);
        context.setPrimitiveTopology(lgfx::Primitive_PointList);
        vertexBuffer_.attach(context, 0, sizeof(ParticleVertex), mapped.offsetInBytes_);
        context.setVertexShader(vs_);
        context.setGeometryShader(gs_);
        context.setPixelShader(ps_);

        lgfx::ShaderResourceView::set(srv_.get(), renderContext.getCamera().getSRView(DeferredRT_Depth));
        lgfx::ShaderResourceView::setPS(context, 0, 2);
        context.setPSSamplers(0, 1, samLinear_);
        renderContext.setConstant(RenderContext::Shader_PS, 4, sizeof(f32)*4, particleConstant_);
        context.draw(vertices_.size(), 0);

        context.setVertexShader(NULL);
        context.setGeometryShader(NULL);
        context.setPixelShader(NULL);
        context.clearPSResources(2);
        vertexBuffer_.end();
    }

    template<class U>
    void ComponentParticleRenderer<U>::setTexture(lgfx::Texture2DRef& texture, lgfx::ShaderResourceViewRef& srv)
    {
        texture_ = texture;
        srv_ = srv;
    }

    template<class U>
    inline s32 ComponentParticleRenderer<U>::capacity() const
    {
        return vertices_.capacity();
    }

    template<class U>
    inline s32 ComponentParticleRenderer<U>::size() const
    {
        return vertices_.size();
    }

    template<class U>
    void ComponentParticleRenderer<U>::clear()
    {
        vertices_.clear();
    }

    template<class U>
    inline void ComponentParticleRenderer<U>::add(const ParticleVertex& vertex0, const U& vertex1)
    {
        vertices_.add(vertex0, vertex1);
    }

    template<class U>
    inline void ComponentParticleRenderer<U>::removeAt(s32 index)
    {
        vertices_.removeAt(index);
    }

    template<class U>
    inline const ParticleVertex& ComponentParticleRenderer<U>::getFirst(s32 index) const
    {
        return vertices_.getFirst(index);
    }

    template<class U>
    inline ParticleVertex& ComponentParticleRenderer<U>::getFirst(s32 index)
    {
        return vertices_.getFirst(index);
    }

    template<class U>
    inline const U& ComponentParticleRenderer<U>::getSecond(s32 index) const
    {
        return vertices_.getSecond(index);
    }

    template<class U>
    inline U& ComponentParticleRenderer<U>::getSecond(s32 index)
    {
        return vertices_.getSecond(index);
    }

    template<class U>
    void ComponentParticleRenderer<U>::resize(s32 capacity)
    {
        if(capacity<=vertices_.capacity()){
            return;
        }
        lgfx::FrameSyncQuery& frameSync = Application::getInstance().getRenderer().getFrameSync();
        vertexBuffer_.terminate();
        s32 totalSize = capacity*frameSync.getFrames()*sizeof(ParticleVertex);
        vertexBuffer_.initialize(&frameSync, totalSize);
        vertices_.reserve(capacity);
    }

    template<class U>
    inline void ComponentParticleRenderer<U>::setSoftParticleDepthDelta(f32 delta)
    {
        particleConstant_[0] = delta;
    }
}
#endif //INC_LFRAMEWORK_COMPONENTPARTICLERENDERER_H__

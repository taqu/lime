#ifndef INC_LFRAMEWORK_COMPONENTVOLUMEPARTICLERENDERER_H__
#define INC_LFRAMEWORK_COMPONENTVOLUMEPARTICLERENDERER_H__
/**
@file ComponentVolumeParticleRenderer.h
@author t-sakai
@date 2016/12/22 create
*/
#include "ComponentRenderer.h"
#include <lcore/Noise.h>
#include <lcore/Random.h>
#include <lmath/Vector3.h>
#include <lmath/Vector4.h>
#include <lgraphics/DynamicBuffer.h>
#include <lgraphics/InputLayoutRef.h>
#include <lgraphics/BlendStateRef.h>
#include <lgraphics/SamplerStateRef.h>
#include <lgraphics/ShaderRef.h>
#include "../System.h"
#include "../Application.h"
#include "../resource/Resources.h"
#include "PairArray.h"
#include "ComponentRendererManager.h"
#include "ComponentGeometric.h"
#include "../render/Renderer.h"
#include "../render/RenderQueue.h"
#include "../render/RenderContext.h"
#include "../render/Camera.h"

namespace lfw
{
    struct VolumeParticleVertex
    {
        lmath::Vector3 position_;
        f32 time_;
        f32 scale_;
        u16 radius_; //FP16
        u16 invRadius_;//FP16
    };

    template<class U>
    class ComponentVolumeParticleRenderer : public ComponentRenderer
    {
    public:
        static const s32 DefaultMaxParticles = 64;
        static const s32 MaxFrames = 4;
        typedef lgfx::DynamicBuffer<lgfx::VertexBufferRef, lgfx::VertexBufferCreator> DynamicVertexBuffer;

        struct NoiseConstant
        {
            s32 noiseOctaves_;
            lmath::Vector3 noiseAnimation_;

            f32 noiseScale_;
            f32 noiseAmplitude_;
            f32 noisePersistence_;
            f32 noiseFrequency_;
        };

        struct VolumeConstant
        {
            s32 maxSteps_;
            f32 stepSize_;
            f32 uvExtent_;
            f32 uvDisplacement_;

            f32 skinThickness_;
            f32 displacement_;
            f32 edgeSoftness_;
            f32 opacity_;

            NoiseConstant noise_;
        };

        struct HullConstant
        {
            float tesselationFactor_;
            float minDistance_;
            float invRange_;
            float tesselationMinFactor_;
        };

        typedef PairArray<VolumeParticleVertex, U> VertexArray;

        ComponentVolumeParticleRenderer();
        virtual ~ComponentVolumeParticleRenderer();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void postUpdate();
        virtual void onDestroy();
        virtual bool addQueue(RenderQueue& queue);
        virtual void drawDepth(RenderContext& renderContext);
        virtual void drawOpaque(RenderContext& renderContext);
        virtual void drawTransparent(RenderContext& renderContext);
        virtual void getAABB(lmath::lm128& bmin, lmath::lm128& bmax);

        void setTexture(lgfx::Texture2DRef& texture, lgfx::ShaderResourceViewRef& srv);
        inline s32 capacity() const;
        inline s32 size() const;
        inline void clear();

        inline void add(const VolumeParticleVertex& vertex0, const U& vertex1);
        inline void removeAt(s32 index);
        inline const VolumeParticleVertex& getFirst(s32 index) const;
        inline VolumeParticleVertex& getFirst(s32 index);
        inline const U& getSecond(s32 index) const;
        inline U& getSecond(s32 index);

        void resize(s32 capacity);

        //inline void setSoftParticleDepthDelta(f32 delta);
    protected:
        ComponentVolumeParticleRenderer(const ComponentVolumeParticleRenderer&);
        ComponentVolumeParticleRenderer& operator=(const ComponentVolumeParticleRenderer&);

        inline ComponentRendererManager* getManager()
        {
            return ECSManager::getInstance().getComponentManager<ComponentRendererManager>();
        }

        VertexArray vertices_;
        DynamicVertexBuffer vertexBuffer_;
        HullConstant hullConstant_;
        VolumeConstant volumeConstant_;

        lgfx::InputLayoutRef inputLayout_;
        lgfx::BlendStateRef blendState_;
        lgfx::SamplerStateRef samLinear_;
        lgfx::SamplerStateRef samLinearWrap_;
        lgfx::DomainShaderRef ds_;
        lgfx::HullShaderRef hs_;
        lgfx::VertexShaderRef vs_;
        lgfx::PixelShaderRef ps_;
        lgfx::Texture3DRef textureNoise_;
        lgfx::ShaderResourceViewRef srvNoise_;
        lgfx::Texture2DRef texture_;
        lgfx::ShaderResourceViewRef srv_;
    };

    template<class U>
    ComponentVolumeParticleRenderer<U>::ComponentVolumeParticleRenderer()
    {
        hullConstant_.tesselationFactor_ = 16.0f;
        hullConstant_.minDistance_ = 0.1f;
        hullConstant_.invRange_ = 1.0f/(10.0f-0.1f);
        hullConstant_.tesselationMinFactor_ = 2.0f;

        volumeConstant_.maxSteps_ =  6;
        volumeConstant_.stepSize_ =  0.005f;
        volumeConstant_.uvExtent_ =  0.5f;
        volumeConstant_.uvDisplacement_ = 1.0f;
        volumeConstant_.skinThickness_ = 0.5f;
        volumeConstant_.displacement_ = 0.1f;
        volumeConstant_.edgeSoftness_ = 0.1f;
        volumeConstant_.opacity_ = 1.0f;

        volumeConstant_.noise_.noiseOctaves_ = 3;
        volumeConstant_.noise_.noiseScale_ = 1.0f;
        volumeConstant_.noise_.noiseAmplitude_ = 1.0f;
        volumeConstant_.noise_.noisePersistence_ = 0.5f;
        volumeConstant_.noise_.noiseFrequency_ = 0.413f;
        volumeConstant_.noise_.noiseAnimation_.set(0.1f,0.1f,0.1f);
    }

    template<class U>
    ComponentVolumeParticleRenderer<U>::~ComponentVolumeParticleRenderer()
    {
        vertexBuffer_.terminate();
    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::onCreate()
    {
        Resources& resources = lfw::System::getResources();
        inputLayout_ = resources.getInputLayoutFactory().get(InputLayout_VolumeParticle);
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

        samLinearWrap_ = lgfx::SamplerState::create(
            lgfx::TexFilter_MinMagMipLinear,
            lgfx::TexAddress_Wrap,
            lgfx::Cmp_Always,
            0.0f);

        ds_ = resources.getShaderManager().getDS(ShaderDS_VolumeParticle);
        hs_ = resources.getShaderManager().getHS(ShaderHS_VolumeParticle);
        vs_ = resources.getShaderManager().getVS(ShaderVS_VolumeParticle);
        ps_ = resources.getShaderManager().getPS(ShaderPS_VolumeParticle);


        {
            static const s32 RandomSize = 64;
            lcore::Noise noise(lcore::getDefaultSeed64());
            lgfx::SubResourceData initData;
            u16 noiseValues[RandomSize*RandomSize*RandomSize];
            initData.mem_ = noiseValues;
            initData.pitch_ = RandomSize * sizeof(u16);
            initData.slicePitch_ = RandomSize * RandomSize * sizeof(u16);

            for(s32 i = 0; i < RandomSize; ++i) {
                for(s32 j = 0; j < RandomSize; ++j) {
                    for(s32 k = 0; k < RandomSize; ++k) {

                        lcore::NoiseSample3 sample = noise.perlin3D(static_cast<f32>(i), static_cast<f32>(j), static_cast<f32>(k), volumeConstant_.noise_.noiseFrequency_);

                        sample.value_ = lcore::clamp01(0.5f*sample.value_+0.5f);
                        u32 index = i*(RandomSize*RandomSize) + j*RandomSize + k;
                        noiseValues[index] = lcore::toBinary16Float(sample.value_);
                    }
                }
            }

            textureNoise_ = lgfx::Texture::create3D (
                RandomSize,
                RandomSize,
                RandomSize,
                1,
                lgfx::Data_R16_Float,
                lgfx::Usage_Immutable,
                lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                &initData);

            lgfx::SRVDesc srvDesc;
            srvDesc.format_ = lgfx::Data_R16_Float;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture3D;
            srvDesc.tex3D_.mipLevels_ = 1;
            srvDesc.tex3D_.mostDetailedMip_ = 0;
            srvNoise_ = textureNoise_.createSRView (srvDesc);
        }
    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::onStart()
    {
    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::update()
    {
    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::postUpdate()
    {

    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::onDestroy()
    {
    }

    template<class U>
    bool ComponentVolumeParticleRenderer<U>::addQueue(RenderQueue& queue)
    {
        if(size()<=0){
            return false;
        }
        const ComponentGeometric* geometric = getEntity().getGeometric();
        f32 depth = lmath::manhattanDistance3(queue.getCamera().getEyePosition(), geometric->getPosition());
        queue.add(RenderPath_Transparent, depth, this);
        return true;
    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::drawDepth(RenderContext& /*renderContext*/)
    {
    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::drawOpaque(RenderContext& /*renderContext*/)
    {
    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::drawTransparent(RenderContext& renderContext)
    {
        vertexBuffer_.begin();
        lgfx::ContextRef& context = renderContext.getContext();

        s32 size = sizeof(VolumeParticleVertex)*vertices_.size();
        lgfx::MappedSubresourceTransientBuffer mapped;
        if(!vertexBuffer_.map(context, 0, size, mapped)){
            vertexBuffer_.end();
            return;
        }
        const VolumeParticleVertex* src = &vertices_.getFirst(0);
        VolumeParticleVertex* dst = mapped.getData<VolumeParticleVertex>();
        lcore::memcpy(dst, src, size);
        vertexBuffer_.unmap(context, 0);

        context.setRasterizerState(lgfx::ContextRef::Rasterizer_FillSolidNoCull);
        context.setInputLayout(inputLayout_);
        context.setBlendState(blendState_);
        context.setPrimitiveTopology(lgfx::Primitive_PatchList1);
        vertexBuffer_.attach(context, 0, sizeof(VolumeParticleVertex), mapped.offsetInBytes_);
        context.setDomainShader(ds_);
        context.setHullShader(hs_);
        context.setVertexShader(vs_);
        context.setPixelShader(ps_);

        lgfx::ShaderSamplerState samplerStates(context, samLinearWrap_, samLinear_);
        samplerStates.setDS(0);
        samplerStates.setPS(0);

        lgfx::ShaderResourceView srvs(context, srvNoise_, srv_);
        srvs.setDS(0);
        srvs.setPS(0);

        renderContext.setConstant(RenderContext::Shader_HS, 4, sizeof(HullConstant), &hullConstant_);
        lgfx::ConstantBufferRef* volumeConstant = renderContext.createConstantBuffer(sizeof(VolumeConstant), &volumeConstant_);
        renderContext.setConstantBuffer(RenderContext::Shader_DS, 3, volumeConstant);
        renderContext.setConstantBuffer(RenderContext::Shader_PS, 3, volumeConstant);

        context.draw(vertices_.size(), 0);

        context.clearVertexBuffers(0, 1);
        context.setDomainShader(NULL);
        context.setHullShader(NULL);
        context.setVertexShader(NULL);
        context.setPixelShader(NULL);

        context.clearDSConstantBuffers(3, 1);
        context.clearPSConstantBuffers(3, 1);
        context.clearHSConstantBuffers(4, 1);
        context.clearDSResources(0, 2);
        context.clearPSResources(0, 2);
        context.clearDSSamplers(0, 2);
        context.clearPSSamplers(0, 2);
        vertexBuffer_.end();
    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::getAABB(lmath::lm128& bmin, lmath::lm128& bmax)
    {
        ComponentRenderer::getAABB(bmin, bmax);
    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::setTexture(lgfx::Texture2DRef& texture, lgfx::ShaderResourceViewRef& srv)
    {
        texture_ = texture;
        srv_ = srv;
    }

    template<class U>
    inline s32 ComponentVolumeParticleRenderer<U>::capacity() const
    {
        return vertices_.capacity();
    }

    template<class U>
    inline s32 ComponentVolumeParticleRenderer<U>::size() const
    {
        return vertices_.size();
    }

    template<class U>
    inline void ComponentVolumeParticleRenderer<U>::clear()
    {
        vertices_.clear();
    }

    template<class U>
    inline void ComponentVolumeParticleRenderer<U>::add(const VolumeParticleVertex& vertex0, const U& vertex1)
    {
        vertices_.add(vertex0, vertex1);
    }

    template<class U>
    inline void ComponentVolumeParticleRenderer<U>::removeAt(s32 index)
    {
        vertices_.removeAt(index);
    }

    template<class U>
    inline const VolumeParticleVertex& ComponentVolumeParticleRenderer<U>::getFirst(s32 index) const
    {
        return vertices_.getFirst(index);
    }

    template<class U>
    inline VolumeParticleVertex& ComponentVolumeParticleRenderer<U>::getFirst(s32 index)
    {
        return vertices_.getFirst(index);
    }

    template<class U>
    inline const U& ComponentVolumeParticleRenderer<U>::getSecond(s32 index) const
    {
        return vertices_.getSecond(index);
    }

    template<class U>
    inline U& ComponentVolumeParticleRenderer<U>::getSecond(s32 index)
    {
        return vertices_.getSecond(index);
    }

    template<class U>
    void ComponentVolumeParticleRenderer<U>::resize(s32 capacity)
    {
        if(capacity<=vertices_.capacity()){
            return;
        }
        lgfx::FrameSyncQuery& frameSync = lfw::System::getRenderer().getFrameSync();
        vertexBuffer_.terminate();
        static const s32 SyncFrames = 3;
        s32 totalSize = capacity*frameSync.getFrames()*sizeof(VolumeParticleVertex);
        vertexBuffer_.initialize(&frameSync, totalSize);
        vertices_.reserve(capacity);
    }
}
#endif //INC_LFRAMEWORK_COMPONENTVOLUMEPARTICLERENDERER_H__

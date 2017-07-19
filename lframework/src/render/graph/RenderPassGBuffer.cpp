/**
@file RenderPassGBuffer.cpp
@author t-sakai
@date 2017/06/10 create
*/
#include "render/graph/RenderPassGBuffer.h"
#include "System.h"
#include "resource/Resources.h"
#include "render/RenderContext.h"
#include "render/Camera.h"
#include "render/RenderQueue.h"
#include "ecs/ComponentRenderer.h"
#include "render/graph/RenderGraph.h"

namespace lfw
{
namespace graph
{

    const u32 RenderPassGBuffer::ID_Albedo = graph::RenderGraph::hashID("GB_Albedo");
    const u32 RenderPassGBuffer::ID_Specular = graph::RenderGraph::hashID("GB_Specular");
    const u32 RenderPassGBuffer::ID_DepthNormal = graph::RenderGraph::hashID("GB_DepthNormal");
    const u32 RenderPassGBuffer::ID_Velocity = graph::RenderGraph::hashID("GB_Velocity");
    const u32 RenderPassGBuffer::ID_DSVDepthStencil = graph::RenderGraph::hashID("GB_DSVDepthStencil");
    const u32 RenderPassGBuffer::ID_Stencil = graph::RenderGraph::hashID("GB_Stencil");
    const u32 RenderPassGBuffer::ID_FullVelocity = graph::RenderGraph::hashID("GB_FullVelocity");
    const u32 RenderPassGBuffer::ID_LinearDepth = graph::RenderGraph::hashID("GB_LinearDepth");

    const u32 RenderPassGBuffer::ID_UAVAlbedo = graph::RenderGraph::hashID("GB_UAVAlbedo");
    const u32 RenderPassGBuffer::ID_UAVSpecular = graph::RenderGraph::hashID("GB_UAVSpecular");
    const u32 RenderPassGBuffer::ID_UAVDepthNormal = graph::RenderGraph::hashID("GB_UAVDepthNormal");
    const u32 RenderPassGBuffer::ID_UAVVelocity = graph::RenderGraph::hashID("GB_UAVVelocity");
    const u32 RenderPassGBuffer::ID_UAVFullVelocity = graph::RenderGraph::hashID("GB_UAVFullVelocity");

    RenderPassGBuffer::RenderPassGBuffer()
        :width_(0)
        ,height_(0)
        ,xthreads_(0)
        ,ythreads_(0)
        ,halfXThreads_(0)
        ,halfYThreads_(0)
    {
    }

    RenderPassGBuffer::~RenderPassGBuffer()
    {
        depthStencilState_.destroy();
    }

    void RenderPassGBuffer::create(const Camera& camera)
    {
        const lgfx::Viewport& viewport = camera.getViewport();
        
        width_ = padSizeForComputeShader(viewport.width_);
        height_ = padSizeForComputeShader(viewport.height_);
        s32 xthreads = width_>>ComputeShader_NumThreads_Shift;
        s32 ythreads = height_>>ComputeShader_NumThreads_Shift;
        if(xthreads != xthreads_ || ythreads != ythreads_){
            destroy();
            xthreads_ = xthreads;
            ythreads_ = ythreads;
        }
        s32 halfWidth = padSizeForComputeShader(viewport.width_>>1);
        s32 halfHeight = padSizeForComputeShader(viewport.height_>>1);
        halfXThreads_ = halfWidth>>ComputeShader_NumThreads_Shift;
        halfYThreads_ = halfHeight>>ComputeShader_NumThreads_Shift;

        if(!depthStencilState_.valid()){
            lgfx::StencilOPDesc front;
            front.failOp_ = lgfx::StencilOp_Keep;
            front.depthFailOp_ = lgfx::StencilOp_Keep;
            front.passOp_ = lgfx::StencilOp_Replace;
            front.cmpFunc_ = lgfx::Cmp_Always;
            lgfx::StencilOPDesc back;
            back.failOp_ = lgfx::StencilOp_Keep;
            back.depthFailOp_ = lgfx::StencilOp_Keep;
            back.passOp_ = lgfx::StencilOp_Keep;
            back.cmpFunc_ = lgfx::Cmp_Always;

            depthStencilState_ = lgfx::DepthStencilState::create(
                true,
                lgfx::DepthWrite_All,
                lgfx::Cmp_Greater,
                true,
                0xFFU,
                0xFFU,
                front,
                back);
        }

        if(!rtvAlbedo_.valid()){
            lgfx::Texture2DRef texAlbedo = lgfx::Texture::create2D(
                width_,
                height_,
                1,
                1,
                lgfx::Data_R8G8B8A8_UNorm,
                lgfx::Usage_Default,
                lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource|lgfx::BindFlag_UnorderedAccess,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);

            lgfx::RTVDesc rtvDesc;
            rtvDesc.dimension_ = lgfx::RTVDimension_Texture2D;
            rtvDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
            rtvDesc.tex2D_.mipSlice_ = 0;
            rtvAlbedo_ = texAlbedo.createRTView(rtvDesc);

            lgfx::UAVDesc uavDesc;
            uavDesc.dimension_ = lgfx::UAVDimension_Texture2D;
            uavDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
            uavDesc.tex2D_.mipSlice_ = 0;
            uavAlbedo_ = texAlbedo.createUAView(uavDesc);

            lgfx::SRVDesc srvDesc;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture2D;
            srvDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
            srvDesc.tex2D_.mostDetailedMip_ = 0;
            srvDesc.tex2D_.mipLevels_ = 1;
            srvAlbedo_ = texAlbedo.createSRView(srvDesc);
        }

        if(!rtvSpecular_.valid()){
            lgfx::Texture2DRef texSpecular = lgfx::Texture::create2D(
                width_,
                height_,
                1,
                1,
                lgfx::Data_R8G8B8A8_UNorm,
                lgfx::Usage_Default,
                lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource|lgfx::BindFlag_UnorderedAccess,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);

            lgfx::RTVDesc rtvDesc;
            rtvDesc.dimension_ = lgfx::RTVDimension_Texture2D;
            rtvDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
            rtvDesc.tex2D_.mipSlice_ = 0;
            rtvSpecular_ = texSpecular.createRTView(rtvDesc);

            lgfx::UAVDesc uavDesc;
            uavDesc.dimension_ = lgfx::UAVDimension_Texture2D;
            uavDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
            uavDesc.tex2D_.mipSlice_ = 0;
            uavSpecular_ = texSpecular.createUAView(uavDesc);

            lgfx::SRVDesc srvDesc;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture2D;
            srvDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
            srvDesc.tex2D_.mostDetailedMip_ = 0;
            srvDesc.tex2D_.mipLevels_ = 1;
            srvSpecular_ = texSpecular.createSRView(srvDesc);
        }

        if(!rtvDepthNormal_.valid()){
            lgfx::Texture2DRef texDepthNormal = lgfx::Texture::create2D(
                width_,
                height_,
                1,
                1,
                lgfx::Data_R16G16B16A16_UNorm,
                lgfx::Usage_Default,
                lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource|lgfx::BindFlag_UnorderedAccess,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);

            lgfx::RTVDesc rtvDesc;
            rtvDesc.dimension_ = lgfx::RTVDimension_Texture2D;
            rtvDesc.format_ = lgfx::Data_R16G16B16A16_UNorm;
            rtvDesc.tex2D_.mipSlice_ = 0;
            rtvDepthNormal_ = texDepthNormal.createRTView(rtvDesc);

            lgfx::UAVDesc uavDesc;
            uavDesc.dimension_ = lgfx::UAVDimension_Texture2D;
            uavDesc.format_ = lgfx::Data_R16G16B16A16_UNorm;
            uavDesc.tex2D_.mipSlice_ = 0;
            uavDepthNormal_ = texDepthNormal.createUAView(uavDesc);

            lgfx::SRVDesc srvDesc;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture2D;
            srvDesc.format_ = lgfx::Data_R16G16B16A16_UNorm;
            srvDesc.tex2D_.mostDetailedMip_ = 0;
            srvDesc.tex2D_.mipLevels_ = 1;
            srvDepthNormal_ = texDepthNormal.createSRView(srvDesc);
        }

        if(!rtvVelocity_.valid()){
            lgfx::Texture2DRef texVelocity = lgfx::Texture::create2D(
                width_,
                height_,
                1,
                1,
                lgfx::Data_R16G16_SNorm,
                lgfx::Usage_Default,
                lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource|lgfx::BindFlag_UnorderedAccess,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);

            lgfx::RTVDesc rtvDesc;
            rtvDesc.dimension_ = lgfx::RTVDimension_Texture2D;
            rtvDesc.format_ = lgfx::Data_R16G16_SNorm;
            rtvDesc.tex2D_.mipSlice_ = 0;
            rtvVelocity_ = texVelocity.createRTView(rtvDesc);

            lgfx::UAVDesc uavDesc;
            uavDesc.dimension_ = lgfx::UAVDimension_Texture2D;
            uavDesc.format_ = lgfx::Data_R16G16_SNorm;
            uavDesc.tex2D_.mipSlice_ = 0;
            uavVelocity_ = texVelocity.createUAView(uavDesc);

            lgfx::SRVDesc srvDesc;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture2D;
            srvDesc.format_ = lgfx::Data_R16G16_SNorm;
            srvDesc.tex2D_.mostDetailedMip_ = 0;
            srvDesc.tex2D_.mipLevels_ = 1;
            srvVelocity_ = texVelocity.createSRView(srvDesc);
        }

        if(!dsvDepthStencil_.valid()){
            lgfx::Texture2DRef texDepthStencil = lgfx::Texture::create2D(
                width_,
                height_,
                1,
                1,
                lgfx::Data_R24G8_TypeLess,
                lgfx::Usage_Default,
                lgfx::BindFlag_DepthStencil|lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);

            lgfx::DSVDesc dsvDesc;
            dsvDesc.dimension_ = lgfx::DSVDimension_Texture2D;
            dsvDesc.format_ = lgfx::Data_D24_UNorm_S8_UInt;
            dsvDesc.tex2D_.mipSlice_ = 0;
            dsvDepthStencil_ = texDepthStencil.createDSView(dsvDesc);

            lgfx::SRVDesc srvDesc;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture2D;
            srvDesc.format_ = lgfx::Data_X24_TypeLess_G8_UInt;
            srvDesc.tex2D_.mostDetailedMip_ = 0;
            srvDesc.tex2D_.mipLevels_ = 1;
            srvStencil_ = texDepthStencil.createSRView(srvDesc);
        }

        if(!uavFullVelocity_.valid()){
            lgfx::Texture2DRef texFullVelocity = lgfx::Texture::create2D(
                halfWidth,
                halfHeight,
                1,
                1,
                lgfx::Data_R16G16_Float,
                lgfx::Usage_Default,
                lgfx::BindFlag_UnorderedAccess|lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);

            lgfx::UAVDesc uavDesc;
            uavDesc.dimension_ = lgfx::UAVDimension_Texture2D;
            uavDesc.format_ = lgfx::Data_R16G16_Float;
            uavDesc.tex2D_.mipSlice_ = 0;
            uavFullVelocity_ = texFullVelocity.createUAView(uavDesc);

            lgfx::SRVDesc srvDesc;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture2D;
            srvDesc.format_ = lgfx::Data_R16G16_Float;
            srvDesc.tex2D_.mostDetailedMip_ = 0;
            srvDesc.tex2D_.mipLevels_ = 1;
            srvFullVelocity_ = texFullVelocity.createSRView(srvDesc);
        }

        if(!uavLinearDepth_.valid()){
            lgfx::Texture2DRef texLinearDepth = lgfx::Texture::create2D(
                halfWidth,
                halfHeight,
                1,
                1,
                lgfx::Data_R16_UNorm,
                lgfx::Usage_Default,
                lgfx::BindFlag_ShaderResource|lgfx::BindFlag_UnorderedAccess,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);

            lgfx::UAVDesc uavDesc;
            uavDesc.dimension_ = lgfx::UAVDimension_Texture2D;
            uavDesc.format_ = lgfx::Data_R16_UNorm;
            uavDesc.tex2D_.mipSlice_ = 0;
            uavLinearDepth_ = texLinearDepth.createUAView(uavDesc);

            lgfx::SRVDesc srvDesc;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture2D;
            srvDesc.format_ = lgfx::Data_R16_UNorm;
            srvDesc.tex2D_.mostDetailedMip_ = 0;
            srvDesc.tex2D_.mipLevels_ = 1;
            srvLinearDepth_ = texLinearDepth.createSRView(srvDesc);
        }

        ShaderManager& shaderManager = System::getResources().getShaderManager();
        csCameraMotion_ = shaderManager.getCS(ShaderCS_CameraMotion);
        csLinearDepth_ = shaderManager.getCS(ShaderCS_LinearDepth);

        graph::RenderGraph& renderGraph = System::getRenderGraph();
        renderGraph.setShared(ID_Albedo, lgfx::ViewRef(srvAlbedo_));
        renderGraph.setShared(ID_Specular, lgfx::ViewRef(srvSpecular_));
        renderGraph.setShared(ID_DepthNormal, lgfx::ViewRef(srvDepthNormal_));
        renderGraph.setShared(ID_Velocity, lgfx::ViewRef(srvVelocity_));
        renderGraph.setShared(ID_DSVDepthStencil, lgfx::ViewRef(dsvDepthStencil_));
        renderGraph.setShared(ID_Stencil, lgfx::ViewRef(srvStencil_));
        renderGraph.setShared(ID_FullVelocity, lgfx::ViewRef(srvFullVelocity_));
        renderGraph.setShared(ID_LinearDepth, lgfx::ViewRef(srvLinearDepth_));


        renderGraph.setShared(ID_UAVAlbedo, lgfx::ViewRef(uavAlbedo_));
        renderGraph.setShared(ID_UAVSpecular, lgfx::ViewRef(uavSpecular_));
        renderGraph.setShared(ID_UAVDepthNormal, lgfx::ViewRef(uavDepthNormal_));
        renderGraph.setShared(ID_UAVVelocity, lgfx::ViewRef(uavVelocity_));
        renderGraph.setShared(ID_UAVFullVelocity, lgfx::ViewRef(uavFullVelocity_));
    }

    void RenderPassGBuffer::destroy()
    {
        srvLinearDepth_.destroy();
        uavLinearDepth_.destroy();

        srvFullVelocity_.destroy();
        uavFullVelocity_.destroy();

        srvStencil_.destroy();
        dsvDepthStencil_.destroy();

        srvVelocity_.destroy();
        rtvVelocity_.destroy();

        srvDepthNormal_.destroy();
        rtvDepthNormal_.destroy();

        srvSpecular_.destroy();
        rtvSpecular_.destroy();

        srvAlbedo_.destroy();
        rtvAlbedo_.destroy();

        depthStencilState_.destroy();
    }

    void RenderPassGBuffer::execute(RenderContext& renderContext, Camera& camera)
    {
        lgfx::ContextRef& context = renderContext.getContext();
        lfw::LightArray& lights = renderContext.getLights();

        //Shadow
        for(s32 i=0; i<lights.size(); ++i){
            switch(lights[i].getType()){
            case LightType_Direction:
                if(lights[i].isCastShadow()){
                    i=lights.size();

                    ShadowMap& shadowMap = renderContext.getShadowMap();
                    shadowMap.update(camera, lights[i]);

                    renderContext.setRenderPath(RenderPath_Shadow);
                    renderContext.setPerShadowMapConstants();

                    RenderQueue& renderQueue = renderContext.getRenderQueue();
                    for(s32 j=0; j<renderQueue[RenderPath_Shadow].size_; ++j){
                        renderQueue[RenderPath_Shadow].entries_[j].component_->drawDepth(renderContext);
                    }
                    context.setVertexShader(NULL);
                }
                break;
            case LightType_Point:
            case LightType_Spot:
            default:
                break;
            }
        }

        //
        switch(camera.getClearType())
        {
        case ClearType_None:
            break;
        case ClearType_Depth:
            context.clearDepthStencilView(dsvDepthStencil_, lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, camera.getClearDepth(), camera.getClearStencil());
            break;
        case ClearType_Color:
            context.clearDepthStencilView(dsvDepthStencil_, lgfx::ClearFlag_Depth|lgfx::ClearFlag_Stencil, camera.getClearDepth(), camera.getClearStencil());
            const lmath::Vector4& color = camera.getClearColor();
            context.clearRenderTargetView(rtvAlbedo_, &color.x_);
            break;
        };
        static const f32 Zero[]={0.0f, 0.0f, 0.0f, 0.0f};

        context.clearRenderTargetView(rtvSpecular_, Zero);
        context.clearRenderTargetView(rtvDepthNormal_, Zero);
        context.clearRenderTargetView(rtvVelocity_, Zero);

        renderContext.setRenderPath(RenderPath_Opaque);

        lgfx::RenderTargetViewRef::pointer_type rtvs[] =
        {
            rtvAlbedo_,
            rtvSpecular_,
            rtvDepthNormal_,
            rtvVelocity_,
        };
        context.setRenderTargets(4, rtvs, dsvDepthStencil_);
        context.setViewport(0, 0, width_, height_);

        context.setBlendState(lgfx::ContextRef::BlendState_NoAlpha);
        context.setDepthStencilState(depthStencilState_, 1);

        RenderQueue& renderQueue = renderContext.getRenderQueue();
        for(s32 i=0; i<renderQueue[RenderPath_Opaque].size_; ++i){
            renderQueue[RenderPath_Opaque].entries_[i].component_->drawOpaque(renderContext);
        }

        context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWEnable);
        context.clearRenderTargets(4);

        lgfx::ShaderResourceView srvs(context, srvDepthNormal_, srvVelocity_);
        srvs.setCS(0);

        renderContext.setDefaultSampler(lfw::RenderContext::Shader_CS);

        //Camera Motion
        context.clearUnorderedAccessView(uavFullVelocity_, lgfx::ContextRef::Zero);
        context.setCSUnorderedAccessViews(0, 1, uavFullVelocity_, NULL);
        context.setComputeShader(csCameraMotion_);
        context.dispatch(halfXThreads_, halfYThreads_, 1);

        //Linear Depth
        context.clearUnorderedAccessView(uavLinearDepth_, lgfx::ContextRef::Zero);
        context.setCSUnorderedAccessViews(0, 1, uavLinearDepth_, NULL);
        context.setComputeShader(csLinearDepth_);
        context.dispatch(halfXThreads_, halfYThreads_, 1);

        context.setComputeShader(NULL);
        context.clearCSUnorderedAccessView(0, 1);
        context.clearCSSamplers(0, 2);
        context.clearCSResources(0, 2);
        context.restoreDefaultRenderTargets();
        context.restoreDefaultViewport();
    }
}
}

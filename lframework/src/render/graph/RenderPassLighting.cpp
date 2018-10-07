/**
@file RenderPassLighting.cpp
@author t-sakai
@date 2017/07/04 creat
*/
#include "render/graph/RenderPassLighting.h"
#include <lmath/Matrix34.h>
#include <lgraphics/Window.h>
#include "System.h"
#include "resource/Resources.h"
#include "render/RenderContext.h"
#include "render/Camera.h"
#include "render/Geometry.h"
#include "render/RenderQueue.h"
#include "ecs/ComponentRenderer.h"
#include "render/graph/RenderGraph.h"
#include "render/graph/RenderPassGBuffer.h"

namespace lfw
{
namespace graph
{
    const u32 RenderPassLighting::ID_FullVelocity = graph::RenderGraph::hashID("GB_FullVelocity");
    const u32 RenderPassLighting::ID_LinearDepth = graph::RenderGraph::hashID("GB_LinearDepth");
    const u32 RenderPassLighting::ID_UAVFullVelocity = graph::RenderGraph::hashID("GB_UAVFullVelocity");

    const u32 RenderPassLighting::ID_RTVAccumLighting = graph::RenderGraph::hashID("RTVAccumLighting");
    const u32 RenderPassLighting::ID_SRVAccumLighting = graph::RenderGraph::hashID("SRVAccumLighting");

    RenderPassLighting::RenderPassLighting()
        :RenderPass(RenderPassID_Lighting)
        ,width_(0)
        ,height_(0)
    {
        lightMatrices_ = (lmath::Matrix34*)LALIGNED_MALLOC(sizeof(lmath::Matrix34)*LFW_CONFIG_MAX_POINT_LIGHTS, 16);
    }

    RenderPassLighting::~RenderPassLighting()
    {
        LALIGNED_FREE(lightMatrices_, 16);
    }

    void RenderPassLighting::create(const Camera& /*camera*/)
    {
        //ボリュームありの光源裏面描画用
        if(!depthStencilStateBack_.valid()){
            lgfx::StencilOPDesc front;
            front.failOp_ = lgfx::StencilOp_Keep;
            front.depthFailOp_ = lgfx::StencilOp_Keep;
            front.passOp_ = lgfx::StencilOp_IncrSat;
            front.cmpFunc_ = lgfx::Cmp_Equal;
            lgfx::StencilOPDesc back;
            back.failOp_ = lgfx::StencilOp_Keep;
            back.depthFailOp_ = lgfx::StencilOp_Keep;
            back.passOp_ = lgfx::StencilOp_Keep;
            back.cmpFunc_ = lgfx::Cmp_Never;

            depthStencilStateBack_ = lgfx::DepthStencilState::create(
                true,
                lgfx::DepthWrite_Zero,
                lgfx::Cmp_LessEqual,
                true,
                0xFFU,
                0xFFU,
                front,
                back);
        }

        //ボリュームありの光源表面描画用
        if(!depthStencilStateFront_.valid()){
            lgfx::StencilOPDesc front;
            front.failOp_ = lgfx::StencilOp_Keep;
            front.depthFailOp_ = lgfx::StencilOp_Keep;
            front.passOp_ = lgfx::StencilOp_Keep;
            front.cmpFunc_ = lgfx::Cmp_Equal;
            lgfx::StencilOPDesc back;
            back.failOp_ = lgfx::StencilOp_Keep;
            back.depthFailOp_ = lgfx::StencilOp_Keep;
            back.passOp_ = lgfx::StencilOp_Keep;
            back.cmpFunc_ = lgfx::Cmp_Never;

            depthStencilStateFront_ = lgfx::DepthStencilState::create(
                true,
                lgfx::DepthWrite_Zero,
                lgfx::Cmp_GreaterEqual,
                true,
                0xFFU,
                0xFFU,
                front,
                back);
        }

        //ディレクショナル描画用
        if(!depthStencilState_.valid()){
            //lgfx::StencilOPDesc front;
            //front.failOp_ = lgfx::StencilOp_Keep;
            //front.depthFailOp_ = lgfx::StencilOp_Keep;
            //front.passOp_ = lgfx::StencilOp_Keep;
            //front.cmpFunc_ = lgfx::Cmp_LessEqual;
            //lgfx::StencilOPDesc back;
            //back.failOp_ = lgfx::StencilOp_Keep;
            //back.depthFailOp_ = lgfx::StencilOp_Keep;
            //back.passOp_ = lgfx::StencilOp_Keep;
            //back.cmpFunc_ = lgfx::Cmp_Always;

            lgfx::StencilOPDesc front;
            front.failOp_ = lgfx::StencilOp_Keep;
            front.depthFailOp_ = lgfx::StencilOp_Keep;
            front.passOp_ = lgfx::StencilOp_Keep;
            front.cmpFunc_ = lgfx::Cmp_Always;
            lgfx::StencilOPDesc back;
            back.failOp_ = lgfx::StencilOp_Keep;
            back.depthFailOp_ = lgfx::StencilOp_Keep;
            back.passOp_ = lgfx::StencilOp_Keep;
            back.cmpFunc_ = lgfx::Cmp_Always;

            depthStencilState_ = lgfx::DepthStencilState::create(
                false,
                lgfx::DepthWrite_Zero,
                lgfx::Cmp_Greater,
                false,//true,
                0xFFU,
                0xFFU,
                front,
                back);
        }

        ShaderManager& shaderManager = System::getResources().getShaderManager();
        csCameraMotion_ = shaderManager.getCS(ShaderCS_CameraMotion);
        csLinearDepth_ = shaderManager.getCS(ShaderCS_LinearDepth);

        graph::RenderGraph& renderGraph = System::getRenderGraph();
        viewAlbedo_ = renderGraph.getShared(RenderPassGBuffer::ID_Albedo);
        viewSpecular_ = renderGraph.getShared(RenderPassGBuffer::ID_Specular);
        viewDepthNormal_ = renderGraph.getShared(RenderPassGBuffer::ID_DepthNormal);
        viewVelocity_ = renderGraph.getShared(RenderPassGBuffer::ID_Velocity);
        viewDepthStencil_ = renderGraph.getShared(RenderPassGBuffer::ID_DSVDepthStencil);
        viewStencil_ = renderGraph.getShared(RenderPassGBuffer::ID_Stencil);

        {
            resourceDepthStencil_ = lgfx::ResourceRef::getResource(viewDepthStencil_);
            lgfx::Texture2DDesc desc;
            resourceDepthStencil_.getDesc(desc);
            width_ = desc.Width;
            height_ = desc.Height;

            if(!dsvDepthStencil_.valid()){
                texDepthStencil_ = lgfx::Texture::create2D(
                    desc.Width,
                    desc.Height,
                    desc.MipLevels,
                    desc.ArraySize,
                    (lgfx::DataFormat)desc.Format,
                    desc.SampleDesc.Count,
                    desc.SampleDesc.Quality,
                    lgfx::Usage_Default,
                    lgfx::BindFlag_DepthStencil|lgfx::BindFlag_ShaderResource,
                    lgfx::CPUAccessFlag_None,
                    lgfx::ResourceMisc_None,
                    NULL);

                lgfx::DSVDesc dsvDesc;
                dsvDesc.dimension_ = lgfx::DSVDimension_Texture2D;
                dsvDesc.format_ = lgfx::Data_D24_UNorm_S8_UInt;
                dsvDesc.tex2D_.mipSlice_ = 0;
                dsvDepthStencil_ = texDepthStencil_.createDSView(dsvDesc);
            }
        }


        //s32 xthreads = width_>>ComputeShader_NumThreads_Shift;
        //s32 ythreads = height_>>ComputeShader_NumThreads_Shift;
        s32 halfWidth = padSizeForComputeShader(width_>>1);
        s32 halfHeight = padSizeForComputeShader(height_>>1);
        halfXThreads_ = halfWidth>>ComputeShader_NumThreads_Shift;
        halfYThreads_ = halfHeight>>ComputeShader_NumThreads_Shift;


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


        GBufferConstant constant;
        constant.width_ = width_;
        constant.height_ = height_;
        constant.invWidth_ = 1.0f/width_;
        constant.invHeight_ = 1.0f/height_;
        constantGBuffer_ = lgfx::ConstantBuffer::create(
            sizeof(GBufferConstant),
            lgfx::Usage_Immutable,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            0,
            &constant,
            sizeof(GBufferConstant));

        vsFullQuadInstanced_ = shaderManager.getVS(ShaderVS_FullQuadInstanced);
        vsDeferredLighting_ = shaderManager.getVS(ShaderVS_DeferredLighting);

        psDeferredLightingDirectional_ = shaderManager.getPS(ShaderPS_DeferredLightingDirectional);
        psDeferredLightingPoint_ = shaderManager.getPS(ShaderPS_DeferredLightingPoint);
        psDeferredLightingCone_ = shaderManager.getPS(ShaderPS_DeferredLightingSpot);

        if(!rtvAccumLighting_.valid()){
            lgfx::Texture2DRef texAccumLighting = lgfx::Texture::create2D(
                width_,
                height_,
                1,
                1,
                lgfx::Data_R16G16B16A16_Float,
                lgfx::Usage_Default,
                lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);

            lgfx::RTVDesc rtvDesc;
            rtvDesc.dimension_ = lgfx::RTVDimension_Texture2D;
            rtvDesc.format_ = lgfx::Data_R16G16B16A16_Float;
            rtvDesc.tex2D_.mipSlice_ = 0;
            rtvAccumLighting_ = texAccumLighting.createRTView(rtvDesc);

            lgfx::SRVDesc srvDesc;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture2D;
            srvDesc.format_ = lgfx::Data_R16G16B16A16_Float;
            srvDesc.tex2D_.mostDetailedMip_ = 0;
            srvDesc.tex2D_.mipLevels_ = 1;
            srvAccumLighting_ = texAccumLighting.createSRView(srvDesc);
        }

        renderGraph.setShared(ID_FullVelocity, lgfx::ViewRef(srvFullVelocity_));
        renderGraph.setShared(ID_LinearDepth, lgfx::ViewRef(srvLinearDepth_));

        renderGraph.setShared(ID_UAVFullVelocity, lgfx::ViewRef(uavFullVelocity_));

        renderGraph.setShared(ID_RTVAccumLighting, lgfx::ViewRef(rtvAccumLighting_));
        renderGraph.setShared(ID_SRVAccumLighting, lgfx::ViewRef(srvAccumLighting_));
    }

    void RenderPassLighting::destroy()
    {
        srvLinearDepth_.destroy();
        uavLinearDepth_.destroy();

        srvFullVelocity_.destroy();
        uavFullVelocity_.destroy();

        dsvDepthStencil_.destroy();
        texDepthStencil_.destroy();
        resourceDepthStencil_.destroy();
        srvAccumLighting_.destroy();
        rtvAccumLighting_.destroy();

        viewStencil_.destroy();
        viewDepthStencil_.destroy();
        viewVelocity_.destroy();
        viewDepthNormal_.destroy();
        viewSpecular_.destroy();
        viewAlbedo_.destroy();

        constantGBuffer_.destroy();

        depthStencilState_.destroy();
        depthStencilStateFront_.destroy();
        depthStencilStateBack_.destroy();
    }

    void RenderPassLighting::execute(RenderContext& renderContext, Camera& /*camera*/)
    {
        lgfx::ContextRef& context = renderContext.getContext();

        renderContext.setDefaultSampler(lfw::RenderContext::Shader_CS);

        //Camera Motion
        {
            lgfx::ShaderResourceView srvs(context, viewDepthNormal_, viewVelocity_);
            srvs.setCS(0);
        }

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

        lfw::LightArray& lights = renderContext.getLights();
        s32 numDirectionals = 0;
        s32 numPoints = 0;
        s32 numSpots = 0;
        for(s32 i=0; i<lights.size(); ++i){
            switch(lights[i].getType()){
            case LightType_Direction:
                if(numDirectionals<LFW_CONFIG_MAX_DIRECTIONAL_LIGHTS){
                    directionalLights_[numDirectionals].color_ = lights[i].getColor();
                    directionalLights_[numDirectionals].direction_ = -lights[i].getDirection();
                    ++numDirectionals;
                }
                break;
            case LightType_Point:
                if(numPoints<LFW_CONFIG_MAX_POINT_LIGHTS){
                    pointLights_[numPoints].color_ = lights[i].getColor();
                    pointLights_[numPoints].position_ = lights[i].getPosition();
                    ++numPoints;
                }
                break;
            case LightType_Spot:
                if(numSpots<LFW_CONFIG_MAX_SPOT_LIGHTS){
                    spotLights_[numSpots].color_ = lights[i].getColor();
                    spotLights_[numSpots].position_ = lights[i].getPosition();
                    spotLights_[numSpots].direction_ = lights[i].getDirection();
                    ++numSpots;
                }
                break;
            }
        }
        context.copyResource(texDepthStencil_, resourceDepthStencil_);
        context.clearRenderTargetView(rtvAccumLighting_, lgfx::ContextRef::Zero);
        context.setViewport(0, 0, width_, height_);
        context.setPrimitiveTopology(lgfx::Primitive_TriangleList);
        context.setBlendState(lgfx::ContextRef::BlendState_Add);
        constantGBuffer_.attachPS(context, 5);

        //ボリューム
        //---------------------------------------
        //裏面マスク作成
        context.setVertexShader(vsDeferredLighting_);
        context.setPixelShader(NULL);
        context.setRenderTargets(0, NULL, dsvDepthStencil_);
        context.setRasterizerState(lgfx::ContextRef::Rasterizer_FillSolidCounterClockwise);
        context.setDepthStencilState(depthStencilStateBack_, 1);

        //ポイントライト
        if(0<numPoints){
            for(s32 i=0; i<numPoints; ++i){
                lightMatrices_[i].identity();
                lightMatrices_[i].setTranslate(pointLights_[i].position_.x_, pointLights_[i].position_.y_, pointLights_[i].position_.z_);
            }
            renderContext.setConstantMatricesAligned16(lfw::RenderContext::Shader_VS, 6, numPoints, LFW_CONFIG_MAX_POINT_LIGHTS, lightMatrices_);
            Geometry& geometry = renderContext.getLightSphere();
            geometry.attach(context);
            context.drawIndexedInstanced(geometry.getNumIndices(), numPoints, 0, 0, 0);
        }

        //表面描画
        context.setRenderTargets(1, rtvAccumLighting_, dsvDepthStencil_);
        context.setRasterizerState(lgfx::ContextRef::Rasterizer_FillSolid);
        context.setDepthStencilState(depthStencilStateFront_, 1);
        context.setPixelShader(psDeferredLightingPoint_);

        lgfx::ShaderResourceView srvs(context, viewAlbedo_, viewDepthNormal_, viewSpecular_, NULL, NULL, renderContext.getShadowMap().getSRV());
        srvs.setPS(0);

        //ポイントライト
        if(0<numPoints){
            renderContext.setConstant(RenderContext::Shader_PS, 6, sizeof(PointLight), numPoints, LFW_CONFIG_MAX_POINT_LIGHTS, pointLights_);
            Geometry& geometry = renderContext.getLightSphere();
            geometry.attach(context);
            context.drawIndexedInstanced(geometry.getNumIndices(), numPoints, 0, 0, 0);
        }

        //ディレクショナル
        //---------------------------------------
        context.setRenderTargets(1, rtvAccumLighting_, viewDepthStencil_);

        context.setVertexShader(vsFullQuadInstanced_);
        context.setPixelShader(psDeferredLightingDirectional_);
        renderContext.setDefaultSampler(lfw::RenderContext::Shader_PS);

        context.setDepthStencilState(depthStencilState_, 0);
        context.clearIndexBuffers();
        context.clearVertexBuffers(0, 1);
        renderContext.setConstant(RenderContext::Shader_PS, 6, sizeof(DirectionalLight), numDirectionals, LFW_CONFIG_MAX_DIRECTIONAL_LIGHTS, directionalLights_);
        context.drawInstanced(4, numDirectionals, 0, 0);

        //----------------------------
        context.clearPSResources(0, 4);
        context.setPixelShader(NULL);
        context.setVertexShader(NULL);
        context.clearPSConstantBuffers(5, 2);
        renderContext.clearDefaultSampler(lfw::RenderContext::Shader_PS);

        context.restoreDefaultRenderTargets();
        context.restoreDefaultViewport();
        context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWEnableReverseZ);
        context.setBlendState(lgfx::ContextRef::BlendState_NoAlpha);
    }
}
}

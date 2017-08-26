/**
@file RenderPassProcedural.cpp
@author t-sakai
@date 2017/06/10 create
*/
#include "RenderPassProcedural.h"
#include <lframework/System.h>
#include <lframework/Timer.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceTexture3D.h>
#include <lframework/render/RenderContext.h>
#include <lframework/render/Camera.h>
#include <lframework/render/RenderQueue.h>
#include <lframework/ecs/ComponentRenderer.h>
#include <lframework/render/graph/RenderGraph.h>
#include <lframework/render/graph/RenderPassGBuffer.h>
#include <lframework/render/graph/RenderPassLighting.h>
#include "ShaderID.h"

namespace debug
{
    RenderPassProcedural::RenderPassProcedural()
        :RenderPass(lfw::graph::RenderPassID_UI+1)
        ,type_(Type_Space)
    {
    }

    RenderPassProcedural::~RenderPassProcedural()
    {
    }

    void RenderPassProcedural::create(const lfw::Camera& camera)
    {
        lfw::graph::RenderGraph& renderGraph = lfw::System::getRenderGraph();

        samLinear_ = lgfx::SamplerState::create(
            lgfx::TexFilter_MinMagMipLinear,
            lgfx::TexAddress_Clamp,
            lgfx::Cmp_Always,
            0.0f);

        samLinearMirror_ = lgfx::SamplerState::create(
            lgfx::TexFilter_MinMagMipLinear,
            lgfx::TexAddress_Wrap,
            lgfx::Cmp_Always,
            0.0f);

        viewDepthStencil_ = renderGraph.getShared(lfw::graph::RenderPassGBuffer::ID_DSVDepthStencil);
        viewAccumLighting_ = renderGraph.getShared(lfw::graph::RenderPassLighting::ID_RTVAccumLighting);

        {
            lgfx::ResourceRef resourceDepthStencil = lgfx::ResourceRef::getResource(viewDepthStencil_);
            lgfx::Texture2DDesc desc;
            resourceDepthStencil.getDesc(desc);
            width_ = desc.Width;
            height_ = desc.Height;
            constant_.width_ = desc.Width;
            constant_.height_ = desc.Height;
            constant_.time_ = 0.0f;
        }

        lfw::ShaderManager& shaderManager = lfw::System::getResources().getShaderManager();
        vsProcedual_[Type_Space] = shaderManager.getVS(ShaderVS_ProceduralSpace);
        psProcedual_[Type_Space] = shaderManager.getPS(ShaderPS_ProceduralSpace);

        lfw::s32 setID = 0;
        lfw::ResourceTexture3D* texNoise = lfw::System::getResources().load(setID, "LDR_RGBA_3D.dds", lfw::ResourceType::ResourceType_Texture3D, lfw::TextureParameter::NoSRGB_)->cast<lfw::ResourceTexture3D>();
        srvNoise_ = texNoise->getShaderResourceView();
    }

    void RenderPassProcedural::destroy()
    {
        viewAccumLighting_.destroy();
        viewDepthStencil_.destroy();
    }

    void RenderPassProcedural::execute(lfw::RenderContext& renderContext, lfw::Camera& camera)
    {
        if(type_ == Type_None){
            return;
        }

        lgfx::ContextRef& context = renderContext.getContext();

        constant_.time_ += lfw::System::getTimer().getDeltaTime();

        if(600.0f<constant_.time_){
            constant_.time_ -= 600.0f;
        }
        context.setViewport(0, 0, width_, height_);
        context.setVertexShader(vsProcedual_[type_]);
        context.setPixelShader(psProcedual_[type_]);

        context.setPrimitiveTopology(lgfx::Primitive_TriangleList);
        context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWDisableReverseZ);
        context.setBlendState(lgfx::ContextRef::BlendState_NoAlpha);

        lgfx::ShaderSamplerState samplerStates(context, samLinearMirror_, samLinear_);
        samplerStates.setPS(0);

        context.setPSResources(0, 1, srvNoise_);

        renderContext.setConstant(lfw::RenderContext::Shader_PS, 6, sizeof(ProcedualConstant), &constant_);
        ID3D11RenderTargetView* rtvs[] = {viewAccumLighting_};
        context.setRenderTargets(1, rtvs, viewDepthStencil_);
        context.draw(4, 0);

        context.setPixelShader(NULL);
        context.setVertexShader(NULL);
        context.clearPSConstantBuffers(6, 1);
        renderContext.setDefaultSampler(lfw::RenderContext::Shader_PS);
        context.restoreDefaultViewport();
        context.restoreDefaultRenderTargets();
    }

    void RenderPassProcedural::set(Type type)
    {
        type_ = type;
    }
}

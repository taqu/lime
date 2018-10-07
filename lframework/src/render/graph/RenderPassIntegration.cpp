/**
@file RenderPassIntegration.cpp
@author t-sakai
@date 2017/07/04 creat
*/
#include "render/graph/RenderPassIntegration.h"
#include <lgraphics/Window.h>
#include "System.h"
#include "resource/Resources.h"
#include "render/RenderContext.h"
#include "render/Camera.h"
#include "render/RenderQueue.h"
#include "ecs/ComponentRenderer.h"
#include "render/graph/RenderGraph.h"
#include "render/graph/RenderPassGBuffer.h"
#include "render/graph/RenderPassLighting.h"

namespace lfw
{
namespace graph
{

    RenderPassIntegration::RenderPassIntegration()
        :RenderPass(RenderPassID_Integration)
    {
    }

    RenderPassIntegration::~RenderPassIntegration()
    {
    }

    void RenderPassIntegration::create(const Camera& /*camera*/)
    {
        lgfx::Window& window = System::getWindow();

        GBufferConstant constant;
        lgfx::Window::Vector2 size = window.getViewSize();
        constant.width_ = size.x_;
        constant.height_ = size.y_;
        constant.invWidth_ = 1.0f/size.x_;
        constant.invHeight_ = 1.0f/size.y_;
        constantGBuffer_ = lgfx::ConstantBuffer::create(
            sizeof(GBufferConstant),
            lgfx::Usage_Immutable,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            0,
            &constant,
            sizeof(GBufferConstant));

        ShaderManager& shaderManager = System::getResources().getShaderManager();
        vsFullQuad_ = shaderManager.getVS(ShaderVS_FullQuad);
        psDeferredIntegration_ = shaderManager.getPS(ShaderPS_DeferredIntegration);

        graph::RenderGraph& renderGraph = System::getRenderGraph();
        viewStencil_ = renderGraph.getShared(RenderPassGBuffer::ID_Stencil);
        viewFullVelocity_ = renderGraph.getShared(RenderPassLighting::ID_FullVelocity);
        viewAccumLighting_ = renderGraph.getShared(RenderPassLighting::ID_SRVAccumLighting);
    }

    void RenderPassIntegration::destroy()
    {
        viewAccumLighting_.destroy();
        viewFullVelocity_.destroy();
        viewStencil_.destroy();

        constantGBuffer_.destroy();
    }

    void RenderPassIntegration::execute(RenderContext& renderContext, Camera& camera)
    {
        lgfx::ContextRef& context = renderContext.getContext();
        context.setViewport(camera.getViewport());
        constantGBuffer_.attachPS(context, 5);
        context.setVertexShader(vsFullQuad_);
        context.setPixelShader(psDeferredIntegration_);
        renderContext.setDefaultSampler(lfw::RenderContext::Shader_PS);

        lgfx::ShaderResourceView srvs(context, viewFullVelocity_, viewAccumLighting_);
        srvs.setPS(0);

        context.setPrimitiveTopology(lgfx::Primitive_TriangleList);
        context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DDisableWDisable);
        context.setBlendState(lgfx::ContextRef::BlendState_NoAlpha);
        context.clearVertexBuffers(0, 1);
        context.draw(4, 0);
        
        context.clearPSResources(0, 2);
        context.setPixelShader(NULL);
        context.setVertexShader(NULL);
        context.clearPSConstantBuffers(5, 1);
        context.restoreDefaultViewport();
    }
}
}

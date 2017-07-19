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
    {
    }

    RenderPassIntegration::~RenderPassIntegration()
    {
    }

    void RenderPassIntegration::create(const Camera& camera)
    {
        lgfx::Window& window = System::getWindow();

        GBufferConstant constant;
        s32 width, height;
        window.getViewSize(width, height);
        constant.width_ = width;
        constant.height_ = height;
        constant.invWidth_ = 1.0f/width;
        constant.invHeight_ = 1.0f/height;
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
        viewFullVelocity_ = renderGraph.getShared(RenderPassGBuffer::ID_FullVelocity);
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
    }
}
}
/**
@file RenderPassGBuffer.cpp
@author t-sakai
@date 2017/06/10 create
*/
#include "render/graph/RenderPassTransparent.h"
#include "System.h"
#include "resource/Resources.h"
#include "render/RenderContext.h"
#include "render/Camera.h"
#include "render/RenderQueue.h"
#include "ecs/ComponentRenderer.h"
#include "render/graph/RenderGraph.h"
#include "render/graph/RenderPassLighting.h"
#include "render/graph/RenderPassGBuffer.h"

namespace lfw
{
namespace graph
{
    RenderPassTransparent::RenderPassTransparent()
        :RenderPass(RenderPassID_Transparent)
        ,width_(0)
        ,height_(0)
    {
    }

    RenderPassTransparent::~RenderPassTransparent()
    {
    }

    void RenderPassTransparent::create(const Camera& camera)
    {
        graph::RenderGraph& renderGraph = System::getRenderGraph();
        rtvAccumLighting_ = renderGraph.getShared(RenderPassLighting::ID_RTVAccumLighting);
        dsvDepthStencil_ = renderGraph.getShared(RenderPassGBuffer::ID_DSVDepthStencil);
        srvLinearDepth_ = renderGraph.getShared(RenderPassLighting::ID_LinearDepth);

        {
            lgfx::ResourceRef resourceDepthStencil = lgfx::ResourceRef::getResource(dsvDepthStencil_);
            lgfx::Texture2DDesc desc;
            resourceDepthStencil.getDesc(desc);
            width_ = desc.Width;
            height_ = desc.Height;
            resourceDepthStencil.destroy();
        }
    }

    void RenderPassTransparent::destroy()
    {
    }

    void RenderPassTransparent::execute(RenderContext& renderContext, Camera& camera)
    {
        lgfx::ContextRef& context = renderContext.getContext();
        lfw::LightArray& lights = renderContext.getLights();

        renderContext.beginRenderPath(RenderPath_Opaque);

        lgfx::RenderTargetViewRef::pointer_type rtvs[] =
        {
            rtvAccumLighting_,
        };
        context.setRenderTargets(1, rtvs, dsvDepthStencil_);
        context.setViewport(0, 0, width_, height_);

        lgfx::ShaderResourceView srvs(context, NULL, NULL, renderContext.getShadowMap().getSRV(), srvLinearDepth_);
        srvs.setPS(3);

        for(s32 i=0; i<lights.size(); ++i){
            if(lights[i].getType() == lfw::LightType_Direction){
                renderContext.setPerLightConstants(lights[i]);
                break;
            }
        }

        context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWEnableGEReverseZ);

        RenderQueue& renderQueue = renderContext.getRenderQueue();
        for(s32 i=0; i<renderQueue[RenderPath_Transparent].size_; ++i){
            renderQueue[RenderPath_Transparent].entries_[i].component_->drawTransparent(renderContext);
        }

        context.setVertexShader(NULL);
        context.setPixelShader(NULL);
        context.clearPSResources(0, 7);
        context.restoreDefaultRenderTargets();
        context.restoreDefaultViewport();
    }
}
}

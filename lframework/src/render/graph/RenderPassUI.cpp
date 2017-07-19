/**
@file RenderPassUI.cpp
@author t-sakai
@date 2017/07/16 create
*/
#include "render/graph/RenderPassUI.h"
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

    RenderPassUI::RenderPassUI()
    {
    }

    RenderPassUI::~RenderPassUI()
    {
    }

    void RenderPassUI::create(const Camera& /*camera*/)
    {
    }

    void RenderPassUI::destroy()
    {
    }

    void RenderPassUI::execute(RenderContext& renderContext, Camera& camera)
    {
        lgfx::ContextRef& context = renderContext.getContext();

        context.restoreDefaultRenderTargets();

        RenderQueue& renderQueue = renderContext.getRenderQueue();
        for(s32 i=0; i<renderQueue[RenderPath_UI].size_; ++i){
            renderQueue[RenderPath_UI].entries_[i].component_->drawTransparent(renderContext);
        }

    }
}
}

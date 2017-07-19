/**
@file ComponentRenderer.cpp
@author t-sakai
@date 2016/10/18 create
*/
#include "ecs/ComponentRenderer.h"
#include "System.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentRendererManager.h"
#include "ecs/Entity.h"
#include "ecs/ComponentGeometric.h"
#include "render/Camera.h"
#include "Application.h"

namespace lfw
{
namespace
{
    inline ComponentRendererManager* getManager()
    {
        return System::getECSManager().getComponentManager<ComponentRendererManager>();
    }
}

    ComponentRenderer::ComponentRenderer()
        :flags_(0)
        ,layer_(Layer_Default)
        ,sortLayer_(0)
    {
    }

    ComponentRenderer::~ComponentRenderer()
    {
    }

    const Entity& ComponentRenderer::getEntity() const
    {
        ComponentRendererManager* componentManager = getManager();
        return componentManager->getData(getID()).entity_;
    }

    Entity& ComponentRenderer::getEntity()
    {
        ComponentRendererManager* componentManager = getManager();
        return componentManager->getData(getID()).entity_;
    }

    u32 ComponentRenderer::getType() const
    {
        ComponentRendererManager* componentManager = getManager();
        return componentManager->getData(getID()).type();
    }

    void ComponentRenderer::onCreate()
    {
    }

    void ComponentRenderer::onStart()
    {
    }

    void ComponentRenderer::update()
    {
    }

    void ComponentRenderer::postUpdate()
    {

    }

    void ComponentRenderer::onDestroy()
    {
    }

    void ComponentRenderer::drawDepth(RenderContext&)
    {
    }

    void ComponentRenderer::drawGBuffer(RenderContext&)
    {
    }

    void ComponentRenderer::drawOpaque(RenderContext&)
    {
    }

    void ComponentRenderer::drawTransparent(RenderContext&)
    {
    }

    bool ComponentRenderer::addQueue(RenderQueue&)
    {
        return false;
    }

    void ComponentRenderer::getAABB(lmath::lm128& bmin, lmath::lm128& bmax)
    {
        bmin = _mm_set1_ps(lcore::numeric_limits<f32>::maximum());
        bmax = _mm_set1_ps(lcore::numeric_limits<f32>::lowest());
    }
}

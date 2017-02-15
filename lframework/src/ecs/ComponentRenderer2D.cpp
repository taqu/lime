/**
@file ComponentRenderer.cpp
@author t-sakai
@date 2016/10/18 create
*/
#include "ecs/ComponentRenderer2D.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentRenderer2DManager.h"
#include "ecs/Entity.h"
#include "ecs/ComponentGeometric.h"
#include "render/Camera.h"
#include "Application.h"

namespace lfw
{
    namespace
    {
        inline ComponentRenderer2DManager* getManager()
        {
            return ECSManager::getInstance().getComponentManager<ComponentRenderer2DManager>();
        }
    }

    ComponentRenderer2D::ComponentRenderer2D()
        :flags_(0)
        ,layer_(Layer_Default2D)
        ,sortLayer_(0)
    {
    }

    ComponentRenderer2D::~ComponentRenderer2D()
    {
    }

    const Entity& ComponentRenderer2D::getEntity() const
    {
        ComponentRenderer2DManager* componentManager = getManager();
        return componentManager->getData(getID()).entity_;
    }

    Entity ComponentRenderer2D::getEntity()
    {
        ComponentRenderer2DManager* componentManager = getManager();
        return componentManager->getData(getID()).entity_;
    }

    u32 ComponentRenderer2D::getType() const
    {
        ComponentRenderer2DManager* componentManager = getManager();
        return componentManager->getData(getID()).type();
    }

    void ComponentRenderer2D::onCreate()
    {
    }

    void ComponentRenderer2D::onStart()
    {
    }

    void ComponentRenderer2D::update()
    {
    }

    void ComponentRenderer2D::onDestroy()
    {
    }

    void ComponentRenderer2D::addQueue(RenderQueue2D&)
    {
    }

    void ComponentRenderer2D::draw(RenderContext2D&) 
    {
    }
}

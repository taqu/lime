/**
@file ComponentBehavior.cpp
@author t-sakai
@date 2016/09/13 create
*/
#include "ecs/ComponentBehavior.h"
#include "System.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentBehaviorManager.h"
#include "ecs/Entity.h"

namespace lfw
{
namespace
{
    inline ComponentBehaviorManager* getManager()
    {
        return System::getECSManager().getComponentManager<ComponentBehaviorManager>();
    }
}

    ComponentBehavior::ComponentBehavior()
    {
    }

    ComponentBehavior::~ComponentBehavior()
    {
    }

    const Entity& ComponentBehavior::getEntity() const
    {
        ComponentBehaviorManager* componentManager = getManager();
        return componentManager->getData(getID()).entity_;
    }

    Entity& ComponentBehavior::getEntity()
    {
        ComponentBehaviorManager* componentManager = getManager();
        return componentManager->getData(getID()).entity_;
    }

    u32 ComponentBehavior::getType() const
    {
        ComponentBehaviorManager* componentManager = getManager();
        return componentManager->getData(getID()).type();
    }

    void ComponentBehavior::onCreate()
    {
    }

    void ComponentBehavior::onStart()
    {
    }

    void ComponentBehavior::update()
    {
    }

    void ComponentBehavior::onDestroy()
    {
    }
}

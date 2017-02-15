/**
@file ComponentLight.cpp
@author t-sakai
@date 2016/11/29 create
*/
#include "ecs/ComponentLight.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentSceneElementManager.h"
#include "ecs/Entity.h"

namespace lfw
{
    namespace
    {
        inline ComponentSceneElementManager* getManager()
        {
            return ECSManager::getInstance().getComponentManager<ComponentSceneElementManager>();
        }
    }

    ComponentLight::ComponentLight()
    {
    }

    ComponentLight::~ComponentLight()
    {
    }

    const Entity& ComponentLight::getEntity() const
    {
        ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getData(getID()).entity_;
    }

    u32 ComponentLight::getType() const
    {
        ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getData(getID()).type();
    }

    void ComponentLight::onCreate()
    {
    }

    void ComponentLight::onStart()
    {
    }

    void ComponentLight::update()
    {
    }

    void ComponentLight::onDestroy()
    {
    }

    s32 ComponentLight::getTargetLayerMask() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        return componentManager->geLight(getID()).getLayerMask();
    }

    void ComponentLight::setTargetLayerMask(s32 layerMask)
    {
        ComponentSceneElementManager* componentManager = getManager();
        componentManager->geLight(getID()).setLayerMask(layerMask);
    }
}

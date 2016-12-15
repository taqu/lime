/**
@file ComponentLogical.cpp
@author t-sakai
@date 2016/06/27 create
*/
#include "ecs/ComponentLogical.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentLogicalManager.h"
#include "Application.h"

namespace lfw
{
namespace
{
    inline ComponentLogicalManager* getManager()
    {
        return ECSManager::getInstance().getComponentManager<ComponentLogicalManager>();
    }
}

    ComponentLogical* ComponentLogical::find(const Char* path)
    {
        ComponentLogicalManager* componentManager = getManager();
        return componentManager->find(path);
    }

    ComponentLogical::ComponentLogical()
    {
    }

    ComponentLogical::ComponentLogical(ID id)
        :Component(id)
    {
    }


    ComponentLogical::~ComponentLogical()
    {
    }

    const Entity& ComponentLogical::getEntity() const
    {
        const ComponentLogicalManager* componentManager = getManager();
        return componentManager->getEntity(getID());
    }

    const NameString& ComponentLogical::getName() const
    {
        const ComponentLogicalManager* componentManager = getManager();
        return componentManager->getName(getID());
    }

    NameString& ComponentLogical::getName()
    {
        ComponentLogicalManager* componentManager = getManager();
        return componentManager->getName(getID());
    }

    u16 ComponentLogical::getNumChildren() const
    {
        ComponentLogicalManager* componentManager = getManager();
        ECSNode& node = componentManager->getNode(getID());
        return node.numChildren();
    }

    ComponentLogical::ConstIterator ComponentLogical::beginChild() const
    {
        const ComponentLogicalManager* componentManager = getManager();
        return componentManager->beginChildren(getID());
    }

    ComponentLogical::Iterator ComponentLogical::beginChild()
    {
        ComponentLogicalManager* componentManager = getManager();
        return componentManager->beginChildren(getID());
    }

    ComponentLogical* ComponentLogical::getParent()
    {
        ComponentLogicalManager* componentManager = getManager();
        ECSNode& node = componentManager->getNode(getID());
        ID id(node.parent());
        return componentManager->get(id);
    }

    void ComponentLogical::setParent(ComponentLogical* parent)
    {
        ComponentLogicalManager* componentManager = getManager();
        componentManager->setParent(getID(), parent);
    }

    void ComponentLogical::setFirstSibling()
    {
        ComponentLogicalManager* componentManager = getManager();
        componentManager->setFirstSibling(getID());
    }

    void ComponentLogical::setLastSibling()
    {
        ComponentLogicalManager* componentManager = getManager();
        componentManager->setLastSibling(getID());
    }

    ECSNode ComponentLogical::getECSNode() const
    {
        ComponentLogicalManager* componentManager = getManager();
        return componentManager->getNode(getID());
    }
}

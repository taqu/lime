/**
@file ComponentGeometric.cpp
@author t-sakai
@date 2016/06/27 create
*/
#include "ecs/ComponentGeometric.h"
#include "System.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentGeometricManager.h"

namespace lfw
{
namespace
{
    inline ComponentGeometricManager* getManager()
    {
        return System::getECSManager().getComponentManager<ComponentGeometricManager>();
    }
}

    ComponentGeometric* ComponentGeometric::find(const Char* path)
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->find(path);
    }

    ComponentGeometric::ComponentGeometric()
    {
    }

    ComponentGeometric::ComponentGeometric(ID id)
        :Component(id)
    {
    }


    ComponentGeometric::~ComponentGeometric()
    {
    }

    const Entity& ComponentGeometric::getEntity() const
    {
        const ComponentGeometricManager* componentManager = getManager();
        return componentManager->getEntity(getID());
    }

    Entity ComponentGeometric::getEntity()
    {
        const ComponentGeometricManager* componentManager = getManager();
        return componentManager->getEntity(getID());
    }

    const NameString& ComponentGeometric::getName() const
    {
        const ComponentGeometricManager* componentManager = getManager();
        return componentManager->getName(getID());
    }

    NameString& ComponentGeometric::getName()
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->getName(getID());
    }

    u16 ComponentGeometric::getNumChildren() const
    {
        ComponentGeometricManager* componentManager = getManager();
        ECSNode& node = componentManager->getNode(getID());
        return node.numChildren();
    }

    ComponentGeometric::ConstIterator ComponentGeometric::beginChild() const
    {
        const ComponentGeometricManager* componentManager = getManager();
        return componentManager->beginChildren(getID());
    }

    ComponentGeometric::Iterator ComponentGeometric::beginChild()
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->beginChildren(getID());
    }

    ComponentGeometric* ComponentGeometric::getParent()
    {
        ComponentGeometricManager* componentManager = getManager();
        ECSNode& node = componentManager->getNode(getID());
        return componentManager->get(ID::construct(node.parent()));
    }

    void ComponentGeometric::setParent(ComponentGeometric* parent)
    {
        ComponentGeometricManager* componentManager = getManager();
        componentManager->setParent(getID(), parent);
    }

    void ComponentGeometric::setFirstSibling()
    {
        ComponentGeometricManager* componentManager = getManager();
        componentManager->setFirstSibling(getID());
    }

    void ComponentGeometric::setLastSibling()
    {
        ComponentGeometricManager* componentManager = getManager();
        componentManager->setLastSibling(getID());
    }

    ECSNode ComponentGeometric::getECSNode() const
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->getNode(getID());
    }

    //---------------------------------------------------
    const lmath::Vector4& ComponentGeometric::getPosition() const
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->getPosition(getID().index());
    }

    lmath::Vector4& ComponentGeometric::getPosition()
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->getPosition(getID().index());
    }

    void ComponentGeometric::setPosition(const lmath::Vector4& position)
    {
        ComponentGeometricManager* componentManager = getManager();
        componentManager->getPosition(getID().index()) = position;
    }

    const lmath::Quaternion& ComponentGeometric::getRotation() const
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->getRotation(getID().index());
    }

    lmath::Quaternion& ComponentGeometric::getRotation()
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->getRotation(getID().index());
    }

    void ComponentGeometric::setRotation(const lmath::Quaternion& rotation)
    {
        ComponentGeometricManager* componentManager = getManager();
        componentManager->getRotation(getID().index()) = rotation;
    }

    const lmath::Vector3& ComponentGeometric::getScale() const
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->getScale(getID().index());
    }

    lmath::Vector3& ComponentGeometric::getScale()
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->getScale(getID().index());
    }

    void ComponentGeometric::setScale(const lmath::Vector3& scale)
    {
        ComponentGeometricManager* componentManager = getManager();
        componentManager->getScale(getID().index()) = scale;
    }

    const lmath::Matrix44& ComponentGeometric::getMatrix() const
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->getMatrix(getID().index());
    }

    lmath::Matrix44& ComponentGeometric::getMatrix()
    {
        ComponentGeometricManager* componentManager = getManager();
        return componentManager->getMatrix(getID().index());
    }

    void ComponentGeometric::updateMatrix()
    {
        ComponentGeometricManager* componentManager = getManager();
        const lmath::Vector3& scale = componentManager->getScale(getID().index());
        const lmath::Quaternion& rotation = componentManager->getRotation(getID().index());
        const lmath::Vector4& position = componentManager->getPosition(getID().index());
        lmath::Matrix44& matrix = componentManager->getMatrix(getID().index());

        rotation.getMatrix(matrix);
        matrix(0,0) *= scale.x_; matrix(1,1) *= scale.y_; matrix(2,2) *= scale.z_;
        matrix.translate(position);

        ECSNode& node = componentManager->getNode(getID());
        if(ECSNode::Root != node.parent()){
            ComponentGeometric* parent = componentManager->get(ID::construct(node.parent()));
            matrix.mul(parent->getMatrix(), matrix);
        }
    }

    void ComponentGeometric::lookAt(const lmath::Vector3& eye, const lmath::Vector3& at)
    {
        ComponentGeometricManager* componentManager = getManager();
        lmath::Vector4& position = componentManager->getPosition(getID().index());
        lmath::Quaternion& rotation = componentManager->getRotation(getID().index());

        position = lmath::Vector4::construct(eye);
        rotation.lookAt(eye, at);
    }

    void ComponentGeometric::lookAt(const lmath::Vector4& eye, const lmath::Vector4& at)
    {
        ComponentGeometricManager* componentManager = getManager();
        lmath::Vector4& position = componentManager->getPosition(getID().index());
        lmath::Quaternion& rotation = componentManager->getRotation(getID().index());

        position = eye;
        rotation.lookAt(eye, at);
    }
}

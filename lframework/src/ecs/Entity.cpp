/**
@file Entity.cpp
@author t-sakai
@date 2016/05/09 create
*/
#include "ecs/Entity.h"
#include "System.h"
#include "ecs/ECSManager.h"
#include "HandleBasedAllocator.h"
#include "ecs/ComponentLogical.h"
#include "ecs/ComponentGeometric.h"
#include "ecs/ComponentLogicalManager.h"
#include "ecs/ComponentGeometricManager.h"

namespace lfw
{
    //----------------------------------------------------------
    //---
    //--- Entity::Components
    //---
    //----------------------------------------------------------
    void Entity::Components::clear()
    {
        capacity_ = 0;
        size_ = 0;
        offset_ = -1;
    }

    void Entity::Components::destroy()
    {
        if(0<=offset_){
            ECSManager& ecsManager = System::getECSManager();
            HandleBasedAllocator& allocator = ecsManager.getComponentHandleAllocator();
            allocator.deallocate(bufferSize(), offset_);
            clear();
        }
    }

    //ID Entity::Components::get(s16 index) const
    //{
    //    LASSERT(0<=index && index<size_);
    //    const HandleBasedAllocator& allocator = System::getECSManager().getComponentHandleAllocator();
    //    return allocator.get<ID>(bufferSize(), offset_)[index];
    //}

    const ID* Entity::Components::get() const
    {
        const HandleBasedAllocator& allocator = System::getECSManager().getComponentHandleAllocator();
        return allocator.getMaybeNull<ID>(bufferSize(), offset_);
    }

    void Entity::Components::add(ID id)
    {
        LASSERT(0<id.category());
        if(id.category() == ECSCategory_Logical || id.category() == ECSCategory_Geometric){
            LASSERT(false);
            return;
        }
        forceAdd(id);
    }

    void Entity::Components::forceAdd(ID id)
    {
        LASSERT(0<id.category());
        HandleBasedAllocator& allocator = System::getECSManager().getComponentHandleAllocator();

        ID* components = NULL;
        if(capacity_<=size_){
            components = resize();
        }else{
            components = allocator.get<ID>(bufferSize(), offset_);
        }
        components[size_] = id;
        ++size_;
    }

    void Entity::Components::remove(ID id)
    {
        if(id.category() == ECSCategory_Logical || id.category() == ECSCategory_Geometric){
            LASSERT(false);
            return;
        }
        forceRemove(id);
    }

    //void Entity::Components::removeAt(s16 index)
    //{
    //    LASSERT(0<=index && index<size_);

    //    HandleBasedAllocator& allocator = System::getECSManager().getComponentHandleAllocator();
    //    ID* components = allocator.getMaybeNull<ID>(bufferSize(), offset_);
    //    if(components[index].category() == ECSCategory_Logical || components[index].category() == ECSCategory_Geometric){
    //        LASSERT(false);
    //        return;
    //    }
    //    for(s16 j=index+1; j<size_; ++j){
    //        components[j-1] = components[j];
    //    }
    //    --size_;
    //}

    void Entity::Components::forceRemove(ID id)
    {
        HandleBasedAllocator& allocator = System::getECSManager().getComponentHandleAllocator();
        ID* components = allocator.getMaybeNull<ID>(bufferSize(), offset_);
        for(s16 i=0; i<size_; ++i){
            if(id == components[i]){
                for(s16 j=i+1; j<size_; ++j){
                    components[j-1] = components[j];
                }
                --size_;
                break;
            }
        }
    }

    ID* Entity::Components::resize()
    {
        static const s32 expand = HandleBasedAllocator::MinSize/sizeof(ID);

        HandleBasedAllocator& allocator = System::getECSManager().getComponentHandleAllocator();

        s16 capacity = capacity_ + expand;
        s32 nextBufferSize = capacity*sizeof(ID);
        HandleBasedAllocator::Handle handle = allocator.allocate(nextBufferSize);

        s32 prevBufferSize = bufferSize();
        ID* newComponents = allocator.get<ID>(handle.size(), handle.offset());
        ID* components = allocator.getMaybeNull<ID>(prevBufferSize, offset_);
        lcore::memcpy(newComponents, components, prevBufferSize);

        allocator.deallocate(prevBufferSize, offset_);

        capacity_ = capacity;
        offset_ = handle.offset();

        return newComponents;
    }

    //----------------------------------------------------------
    //---
    //--- Entity
    //---
    //----------------------------------------------------------
    bool Entity::checkFlag(u8 flag) const
    {
        ECSManager& manager = System::getECSManager();
        return manager.checkFlag(*this, flag);
    }

    void Entity::setFlag(u8 flag)
    {
        LASSERT(0 == (flag & EntityFlag_Update));
        LASSERT(0 == (flag & EntityFlag_Render));
        ECSManager& manager = System::getECSManager();
        manager.setFlag(*this, flag);
        setTreeDirty();
    }

    void Entity::resetFlag(u8 flag)
    {
        LASSERT(0 == (flag & EntityFlag_Update));
        LASSERT(0 == (flag & EntityFlag_Render));
        ECSManager& manager = System::getECSManager();
        manager.resetFlag(*this, flag);
        setTreeDirty();
    }

    void Entity::addComponent(u8 category, u32 type, Behavior* behavior)
    {
        ECSManager& manager = System::getECSManager();
        manager.addComponent(*this, category, type, behavior);
    }

    const NameString& Entity::getName() const
    {
        ECSManager& manager = System::getECSManager();
        IDConstAccess access = manager.getComponents(*this);
        return (ECSCategory_Logical == access[0].category())
            ? manager.getLogical(*this, access[0])->getName()
            : manager.getGeometric(*this, access[0])->getName();
    }

    NameString& Entity::getName()
    {
        ECSManager& manager = System::getECSManager();
        IDConstAccess access = manager.getComponents(*this);
        return (ECSCategory_Logical == access[0].category())
            ? manager.getLogical(*this, access[0])->getName()
            : manager.getGeometric(*this, access[0])->getName();
    }

    bool Entity::isLogical() const
    {
        ECSManager& manager = System::getECSManager();
        IDConstAccess access = manager.getComponents(*this);
        return ECSCategory_Logical == access[0].category();
    }

    const ComponentLogical* Entity::getLogical() const
    {
        ECSManager& manager = System::getECSManager();
        IDConstAccess access = manager.getComponents(*this);
        LASSERT(ECSCategory_Logical == access[0].category());
        return manager.getLogical(*this, access[0]);
    }

    ComponentLogical* Entity::getLogical()
    {
        ECSManager& manager = System::getECSManager();
        IDConstAccess access = manager.getComponents(*this);
        LASSERT(ECSCategory_Logical == access[0].category());
        return manager.getLogical(*this, access[0]);
    }

    bool Entity::isGeometric() const
    {
        ECSManager& manager = System::getECSManager();
        IDConstAccess access = manager.getComponents(*this);
        return ECSCategory_Geometric == access[0].category();
    }

    const ComponentGeometric* Entity::getGeometric() const
    {
        ECSManager& manager = System::getECSManager();
        IDConstAccess access = manager.getComponents(*this);
        LASSERT(ECSCategory_Geometric == access[0].category());
        return manager.getGeometric(*this, access[0]);
    }

    ComponentGeometric* Entity::getGeometric()
    {
        ECSManager& manager = System::getECSManager();
        IDConstAccess access = manager.getComponents(*this);
        LASSERT(ECSCategory_Geometric == access[0].category());
        return manager.getGeometric(*this, access[0]);
    }

    IDConstAccess Entity::getComponents() const
    {
        const ECSManager& manager = System::getECSManager();
        return manager.getComponents(*this);
    }

    Behavior* Entity::getComponent(u8 category, u32 type)
    {
        ECSManager& manager = System::getECSManager();
        return manager.getComponent(*this, category, type);
    }

    Behavior* Entity::getComponent(ID id)
    {
        ECSManager& manager = System::getECSManager();
        return manager.getComponent(*this, id);
    }

    void Entity::removeComponent(ID id)
    {
        ECSManager& manager = System::getECSManager();
        Behavior* component = manager.getComponent(*this, id);
        if(NULL != component){
            manager.removeComponent(*this, id);
        }
    }

    void Entity::setTreeDirty()
    {
        ECSManager& manager = System::getECSManager();
        IDConstAccess access = manager.getComponents(*this);
        if(ECSCategory_Geometric == access[0].category()){
            ComponentGeometricManager* geometricManager = manager.getComponentManager<ComponentGeometricManager>();
            geometricManager->setDirty();

        }else{
            ComponentLogicalManager* logicalManager = manager.getComponentManager<ComponentLogicalManager>();
            logicalManager->setDirty();
        }
    }
}

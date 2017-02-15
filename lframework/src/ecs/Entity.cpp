/**
@file Entity.cpp
@author t-sakai
@date 2016/05/09 create
*/
#include "ecs/Entity.h"
#include "ecs/ECSManager.h"
#include "HandleBasedAllocator.h"
#include "Application.h"
#include "ecs/ComponentLogical.h"
#include "ecs/ComponentGeometric.h"

namespace lfw
{
    //----------------------------------------------------------
    //---
    //--- Entity::Components
    //---
    //----------------------------------------------------------
    Entity::Components::Components()
        :capacity_(0)
        ,size_(0)
        ,offset_(-1)
    {
    }

    Entity::Components::~Components()
    {
        ECSManager& ecsManager = ECSManager::getInstance();
        HandleBasedAllocator& allocator = ecsManager.getComponentHandleAllocator();
        allocator.deallocate(bufferSize(), offset_);

        capacity_ = 0;
        size_ = 0;
        offset_ = -1;
    }

    ID Entity::Components::get(s16 index) const
    {
        LASSERT(0<=index && index<size_);
        const HandleBasedAllocator& allocator = ECSManager::getInstance().getComponentHandleAllocator();
        return allocator.get<ID>(bufferSize(), offset_)[index];
    }

    const ID* Entity::Components::get() const
    {
        const HandleBasedAllocator& allocator = ECSManager::getInstance().getComponentHandleAllocator();
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
        HandleBasedAllocator& allocator = ECSManager::getInstance().getComponentHandleAllocator();

        ID* components = NULL;
        if(capacity_<=size_){
            components = resize();
        }else{
            components = allocator.get<ID>(bufferSize(), offset_);
        }
        components[size_] = id;
        ++size_;
#ifdef _DEBUG
        for(s32 i=0; i<size_; ++i){
            u8 category = components[i].category();
            LASSERT(0<category && category<255);
            LASSERT(components[i].valid());
        }
#endif
    }

    void Entity::Components::remove(ID id)
    {
        if(id.category() == ECSCategory_Logical || id.category() == ECSCategory_Geometric){
            LASSERT(false);
            return;
        }
        forceRemove(id);
    }

    void Entity::Components::removeAt(s16 index)
    {
        LASSERT(0<=index && index<size_);

        HandleBasedAllocator& allocator = ECSManager::getInstance().getComponentHandleAllocator();
        ID* components = allocator.getMaybeNull<ID>(bufferSize(), offset_);
        if(components[index].category() == ECSCategory_Logical || components[index].category() == ECSCategory_Geometric){
            LASSERT(false);
            return;
        }
        for(s16 j=index+1; j<size_; ++j){
            components[j-1] = components[j];
        }
        --size_;
#ifdef _DEBUG
        for(s32 i=0; i<size_; ++i){
            LASSERT(components[i].valid());
        }
#endif
    }

    void Entity::Components::forceRemove(ID id)
    {
        HandleBasedAllocator& allocator = ECSManager::getInstance().getComponentHandleAllocator();
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
#ifdef _DEBUG
        for(s32 i=0; i<size_; ++i){
            LASSERT(components[i].valid());
        }
#endif
    }

    ID* Entity::Components::resize()
    {
        static const s32 expand = HandleBasedAllocator::MinSize/sizeof(ID);

        HandleBasedAllocator& allocator = ECSManager::getInstance().getComponentHandleAllocator();

        s16 capacity = capacity_ + expand;
        s32 nextBufferSize = capacity*sizeof(ID);
        HandleBasedAllocator::Handle handle = allocator.allocate(nextBufferSize);

        s32 prevBufferSize = bufferSize();
        ID* newComponents = allocator.get<ID>(handle.size(), handle.offset());
        ID* components = allocator.getMaybeNull<ID>(prevBufferSize, offset_);
        lcore::memcpy(newComponents, components, prevBufferSize);

        allocator.deallocate(prevBufferSize, offset_);

#ifdef _DEBUG
        for(s32 i=capacity_; i<capacity; ++i){
            newComponents[i].clear();
        }
#endif
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
        ECSManager& manager = ECSManager::getInstance();
        return manager.checkFlag(*this, flag);
    }

    void Entity::setFlag(u8 flag)
    {
        LASSERT(0 == (flag & EntityFlag_Update));
        LASSERT(0 == (flag & EntityFlag_Render));
        ECSManager& manager = ECSManager::getInstance();
        manager.setFlag(*this, flag);
    }

    void Entity::resetFlag(u8 flag)
    {
        LASSERT(0 == (flag & EntityFlag_Update));
        LASSERT(0 == (flag & EntityFlag_Render));
        ECSManager& manager = ECSManager::getInstance();
        manager.resetFlag(*this, flag);
    }

    void Entity::addComponent(u8 category, u32 type, Behavior* behavior)
    {
        ECSManager& manager = ECSManager::getInstance();
        manager.addComponent(*this, category, type, behavior);
    }

    const NameString& Entity::getName() const
    {
        ECSManager& manager = ECSManager::getInstance();
        IDConstAccess access = manager.getComponents(*this);
        return (ECSCategory_Logical == access[0].category())
            ? manager.getLogical(*this, access[0])->getName()
            : manager.getGeometric(*this, access[0])->getName();
    }

    NameString& Entity::getName()
    {
        ECSManager& manager = ECSManager::getInstance();
        IDConstAccess access = manager.getComponents(*this);
        return (ECSCategory_Logical == access[0].category())
            ? manager.getLogical(*this, access[0])->getName()
            : manager.getGeometric(*this, access[0])->getName();
    }

    bool Entity::isLogical() const
    {
        ECSManager& manager = ECSManager::getInstance();
        IDConstAccess access = manager.getComponents(*this);
        return ECSCategory_Logical == access[0].category();
    }

    const ComponentLogical* Entity::getLogical() const
    {
        ECSManager& manager = ECSManager::getInstance();
        IDConstAccess access = manager.getComponents(*this);
        LASSERT(ECSCategory_Logical == access[0].category());
        return manager.getLogical(*this, access[0]);
    }

    ComponentLogical* Entity::getLogical()
    {
        ECSManager& manager = ECSManager::getInstance();
        IDConstAccess access = manager.getComponents(*this);
        LASSERT(ECSCategory_Logical == access[0].category());
        return manager.getLogical(*this, access[0]);
    }

    bool Entity::isGeometric() const
    {
        ECSManager& manager = ECSManager::getInstance();
        IDConstAccess access = manager.getComponents(*this);
        return ECSCategory_Geometric == access[0].category();
    }

    const ComponentGeometric* Entity::getGeometric() const
    {
        ECSManager& manager = ECSManager::getInstance();
        IDConstAccess access = manager.getComponents(*this);
        LASSERT(ECSCategory_Geometric == access[0].category());
        return manager.getGeometric(*this, access[0]);
    }

    ComponentGeometric* Entity::getGeometric()
    {
        ECSManager& manager = ECSManager::getInstance();
        IDConstAccess access = manager.getComponents(*this);
        LASSERT(ECSCategory_Geometric == access[0].category());
        return manager.getGeometric(*this, access[0]);
    }

    IDConstAccess Entity::getComponents() const
    {
        const ECSManager& manager = ECSManager::getInstance();
        return manager.getComponents(*this);
    }

    Behavior* Entity::getComponent(u8 category, u32 type)
    {
        ECSManager& manager = ECSManager::getInstance();
        return manager.getComponent(*this, category, type);
    }

    Behavior* Entity::getComponent(ID id)
    {
        ECSManager& manager = ECSManager::getInstance();
        return manager.getComponent(*this, id);
    }

    void Entity::removeComponent(ID id)
    {
        ECSManager& manager = ECSManager::getInstance();
        Behavior* component = manager.getComponent(*this, id);
        if(NULL != component){
            manager.removeComponent(*this, id);
        }
    }
}

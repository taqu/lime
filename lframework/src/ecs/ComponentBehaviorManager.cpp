/**
@file ComponentBehaviorManager.cpp
@author t-sakai
@date 2016/09/13 create
*/
#include "ecs/ComponentBehaviorManager.h"
#include "ecs/ComponentBehavior.h"
#include "ecs/ECSManager.h"

namespace lfw
{
    ComponentBehaviorManager::ComponentBehaviorManager(s32 capacity, s32 expandSize)
        :ids_(capacity, expandSize)
        ,components_(NULL)
        ,data_(NULL)
    {
        LASSERT(0<=capacity);
        components_ = LNEW ComponentBehavior*[capacity];
        lcore::memset(components_, 0, sizeof(ComponentBehavior*)*capacity);

        data_ = LNEW Data[capacity];
    }

    ComponentBehaviorManager::~ComponentBehaviorManager()
    {
        LDELETE_ARRAY(data_);

        for(s32 i=0; i<ids_.capacity(); ++i){
            LDELETE(components_[i]);
        }
        LDELETE_ARRAY(components_);
    }

    u8 ComponentBehaviorManager::category() const
    {
        return Category;
    }

    ID ComponentBehaviorManager::create(Entity entity, u32 type, ComponentBehavior* component)
    {
        LASSERT(NULL != component);
        s32 capacity = ids_.capacity();
        ID id = ids_.create(Category);
        component->setID(id);
        if(capacity<ids_.capacity()){
            expand(components_, capacity, ids_.capacity());
            lcore::memset(components_+capacity, 0, sizeof(ComponentBehavior*)*(ids_.capacity()-capacity));
            expand(data_, capacity, ids_.capacity());
        }
        LDELETE(components_[id.index()]);

        s32 index = id.index();
        components_[index] = component;
        data_[index].entity_ = entity;
        data_[index].typeFlags_ = 0;
        data_[index].clear(type);
        if(NULL != component){
            component->onCreate();
        }
        return id;
    }

    void ComponentBehaviorManager::destroy(ID id)
    {
        LASSERT(id.valid());
        u16 index = id.index();
        if(NULL != components_[index]){
            components_[index]->onDestroy();
            LDELETE(components_[index]);
        }
        ids_.destroy(ID(0, index));
    }

    Behavior* ComponentBehaviorManager::getBehavior(ID id)
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        return components_[id.index()];
    }

    void ComponentBehaviorManager::initialize()
    {
    }

    void ComponentBehaviorManager::terminate()
    {
    }

    void ComponentBehaviorManager::updateComponent(ID id)
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        LASSERT(NULL != components_[id.index()]);

        ComponentBehavior* behavior = components_[id.index()];
        Data& data = data_[id.index()];
        if(!data.checkFlag(CommonFlag_Start)){
            behavior->onStart();
            data.setFlag(CommonFlag_Start);
        }
        behavior->update();
    }
}

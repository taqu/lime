/**
@file ComponentRendererManager.cpp
@author t-sakai
@date 2016/10/18 create
*/
#include "ecs/ComponentRendererManager.h"
#include "ecs/ComponentRenderer.h"
#include "ecs/ECSManager.h"

namespace lfw
{
    ComponentRendererManager::ComponentRendererManager(s32 capacity, s32 expandSize)
        :ids_(capacity, expandSize)
        ,components_(NULL)
        ,data_(NULL)
    {
        LASSERT(0<=capacity);
        components_ = LNEW ComponentRenderer*[capacity];
        lcore::memset(components_, 0, sizeof(ComponentRenderer*)*capacity);

        data_ = LNEW Data[capacity];
    }

    ComponentRendererManager::~ComponentRendererManager()
    {
        LDELETE_ARRAY(data_);

        for(s32 i=0; i<ids_.capacity(); ++i){
            LDELETE(components_[i]);
        }
        LDELETE_ARRAY(components_);
    }

    u8 ComponentRendererManager::category() const
    {
        return Category;
    }

    ID ComponentRendererManager::create(Entity entity, u32 type, ComponentRenderer* component)
    {
        LASSERT(NULL != component);
        s32 capacity = ids_.capacity();
        ID id = ids_.create(Category);
        component->setID(id);
        if(capacity<ids_.capacity()){
            expand(components_, capacity, ids_.capacity());
            lcore::memset(components_+capacity, 0, sizeof(ComponentRenderer*)*(ids_.capacity()-capacity));

            expand(data_, capacity, ids_.capacity());
        }
        LDELETE(components_[id.index()]);

        s32 index = id.index();
        components_[index] = component;
        data_[index].entity_ = entity;
        data_[index].clear(type);
        if(NULL != component){
            component->onCreate();
        }
        return id;
    }

    void ComponentRendererManager::destroy(ID id)
    {
        LASSERT(id.valid());
        u16 index = id.index();
        if(NULL != components_[index]){
            components_[index]->onDestroy();
            LDELETE(components_[index]);
        }
        ids_.destroy(ID(0, index));
    }

    Behavior* ComponentRendererManager::getBehavior(ID id)
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        return components_[id.index()];
    }

    void ComponentRendererManager::initialize()
    {
    }

    void ComponentRendererManager::terminate()
    {
    }

    void ComponentRendererManager::preUpdateComponents()
    {
        componentRenderers_.clear();
    }

    void ComponentRendererManager::updateComponent(ID id)
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        LASSERT(NULL != components_[id.index()]);

        ComponentRenderer* renderer = components_[id.index()];
        Data& data = data_[id.index()];
        if(!data.checkFlag(CommonFlag_Start)){
            renderer->onStart();
            data.setFlag(CommonFlag_Start);
        }
        renderer->update();
        componentRenderers_.push_back(renderer);
    }

    void ComponentRendererManager::postUpdateComponents()
    {
        for(s32 i=0; i<ids_.capacity(); ++i){
            if(NULL == components_[i]){
                continue;
            }
            components_[i]->postUpdate();
        }
    }
}

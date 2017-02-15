/**
@file ComponentRenderer2DManager.cpp
@author t-sakai
@date 2016/12/20 create
*/
#include "ecs/ComponentRenderer2DManager.h"
#include <lmath/Vector4.h>
#include "ecs/ComponentRenderer2D.h"
#include "ecs/ECSManager.h"

namespace lfw
{
    ComponentRenderer2DManager::ComponentRenderer2DManager(s32 capacity, s32 expandSize)
        :ids_(capacity, expandSize)
        ,components_(NULL)
        ,data_(NULL)
    {
        LASSERT(0<=capacity);
        components_ = LNEW ComponentRenderer2D*[capacity];
        lcore::memset(components_, 0, sizeof(ComponentRenderer2D*)*capacity);

        data_ = LNEW Data[capacity];
    }

    ComponentRenderer2DManager::~ComponentRenderer2DManager()
    {
        LDELETE_ARRAY(data_);

        for(s32 i=0; i<ids_.capacity(); ++i){
            LDELETE(components_[i]);
        }
        LDELETE_ARRAY(components_);
    }

    u8 ComponentRenderer2DManager::category() const
    {
        return Category;
    }

    ID ComponentRenderer2DManager::create(Entity entity, u32 type, ComponentRenderer2D* component)
    {
        LASSERT(NULL != component);
        s32 capacity = ids_.capacity();
        ID id = ids_.create(Category);
        component->setID(id);
        if(capacity<ids_.capacity()){
            expand(components_, capacity, ids_.capacity());
            lcore::memset(components_+capacity, 0, sizeof(ComponentRenderer2D*)*(ids_.capacity()-capacity));

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

    void ComponentRenderer2DManager::destroy(ID id)
    {
        LASSERT(id.valid());
        u16 index = id.index();
        if(NULL != components_[index]){
            components_[index]->onDestroy();
            LDELETE(components_[index]);
        }
        ids_.destroy(ID::construct(0, index));
    }

    Behavior* ComponentRenderer2DManager::getBehavior(ID id)
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        return components_[id.index()];
    }

    void ComponentRenderer2DManager::initialize()
    {
    }

    void ComponentRenderer2DManager::terminate()
    {
    }

    void ComponentRenderer2DManager::preUpdateComponents()
    {
        componentRenderers_.clear();
    }

    void ComponentRenderer2DManager::updateComponent(ID id)
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        LASSERT(NULL != components_[id.index()]);

        ComponentRenderer2D* renderer = components_[id.index()];
        Data& data = data_[id.index()];
        if(!data.checkFlag(CommonFlag_Start)){
            renderer->onStart();
            data.setFlag(CommonFlag_Start);
        }
        renderer->update();
    }

    void ComponentRenderer2DManager::postUpdateComponents()
    {
        for(s32 i=0; i<ids_.capacity(); ++i){
            if(NULL == components_[i]){
                continue;
            }
            componentRenderers_.push_back(components_[i]);
        }
    }
}

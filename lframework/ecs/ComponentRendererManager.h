#ifndef INC_LFRAMEWORK_COMPONENTRENDERERMANAGER_H__
#define INC_LFRAMEWORK_COMPONENTRENDERERMANAGER_H__
/**
@file ComponentRendererManager.h
@author t-sakai
@date 2016/10/18 create
*/
#include "ComponentManager.h"
#include <lcore/BitSet.h>

namespace lfw
{
    class ComponentRenderer;

    class ComponentRendererManager : public ComponentManager
    {
    public:
        static const u8 Category = ECSCategory_Renderer;

        typedef lcore::ArrayPOD<ComponentRenderer*, lcore::array_static_inc_size<128> > ComponentRendererArray;

        struct Data : public ComponentDataCommon<Data>
        {

        };

        ComponentRendererManager(s32 capacity=DefaultComponentManagerCapacity, s32 expandSize=DefaultComponentManagerExpandSize);

        virtual ~ComponentRendererManager();

        virtual u8 category() const;

        ID create(Entity entity, u32 type, ComponentRenderer* component);
        virtual void destroy(ID id);

        virtual Behavior* getBehavior(ID id);
        virtual void initialize();
        virtual void terminate();

        void preUpdateComponents();
        virtual void updateComponent(ID id);
        void postUpdateComponents();

        inline const Data& getData(ID id) const;
        inline Data& getData(ID id);

        inline ComponentRendererArray& getComponents();
    protected:
        ComponentRendererManager(const ComponentRendererManager&);
        ComponentRendererManager& operator=(const ComponentRendererManager&);

        IDTable ids_;
        ComponentRenderer** components_;
        Data* data_;

        ComponentRendererArray componentRenderers_;
    };

    inline const ComponentRendererManager::Data& ComponentRendererManager::getData(ID id) const
    {
        return data_[id.index()];
    }

    inline ComponentRendererManager::Data& ComponentRendererManager::getData(ID id)
    {
        return data_[id.index()];
    }

    inline ComponentRendererManager::ComponentRendererArray& ComponentRendererManager::getComponents()
    {
        return componentRenderers_;
    }
}
#endif //INC_LFRAMEWORK_COMPONENTRENDERERMANAGER_H__

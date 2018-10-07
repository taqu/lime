#ifndef INC_LFRAMEWORK_COMPONENTBEHAVIORMANAGER_H_
#define INC_LFRAMEWORK_COMPONENTBEHAVIORMANAGER_H_
/**
@file ComponentBehaviorManager.h
@author t-sakai
@date 2016/09/13 create
*/
#include "ComponentManager.h"

namespace lfw
{
    class ComponentBehavior;

    class ComponentBehaviorManager : public ComponentManager
    {
    public:
        static const u8 Category = ECSCategory_Behavior;

        struct Data : public ComponentDataCommon<Data>
        {
        };

        ComponentBehaviorManager(s32 capacity=DefaultComponentManagerCapacity, s32 expandSize=DefaultComponentManagerExpandSize);

        virtual ~ComponentBehaviorManager();

        virtual u8 category() const;

        ID create(Entity entity, u32 type, ComponentBehavior* component);
        virtual void destroy(ID id);

        virtual Behavior* getBehavior(ID id);
        virtual void initialize();
        virtual void terminate();

        virtual void updateComponent(ID id);

        inline const Data& getData(ID id) const;
        inline Data& getData(ID id);
    protected:
        ComponentBehaviorManager(const ComponentBehaviorManager&);
        ComponentBehaviorManager& operator=(const ComponentBehaviorManager&);

        IDTable ids_;
        ComponentBehavior** components_;
        Data* data_;
    };

    inline const ComponentBehaviorManager::Data& ComponentBehaviorManager::getData(ID id) const
    {
        return data_[id.index()];
    }

    inline ComponentBehaviorManager::Data& ComponentBehaviorManager::getData(ID id)
    {
        return data_[id.index()];
    }
}
#endif //INC_LFRAMEWORK_COMPONENTBEHAVIORMANAGER_H_

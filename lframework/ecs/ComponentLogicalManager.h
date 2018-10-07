#ifndef INC_LFRAMEWORK_COMPONENTLOGICALMANAGER_H_
#define INC_LFRAMEWORK_COMPONENTLOGICALMANAGER_H_
/**
@file ComponentLogicalManager.h
@author t-sakai
@date 2016/08/11 create
*/
#include "ComponentTreeManager.h"
#include "ComponentLogical.h"

namespace lfw
{
    class ComponentLogicalManager : public ComponentTreeManager<ComponentLogical>
    {
    public:
        typedef ComponentTreeManager<ComponentLogical> parent_type;
        using parent_type::iterator;
        using parent_type::const_iterator;

        using parent_type::EntityArray;

        static const u8 Category = ECSCategory_Logical;

        ComponentLogicalManager(s32 capacity=DefaultComponentManagerCapacity, s32 expandSize=DefaultComponentManagerExpandSize);
        virtual ~ComponentLogicalManager();

        virtual u8 category() const;

        virtual ID create(Entity entity);

        virtual void initialize();
        virtual void terminate();

    protected:
        ComponentLogicalManager(const ComponentLogicalManager&);
        ComponentLogicalManager& operator=(const ComponentLogicalManager&);
    };
}
#endif //INC_LFRAMEWORK_COMPONENTLOGICALMANAGER_H_

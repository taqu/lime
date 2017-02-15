#ifndef INC_LFRAMEWORK_COMPONENTBEHAVIOR_H__
#define INC_LFRAMEWORK_COMPONENTBEHAVIOR_H__
/**
@file ComponentBehavior.h
@author t-sakai
@date 2016/09/13 create
*/
#include "Component.h"

namespace lfw
{
    class Entity;

    class ComponentBehavior : public Behavior
    {
    public:
        static u8 category(){ return ECSCategory_Behavior;}
        static u32 type(){ return ECSType_Behavior;}

        virtual ~ComponentBehavior();

        Entity& getEntity();
        const Entity& getEntity() const;
        virtual u32 getType() const;

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();

    protected:
        ComponentBehavior(const ComponentBehavior&);
        ComponentBehavior& operator=(const ComponentBehavior&);

        ComponentBehavior();
    };
}
#endif //INC_LFRAMEWORK_COMPONENTBEHAVIOR_H__

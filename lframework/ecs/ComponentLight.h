#ifndef INC_LFRAMEWORK_COMPONENTLIGHT_H_
#define INC_LFRAMEWORK_COMPONENTLIGHT_H_
/**
@file ComponentLight.h
@author t-sakai
@date 2016/11/29 create
*/
#include "Component.h"

namespace lfw
{
    class Entity;
    class Light;

    class ComponentLight : public Behavior
    {
    public:
        static u8 category(){ return ECSCategory_SceneElement;}
        static u32 type(){ return ECSType_Light;}

        ComponentLight();
        virtual ~ComponentLight();

        const Entity& getEntity() const;
        virtual u32 getType() const;

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();

        const Light& getLight() const;
        Light& getLight();
    protected:
        ComponentLight(const ComponentLight&);
        ComponentLight& operator=(const ComponentLight&);
    };
}
#endif //INC_LFRAMEWORK_COMPONENTLIGHT_H_

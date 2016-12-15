#ifndef INC_LFRAMEWORK_COMPONENTLIGHT_H__
#define INC_LFRAMEWORK_COMPONENTLIGHT_H__
/**
@file ComponentLight.h
@author t-sakai
@date 2016/11/29 create
*/
#include "Component.h"

namespace lfw
{
    class Entity;

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

        s32 getTargetLayerMask() const;
        void setTargetLayerMask(s32 layerMask);
    protected:
        ComponentLight(const ComponentLight&);
        ComponentLight& operator=(const ComponentLight&);
    };
}
#endif //INC_LFRAMEWORK_COMPONENTLIGHT_H__

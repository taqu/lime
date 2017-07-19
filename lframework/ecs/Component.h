#ifndef INC_LFRAMEWORK_COMPONENT_H__
#define INC_LFRAMEWORK_COMPONENT_H__
/**
@file Component.h
@author t-sakai
@date 2016/05/09 create
*/
#include "lecs.h"

namespace lfw
{
    class Entity;

    class Component
    {
    public:
        inline ID getID() const{ return id_;}
        inline void setID(ID id){ id_ = id;}
    protected:
        Component()
        {
            id_ = ID::construct();
        }

        explicit Component(ID id)
            :id_(id)
        {}

        ~Component()
        {}
    private:
        ID id_;
    };

    class Behavior : public Component
    {
    public:
        virtual ~Behavior()
        {}

        virtual u32 getType() const =0;
        virtual void onCreate() =0;
        virtual void onStart() =0;
        virtual void update() =0;
        virtual void onDestroy() =0;

    protected:
        Behavior()
        {}
        explicit Behavior(ID id)
            :Component(id)
        {}
    };
}
#endif //INC_LFRAMEWORK_COMPONENT_H__

#ifndef INC_SAMPLES_BEHAVIOR00_H_
#define INC_SAMPLES_BEHAVIOR00_H_
/**
@file Behavior00.h
@author t-sakai
@date 2016/11/29 create
*/
#include <lframework/ecs/ComponentBehavior.h>

namespace lfw
{
    class ComponentCamera;
    class ComponentMeshRenderer;
}

namespace debug
{
    class Behavior00 : public lfw::ComponentBehavior
    {
    public:
        Behavior00();
        virtual ~Behavior00();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();

    private:
        lfw::ComponentCamera* camera_;
        lfw::ComponentMeshRenderer* mesh_;
        lfw::f32 time_;
    };
}
#endif //INC_SAMPLES_BEHAVIOR00_H_

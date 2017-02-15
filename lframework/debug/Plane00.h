#ifndef INC_SAMPLES_PLANE00_H__
#define INC_SAMPLES_PLANE00_H__
/**
@file Plane00.h
@author t-sakai
@date 2017/02/14 create
*/
#include <lframework/ecs/ComponentBehavior.h>

namespace lfw
{
    class ResourceModel;
    class ComponentMeshRenderer;
}

namespace debug
{
    class Plane00 : public lfw::ComponentBehavior
    {
    public:
        Plane00();
        virtual ~Plane00();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();

    private:
        lfw::ResourceModel* model_;
        lfw::ComponentMeshRenderer* mesh_;
    };
}
#endif //INC_SAMPLES_PLANE00_H__

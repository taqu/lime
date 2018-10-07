#ifndef INC_SAMPLES_CAMERA00_H_
#define INC_SAMPLES_CAMERA00_H_
/**
@file MainCamera00.h
@author t-sakai
@date 2016/12/09 create
*/
#include <lframework/ecs/ComponentCamera.h>

namespace debug
{
    class RenderPassProcedural;

    class MainCamera00 : public lfw::ComponentCamera
    {
    public:
        MainCamera00();
        virtual ~MainCamera00();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();

        virtual void resetRenderPasses();

    private:
        RenderPassProcedural* renderPass_;
    };
}
#endif //INC_SAMPLES_CAMERA00_H_

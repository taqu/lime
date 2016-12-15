#ifndef INC_SAMPLES_CAMERA00_H__
#define INC_SAMPLES_CAMERA00_H__
/**
@file MainCamera00.h
@author t-sakai
@date 2016/12/09 create
*/
#include <lframework/ecs/ComponentCamera.h>

namespace debug
{
    class MainCamera00 : public lfw::ComponentCamera
    {
    public:
        MainCamera00();
        virtual ~MainCamera00();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();
    };
}
#endif //INC_SAMPLES_CAMERA00_H__

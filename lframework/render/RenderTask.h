#ifndef INC_LFRAMEWORK_RENDERERTASK__
#define INC_LFRAMEWORK_RENDERERTASK__
/**
@file RenderTask.h
@author t-sakai
@date 2017/02/03 create
*/
#include "../lframework.h"
#include "../ecs/ComponentSceneElementManager.h"

namespace lfw
{
    class RenderContext;
    class RenderQueue;
    class RenderContext2D;
    class RenderQueue2D;

    struct RenderTask
    {
        ComponentSceneElementManager::LightArray* lights_;
        RenderContext* renderContext_;
        RenderQueue* renderQueue_;
    };

    struct RenderTask2D
    {
        ComponentSceneElementManager::CameraArray* cameras_;
        RenderContext2D* renderContext_;
        RenderQueue2D* renderQueue_;
    };
}
#endif //INC_LFRAMEWORK_RENDERERTASK__

#ifndef INC_LFRAMEWORK_RENDERERINITPARAM__
#define INC_LFRAMEWORK_RENDERERINITPARAM__
/**
@file RendererInitParam.h
@author t-sakai
@date 2016/11/12 create
*/
#include "../lframework.h"

namespace lfw
{
    struct RendererInitParam
    {
        s32 numSyncFrames_;
        s32 shadowMapSize_;
        f32 shadowMapZNear_;
        f32 shadowMapZFar_;

        RendererInitParam()
            :numSyncFrames_(3)
            ,shadowMapSize_(1024)
            ,shadowMapZNear_(0.1f)
            ,shadowMapZFar_(100.0f)
        {}
    };
}
#endif //INC_LFRAMEWORK_RENDERERINITPARAM__

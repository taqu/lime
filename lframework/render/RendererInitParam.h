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
        s32 renderType_;
        s32 renderWidth_;
        s32 renderHeight_;
        s32 shadowMapSize_;
        f32 shadowMapZNear_;
        f32 shadowMapZFar_;
        s32 max2DVerticesPerFrame_;
        s32 max2DIndicesPerFrame_;

        RendererInitParam()
            :numSyncFrames_(3)
            ,renderType_(RenderType_Deferred)
            ,renderWidth_(0)
            ,renderHeight_(0)
            ,shadowMapSize_(1024)
            ,shadowMapZNear_(0.1f)
            ,shadowMapZFar_(100.0f)
            ,max2DVerticesPerFrame_(4096)
            ,max2DIndicesPerFrame_(4096)
        {}
    };
}
#endif //INC_LFRAMEWORK_RENDERERINITPARAM__

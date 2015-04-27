﻿#ifndef INC_LFRAMEWORK_SCENE_CPP__
#define INC_LFRAMEWORK_SCENE_CPP__
/**
@file Scene.cpp
@author t-sakai
@date 2012/05/06 create
*/
#include "Scene.h"

#include <lmath/lmathutil.h>
#include <lframework/scene/Frustum.h>

namespace lscene
{
    Scene::Scene()
    {
        u8* p = lcore::align16(buffer_);
        camera_ = reinterpret_cast<Camera*>(p);
        lightEnv_ = reinterpret_cast<LightEnvironment*>(p + sizeof(Camera));
    }

    Scene::~Scene()
    {
    }
}
#endif //INC_LFRAMEWORK_SCENE_CPP__

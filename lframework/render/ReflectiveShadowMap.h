#ifndef INC_LFRAMEWORK_REFLECTIVESHADOWMAP_H_
#define INC_LFRAMEWORK_REFLECTIVESHADOWMAP_H_
/**
@file ReflectiveShadowMap.h
@author t-sakai
@date 2016/12/03 create
*/
#include <lmath/Vector4.h>
#include <lmath/Matrix44.h>
#include "../lframework.h"

namespace lfw
{
    class Light;

    class ReflectiveShadowMap
    {
    public:
        static const u8 FitNearFar_None = 0;
        static const s8 FitNearFar_AABB = 1;
        static const s8 FitNearFar_SceneAABB = 2;

        ReflectiveShadowMap();
        ~ReflectiveShadowMap();

        void initialize(s32 resolution);
        void update(const Light& light);


        s32 getResolution() const
        {
            return resolution_;
        }

        void setSceneAABB(const lmath::Vector4& aabbMin, const lmath::Vector4& aabbMax)
        {
            sceneAABBMin_ = aabbMin;
            sceneAABBMax_ = aabbMax;
        }

        const lmath::Matrix44& getLightProjection() const
        {
            return lightProjection_;
        }

    private:
        lmath::Matrix44 lightProjection_;
        lmath::Vector4 sceneAABBMin_;
        lmath::Vector4 sceneAABBMax_;

        s32 resolution_;

        void createSceneAABBPoints(lmath::Vector4 dst[8]);
    };
}

#endif //INC_LFRAMEWORK_REFLECTIVESHADOWMAP_H_

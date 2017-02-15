/**
@file ReflectiveShadowMap.cpp
@author t-sakai
@date 2016/12/03 create
*/
#include "render/ReflectiveShadowMap.h"
#include "render/Frustum.h"
#include "render/Light.h"

namespace lfw
{
    ReflectiveShadowMap::ReflectiveShadowMap()
        :resolution_(1)
    {
        sceneAABBMin_.zero();
        sceneAABBMax_.zero();
    }

    ReflectiveShadowMap::~ReflectiveShadowMap()
    {
    }

    void ReflectiveShadowMap::initialize(s32 resolution)
    {
        resolution_ = resolution;
    }

    void ReflectiveShadowMap::update(const Light& light)
    {
        lmath::Matrix44 lightView;
        light.getLightView(lightView);
        lmath::Vector4 sceneAABBPointsInLightView[8];
        createSceneAABBPoints(sceneAABBPointsInLightView);

        lmath::Vector4 lightViewOrthoMin;
        lmath::Vector4 lightViewOrthoMax;
        lightViewOrthoMin.set(FLT_MAX, FLT_MAX, FLT_MAX, 1.0f);
        lightViewOrthoMax.set(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1.0f);

        for(s32 i=0; i<8; ++i){
            sceneAABBPointsInLightView[i] = lmath::Vector4::construct(mul(lightView, sceneAABBPointsInLightView[i]));

            lightViewOrthoMin = lmath::Vector4::construct(minimum(lightViewOrthoMin, sceneAABBPointsInLightView[i]));
            lightViewOrthoMax = lmath::Vector4::construct(maximum(lightViewOrthoMax, sceneAABBPointsInLightView[i]));
        }


        f32 invResolution = 1.0f/resolution_;
        lmath::Vector4 vInvResolution = lmath::Vector4::construct(invResolution, invResolution, 0.0f, 0.0f);

        lmath::lm128 worldUnitsPerTexel = sub(lightViewOrthoMax, lightViewOrthoMin);
        worldUnitsPerTexel = worldUnitsPerTexel * vInvResolution;

        lmath::lm128 tmpLightViewOrthoMin = lightViewOrthoMin / worldUnitsPerTexel;
        tmpLightViewOrthoMin = floor(tmpLightViewOrthoMin);
        lightViewOrthoMin = lmath::Vector4::construct(tmpLightViewOrthoMin * worldUnitsPerTexel);

        lmath::lm128 tmpLightViewOrthoMax = lightViewOrthoMax / worldUnitsPerTexel;
        tmpLightViewOrthoMax = floor(tmpLightViewOrthoMax);
        lightViewOrthoMax = lmath::Vector4::construct(tmpLightViewOrthoMax * worldUnitsPerTexel);

        f32 nearPlane = FLT_MAX;
        f32 farPlane = -FLT_MAX;
        for(s32 i=0; i<8; ++i){
            nearPlane = lcore::minimum(sceneAABBPointsInLightView[i].z_, nearPlane);
            farPlane = lcore::maximum(sceneAABBPointsInLightView[i].z_, farPlane);
        }

        lightProjection_.orthoOffsetCenter(
                lightViewOrthoMin.x_,
                lightViewOrthoMax.x_,
                lightViewOrthoMax.y_,
                lightViewOrthoMin.y_,
                nearPlane,
                farPlane);
    }

    void ReflectiveShadowMap::createSceneAABBPoints(lmath::Vector4 dst[8])
    {
        static const lmath::Vector4 map[] = 
        { 
            lmath::Vector4::construct(0.5f, 0.5f, -0.5f, 0.5f),
            lmath::Vector4::construct(-0.5f, 0.5f, -0.5f, 0.5f),
            lmath::Vector4::construct(0.5f, -0.5f, -0.5f, 0.5f),
            lmath::Vector4::construct(-0.5f, -0.5f, -0.5f, 0.5f),
            lmath::Vector4::construct(0.5f, 0.5f, 0.5f, 0.5f),
            lmath::Vector4::construct(-0.5f, 0.5f, 0.5f, 0.5f),
            lmath::Vector4::construct(0.5f, -0.5f, 0.5f, 0.5f),
            lmath::Vector4::construct(-0.5f, -0.5f, 0.5f, 0.5f),
        };

        lmath::lm128 sceneCenter = add(sceneAABBMin_, sceneAABBMax_);
        sceneCenter  = 0.5f * sceneCenter;
        lmath::lm128 sceneExtent = sub(sceneAABBMax_, sceneAABBMin_);
        for(s32 i=0; i<8; ++i){
            dst[i] = lmath::Vector4::construct(muladd((lmath::lm128)map[i], sceneExtent, sceneCenter));
        }
    }
}

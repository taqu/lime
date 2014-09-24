/**
@file ReflectiveShadowMap.cpp
@author t-sakai
@date 2014/09/15 create
*/
#include "ReflectiveShadowMap.h"
#include "Scene.h"
#include "Frustum.h"

namespace lscene
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

    void ReflectiveShadowMap::update(const Scene& scene)
    {
        const lmath::Matrix44& lightView = scene.getLightEnv().getDirectionalLight().getLightView();

        lmath::Vector4 sceneAABBPointsInLightView[8];
        createSceneAABBPoints(sceneAABBPointsInLightView);

        lmath::Vector4 lightViewOrthoMin;
        lmath::Vector4 lightViewOrthoMax;
        lightViewOrthoMin.set(FLT_MAX, FLT_MAX, FLT_MAX, 1.0f);
        lightViewOrthoMax.set(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1.0f);

        for(s32 i=0; i<8; ++i){
            sceneAABBPointsInLightView[i].mul(lightView, sceneAABBPointsInLightView[i]);

            lightViewOrthoMin.minimum(lightViewOrthoMin, sceneAABBPointsInLightView[i]);
            lightViewOrthoMax.maximum(lightViewOrthoMax, sceneAABBPointsInLightView[i]);
        }


        f32 invResolution = 1.0f/resolution_;
        lmath::Vector4 vInvResolution(invResolution, invResolution, 0.0f, 0.0f);

        lmath::Vector4 worldUnitsPerTexel;

        worldUnitsPerTexel.sub(lightViewOrthoMax, lightViewOrthoMin);
        worldUnitsPerTexel *= vInvResolution;
        lightViewOrthoMin /= worldUnitsPerTexel;
        lightViewOrthoMin.floor();
        lightViewOrthoMin *= worldUnitsPerTexel;

        lightViewOrthoMax /= worldUnitsPerTexel;
        lightViewOrthoMax.floor();
        lightViewOrthoMax *= worldUnitsPerTexel;


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
            lmath::Vector4(0.5f, 0.5f, -0.5f, 0.5f),
            lmath::Vector4(-0.5f, 0.5f, -0.5f, 0.5f),
            lmath::Vector4(0.5f, -0.5f, -0.5f, 0.5f),
            lmath::Vector4(-0.5f, -0.5f, -0.5f, 0.5f),
            lmath::Vector4(0.5f, 0.5f, 0.5f, 0.5f),
            lmath::Vector4(-0.5f, 0.5f, 0.5f, 0.5f),
            lmath::Vector4(0.5f, -0.5f, 0.5f, 0.5f),
            lmath::Vector4(-0.5f, -0.5f, 0.5f, 0.5f),
        };

        lmath::Vector4 sceneCenter;
        sceneCenter.add(sceneAABBMin_, sceneAABBMax_);
        sceneCenter *= 0.5f;

        lmath::Vector4 sceneExtent;
        sceneExtent.sub(sceneAABBMax_, sceneAABBMin_);

        for(s32 i=0; i<8; ++i){
            dst[i].muladd(map[i], sceneExtent, sceneCenter);
        }
    }
}

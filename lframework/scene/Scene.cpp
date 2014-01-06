#ifndef INC_LFRAMEWORK_SCENE_CPP__
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
        :shadowMapZNear_(0.001f)
        ,shadowMapZFar_(1.0f)
        ,numCascades_(NumMinCascades)
    {
        setShadowMapSize(512);

        for(s32 i=0; i<NumMaxCascades; ++i){
            lightViewProjection_[i].identity();
        }

        calcCascadeSplitDepth();

        //sceneAABBMin_.identity();
        //sceneAABBMax_.identity();
    }

    Scene::~Scene()
    {
    }

    void Scene::calcCascadeSplitDepth()
    {
        f32 zextent = shadowMapZFar_ - shadowMapZNear_;
        f32 invZFar = 1.0f/getCamera().getZFar();

        f32 farOverNear = shadowMapZFar_ / shadowMapZNear_;
        f32 invNumCascades = 1.0f/numCascades_;

        for(s32 i=0; i<numCascades_; ++i){
            f32 step = static_cast<f32>(i+1) * invNumCascades;
            f32 logZ = shadowMapZNear_ * lmath::pow(farOverNear, step);
            f32 uniformZ = shadowMapZNear_ + zextent * step;

            f32 zfar = lcore::lerp(logZ, uniformZ, 0.5f);

            cascadeSplitDepth_[i] = zfar;
        }
    }

    void Scene::calcLightViewProjection()
    {
        f32 znear = shadowMapZNear_;

        for(s32 i=0; i<numCascades_; ++i){
            f32 zfar = cascadeSplitDepth_[i];
            calcLightViewProjection(i, znear, zfar);
            znear = zfar;
        }
    }

    void Scene::calcLightViewProjection(s32 cascade, f32 shadowMapZNear, f32 shadowMapZFar)
    {
        //const lmath::Matrix44& viewProjection = scene.getViewProjMatrix();
        const lmath::Matrix44& view = getViewMatrix();
        const lmath::Matrix44& lightView = getLightEnv().getDirectionalLight().getLightView();

        lmath::Matrix44 invView(view);
        invView.invert();

        f32 znear;
        f32 zfar;

        //f32 intervalBegin = 0.0f;
        //f32 intervalEnd = 1.0f;
        lmath::Vector4 frustumPoints[8];

        lscene::Frustum frustum;
        frustum.calc(getCamera(), shadowMapZNear, shadowMapZFar);
        frustum.getPoints(frustumPoints);

        lmath::Vector4 lightCameraOrhoMin(FLT_MAX, FLT_MAX, FLT_MAX, 1.0f);
        lmath::Vector4 lightCameraOrhoMax(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1.0f);

        lmath::Vector4 worldUnitsPerTexel;

        lmath::Vector4 worldPoint;
        for(s32 i=0; i<8; ++i){
            //ワールド座標へ変換
            frustumPoints[i].mul(invView, frustumPoints[i]);
            //ライトの座標へ変換
            worldPoint.mul(lightView, frustumPoints[i]);
            lightCameraOrhoMin.min(lightCameraOrhoMin, worldPoint);
            lightCameraOrhoMax.max(lightCameraOrhoMax, worldPoint);
        }

        lmath::Vector4 diagonal = frustumPoints[0];
        diagonal -= frustumPoints[6];
        diagonal.setLength();
        f32 cascadeBound = diagonal.x_;
        lmath::Vector4 extent = lightCameraOrhoMax;
        extent -= lightCameraOrhoMin;

        lmath::Vector4 boarderOffset = diagonal;
        boarderOffset -= extent;
        boarderOffset *= 0.5f;
        boarderOffset.z_ = boarderOffset.w_ = 0.0f;

        lightCameraOrhoMax += boarderOffset;
        lightCameraOrhoMin -= boarderOffset;

        f32 invScale = 1.0f/ getShadowMapSize();
        f32 units = cascadeBound * invScale;
        worldUnitsPerTexel.set(units, units, 0.1f, 0.1f);

        lightCameraOrhoMin /= worldUnitsPerTexel;
        lightCameraOrhoMin.floor();
        lightCameraOrhoMin *= worldUnitsPerTexel;

        lightCameraOrhoMax /= worldUnitsPerTexel;
        lightCameraOrhoMax.floor();
        lightCameraOrhoMax *= worldUnitsPerTexel;

        //lmath::Vector4 lightSceneAABBMin(FLT_MAX, FLT_MAX, FLT_MAX, 1.0f);
        //lmath::Vector4 lightSceneAABBMax(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1.0f);

        //lmath::Vector4 sceneAABBPointsLightSpace[8];
        //lmath::calcAABBPoints(sceneAABBPointsLightSpace, sceneAABBMin_, sceneAABBMax_);
        //for(s32 i=0; i<8; ++i){
        //    sceneAABBPointsLightSpace[i].mul(lightView, sceneAABBPointsLightSpace[i]);
        //    lightSceneAABBMin.min(sceneAABBPointsLightSpace[i], lightSceneAABBMin);
        //    lightSceneAABBMax.max(sceneAABBPointsLightSpace[i], lightSceneAABBMax);
        //}

        //znear = lightSceneAABBMin.z_;
        //zfar = lightSceneAABBMax.z_;

        znear = lightCameraOrhoMin.z_;
        zfar = lightCameraOrhoMax.z_;

        lightViewProjection_[cascade].orthoOffsetCenter(
            lightCameraOrhoMin.x_,
            lightCameraOrhoMax.x_,
            lightCameraOrhoMax.y_,
            lightCameraOrhoMin.y_,
            znear,
            zfar);

#if !defined(LIME_DX11)
        f32 scale = static_cast<f32>(getShadowMapSize()-2)*invScale;
        lightViewProjection_.m_[0][0] *= scale;
        lightViewProjection_.m_[1][1] *= scale;
#endif
        lightViewProjection_[cascade].mul(lightViewProjection_[cascade], lightView);
    }

    void Scene::getShadowMapProjection(lmath::Matrix44& mat, s32 cascade) const
    {
        LASSERT(0<=cascade && cascade<numCascades_);

        lmath::Matrix44 tmp;
        tmp.set(
            0.5f, 0.0f, 0.0f, 0.5f,
            0.0f, -0.5f, 0.0f, 0.5f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        mat.mul(tmp, lightViewProjection_[cascade]);
    }

    void Scene::getCascadeSplitDepth(f32* dst)
    {
        _mm_storeu_ps(dst, _mm_loadu_ps(cascadeSplitDepth_));
    }
}
#endif //INC_LFRAMEWORK_SCENE_CPP__

/**
@file ShadowMap.cpp
@author t-sakai
@date 2014/09/04 create
*/
#include "ShadowMap.h"
#include "Scene.h"
#include "Frustum.h"

namespace lscene
{
    ShadowMap::ShadowMap()
        :cascadeLevels_(1)
        ,resolution_(1)
        ,invResolution_(1.0f)
        ,pcfBlurSize_(0.0f)
        ,znear_(0.0f)
        ,zfar_(1.0f)
        ,fitType_(FitType_ToCascades)
        ,moveLightTexelSize_(1)
        ,fitNearFar_(FitNearFar_AABB)
    {
        sceneAABBMin_.zero();
        sceneAABBMax_.zero();

        u8* p = lcore::align16(bufferMatrices_);
        lightProjection_ = reinterpret_cast<lmath::Matrix44*>(p);
        lightViewProjection_ = reinterpret_cast<lmath::Matrix44*>(p + sizeof(lmath::Matrix44)*MaxCascades);
    }

    ShadowMap::~ShadowMap()
    {
    }

    void ShadowMap::initialize(s32 cascadeLevels, s32 resolution, f32 znear, f32 zfar, f32 logRatio)
    {
        cascadeLevels_ = cascadeLevels;
        resolution_ = lcore::maximum(1, resolution);
        invResolution_ = 1.0f/resolution_;
        znear_ = znear;
        zfar_ = zfar;
        calcCascadePartitions(logRatio);
    }

    void ShadowMap::update(const Scene& scene)
    {
        const lmath::Matrix44& view = scene.getViewMatrix();
        const lmath::Matrix44& lightView = scene.getLightEnv().getDirectionalLight().getLightView();

        lmath::Matrix44 invView(view);
        invView.invert();

        lmath::Vector4 sceneAABBPointsInLightView[8];
        createSceneAABBPoints(sceneAABBPointsInLightView);
        for(s32 i=0; i<8; ++i){
            sceneAABBPointsInLightView[i].mul(lightView, sceneAABBPointsInLightView[i]);
        }

        f32 frustumIntervalBegin, frustumIntervalEnd;
        lmath::Vector4 lightViewOrthoMin;
        lmath::Vector4 lightViewOrthoMax;

        lscene::Frustum frustum;
        frustum.calcFromProjection(scene.getCamera().getProjMatrix(), scene.getCamera().getZNear(), scene.getCamera().getZFar());

        lmath::Vector4 frustumPoints[8];
        f32 offsetForPCFBlur = static_cast<f32>(pcfBlurSize_*2+1)/resolution_;
        lmath::Vector4 scaleForPCFBlur(0.5f*offsetForPCFBlur, 0.5f*offsetForPCFBlur, 0.0f, 0.0f);
        f32 invResolution = 1.0f/resolution_;
        lmath::Vector4 vInvResolution(invResolution, invResolution, 0.0f, 0.0f);

        lmath::Vector4 worldUnitsPerTexel;
        for(s32 cascade=0; cascade<cascadeLevels_; ++cascade){
            frustumIntervalBegin = (FitType_ToCascades == fitType_)? cascadePartitions_[cascade] : 0.0f;
            frustumIntervalEnd = cascadePartitions_[cascade+1];


            frustum.getPoints(frustumPoints, frustumIntervalBegin, frustumIntervalEnd);

            lightViewOrthoMin.set(FLT_MAX, FLT_MAX, FLT_MAX, 1.0f);
            lightViewOrthoMax.set(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1.0f);

            lmath::Vector4 worldPoint;
            for(s32 i=0; i<8; ++i){
                //ワールド座標へ変換
                frustumPoints[i].mul(invView, frustumPoints[i]);
                //ライトの座標へ変換
                worldPoint.mul(lightView, frustumPoints[i]);
                lightViewOrthoMin.minimum(lightViewOrthoMin, worldPoint);
                lightViewOrthoMax.maximum(lightViewOrthoMax, worldPoint);
            }

            switch(fitType_)
            {
            case FitType_ToCascades:
                {
                    lmath::Vector4 boarderOffset;
                    boarderOffset.sub(lightViewOrthoMax, lightViewOrthoMin);
                    boarderOffset *= scaleForPCFBlur;
                    lightViewOrthoMin -= boarderOffset;
                    lightViewOrthoMax += boarderOffset;

                    worldUnitsPerTexel.sub(lightViewOrthoMax, lightViewOrthoMin);
                    worldUnitsPerTexel *= vInvResolution;
                }
                break;

            case FitType_ToScene:
            default:
                {
                    lmath::Vector4 diagonal;
                    diagonal.sub(frustumPoints[0], frustumPoints[6]);
                    f32 cascadeBound = diagonal.length();
                    lmath::Vector4 boarderOffset;
                    boarderOffset.sub(lightViewOrthoMax, lightViewOrthoMin);
                    boarderOffset.sub(diagonal, boarderOffset);
                    boarderOffset *= 0.5f;
                    boarderOffset.z_ = boarderOffset.w_ = 0.0f;

                    lightViewOrthoMin -= boarderOffset;
                    lightViewOrthoMax += boarderOffset;
                    f32 units = cascadeBound*invResolution;
                    worldUnitsPerTexel.set(units, units, 0.0f, 0.0f);
                }
                break;
            }

            if(moveLightTexelSize_ == 1){
                lightViewOrthoMin /= worldUnitsPerTexel;
                lightViewOrthoMin.floor();
                lightViewOrthoMin *= worldUnitsPerTexel;

                lightViewOrthoMax /= worldUnitsPerTexel;
                lightViewOrthoMax.floor();
                lightViewOrthoMax *= worldUnitsPerTexel;
            }

            f32 nearPlane = 0.0f;
            f32 farPlane = 10000.0f;

            switch(fitNearFar_)
            {
            case FitNearFar_None:
                break;

            case FitNearFar_AABB:
                {
                    nearPlane = FLT_MAX;
                    farPlane = -FLT_MAX;
                    for(s32 i=0; i<8; ++i){
                        nearPlane = lcore::minimum(sceneAABBPointsInLightView[i].z_, nearPlane);
                        farPlane = lcore::maximum(sceneAABBPointsInLightView[i].z_, farPlane);
                    }
                }
                break;

            case FitNearFar_SceneAABB:
                Frustum::calcFitNearFar(nearPlane, farPlane, lightViewOrthoMin, lightViewOrthoMax, scene.getCamera());
                break;
            }

            lightProjection_[cascade].orthoOffsetCenter(
                lightViewOrthoMin.x_,
                lightViewOrthoMax.x_,
                lightViewOrthoMax.y_,
                lightViewOrthoMin.y_,
                nearPlane,
                farPlane);

            lightViewProjection_[cascade].mul(lightProjection_[cascade], lightView);

            //cascadePartitionsFrustum_[cascade] = frustumIntervalEnd;
        }
    }

    void ShadowMap::calcCascadePartitions(f32 logRatio)
    {
        f32 zextent = zfar_ - znear_;
        //f32 invZFar = 1.0f/zfar_;

        f32 farOverNear = zfar_ / znear_;
        f32 invNumCascades = 1.0f/cascadeLevels_;

        cascadePartitions_[0] = 0.0f;

        for(s32 i=1; i<=cascadeLevels_; ++i){
            f32 step = static_cast<f32>(i) * invNumCascades;
            f32 logZ = znear_ * lmath::pow(farOverNear, step);
            f32 uniformZ = zextent * step;

            f32 zfar = lcore::lerp(uniformZ, logZ, logRatio);

            cascadePartitions_[i] = zfar;
        }
    }

    void ShadowMap::createSceneAABBPoints(lmath::Vector4 dst[8])
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

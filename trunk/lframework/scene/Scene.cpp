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
        :shadowMapZFar_(1.0f)
    {
        setShadowMapSize(512);

        lightViewProjection_.identity();

        sceneAABBMin_.identity();
        sceneAABBMax_.identity();

        //depthSamplerState_.setAddressU(lgraphics::TexAddress_Clamp);
        //depthSamplerState_.setAddressV(lgraphics::TexAddress_Clamp);
        //depthSamplerState_.setMagFilter(lgraphics::TexFilter_Linear);
        //depthSamplerState_.setMinFilter(lgraphics::TexFilter_Linear);
        //depthSamplerState_.setMipFilter(lgraphics::TexFilter_Point);


        //depthState_.setAlphaBlendEnable(false);
        //depthState_.setAlphaTest(false);
        //depthState_.setZEnable(true);
        //depthState_.setZWriteEnable(true);
    }

    Scene::~Scene()
    {
    }

    //void Scene::resetDepthTexture()
    //{
    //    //デプスバッファ作成
    //    depthSurface_.destroy();
    //    depthTexture_.destroy();

    //    depthTexture_ = lgraphics::Texture::create(
    //        getShadowMapSize(),
    //        getShadowMapSize(),
    //        1,
    //        lgraphics::Usage_RenderTarget,
    //        //lgraphics::Buffer_A8R8G8B8,
    //        lgraphics::Buffer_R32F,
    //        lgraphics::Pool_Default);

    //    depthTexture_.getSurface(0, depthSurface_);
    //}

    void Scene::calcLightViewProjection()
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
        frustum.calc(getCamera(), shadowMapZFar_);
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

        lightViewProjection_.orthoOffsetCenter(
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
        lightViewProjection_.mul(lightViewProjection_, lightView);
    }

    void Scene::getShadowMapProjection(lmath::Matrix44& mat) const
    {
        lmath::Matrix44 tmp;
        tmp.set(
            0.5f, 0.0f, 0.0f, 0.5f,
            0.0f, -0.5f, 0.0f, 0.5f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        mat.mul(tmp, lightViewProjection_);
    }
}
#endif //INC_LFRAMEWORK_SCENE_CPP__

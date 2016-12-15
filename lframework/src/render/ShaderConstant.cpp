/**
@file ShaderConstant.cpp
@author t-sakai
@date 2016/11/20 create
*/
#include "render/ShaderConstant.h"

namespace lfw
{
namespace
{
    inline void copy(s32* dst, const s32* src)
    {
        _mm_store_ps((f32*)dst, _mm_load_ps((f32*)src));
    }

    inline void copy(lmath::Vector4& dst, const lmath::Vector4& src)
    {
        _mm_store_ps(&dst.x_, _mm_load_ps(&src.x_));
    }

    inline void copy(lmath::Vector3& dst, const lmath::Vector3& src)
    {
        _mm_store_ps(&dst.x_, _mm_load_ps(&src.x_));
    }

    inline void copy(lmath::Matrix44& dst, const lmath::Matrix44& src)
    {
        _mm_store_ps(dst.m_[0], _mm_load_ps(src.m_[0]));
        _mm_store_ps(dst.m_[1], _mm_load_ps(src.m_[1]));
        _mm_store_ps(dst.m_[2], _mm_load_ps(src.m_[2]));
        _mm_store_ps(dst.m_[3], _mm_load_ps(src.m_[3]));
    }
}

    //void setSceneConstantVS(SceneConstantVS& dst, const Scene& scene, const ShadowMap& shadowMap)
    //{
    //    const lscene::Camera& camera = scene.getCamera();

    //    copy(dst.v_, camera.getViewMatrix());
    //    //copy(dst.p_, camera.getProjMatrix());
    //    copy(dst.vp0_, camera.getPrevViewProjMatrix());
    //    copy(dst.vp1_, camera.getViewProjMatrix());
    //    shadowMap.getLightViewProjectionAlign16(dst.lvp_);
    //    dst.cameraPos_ = camera.getEyePosition();
    //    //dst.zrange_ = 1.0f/(camera.getZFar() - camera.getZNear());
    //    //dst.znear_ = camera.getZNear();
    //}

    //void setSceneConstantDS(SceneConstantDS& dst, const Scene& scene)
    //{
    //    const lscene::Camera& camera = scene.getCamera();

    //    copy(dst.v_, camera.getViewMatrix());
    //    camera.getViewMatrix().getInvert(dst.iv_);

    //    copy(dst.vp0_, camera.getPrevViewProjMatrix());
    //    copy(dst.vp1_, camera.getViewProjMatrix());
    //    dst.cameraPos_ = camera.getEyePosition();
    //}

    //void setSceneConstantPS(SceneConstantPS& dst, const SceneConstantPS& src, const Scene& scene)
    //{
    //    copyAlign16Size16Times(&dst, &src, sizeof(SceneConstantPS));

    //    const lscene::Camera& camera = scene.getCamera();
    //    const DirectionalLight& dlight = scene.getLightEnv().getDirectionalLight();
    //    copy(dst.dlDirection_, dlight.getDirection());
    //    copy(dst.dlColor_, dlight.getColor());
    //    dst.cameraPos_ = camera.getEyePosition();
    //    //dst.cameraNear_ = camera.getZNear();
    //    //dst.cameraFar_ = camera.getZFar();
    //    //f32 range = camera.getZFar() - camera.getZNear();
    //    //dst.cameraRange_ = range;
    //    //dst.cameraInvRange_ = 1.0f/range;

    //    copy(&dst.screenWidth_, &src.screenWidth_);
    //}

    //void setSceneConstantPS(
    //    SceneConstantPS& dst,
    //    const ShadowMap& shadowMap,
    //    f32 specularMapMipLevels,
    //    f32 shadowMapMomentBias,
    //    f32 shadowMapDepthBias,
    //    f32 lightBleedingBias)
    //{
    //    dst.specularMapMipLevels_ = specularMapMipLevels;
    //    dst.shadowMapSize_ = static_cast<f32>( shadowMap.getResolution() );
    //    dst.invShadowMapSize_ = shadowMap.getInvResolution();
    //    dst.shadowMapUVToTexel_ = shadowMap.getResolution()-1;
    //    dst.shadowMapMomentBias_ = shadowMapMomentBias;
    //    dst.shadowMapDepthBias_ = shadowMapDepthBias;
    //    dst.lightBreedingBias_ = lightBleedingBias;
    //}

    //void setSceneConstantCameraMotionPS(SceneConstantCameraMotionPS& dst, const Scene& scene, f32 width, f32 height, f32 exposure, f32 maxMagnitude)
    //{
    //    const lscene::Camera& camera = scene.getCamera();

    //    copy(dst.vp0_, camera.getPrevViewProjMatrix());
    //    //copy(dst.vp1_, camera.getViewProjMatrix());
    //    //copy(dst.ivp0_, camera.getPrevViewProjMatrix());
    //    copy(dst.ivp1_, camera.getViewProjMatrix());
    //    //dst.ivp0_.invert();
    //    dst.ivp1_.invert();
    //    dst.width_ = width;
    //    dst.height_ = height;
    //    dst.exposure_ = exposure;
    //    dst.maxMagnitude_ = maxMagnitude;
    //}
}

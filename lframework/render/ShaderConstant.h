#ifndef INC_LFRAMEWORK_SHADERCONSTANT_H_
#define INC_LFRAMEWORK_SHADERCONSTANT_H_
/**
@file ShaderConstant.h
@author t-sakai
@date 2016/11/20 create
*/
#include "../lframework.h"
#include <lmath/Vector2.h>
#include <lmath/Vector4.h>
#include <lmath/Matrix44.h>
#include <lgraphics/lgraphics.h>

namespace lfw
{
    class ShadowMap;

    struct LALIGN16 PerFrameConstant
    {
        f32 velocityScale_;
        f32 velocityMaxMagnitude_;
        f32 reserved0_;
        f32 reserved1_;
    };

    struct LALIGN16 PerCameraConstant
    {
        lmath::Matrix44 view_; //view matrix
        lmath::Matrix44 invview_;
        lmath::Matrix44 projection_; //projection matrix
        lmath::Matrix44 invprojection_;
        lmath::Matrix44 vp0_; //previous view projection
        lmath::Matrix44 vp1_; //current view projection
        lmath::Matrix44 invvp1_;
        lmath::Vector4 cameraPos_;
        s32 screenWidth_;
        s32 screenHeight_;
        f32 screenInvWidth_;
        f32 screenInvHeight_;
        lmath::Vector4 linearZParam_;
    };

    struct LALIGN16 PerModelConstant0
    {
        lmath::Matrix44 world0_;
        lmath::Matrix44 world1_;
    };

    struct LALIGN16 PerModelConstant1
    {
        lmath::Vector4 matrices0_[LGFX_CONFIG_MAX_SKINNING_MATRICES*3];
        lmath::Vector4 matrices1_[LGFX_CONFIG_MAX_SKINNING_MATRICES*3];
    };

    struct LALIGN16 PerShadowMapConstant
    {
        lmath::Matrix44 lvp_[LFW_CONFIG_SHADOW_NUMCASCADES];
        f32 cascadeScales_[LFW_CONFIG_SHADOW_MAXCASCADES];
        lmath::Vector4 shadowMapLightDir_;

        s32 shadowMapSize_;
        f32 invShadowMapSize_;
        f32 shadowMapDepthBias_;
        f32 shadowMapSlopeScaledDepthBias_;
    };

    struct LALIGN16 PerCameraConstantDS
    {
        lmath::Matrix44 view_; //view matrix
        lmath::Matrix44 iview_; //inverse view matrix
        lmath::Matrix44 vp0_; //previous vew projection
        lmath::Matrix44 vp1_; //current view projection
        lmath::Vector4 cameraPos_;
    };

    struct LALIGN16 PerLightConstant
    {
        lmath::Vector4 dlDir_;
        lmath::Vector4 dlColor_;

        f32 specularMapMipLevels_;
        f32 lightBleedingBias_;
        s32 clusterGridScale_;
        f32 reserved0_;
    };

    //struct LALIGN16 LightClusterConstantPS
    //{
    //    s32 width_;
    //    s32 height_;
    //    s32 depth_;
    //    s32 depthPitch_;

    //    lmath::Vector3 lightClusterScale_;
    //    f32 reserved1_;
    //    lmath::Vector3 lightClusterBias_;
    //    f32 reserved2_;

    //    s32 getCellIndex(s32 x, s32 y, s32 z) const
    //    {
    //        return (z*depthPitch_ + y*width_ + x);
    //    }
    //};

    struct LALIGN16 SceneConstantCameraMotionPS
    {
        lmath::Matrix44 vp0_; //prev frame view * projection
        //lmath::Matrix44 vp1_; //current frame view * projection
        //lmath::Matrix44 ivp0_; //prev frame inverse view * projection
        lmath::Matrix44 ivp1_; //current frame inverse view * projection

        f32 width_;
        f32 height_;
        f32 exposure_;
        f32 maxMagnitude_;
    };

    struct LALIGN16 MaterialConstant
    {
        lmath::Vector4 diffuse_; //rgba
        lmath::Vector4 specular_; //rgb roughness
        lmath::Vector4 ambient_; //rgb ambient shadow
        lmath::Vector4 shadow_; //rgb metalic
    };

    //void setSceneConstantVS(SceneConstantVS& dst, const Scene& scene, const ShadowMap& shadowMap);
    //void setSceneConstantDS(SceneConstantDS& dst, const Scene& scene);
    //void setSceneConstantPS(SceneConstantPS& dst, const SceneConstantPS& src, const Scene& scene);
    //void setSceneConstantPS(
    //    SceneConstantPS& dst,
    //    const ShadowMap& shadowMap,
    //    f32 specularMapMipLevels,
    //    f32 shadowMapMomentBias,
    //    f32 shadowMapDepthBias,
    //    f32 lightBleedingBias);

    //void setSceneConstantCameraMotionPS(SceneConstantCameraMotionPS& dst, const Scene& scene, f32 width, f32 height, f32 exposure, f32 maxMagnitude);

}
#endif //INC_LFRAMEWORK_SHADERCONSTANT_H_

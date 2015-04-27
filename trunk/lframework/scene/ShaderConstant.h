#ifndef INC_LSCENE_SHADERCONSTANT_H__
#define INC_LSCENE_SHADERCONSTANT_H__
/**
@file ShaderConstant.h
@author t-sakai
@date 2014/12/19 create
*/
#include "lscene.h"
#include <lmath/Vector2.h>
#include <lmath/Vector4.h>
#include <lmath/Matrix44.h>

namespace lscene
{
    class Scene;
    class ShadowMap;

    struct LIME_ALIGN16 SceneConstantVS
    {
        //lmath::Matrix44 v_;
        //lmath::Matrix44 p_;
        lmath::Matrix44 vp0_; //previous view projection
        lmath::Matrix44 vp1_; //current view projection
        lmath::Matrix44 lvp_[MaxCascades];
        lmath::Vector4 cameraPos_;
        //f32 zrange_;
        //f32 znear_;
        //f32 reserved0_;
        //f32 reserved1_;
    };

    struct LIME_ALIGN16 SceneConstantPS
    {
        lmath::Vector4 dlDirection_;
        lmath::Vector4 dlColor_;
        lmath::Vector4 cameraPos_;
        f32 specularMapMipLevels_;
        f32 shadowMapSize_;
        f32 invShadowMapSize_;
        s32 shadowMapUVToTexel_;

        f32 shadowMapMomentBias_;
        f32 shadowMapDepthBias_;
        f32 lightBreedingBias_;
        f32 reserved_;
    };

    struct LIME_ALIGN16 SceneConstantCameraMotionPS
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

    struct LIME_ALIGN16 MaterialConstant
    {
#ifdef LIME_NPBR
        lmath::Vector4 diffuse_; //rgba
        lmath::Vector4 specular_; //rgb shininess
        lmath::Vector4 ambient_; //rgb refractive index or fresnel
        lmath::Vector4 shadow_; //rgb roughness
#else
        lmath::Vector4 diffuse_; //rgba
        lmath::Vector4 specular_; //rgb roughness
        lmath::Vector4 ambient_; //rgb ambient shadow
        lmath::Vector4 shadow_; //rgb metalic
#endif
    };

    struct LIME_ALIGN16 NodeConstant
    {
        lmath::Matrix44 w_;
    };

    struct LIME_ALIGN16 NodeMotionVelocityConstant
    {
        lmath::Matrix44 w0_;
        lmath::Matrix44 w1_;
    };

    void setSceneConstantVS(SceneConstantVS& dst, const Scene& scene, const ShadowMap& shadowMap);
    void setSceneConstantPS(SceneConstantPS& dst, const SceneConstantPS& src, const Scene& scene);
    void setSceneConstantPS(SceneConstantPS& dst, const ShadowMap& shadowMap, f32 specularMapMipLevels, f32 shadowMapMomentBias, f32 shadowMapDepthBias, f32 lightBleedingBias);
    void setSceneConstantCameraMotionPS(SceneConstantCameraMotionPS& dst, const Scene& scene, f32 width, f32 height, f32 exposure, f32 maxMagnitude);
}
#endif //INC_LSCENE_SHADERCONSTANT_H__

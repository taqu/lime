﻿/**
@file Camera.cpp
@author t-sakai
@date 2011/11/03 create

*/
#include "Camera.h"

namespace lscene
{
    Camera::Camera()
        :znear_(0.1f)
        ,zfar_(1000.0f)
        ,width_(1.0f)
        ,height_(1.0f)
        ,aspect_(1.0f)
        ,fovy_(45.0f/180.0f*PI)
        ,isJitter_(0)
        ,jitterWidth_(0.0f)
        ,jitterHeight_(0.0f)
        ,haltonX_(0.0f)
        ,haltonY_(0.0f)
    {
        viewMatrix_.identity();
        projMatrix_.identity();
        viewProjMatrix_.identity();
        prevVewProjMatrix_.identity();
        eyePosition_.zero();
    }

    void Camera::setProjMatrix(const lmath::Matrix44& proj)
    {
        projMatrix_ = proj;

        if(proj.m_[3][3] < F32_EPSILON){
            //透視投影
            znear_ = -proj.m_[2][3]/proj.m_[2][2];
            
            f32 zratio = -proj.m_[2][3]/proj.m_[2][2];
            if(lmath::isEqual(zratio, znear_)){
                zfar_ = 1.0f/proj.m_[2][3] + znear_;
            }else{
                zfar_ = proj.m_[2][2]/proj.m_[2][3] * znear_;
            }

            width_ = 2.0f*znear_/proj.m_[0][0];
            height_ = 2.0f*znear_/proj.m_[1][1];

        }else{
            //平行投影
            znear_ = -proj.m_[2][3]/proj.m_[2][2];
            zfar_ = 1.0f/proj.m_[2][3] + znear_;
            width_ = 2.0f/proj.m_[0][0];
            height_ = 2.0f/proj.m_[1][1];
        }
    }

    // 透視投影
    void Camera::perspective(f32 width, f32 height, f32 znear, f32 zfar)
    {
        znear_ = znear;
        zfar_ = zfar;
        width_ = width;
        height_ = height;
        aspect_ = width/height;
        fovy_ = lmath::calcFOVY(height, znear);
        projMatrix_.perspective(width, height, znear, zfar);
    }

    // 透視投影
    void Camera::perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar)
    {
        znear_ = znear;
        zfar_ = zfar;
        height_ = 2.0f*znear*lmath::tan(0.5f*fovy);
        width_ = height_*aspect;
        aspect_ = aspect;
        fovy_ = fovy;
        projMatrix_.perspectiveFov(fovy, aspect, znear, zfar);
    }

    // 透視投影
    void Camera::perspectiveLens(f32 focalLength, f32 frameHeight, f32 aspect, f32 znear, f32 zfar)
    {
        if(frameHeight<0.00001f){
            frameHeight = 0.024f;
        }
        if(focalLength<0.00001f){
            focalLength = 0.05f;
        }

        f32 fovy = 2.0f * lmath::atan(frameHeight/(2.0f*focalLength));

        znear_ = znear;
        zfar_ = zfar;
        height_ = 2.0f*znear*lmath::tan(0.5f*fovy);
        width_ = height_*aspect;
        aspect_ = aspect;
        fovy_ = fovy;
        projMatrix_.perspectiveFov(fovy, aspect, znear, zfar);
    }

    // 平行投影
    void Camera::ortho(f32 width, f32 height, f32 znear, f32 zfar)
    {
        znear_ = znear;
        zfar_ = zfar;
        width_ = width;
        height_ = height;
        aspect_ = width/height;
        fovy_ = 0.0f;
        projMatrix_.ortho(width, height, znear, zfar);
    }

    // 透視投影。リニアＺ版
    void Camera::perspectiveLinearZ(f32 width, f32 height, f32 znear, f32 zfar)
    {
        znear_ = znear;
        zfar_ = zfar;
        width_ = width;
        height_ = height;
        aspect_ = width/height;
        fovy_ = lmath::calcFOVY(height, znear);
        projMatrix_.perspectiveLinearZ(width, height, znear, zfar);
    }

    // 透視投影。リニアＺ版
    void Camera::perspectiveFovLinearZ(f32 fovy, f32 aspect, f32 znear, f32 zfar)
    {
        znear_ = znear;
        zfar_ = zfar;
        height_ = 2.0f*znear*lmath::tan(0.5f*fovy);
        width_ = height_/aspect;
        aspect_ = aspect;
        fovy_ = fovy;
        projMatrix_.perspectiveFovLinearZ(fovy, aspect, znear, zfar);
    }

    void Camera::updateMatrix()
    {
        viewProjMatrix_ = projMatrix_;

        if(isJitter()){
            haltonX_ = halton_next(haltonX_, JitterPrime0);
            haltonY_ = halton_next(haltonY_, JitterPrime1);

            f32 jitterX = (2.0f*haltonX_ - 1.0f)*jitterWidth_;
            f32 jitterY = (2.0f*haltonY_ - 1.0f)*jitterHeight_;
            viewProjMatrix_.m_[0][3] += jitterX;
            viewProjMatrix_.m_[1][3] += jitterY;
        }
        viewProjMatrix_ *= viewMatrix_;

        getEyePosition(eyePosition_);
    }

    void Camera::getEyePosition(lmath::Vector4& eye) const
    {
        const lmath::Matrix44& view = viewMatrix_;

        eye.x_ = -(view.m_[0][0] * view.m_[0][3] + view.m_[1][0] * view.m_[1][3] + view.m_[2][0] * view.m_[2][3]);
        eye.y_ = -(view.m_[0][1] * view.m_[0][3] + view.m_[1][1] * view.m_[1][3] + view.m_[2][1] * view.m_[2][3]);
        eye.z_ = -(view.m_[0][2] * view.m_[0][3] + view.m_[1][2] * view.m_[1][3] + view.m_[2][2] * view.m_[2][3]);
        eye.w_ = 0.0f;
    }

    f32 Camera::halton_next(f32 prev, s32 prime)
    {
        float r = 1.0f - prev - 0.000001f;
        float f = 1.0f/prime;
        if(f < r) {
            return prev + f;
        } else {
            float h = f;
            float hh;
            do {
                hh = h;
                h *= f;
            } while(h >= r);
            return prev + hh + h - 1.0f;
        }
    }
}

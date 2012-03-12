#ifndef INC_LSCENE_CAMERA_H__
#define INC_LSCENE_CAMERA_H__
/**
@file Camera.h
@author t-sakai
@date 2009/12/20 create
*/
#include <lmath/lmath.h>
#include "lscene.h"

namespace lscene
{
    class Camera
    {
    public:
        Camera();

        inline f32 getWidth() const{ return width_;}
        inline f32 getHeight() const{ return height_;}
        inline f32 getZNear() const{ return znear_;}
        inline f32 getZFar() const{ return zfar_;}
        inline f32 getAspect() const{ return aspect_;}

        const lmath::Matrix44& getViewMatrix() const
        {
            return viewMatrix_;
        }

        lmath::Matrix44& getViewMatrix()
        {
            return viewMatrix_;
        }

        void setViewMatrix(const lmath::Matrix44& view)
        {
            viewMatrix_ = view;
        }

        const lmath::Matrix44& getProjMatrix() const
        {
            return projMatrix_;
        }

        lmath::Matrix44& getProjMatrix()
        {
            return projMatrix_;
        }

        void setProjMatrix(const lmath::Matrix44& proj);

        const lmath::Matrix44& getViewProjMatrix() const
        {
            return viewProjMatrix_;
        }

        /**
        @brief �������e
        */
        void perspective(f32 width, f32 height, f32 znear, f32 zfar)
        {
            znear_ = znear;
            zfar_ = zfar;
            width_ = width;
            height_ = height;
            aspect_ = width/height;
            projMatrix_.perspective(width, height, znear, zfar);
        }

        /**
        @brief �������e
        */
        void perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar)
        {
            znear_ = znear;
            zfar_ = zfar;
            height_ = znear*lmath::tan(0.5f*fovy);
            width_ = aspect * height_;
            aspect_ = aspect;
            projMatrix_.perspectiveFov(fovy, aspect, znear, zfar);
        }

        /**
        @brief ���s���e
        */
        void ortho(f32 width, f32 height, f32 znear, f32 zfar)
        {
            znear_ = znear;
            zfar_ = zfar;
            width_ = width;
            height_ = height;
            aspect_ = width/height;
            projMatrix_.ortho(width, height, znear, zfar);
        }

        /**
        @brief �������e�B���j�A�y��
        */
        void perspectiveLinearZ(f32 width, f32 height, f32 znear, f32 zfar)
        {
            znear_ = znear;
            zfar_ = zfar;
            width_ = width;
            height_ = height;
            aspect_ = width/height;
            projMatrix_.perspectiveLinearZ(width, height, znear, zfar);
        }

        /**
        @brief �������e�B���j�A�y��
        */
        void perspectiveFovLinearZ(f32 fovy, f32 aspect, f32 znear, f32 zfar)
        {
            znear_ = znear;
            zfar_ = zfar;
            height_ = znear*lmath::tan(0.5f*fovy);
            width_ = aspect * height_;
            aspect_ = aspect;
            projMatrix_.perspectiveFovLinearZ(fovy, aspect, znear, zfar);
        }

        void updateMatrix();

        inline void getViewTranslation(lmath::Vector3& pos) const;
    protected:
        f32 znear_;
        f32 zfar_;
        f32 width_;
        f32 height_;
        f32 aspect_;

        lmath::Matrix44 viewMatrix_;
        lmath::Matrix44 projMatrix_;
        lmath::Matrix44 viewProjMatrix_;
    };

    inline void Camera::getViewTranslation(lmath::Vector3& pos) const
    {
        pos.x_ = viewMatrix_.m_[0][3];
        pos.y_ = viewMatrix_.m_[1][3];
        pos.z_ = viewMatrix_.m_[2][3];
    }
}

#endif //INC_LSCENE_CAMERA_H__

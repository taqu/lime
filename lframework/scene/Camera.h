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
        inline f32 getFovy() const{ return fovy_;}

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
        @brief 透視投影
        */
        void perspective(f32 width, f32 height, f32 znear, f32 zfar);

        /**
        @brief 透視投影
        */
        void perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar);

        /**
        @brief 透視投影
        */
        void perspectiveLens(f32 focalLength, f32 frameHeight, f32 aspect, f32 znear, f32 zfar);

        /**
        @brief 平行投影
        */
        void ortho(f32 width, f32 height, f32 znear, f32 zfar);

        /**
        @brief 透視投影。リニアＺ版
        */
        void perspectiveLinearZ(f32 width, f32 height, f32 znear, f32 zfar);

        /**
        @brief 透視投影。リニアＺ版
        */
        void perspectiveFovLinearZ(f32 fovy, f32 aspect, f32 znear, f32 zfar);

        void updateMatrix();

        const lmath::Matrix44& getPrevViewProjMatrix() const
        {
            return prevVewProjMatrix_;
        }

        const lmath::Vector4& getEyePosition() const
        {
            return eyePosition_;
        }

        void pushMatrix()
        {
            prevVewProjMatrix_ = viewProjMatrix_;
        }

        inline void setJitter(bool enable);
        inline bool isJitter() const;

        inline void setJitterSize(f32 width, f32 height);
    private:
        static const s32 JitterPrime0 = 2;
        static const s32 JitterPrime1 = 3;
        static const s32 NumJitterSamples = 64;

        struct Sample2D
        {
            f32 x_;
            f32 y_;
        };

        void getEyePosition(lmath::Vector4& eye) const;
        void generateJitterSamples();

        lmath::Matrix44 viewMatrix_;
        lmath::Matrix44 projMatrix_;
        lmath::Matrix44 viewProjMatrix_;
        lmath::Matrix44 prevVewProjMatrix_;
        lmath::Vector4 eyePosition_;

        f32 znear_;
        f32 zfar_;
        f32 width_;
        f32 height_;

        f32 aspect_;
        f32 fovy_;
        s32 isJitter_;
        f32 reserved0_;

        f32 jitterWidth_;
        f32 jitterHeight_;
        s32 sampleIndex_;
        s32 reserved_;

        Sample2D samples_[NumJitterSamples];
    };

    inline void Camera::setJitter(bool enable)
    {
        isJitter_ = (enable)? 1 : 0;
    }

    inline bool Camera::isJitter() const
    {
        return 0 != isJitter_;
    }

    inline void Camera::setJitterSize(f32 width, f32 height)
    {
        jitterWidth_ = width;
        jitterHeight_ = height;
        generateJitterSamples();
    }
}

#endif //INC_LSCENE_CAMERA_H__

#ifndef INC_LRENDER_CAMERA_H__
#define INC_LRENDER_CAMERA_H__
/**
@file Camera.h
@author t-sakai
@date 2009/12/24 create
*/
#include "../lrendercore.h"

namespace lmath
{
    class Ray;
}

namespace lrender
{
    class Sampler;

    class Camera
    {
    public:
        Camera();
        ~Camera();

        void perspective(f32 width, f32 height, f32 znear, f32 zfar);
        void perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar);
        void ortho(f32 width, f32 height, f32 znear, f32 zfar);

        const Matrix44& getViewMatrix() const
        {
            return viewMatrix_;
        }

        void setViewMatrix(const lmath::Matrix44& view)
        {
            viewMatrix_ = view;
            invViewMatrix_ = viewMatrix_;
            invViewMatrix_.invert();
        }

        virtual void calcRayFromVirtualCoord(lmath::Ray& ray, f32 vx, f32 vy) const;
    protected:
        f32 viewWidth_;
        f32 viewHeight_;
        f32 near_;
        f32 far_;
        f32 fovy_;
        f32 focalLength_;

        Matrix44 viewMatrix_;
        Matrix44 invViewMatrix_;
    };
}
#endif //INC_LRENDER_CAMERA_H__

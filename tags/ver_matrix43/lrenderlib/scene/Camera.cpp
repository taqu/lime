/**
@file Camera.cpp
@author t-sakai
@date 2009/12/24
*/
#include "Camera.h"

#include <lmath/geometry/Ray.h>

namespace lrender
{
    using namespace lmath;

    Camera::Camera()
    {
        viewMatrix_.identity();
        invViewMatrix_.identity();
    }

    Camera::~Camera()
    {
    }

    void Camera::perspective(f32 width, f32 height, f32 znear, f32 zfar)
    {
        viewWidth_ = width;
        viewHeight_ = height;
        near_ = znear;
        far_ = zfar;
        fovy_ = height / znear;
        focalLength_ = znear;
    }

    void Camera::perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar)
    {
        viewHeight_ = znear * fovy;
        viewWidth_ = aspect * viewHeight_;
        near_ = znear;
        far_ = zfar;
        fovy_ = fovy;
        focalLength_ = znear;
    }

    void Camera::ortho(f32 width, f32 height, f32 znear, f32 zfar)
    {
        viewWidth_ = width;
        viewHeight_ = height;
        near_ = znear;
        far_ = zfar;
        fovy_ = height / znear;
        focalLength_ = znear;
    }

    void Camera::calcRayFromVirtualCoord(lmath::Ray& ray, f32 vx, f32 vy) const
    {
        Vector3 direction(vx*viewWidth_*0.5f, vy*viewHeight_*0.5f, near_);
        direction.normalize();
        ray.direction_.mul33(direction, invViewMatrix_);

        Vector3 origin(0.0f, 0.0f, 0.0f);
        invViewMatrix_.getTranslate(origin);
        ray.origin_ = origin;
        ray.t_ = std::numeric_limits<f32>::max();
    }
}

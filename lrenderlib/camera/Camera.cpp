/**
@file Camera.cpp
@author t-sakai
@date 2013/05/09 create
*/
#include "Camera.h"
#include <lmath/geometry/Ray.h>

namespace lrender
{
    Camera::Camera()
        :aspect_(1.0f)
        ,fovy_(0.0f)
        ,near_(0.0f)
        ,focalLength_(0.0f)
    {
        viewMatrix_.identity();
        invViewMatrix_.identity();
    }

    Camera::~Camera()
    {
    }

    void Camera::perspective(f32 aspect, f32 fovy)
    {
        LASSERT(!lmath::isZero(fovy) && 0.0f<fovy);

        aspect_ = aspect;
        fovy_ = fovy;
        near_ = 1.0f/lmath::tan(0.5f*fovy_);
        focalLength_ = 0.0f;
    }

    void Camera::lookAt(const lmath::Vector4& eye, const lmath::Vector4& at, const lmath::Vector4& up)
    {
        viewMatrix_.lookAt(eye, at, up);
        invViewMatrix_ = viewMatrix_;
        invViewMatrix_.invert();
    }

    void Camera::calcRayFromVirtualCoord(lmath::Ray& ray, f32 vx, f32 vy) const
    {
        lmath::Vector3 direction(vx/aspect_, vy, near_);
        direction.normalize();
        ray.direction_.mul33(invViewMatrix_, direction);

        lmath::Vector3 origin;
        invViewMatrix_.getTranslate(origin);
        ray.origin_ = origin;
        ray.t_ = std::numeric_limits<f32>::max();
        ray.invertDirection();
    }
}

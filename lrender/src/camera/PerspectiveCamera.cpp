/**
@file PerspectiveCamera.cpp
@author t-sakai
@date 2015/09/16 create
*/
#include "camera/PerspectiveCamera.h"
#include "core/Ray.h"
#include "core/Spectrum.h"

namespace lrender
{
    PerspectiveCamera::PerspectiveCamera()
        :right_(1.0f, 0.0f, 0.0f)
        ,up_(0.0f, 1.0f, 0.0f)
        ,forward_(0.0f, 0.0f, 1.0f)
        ,fovx_(60.0f/180.0f*static_cast<f32>(PI))
        ,aspect_(1.0f)
        ,nearClip_(1.0e-2f)
        ,farClip_(100.0f)
        ,cameraDx_(1.0f)
        ,cameraDy_(1.0f)
    {
    }

    PerspectiveCamera::~PerspectiveCamera()
    {
    }

    Color3 PerspectiveCamera::generateRay(Ray& ray, f32 screenX, f32 screenY, f32 tmax) const
    {
        Vector3 dx = mul((invHalfWidth_*screenX - 1.0f)*cameraDx_, right_);
        Vector3 dy = mul((1.0f-invHalfHeight_*screenY)*cameraDy_, up_);

        ray.origin_ = position_;
        ray.direction_ = normalize(dx+dy+forward_);
        ray.invertDirection();
        ray.tmin_ = 0.0f;
        ray.tmax_ = tmax;

        return Color3(1.0f);
    }

    Color3 PerspectiveCamera::generateRayDifferential(RayDifferential& rayDifferential, f32 screenX, f32 screenY, f32 tmax) const
    {
        Vector3 dx = mul((invHalfWidth_*screenX - 1.0f)*cameraDx_, right_);
        Vector3 dy = mul((1.0f-invHalfHeight_*screenY)*cameraDy_, up_);

        Vector3 camera_point = dx+dy+forward_;

        rayDifferential.origin_ = position_;
        rayDifferential.direction_ = normalize(camera_point);
        rayDifferential.invertDirection();
        rayDifferential.tmin_ = 0.0f;
        rayDifferential.tmax_ = tmax;

        rayDifferential.rxOrigin_ = rayDifferential.ryOrigin_ = rayDifferential.origin_;
        rayDifferential.rxDirection_ = cameraDx_ * right_ + camera_point;
        rayDifferential.ryDirection_ = cameraDy_ * up_ + camera_point;
        return Color3(1.0f);
    }

    void PerspectiveCamera::perspective(f32 aspect, f32 fovx)
    {
        fovx_ = fovx;
        aspect_ = aspect;

        f32 fov = lmath::tan(0.5f * fovx);
        cameraDx_ = fov;
        cameraDy_ = fov/aspect_;
    }

    void PerspectiveCamera::lookAt(const lmath::Vector3& eye, const lmath::Vector3& at, const lmath::Vector3& up)
    {
        position_ = eye;
        forward_ = normalize(at - eye);

        f32 cs = dot(forward_, up);
        up_ = (0.999f<cs)? Vector3(0.0f, 0.0f, 1.0f) : up;

        right_ = normalize(cross(forward_, up_));
        up_ = normalize(cross(right_, forward_));
    }
}

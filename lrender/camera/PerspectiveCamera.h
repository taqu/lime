#ifndef INC_LRENDER_PERSPECTIVECAMERA_H__
#define INC_LRENDER_PERSPECTIVECAMERA_H__
/**
@file PerspectiveCamera.h
@author t-sakai
@date 2015/09/16 create
*/
#include "Camera.h"

namespace lrender
{
    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera();
        virtual ~PerspectiveCamera();

        virtual Color3 generateRay(Ray& ray, f32 screenX, f32 screenY, f32 tmax) const;
        virtual Color3 generateRayDifferential(RayDifferential& rayDifferential, f32 screenX, f32 screenY, f32 tmax) const;

        void perspective(f32 aspect, f32 fovx);
        void lookAt(const Vector3& eye, const Vector3& at, const Vector3& up);

    protected:
        Vector3 right_;
        Vector3 up_;
        Vector3 forward_;
        f32 fovx_;
        f32 aspect_;
        f32 nearClip_;
        f32 farClip_;
        f32 cameraDx_;
        f32 cameraDy_;
    };
}
#endif //INC_LRENDER_PERSPECTIVECAMERA_H__

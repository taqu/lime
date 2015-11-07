#ifndef INC_LRENDER_CAMERA_H__
#define INC_LRENDER_CAMERA_H__
/**
@file Camera.h
@author t-sakai
@date 2013/05/09 create
*/
#include <lcore/smart_ptr/intrusive_ptr.h>
#include "../lrender.h"

namespace lrender
{
    class Ray;
    class RayDifferential;

    class Camera : public ReferenceCounted
    {
    public:
        typedef lcore::intrusive_ptr<Camera> pointer;

        virtual ~Camera()
        {}

        virtual Color3 generateRay(Ray& ray, f32 screenX, f32 screenY) const=0;
        virtual Color3 generateRayDifferential(RayDifferential& rayDifferential, f32 screenX, f32 screenY) const=0;

        inline void setResolution(s32 width, s32 height);
    protected:
        Camera()
            :width_(0)
            ,height_(0)
            ,invHalfWidth_(1.0f)
            ,invHalfHeight_(1.0f)
        {}

        s32 width_;
        s32 height_;
        f32 invHalfWidth_;
        f32 invHalfHeight_;
    };

    inline void Camera::setResolution(s32 width, s32 height)
    {
        width_ = width;
        height_ = height;

        invHalfWidth_ = 2.0f/width_;
        invHalfHeight_ = 2.0f/height_;
    }
}
#endif //INC_LRENDER_CAMERA_H__

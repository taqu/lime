#ifndef INC_LRENDER_RAY_H__
#define INC_LRENDER_RAY_H__
/**
@file Ray.h
@author t-sakai
@date 2015/09/11 create
*/
#include "../lrender.h"

namespace lrender
{
    class Ray
    {
    public:
        Ray();
        Ray(const Vector3& origin,
            const Vector3& direction,
            f32 tmin,
            f32 tmax,
            f32 epsilon);

        Ray(const Vector3& origin,
            const Vector3& direction,
            const Vector3& invDirection,
            f32 tmin,
            f32 tmax,
            f32 epsilon);

        ~Ray();

        void invertDirection();

        Vector3 origin_;
        Vector3 direction_;
        Vector3 invDirection_;

        f32 tmin_;
        f32 tmax_;
        f32 epsilon_;
    };

    class RayDifferential : public Ray
    {
    public:
        RayDifferential();
        RayDifferential(const Ray& ray);
        RayDifferential(
            const Vector3& origin,
            const Vector3& direction,
            f32 tmin,
            f32 tmax,
            f32 epsilon);

        ~RayDifferential();

        bool hasDifferentials_;
        Vector3 rxOrigin_;
        Vector3 ryOrigin_;
        Vector3 rxDirection_;
        Vector3 ryDirection_;
    };
}
#endif //INC_LRENDER_RAY_H__

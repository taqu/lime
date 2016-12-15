#ifndef INC_LRENDER_RAY_H__
#define INC_LRENDER_RAY_H__
/**
@file Ray.h
@author t-sakai
@date 2015/09/11 create
*/
#include "../lrender.h"
#include <lmath/geometry/Ray.h>

namespace lmath
{
    class Sphere;
}

namespace lrender
{
    class Ray : public lmath::Ray
    {
    public:
        Ray();
        Ray(const Vector3& origin,
            const Vector3& direction,
            f32 tmin,
            f32 tmax,
            f32 epsilon);

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

    //-----------------------------------------------------------
    /**
    @brief 線分と球表面の交差判定
    @return 交差するか
    @param t ... 交差点の陰関数パラメータ。交差点 = t*ray.direction_ + ray.origin_
    @param ray ...
    @param sphere ...
    */
    bool testRaySphereSurface(f32& t, const Ray& ray, const lmath::Sphere& sphere);
}
#endif //INC_LRENDER_RAY_H__

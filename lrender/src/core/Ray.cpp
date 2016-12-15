/**
@file Ray.cpp
@author t-sakai
@date 2015/09/11 create
*/
#include "core/Ray.h"
#include <lmath/geometry/Sphere.h>

namespace lrender
{
    //------------------------------------------------------------
    //---
    //--- Ray
    //---
    //------------------------------------------------------------
    Ray::Ray()
        :tmin_(0.0f)
        ,tmax_(LRENDER_INFINITY)
        ,epsilon_(RAY_EPSILON)
    {
        reset();
    }

    Ray::Ray(const Vector3& origin,
        const Vector3& direction,
        f32 tmin,
        f32 tmax,
        f32 epsilon)
        :lmath::Ray(origin, direction, tmax)
        ,tmin_(tmin)
        ,tmax_(tmax)
        ,epsilon_(epsilon)
    {
    }

    //------------------------------------------------------------
    //---
    //--- RayDifferential
    //---
    //------------------------------------------------------------
    RayDifferential::RayDifferential()
        :hasDifferentials_(false)
        ,rxOrigin_(0.0f)
        ,ryOrigin_(0.0f)
        ,rxDirection_(0.0f)
        ,ryDirection_(0.0f)
    {
    }

    RayDifferential::RayDifferential(const Ray& ray)
        :Ray(ray.origin_, ray.direction_, ray.tmin_, ray.tmax_, ray.epsilon_)
        ,hasDifferentials_(false)
        ,rxOrigin_(0.0f)
        ,ryOrigin_(0.0f)
        ,rxDirection_(0.0f)
        ,ryDirection_(0.0f)
    {
    }

    RayDifferential::RayDifferential(
        const Vector3& origin,
        const Vector3& direction,
        f32 tmin,
        f32 tmax,
        f32 epsilon)
        :Ray(origin, direction, tmin, tmax, epsilon)
        ,hasDifferentials_(false)
        ,rxOrigin_(0.0f)
        ,ryOrigin_(0.0f)
        ,rxDirection_(0.0f)
        ,ryDirection_(0.0f)
    {
        invertDirection();
    }

    RayDifferential::~RayDifferential()
    {
    }


    //-----------------------------------------------------------
    // 線分と球表面の交差判定
    bool testRaySphereSurface(f32& t, const Ray& ray, const lmath::Sphere& sphere)
    {
        Vector3 m = sub(ray.origin_, sphere.position());

        f32 b = dot(m, ray.direction_);
        f32 c = dot(m, m) - sphere.radius() * sphere.radius();

        // 線分の起点が球の外で、向きが球の方向と逆
        if(0.0f<c){
            if(0.0f<b){// 線分の起点が球の外で、向きが球の方向と逆
                return false;
            }
        }

        f32 discr = b*b - c; //判別式
        if(discr < 0.0f){
            return false;
        }

        discr = lmath::sqrt(discr);
        b = -b;
        t = b+discr;
        return (ray.tmin_<=t && t<=ray.tmax_);
    }
}

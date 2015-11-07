/**
@file Ray.cpp
@author t-sakai
@date 2015/09/11 create
*/
#include "Ray.h"

namespace lrender
{
    //------------------------------------------------------------
    //---
    //--- Ray
    //---
    //------------------------------------------------------------
    Ray::Ray()
        :origin_(0.0f)
        ,direction_(0.0f)
        ,invDirection_(LRENDER_INFINITY)
        ,tmin_(0.0f)
        ,tmax_(LRENDER_INFINITY)
        ,epsilon_(RAY_EPSILON)
    {
    }

    Ray::Ray(const Vector3& origin,
        const Vector3& direction,
        f32 tmin,
        f32 tmax,
        f32 epsilon)
        :origin_(origin)
        ,direction_(direction)
        ,tmin_(tmin)
        ,tmax_(tmax)
        ,epsilon_(epsilon)
    {
        invertDirection();
    }

    Ray::Ray(const Vector3& origin,
        const Vector3& direction,
        const Vector3& invDirection,
        f32 tmin,
        f32 tmax,
        f32 epsilon)
        :origin_(origin)
        ,direction_(direction)
        ,invDirection_(invDirection)
        ,tmin_(tmin)
        ,tmax_(tmax)
        ,epsilon_(epsilon)
    {
    }

    Ray::~Ray()
    {
    }

    void Ray::invertDirection()
    {
        for(s32 i=0; i<3; ++i){
            if(lmath::isPositive(direction_[i])){
                invDirection_[i] = (lmath::isZeroPositive(direction_[i])) ? lcore::numeric_limits<f32>::maximum() : 1.0f / direction_[i];
            } else{
                invDirection_[i] = (lmath::isZeroNegative(direction_[i])) ? -lcore::numeric_limits<f32>::maximum() : 1.0f / direction_[i];
            }
        }
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
        :Ray(ray.origin_, ray.direction_, ray.invDirection_, ray.tmin_, ray.tmax_, ray.epsilon_)
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
}

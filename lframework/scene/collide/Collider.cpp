/**
@file Collider.cpp
@author t-sakai
@date 2015/02/18 create
*/
#include "Collider.h"
#include <lmath/geometry/RayTest.h>

namespace lcollide
{
    //-------------------------------------------------------
    //---
    //--- ColliderSphere
    //---
    //-------------------------------------------------------
    bool ColliderSphere::test(const lmath::Ray& ray, f32& t) const
    {
        return lmath::testRaySphere(t, ray, bsphere_);
    }

    void ColliderSphere::getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax)
    {
        bmin = bsphere_.s_;
        bmin.sub(bsphere_.s_.w_);

        bmax = bsphere_.s_;
        bmax.add(bsphere_.s_.w_);
    }

    //-------------------------------------------------------
    //---
    //--- ColliderRay
    //---
    //-------------------------------------------------------
    bool ColliderRay::test(const lmath::Ray& ray, f32& t) const
    {
        return false;
    }

    void ColliderRay::getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax)
    {
        lmath::Vector4 d(ray_.direction_);
        d *= 0.5f;

        lmath::Vector4 o(ray_.origin_);

        lmath::Vector4 t0, t1;
        t0.sub(o, d);
        t1.add(o, d);

        bmin.minimum(t0, t1);
        bmax.maximum(t0, t1);
    }

    //-------------------------------------------------------
    //---
    //--- ColliderAABB
    //---
    //-------------------------------------------------------
    bool ColliderAABB::test(const lmath::Ray& ray, f32& t) const
    {
        f32 tmax;
        return lmath::testRayAABB(t, tmax, ray, bmin_, bmax_);
    }

    void ColliderAABB::getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax)
    {
        bmin = bmin_;
        bmax = bmax_;
    }
}

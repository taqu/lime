/**
@file Collider.cpp
@author t-sakai
@date 2016/12/19 create
*/
#include "physics/Collider.h"
#include <lmath/geometry/RayTest.h>

namespace lfw
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
        lmath::Vector4 r = lmath::Vector4::construct(bsphere_.r_);
        bmin = bsphere_;
        bmin -= r;

        bmax = bsphere_;
        bmax += r;

        bmin.w_ = bmax.w_ = 0.0f;
    }

    //-------------------------------------------------------
    //---
    //--- ColliderRay
    //---
    //-------------------------------------------------------
    bool ColliderRay::test(const lmath::Ray& /*ray*/, f32& /*t*/) const
    {
        return false;
    }

    void ColliderRay::getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax)
    {
        lmath::lm128 d = lmath::Vector4::construct(ray_.direction_);
        d = 0.5f * d;

        lmath::lm128 o = lmath::Vector4::construct(ray_.origin_);
        lmath::lm128 t0 = o-d;
        lmath::lm128 t1 = o+d;

        bmin = lmath::Vector4::construct(minimum(t0, t1));
        bmax = lmath::Vector4::construct(maximum(t0, t1));
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

    //-------------------------------------------------------
    //---
    //--- ColliderCapsule
    //---
    //-------------------------------------------------------
    bool ColliderCapsule::test(const lmath::Ray& /*ray*/, f32& /*t*/) const
    {
        //f32 tmax;
        //return lmath::testRayAABB(t, tmax, ray, bmin_, bmax_);
        return false;
    }

    void ColliderCapsule::getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax)
    {
        lmath::lm128 tp0 = lmath::Vector4::construct(p0_);
        lmath::lm128 tp1 = lmath::Vector4::construct(p1_);
        lmath::lm128 tbmin = minimum(tp0, tp1);
        lmath::lm128 tbmax = maximum(tp0, tp1);
        lmath::lm128 r = _mm_set1_ps(radius_);

        tbmin = tbmin - r;
        tbmax = tbmax + r;
        bmin = lmath::Vector4::construct(tbmin);
        bmax = lmath::Vector4::construct(tbmax);
        bmin.w_ = bmax.w_ = 0.0f;
    }
}

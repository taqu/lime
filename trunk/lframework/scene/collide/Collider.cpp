/**
@file Collider.cpp
@author t-sakai
@date 2015/02/18 create
*/
#include "Collider.h"

namespace lcollide
{
    //-------------------------------------------------------
    //---
    //--- ColliderSphere
    //---
    //-------------------------------------------------------
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
    void ColliderAABB::getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax)
    {
        bmin = bmin_;
        bmax = bmax_;
    }
}

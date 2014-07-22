﻿/**
@file Plane.cpp
@author t-sakai
@date 2012/05/02 create
*/
#include "Plane.h"

namespace lmath
{
    void Plane::normalize()
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = _mm_loadu_ps(&v_.x_);
        lm128 r1 = r0;
        r0 = _mm_mul_ps(r0, r0);
        lm128 r2 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0xE1), r0);
        r2 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0xE2), r2);

        r2 = _mm_sqrt_ss(r2);
        r2 = _mm_shuffle_ps(r2, r2, 0);
        
        r1 = _mm_div_ps(r1, r2);
        _mm_storeu_ps(&v_.x_, r1);
#else
        f32 l = v_.x_*v_.x_ + v_.y_*v_.y_ + v_.z_*v_.z_;

        LASSERT( !(lmath::isEqual(l, 0.0f)) );
        l = 1.0f/ lmath::sqrt(l);
        v_ *= l;
#endif
    }

    void Plane::translate(f32 x, f32 y, f32 z)
    {
        lmath::Vector4 offset(x, y, z, 0.0f);

        lmath::Vector4 n = v_;
        n.w_ = 0.0f;

        v_.w_ -= n.dot(offset);
    }
}

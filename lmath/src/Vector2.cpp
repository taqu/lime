/**
@file Vector2.cpp
@author t-sakai
@date 2009/01/17 create
*/
#include "Vector2.h"

namespace lmath
{
    Vector2 normalize(const Vector2& v)
    {
        f32 l = v.lengthSqr();
        LASSERT( !lmath::isZero(l) );
        l = 1.0f/lmath::sqrt(l);
        return {v.x_*l, v.y_*l};
    }

    Vector2 normalizeChecked(const Vector2& v)
    {
        f32 l = v.lengthSqr();
        if(lmath::isZero(l)){
            return Vector2::construct(0.0f);
        }
        l = 1.0f/lmath::sqrt(l);
        return {v.x_*l, v.y_*l};
    }

    Vector2&& lerp(const Vector2& v0, const Vector2& v1, f32 t)
    {
        Vector2 d = {v1.x_-v0.x_, v1.y_-v0.y_};
        d *= t;
        d += v0;
        return lcore::move(d);
    }

    Vector2 muladd(f32 f, const Vector2& v0, const Vector2& v1)
    {
        f32 x = f*v0.x_ + v1.x_;
        f32 y = f*v0.y_ + v1.y_;
        return {x,y};
    } 
}

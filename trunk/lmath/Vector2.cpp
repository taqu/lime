/**
@file Vector2.cpp
@author t-sakai
@date 2009/01/17 create
*/
#include "Vector2.h"

namespace lmath
{
    Vector2& Vector2::lerp(const Vector2& v0, const Vector2& v1, f32 t)
    {
        Vector2 tmp;
        tmp.sub(v1, v0);
        tmp *= t;

        muladd(t, tmp, v0);
        return *this;
    }

    void Vector2::muladd(f32 t, const Vector2& v0, const Vector2& v1)
    {
        x_ = t*v0.x_ + v1.x_;
        y_ = t*v0.y_ + v1.y_;
    }
}

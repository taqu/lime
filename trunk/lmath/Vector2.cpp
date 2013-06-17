/**
@file Vector2.cpp
@author t-sakai
@date 2009/01/17 create
*/
#include "Vector2.h"

namespace lmath
{
    void Vector2::setLerp(const Vector2& v1, const Vector2& v2, f32 f)
    {
        if( f <= 0.0f ){
            *this = v1;
        }else if( f >= 1.0f ){
            *this = v2;
        }else{
            *this = v2;
            *this -= v1;
            *this *= f;
            *this += v1;
        }
    }
}

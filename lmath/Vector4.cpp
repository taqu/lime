/**
@file Vector4.cpp
@author t-sakai
@date 2009/01/17 create
*/
#include "Vector4.h"
#include "Matrix44.h"

namespace lmath
{
    bool Vector4::isEqual(const Vector4& v) const
    {
        return ( lmath::isEqual(_x, v._x)
            && lmath::isEqual(_y, v._y)
            && lmath::isEqual(_z, v._z)
            && lmath::isEqual(_w, v._w));
    }

    bool Vector4::isEqual(const Vector4& v, f32 epsilon) const
    {
        return ( lmath::isEqual(_x, v._x, epsilon)
            && lmath::isEqual(_y, v._y, epsilon)
            && lmath::isEqual(_z, v._z, epsilon)
            && lmath::isEqual(_w, v._w, epsilon));
    }

    f32 Vector4::distanceSqr(const Vector4& v) const
    {
        const f32 dx = _x - v._x;
        const f32 dy = _y - v._y;
        const f32 dz = _z - v._z;
        const f32 dw = _w - v._w;
        return ( dx * dx + dy * dy + dz * dz + dw * dw);
    }

    void Vector4::setLerp(const Vector4& v1, const Vector4& v2, f32 f)
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

    void Vector4::mul(const Vector4& vector, const Matrix44& matrix)
    {
        for(s32 i=0; i<4; ++i){
            (*this)[i] = vector[0] * matrix(0, i) + vector[1] * matrix(1, i) + vector[2] * matrix(2, i) + vector[3] * matrix(i, 3);
        }
    }

    void Vector4::mul(const Matrix44& matrix, const Vector4& vector)
    {
        for(s32 i=0; i<4; ++i){
            (*this)[i] = vector[0] * matrix(i, 0) + vector[1] * matrix(i, 1) + vector[2] * matrix(i, 2) + vector[3] * matrix(i, 3);
        }
    }
}

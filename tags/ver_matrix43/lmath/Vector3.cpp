/**
@file Vector3.cpp
@author t-sakai
@date 2009/01/17 create
*/
#include "Vector3.h"
#include "Matrix43.h"
#include "Matrix44.h"
#include "Quaternion.h"

namespace lmath
{
    f32 Vector3::distanceSqr(const Vector3& v) const
    {
        const f32 dx = _x - v._x;
        const f32 dy = _y - v._y;
        const f32 dz = _z - v._z;
        return ( dx * dx + dy * dy + dz * dz);
    }

    Vector3& Vector3::lerp(const Vector3& v0, const Vector3& v1, f32 t)
    {
        if( t <= 0.0f ){
            *this = v0;
        }else if( t >= 1.0f ){
            *this = v1;
        }else{
            *this = v0;
            *this *= (1.0f - t);
            Vector3 tmp(v1);
            tmp *= t;
            *this += tmp;
        }
        return *this;
    }

    Vector3& Vector3::lerp(const Vector3& v0, const Vector3& v1, f32 f0, f32 f1)
    {
        *this = v0;
        *this *= f1;

        Vector3 tmp(v1);
        tmp *= f0;
        *this += tmp;
        return *this;
    }

    void Vector3::mul(const Vector3& v, const Matrix43& m)
    {
        f32 value[3];

        value[0] = v[0] * m(0,0) + v[1] * m(1,0) + v[2] * m(2,0);
        value[1] = v[0] * m(0,1) + v[1] * m(1,1) + v[2] * m(2,1);
        value[2] = v[0] * m(0,2) + v[1] * m(1,2) + v[2] * m(2,2);

        (*this)[0] = value[0] + m(3, 0);
        (*this)[1] = value[1] + m(3, 1);
        (*this)[2] = value[2] + m(3, 2);
    }

    void Vector3::mul(const Matrix43& matrix, const Vector3& vector)
    {
        f32 value[3];
        for(s32 i=0; i<3; ++i){
            value[i] = vector[0] * matrix(i, 0) + vector[1] * matrix(i, 1) + vector[2] * matrix(i, 2);
        }
        f32 w = vector[0] * matrix(3, 0) + vector[1] * matrix(3, 1) + vector[2] * matrix(3, 2);

        for(s32 i=0; i<3; ++i){
            (*this)[i] = value[i];
        }
        if( !lmath::isEqual(w, 0.0f) ){
            (*this) /= w;
        }
    }

    void Vector3::mul33(const Vector3& vector, const Matrix43& matrix)
    {
        f32 value[3];
        for(s32 i=0; i<3; ++i){
            value[i] = vector[0] * matrix(0, i) + vector[1] * matrix(1, i) + vector[2] * matrix(2, i);
        }

        for(s32 i=0; i<3; ++i){
            (*this)[i] = value[i];
        }
    }

    void Vector3::mul33(const Matrix43& matrix, const Vector3& vector)
    {
        f32 value[3];
        for(s32 i=0; i<3; ++i){
            value[i] = vector[0] * matrix(i, 0) + vector[1] * matrix(i, 1) + vector[2] * matrix(i, 2);
        }

        for(s32 i=0; i<3; ++i){
            (*this)[i] = value[i];
        }
    }

    void Vector3::mul33(const Vector3& vector, const Matrix44& matrix)
    {
        f32 value[3];
        for(s32 i=0; i<3; ++i){
            value[i] = vector[0] * matrix(i, 0) + vector[1] * matrix(i, 1) + vector[2] * matrix(i, 2);
        }

        for(s32 i=0; i<3; ++i){
            (*this)[i] = value[i];
        }
    }

    void Vector3::mul33(const Matrix44& matrix, const Vector3& vector)
    {
        f32 value[3];
        for(s32 i=0; i<3; ++i){
            value[i] = vector[0] * matrix(i, 0) + vector[1] * matrix(i, 1) + vector[2] * matrix(i, 2);
        }

        for(s32 i=0; i<3; ++i){
            (*this)[i] = value[i];
        }
    }

    void Vector3::rotate(const Quaternion& rotation)
    {
        //Q x V x conjugate(Q)
        Quaternion conjugate(rotation.w_, -rotation.x_, -rotation.y_, -rotation.z_);

        Quaternion rot;
        rot.mul(conjugate, *this);
        rot.mul(rot, rotation);

        _x = rot.x_;
        _y = rot.y_;
        _z = rot.z_;
    }
}

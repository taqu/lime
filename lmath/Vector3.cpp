/**
@file Vector3.cpp
@author t-sakai
@date 2009/01/17 create
*/
#include "Vector3.h"
#include "Matrix34.h"
#include "Matrix44.h"
#include "Quaternion.h"

namespace lmath
{
    f32 Vector3::distanceSqr(const Vector3& v) const
    {
        const f32 dx = x_ - v.x_;
        const f32 dy = y_ - v.y_;
        const f32 dz = z_ - v.z_;
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

    void Vector3::mul(const Matrix34& m, const Vector3& v)
    {
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2] + m.m_[0][3];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2] + m.m_[1][3];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2] + m.m_[2][3];
        x_ = x; y_ = y; z_ = z;
    }

    void Vector3::mul(const Vector3& v, const Matrix34& m)
    {
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0] + m.m_[0][3];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1] + m.m_[1][3];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2] + m.m_[2][3];
        x_ = x; y_ = y; z_ = z;
    }

    void Vector3::mul33(const Matrix34& m, const Vector3& v)
    {
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2];
        x_ = x; y_ = y; z_ = z;
    }

    void Vector3::mul33(const Vector3& v, const Matrix34& m)
    {
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2];
        x_ = x; y_ = y; z_ = z;
    }

    void Vector3::mul33(const Matrix44& m, const Vector3& v)
    {
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2];
        x_ = x; y_ = y; z_ = z;
    }

    void Vector3::mul33(const Vector3& v, const Matrix44& m)
    {
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2];
        x_ = x; y_ = y; z_ = z;
    }

    void Vector3::rotate(const Quaternion& rotation)
    {
        //Q x V x conjugate(Q)
        Quaternion conjugate(rotation.w_, -rotation.x_, -rotation.y_, -rotation.z_);

        Quaternion rot;

#if 0
        rot.mul(rotation, conjugate);
        rot.mul(rot, *this);
#else
        rot.mul(rotation, *this);
        rot.mul(rot, conjugate);
#endif

        x_ = rot.x_;
        y_ = rot.y_;
        z_ = rot.z_;
    }
}

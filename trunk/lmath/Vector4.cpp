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
        return ( lmath::isEqual(x_, v.x_)
            && lmath::isEqual(y_, v.y_)
            && lmath::isEqual(z_, v.z_)
            && lmath::isEqual(w_, v.w_));
    }

    bool Vector4::isEqual(const Vector4& v, f32 epsilon) const
    {
        return ( lmath::isEqual(x_, v.x_, epsilon)
            && lmath::isEqual(y_, v.y_, epsilon)
            && lmath::isEqual(z_, v.z_, epsilon)
            && lmath::isEqual(w_, v.w_, epsilon));
    }

    f32 Vector4::distanceSqr(const Vector4& v) const
    {
        const f32 dx = x_ - v.x_;
        const f32 dy = y_ - v.y_;
        const f32 dz = z_ - v.z_;
        const f32 dw = w_ - v.w_;
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

    void Vector4::mul(const Matrix44& m, const Vector4& v)
    {
        f32 x, y, z, w;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2] + v.w_ * m.m_[0][3];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2] + v.w_ * m.m_[1][3];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2] + v.w_ * m.m_[2][3];
        w = v.x_ * m.m_[3][0] + v.y_ * m.m_[3][1] + v.z_ * m.m_[3][2] + v.w_ * m.m_[3][3];
        x_ = x;
        y_ = y;
        z_ = z;
        w_ = w;
    }

    void Vector4::mul(const Vector4& v, const Matrix44& m)
    {
        f32 x, y, z, w;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0] + v.w_ * m.m_[3][0];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1] + v.w_ * m.m_[3][1];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2] + v.w_ * m.m_[3][2];
        w = v.x_ * m.m_[0][3] + v.y_ * m.m_[1][3] + v.z_ * m.m_[2][3] + v.w_ * m.m_[3][3];

        x_ = x;
        y_ = y;
        z_ = z;
        w_ = w;
    }
}

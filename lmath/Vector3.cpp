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
    void Vector3::cross(const Vector3& v0, const Vector3& v1)
    {
//#if defined(LMATH_USE_SSE)
#if 0
        static const u32 RotMaskR = 201;
        static const u32 RotMaskL = 210;

        lm128 xv0 = load(v0.y_, v0.z_, v0.x_);
        lm128 xv1 = load(v1.z_, v1.x_, v1.y_);
        lm128 xv2 = _mm_mul_ps(xv0, xv1);

        xv0 = _mm_shuffle_ps(xv0, xv0, RotMaskR);
        xv1 = _mm_shuffle_ps(xv1, xv1, RotMaskL);

        lm128 xv3 = _mm_mul_ps(xv0, xv1);

        xv3 = _mm_sub_ps(xv2, xv3);
        store(*this, xv3);

#else
        f32 x = v0.y_ * v1.z_ - v0.z_ * v1.y_;
        f32 y = v0.z_ * v1.x_ - v0.x_ * v1.z_;
        f32 z = v0.x_ * v1.y_ - v0.y_ * v1.x_;
        x_ = x;
        y_ = y;
        z_ = z;
#endif
    }

    Vector3& Vector3::operator/=(f32 f)
    {
        LASSERT(false == lmath::isEqual(f, 0.0f));

#if defined(LMATH_USE_SSE)
        lm128 xv0 = load(*this);
        lm128 xv1 = load(f);
        lm128 xv2 = _mm_div_ps(xv0, xv1);
        store(*this, xv2);

#else
        f = 1.0f / f;
        x_ *= f;
        y_ *= f;
        z_ *= f;
#endif
        return *this;
    }

    void Vector3::normalize()
    {
        f32 l = lengthSqr();
        LASSERT( !(lmath::isEqual(l, 0.0f)) );

#if defined(LMATH_USE_SSE)
        lm128 xv0 = load(*this);
        lm128 xv1 = load(l);
        xv1 = _mm_sqrt_ps(xv1);
        lm128 xv2 = _mm_div_ps(xv0, xv1);
        store(*this, xv2);
#else
        //l = lmath::rsqrt(l);
        l = 1.0f/ lmath::sqrt(l);
        *this *= l;
#endif
    }

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
            Vector3 tmp = v0;
            tmp -= v1;
            tmp *= t;
            tmp += v1;
            *this = tmp;
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
#if defined(LMATH_USE_SSE)
        lm128 tv0 = Vector3::load(v.x_);
        lm128 tv1 = Vector3::load(v.y_);
        lm128 tv2 = Vector3::load(v.z_);

        lm128 tm0 = Vector3::load(m.m_[0][0], m.m_[1][0], m.m_[2][0]);
        lm128 tm1 = Vector3::load(m.m_[0][1], m.m_[1][1], m.m_[2][1]);
        lm128 tm2 = Vector3::load(m.m_[0][2], m.m_[1][2], m.m_[2][2]);
        lm128 tm3 = Vector3::load(m.m_[0][3], m.m_[1][3], m.m_[2][3]);

        tm0 = _mm_mul_ps(tm0, tv0);
        tm1 = _mm_mul_ps(tm1, tv1);
        tm2 = _mm_mul_ps(tm2, tv2);

        tm0 = _mm_add_ps(tm0, tm1);
        tm0 = _mm_add_ps(tm0, tm2);
        tm0 = _mm_add_ps(tm0, tm3);

        Vector3::store(*this, tm0);

#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2] + m.m_[0][3];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2] + m.m_[1][3];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2] + m.m_[2][3];
        x_ = x; y_ = y; z_ = z;
#endif
    }

    void Vector3::mul(const Vector3& v, const Matrix34& m)
    {
#if defined(LMATH_USE_SSE)
        lm128 tv0 = Vector3::load(v.x_);
        lm128 tv1 = Vector3::load(v.y_);
        lm128 tv2 = Vector3::load(v.z_);

        lm128 tm0 = _mm_loadu_ps(&m.m_[0][0]);
        lm128 tm1 = _mm_loadu_ps(&m.m_[1][0]);
        lm128 tm2 = _mm_loadu_ps(&m.m_[2][0]);
        lm128 tm3 = Vector3::load(m.m_[0][3], m.m_[1][3], m.m_[2][3]);

        tm0 = _mm_mul_ps(tm0, tv0);
        tm1 = _mm_mul_ps(tm1, tv1);
        tm2 = _mm_mul_ps(tm2, tv2);

        tm0 = _mm_add_ps(tm0, tm1);
        tm0 = _mm_add_ps(tm0, tm2);
        tm0 = _mm_add_ps(tm0, tm3);

        Vector3::store(*this, tm0);

#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0] + m.m_[0][3];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1] + m.m_[1][3];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2] + m.m_[2][3];
        x_ = x; y_ = y; z_ = z;
#endif
    }

    void Vector3::mul33(const Matrix34& m, const Vector3& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 tv0 = Vector3::load(v.x_);
        lm128 tv1 = Vector3::load(v.y_);
        lm128 tv2 = Vector3::load(v.z_);

        lm128 tm0 = Vector3::load(m.m_[0][0], m.m_[1][0], m.m_[2][0]);
        lm128 tm1 = Vector3::load(m.m_[0][1], m.m_[1][1], m.m_[2][1]);
        lm128 tm2 = Vector3::load(m.m_[0][2], m.m_[1][2], m.m_[2][2]);

        tm0 = _mm_mul_ps(tm0, tv0);
        tm1 = _mm_mul_ps(tm1, tv1);
        tm2 = _mm_mul_ps(tm2, tv2);

        tm0 = _mm_add_ps(tm0, tm1);
        tm0 = _mm_add_ps(tm0, tm2);

        Vector3::store(*this, tm0);

#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2];
        x_ = x; y_ = y; z_ = z;
#endif
    }

    void Vector3::mul33(const Vector3& v, const Matrix34& m)
    {
#if defined(LMATH_USE_SSE)
        lm128 tv0 = Vector3::load(v.x_);
        lm128 tv1 = Vector3::load(v.y_);
        lm128 tv2 = Vector3::load(v.z_);

        lm128 tm0 = _mm_loadu_ps(&m.m_[0][0]);
        lm128 tm1 = _mm_loadu_ps(&m.m_[1][0]);
        lm128 tm2 = _mm_loadu_ps(&m.m_[2][0]);

        tm0 = _mm_mul_ps(tm0, tv0);
        tm1 = _mm_mul_ps(tm1, tv1);
        tm2 = _mm_mul_ps(tm2, tv2);

        tm0 = _mm_add_ps(tm0, tm1);
        tm0 = _mm_add_ps(tm0, tm2);

        Vector3::store(*this, tm0);

#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2];
        x_ = x; y_ = y; z_ = z;
#endif
    }

    void Vector3::mul33(const Matrix44& m, const Vector3& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 tv0 = Vector3::load(v.x_);
        lm128 tv1 = Vector3::load(v.y_);
        lm128 tv2 = Vector3::load(v.z_);

        lm128 tm0 = Vector3::load(m.m_[0][0], m.m_[1][0], m.m_[2][0]);
        lm128 tm1 = Vector3::load(m.m_[0][1], m.m_[1][1], m.m_[2][1]);
        lm128 tm2 = Vector3::load(m.m_[0][2], m.m_[1][2], m.m_[2][2]);

        tm0 = _mm_mul_ps(tm0, tv0);
        tm1 = _mm_mul_ps(tm1, tv1);
        tm2 = _mm_mul_ps(tm2, tv2);

        tm0 = _mm_add_ps(tm0, tm1);
        tm0 = _mm_add_ps(tm0, tm2);

        Vector3::store(*this, tm0);

#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2];
        x_ = x; y_ = y; z_ = z;
#endif
    }

    void Vector3::mul33(const Vector3& v, const Matrix44& m)
    {
#if defined(LMATH_USE_SSE)
        lm128 tv0 = Vector3::load(v.x_);
        lm128 tv1 = Vector3::load(v.y_);
        lm128 tv2 = Vector3::load(v.z_);

        lm128 tm0 = _mm_loadu_ps(&m.m_[0][0]);
        lm128 tm1 = _mm_loadu_ps(&m.m_[1][0]);
        lm128 tm2 = _mm_loadu_ps(&m.m_[2][0]);

        tm0 = _mm_mul_ps(tm0, tv0);
        tm1 = _mm_mul_ps(tm1, tv1);
        tm2 = _mm_mul_ps(tm2, tv2);

        tm0 = _mm_add_ps(tm0, tm1);
        tm0 = _mm_add_ps(tm0, tm2);

        Vector3::store(*this, tm0);

#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2];
        x_ = x; y_ = y; z_ = z;
#endif
    }

    void Vector3::rotate(const Quaternion& rotation)
    {
        //Q x V x conjugate(Q)
        Quaternion conj;
        conj.conjugate(rotation);
        Quaternion rot;

#if 0
        rot.mul(rotation, conjugate);
        rot.mul(rot, *this);
#else
        rot.mul(rotation, *this);
        rot.mul(rot, conj);
#endif
        x_ = rot.x_;
        y_ = rot.y_;
        z_ = rot.z_;
    }
}

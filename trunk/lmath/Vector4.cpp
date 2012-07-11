/**
@file Vector4.cpp
@author t-sakai
@date 2009/01/17 create
*/
#include "Vector4.h"
#include "Matrix34.h"
#include "Matrix44.h"
#include "Quaternion.h"

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

    void Vector4::setLength()
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        r0 = _mm_mul_ps(r0, r0);
        r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0x4E), r0);
        r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0xB1), r0);

        r0 = _mm_sqrt_ss(r0);
        r0 = _mm_shuffle_ps(r0, r0, 0x00);
        store(*this, r0);
#else
        x_ = x_ * x_;
        y_ = y_ * y_;
        z_ = z_ * z_;
        w_ = w_ * w_;
#endif
    }

    f32 Vector4::length() const
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        r0 = _mm_mul_ps(r0, r0);
        r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0x4E), r0);
        r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0xB1), r0);

        r0 = _mm_sqrt_ss(r0);
        f32 ret;
        _mm_store_ss(&ret, r0);
        return ret;
#else
        return lmath::sqrt( lengthSqr() );
#endif
    }

    f32 Vector4::lengthSqr() const
    {
        return ( x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
    }

    void Vector4::normalize()
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = r0;
        r0 = _mm_mul_ps(r0, r0);
        r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0x4E), r0);
        r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0xB1), r0);

        r0 = _mm_sqrt_ss(r0);
        r0 = _mm_shuffle_ps(r0, r0, 0);
        
#if 1
        r0 = rcp(r0);
        r1 = _mm_mul_ps(r1, r0);
#else
        r1 = _mm_div_ps(r1, r0);
#endif
        store(*this, r1);
#else
        f32 l = lengthSqr();
        LASSERT( !(lmath::isEqual(l, 0.0f)) );
        //l = lmath::rsqrt(l);
        l = 1.0f/ lmath::sqrt(l);
        *this *= l;
#endif
    }

    f32 Vector4::dot(const Vector4& v) const
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = load(v);
        r0 = _mm_mul_ps(r0, r1);
        r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0x4E), r0);
        r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0xB1), r0);

        f32 ret;
        _mm_store_ss(&ret, r0);
        return ret;
#else
        return ( x_ * v.x_ + y_ * v.y_ + z_ * v.z_ + w_ * v.w_);
#endif
    }

    void Vector4::cross3(const Vector4& v0, const Vector4& v1)
    {
#if defined(LMATH_USE_SSE)
        lm128 xv0 = load(v0);
        lm128 xv1 = load(v1);

        lm128 xv2 = _mm_shuffle_ps(xv1, xv1, 0xC9);

        lm128 tmp0 = _mm_mul_ps(xv0, xv2);
        xv0 = _mm_shuffle_ps(xv0, xv0, 0xC9);
        lm128 tmp1 = _mm_mul_ps(xv0, xv1);

        tmp0 = _mm_sub_ps(tmp0, tmp1);

        tmp0 = _mm_shuffle_ps(tmp0, tmp0, 0xC9);

        store(*this, tmp0);

#else
        f32 x = v0.y_ * v1.z_ - v0.z_ * v1.y_;
        f32 y = v0.z_ * v1.x_ - v0.x_ * v1.z_;
        f32 z = v0.x_ * v1.y_ - v0.y_ * v1.x_;
        x_ = x;
        y_ = y;
        z_ = z;
#endif
    }


    f32 Vector4::distance(const Vector4& v) const
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = load(v);
        lm128 d = _mm_sub_ps(r0, r1);

        d = _mm_mul_ps(d, d);
        d = _mm_add_ps( _mm_shuffle_ps(d, d, 0x4E), d);
        d = _mm_add_ps( _mm_shuffle_ps(d, d, 0xB1), d);

        d = _mm_sqrt_ss(d);

        f32 ret;
        _mm_store_ss(&ret, d);
        return ret;

#else
        return lmath::sqrt( distanceSqr(v) );
#endif
    }

    f32 Vector4::distanceSqr(const Vector4& v) const
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = load(v);
        lm128 d = _mm_sub_ps(r0, r1);

        d = _mm_mul_ps(d, d);
        d = _mm_add_ps( _mm_shuffle_ps(d, d, 0x4E), d);
        d = _mm_add_ps( _mm_shuffle_ps(d, d, 0xB1), d);

        f32 ret;
        _mm_store_ss(&ret, d);
        return ret;

#else
        const f32 dx = x_ - v.x_;
        const f32 dy = y_ - v.y_;
        const f32 dz = z_ - v.z_;
        const f32 dw = w_ - v.w_;
        return ( dx * dx + dy * dy + dz * dz + dw * dw);
#endif
    }

    f32 Vector4::distance3(const Vector4& v) const
    {
        const f32 dx = x_ - v.x_;
        const f32 dy = y_ - v.y_;
        const f32 dz = z_ - v.z_;
        return lmath::sqrt( dx * dx + dy * dy + dz * dz );
    }

    void Vector4::setLerp(const Vector4& v1, const Vector4& v2, f32 f)
    {
#if defined(LMATH_USE_SSE)
        lm128 tmp0 = load(v1);
        lm128 tmp1 = load(v2);
        lm128 tmp2 = _mm_load1_ps(&f);

        tmp1 = _mm_sub_ps(tmp1, tmp0);
        tmp1 = _mm_mul_ps(tmp1, tmp2);
        tmp1 = _mm_add_ps(tmp1, tmp0);

        store(*this, tmp1);

#else
        Vector4 tmp = v2;
        tmp -= v1;
        tmp *= f;
        tmp += v1;
        *this = tmp;
#endif
    }


    void Vector4::mul(const Matrix34& m, const Vector4& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 tm0 = _mm_loadu_ps(&m.m_[0][0]);
        lm128 tm1 = _mm_loadu_ps(&m.m_[1][0]);
        lm128 tm2 = _mm_loadu_ps(&m.m_[2][0]);
        lm128 tm3 = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);

        _MM_TRANSPOSE4_PS(tm0, tm1, tm2, tm3);

        lm128 tv0 = _mm_load1_ps(&v.x_);
        lm128 tv1 = _mm_load1_ps(&v.y_);
        lm128 tv2 = _mm_load1_ps(&v.z_);
        lm128 tv3 = _mm_load1_ps(&v.w_);


        tm0 = _mm_mul_ps(tm0, tv0);
        tm1 = _mm_mul_ps(tm1, tv1);
        tm2 = _mm_mul_ps(tm2, tv2);
        tm3 = _mm_mul_ps(tm3, tv3);

        tm0 = _mm_add_ps(tm0, tm1);
        tm0 = _mm_add_ps(tm0, tm2);
        tm0 = _mm_add_ps(tm0, tm3);

        store(*this, tm0);

#else
        f32 x, y, z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2] + v.w_ * m.m_[0][3];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2] + v.w_ * m.m_[1][3];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2] + v.w_ * m.m_[2][3];
        x_ = x;
        y_ = y;
        z_ = z;
        w_ = v.w_;
#endif
    }

    void Vector4::mul(const Vector4& v, const Matrix34& m)
    {
#if defined(LMATH_USE_SSE)
        lm128 tv0 = _mm_load1_ps(&v.x_);
        lm128 tv1 = _mm_load1_ps(&v.y_);
        lm128 tv2 = _mm_load1_ps(&v.z_);
        lm128 tv3 = _mm_load1_ps(&v.w_);

        lm128 tm0 = _mm_loadu_ps(&m.m_[0][0]);
        lm128 tm1 = _mm_loadu_ps(&m.m_[1][0]);
        lm128 tm2 = _mm_loadu_ps(&m.m_[2][0]);
#if 0
        const f32 one = 1.0f;
        lm128 tm3 = _mm_load_ss(&one);
        tm3 = _mm_shuffle_ps(tm3, tm3, 0x2F);
#else
        f32 buffer[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        lm128 tm3 = _mm_loadu_ps(buffer);
#endif

        tm0 = _mm_mul_ps(tm0, tv0);
        tm1 = _mm_mul_ps(tm1, tv1);
        tm2 = _mm_mul_ps(tm2, tv2);
        tm3 = _mm_mul_ps(tm3, tv3);

        tm0 = _mm_add_ps(tm0, tm1);
        tm0 = _mm_add_ps(tm0, tm2);
        tm0 = _mm_add_ps(tm0, tm3);

        store(*this, tm0);

#else
        f32 x, y, z, w;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2];
        w = v.x_ * m.m_[0][3] + v.y_ * m.m_[1][3] + v.z_ * m.m_[2][3] + v.w_;

        x_ = x;
        y_ = y;
        z_ = z;
        w_ = w;
#endif
    }



    void Vector4::mul(const Matrix44& m, const Vector4& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 tm0 = _mm_loadu_ps(&m.m_[0][0]);
        lm128 tm1 = _mm_loadu_ps(&m.m_[1][0]);
        lm128 tm2 = _mm_loadu_ps(&m.m_[2][0]);
        lm128 tm3 = _mm_loadu_ps(&m.m_[3][0]);

        _MM_TRANSPOSE4_PS(tm0, tm1, tm2, tm3);


        lm128 tv;
        tv = _mm_load1_ps(&v.x_);
        tm0 = _mm_mul_ps(tm0, tv);

        tv = _mm_load1_ps(&v.y_);
        tm1 = _mm_mul_ps(tm1, tv);

        tv = _mm_load1_ps(&v.z_);
        tm2 = _mm_mul_ps(tm2, tv);

        tv = _mm_load1_ps(&v.w_);
        tm3 = _mm_mul_ps(tm3, tv);

        tm0 = _mm_add_ps(tm0, tm1);
        tm0 = _mm_add_ps(tm0, tm2);
        tm0 = _mm_add_ps(tm0, tm3);

        store(*this, tm0);

#else
        f32 x, y, z, w;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2] + v.w_ * m.m_[0][3];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2] + v.w_ * m.m_[1][3];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2] + v.w_ * m.m_[2][3];
        w = v.x_ * m.m_[3][0] + v.y_ * m.m_[3][1] + v.z_ * m.m_[3][2] + v.w_ * m.m_[3][3];
        x_ = x;
        y_ = y;
        z_ = z;
        w_ = w;
#endif
    }

    void Vector4::mul(const Vector4& v, const Matrix44& m)
    {
#if defined(LMATH_USE_SSE)
        lm128 tv0 = _mm_load1_ps(&v.x_);
        lm128 tv1 = _mm_load1_ps(&v.y_);
        lm128 tv2 = _mm_load1_ps(&v.z_);
        lm128 tv3 = _mm_load1_ps(&v.w_);

        lm128 tm0 = _mm_loadu_ps(&m.m_[0][0]);
        lm128 tm1 = _mm_loadu_ps(&m.m_[1][0]);
        lm128 tm2 = _mm_loadu_ps(&m.m_[2][0]);
        lm128 tm3 = _mm_loadu_ps(&m.m_[3][0]);

        tm0 = _mm_mul_ps(tm0, tv0);
        tm1 = _mm_mul_ps(tm1, tv1);
        tm2 = _mm_mul_ps(tm2, tv2);
        tm3 = _mm_mul_ps(tm3, tv3);

        tm0 = _mm_add_ps(tm0, tm1);
        tm0 = _mm_add_ps(tm0, tm2);
        tm0 = _mm_add_ps(tm0, tm3);

        store(*this, tm0);

#else
        f32 x, y, z, w;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0] + v.w_ * m.m_[3][0];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1] + v.w_ * m.m_[3][1];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2] + v.w_ * m.m_[3][2];
        w = v.x_ * m.m_[0][3] + v.y_ * m.m_[1][3] + v.z_ * m.m_[2][3] + v.w_ * m.m_[3][3];

        x_ = x;
        y_ = y;
        z_ = z;
        w_ = w;
#endif
    }


    void Vector4::rotate(const Quaternion& rotation)
    {
        Quaternion conj;
        conj.conjugate(rotation);
        Quaternion rot;

        rot.mul(rotation, *this);
        rot.mul(rot, conj);

        x_ = rot.x_;
        y_ = rot.y_;
        z_ = rot.z_;
    }
}

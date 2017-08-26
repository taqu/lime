/**
@file Vector3.cpp
@author t-sakai
@date 2009/01/17 create
*/
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix34.h"
#include "Matrix44.h"
#include "Quaternion.h"

namespace lmath
{
    const Vector3 Vector3::Forward = {0.0f, 0.0f, 1.0f};
    const Vector3 Vector3::Backward= {0.0f, 0.0f, -1.0f};
    const Vector3 Vector3::Up = {0.0f, 1.0f, 0.0f};
    const Vector3 Vector3::Down = {0.0f, -1.0f, 0.0f};
    const Vector3 Vector3::Right = {1.0f, 0.0f, 0.0f};
    const Vector3 Vector3::Left = {-1.0f, 0.0f, 0.0f};

    Vector3 Vector3::construct(const Vector4& v)
    {
        return {v.x_, v.y_, v.z_};
    }

    void Vector3::set(const lmath::Vector4& v)
    {
        x_ = v.x_; y_ = v.y_; z_ = v.z_;
    }

    Vector3& Vector3::operator/=(f32 f)
    {
        LASSERT(!lmath::isZero(f));

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

    Vector3& Vector3::operator*=(const Vector3& v)
    {
        x_ *= v.x_;
        y_ *= v.y_;
        z_ *= v.z_;
        return *this;
    }

    Vector3& Vector3::operator/=(const Vector3& v)
    {
        LASSERT(!lmath::isZero(v.x_));
        LASSERT(!lmath::isZero(v.y_));
        LASSERT(!lmath::isZero(v.z_));

        x_ /= v.x_;
        y_ /= v.y_;
        z_ /= v.z_;
        return *this;
    }

    //--- Friend functions
    //--------------------------------------------------
    Vector3 operator+(const Vector3& v0, const Vector3& v1)
    {
        return {v0.x_+v1.x_, v0.y_+v1.y_, v0.z_+v1.z_};
    }

    Vector3 operator-(const Vector3& v0, const Vector3& v1)
    {
        return {v0.x_-v1.x_, v0.y_-v1.y_, v0.z_-v1.z_};
    }

    Vector3 operator*(f32 f, const Vector3& v)
    {
        return {f*v.x_, f*v.y_, f*v.z_};
    }

    Vector3 operator*(const Vector3& v, f32 f)
    {
        return {v.x_*f, v.y_*f, v.z_*f};
    }

    Vector3 operator*(const Vector3& v0, const Vector3& v1)
    {
        return {v0.x_*v1.x_, v0.y_*v1.y_, v0.z_*v1.z_};
    }

    Vector3&& operator/(const Vector3& v, f32 f)
    {
        LASSERT(!lmath::isZero(f));

#if defined(LMATH_USE_SSE)
        lm128 xv0 = Vector3::load(v);
        lm128 xv1 = Vector3::load(f);
        lm128 xv2 = _mm_div_ps(xv0, xv1);
        return Vector3::store(xv2);
#else
        f = 1.0f / f;
        return Vector3(v.x_*f, v.y_*f, v.z_*f);
#endif
    }

    Vector3&& operator/(const Vector3& v0, const Vector3& v1)
    {
        LASSERT(!lmath::isZero(v1.x_));
        LASSERT(!lmath::isZero(v1.y_));
        LASSERT(!lmath::isZero(v1.z_));

        lm128 xv0 = Vector3::load(v0);
        lm128 xv1 = Vector3::load(v1);
        lm128 xv2 = _mm_div_ps(xv0, xv1);
        return Vector3::store(xv2);
    }

    Vector3&& normalize(const Vector3& v)
    {
        f32 l = v.lengthSqr();
        LASSERT( !lmath::isZero(l) );

#if defined(LMATH_USE_SSE)
        lm128 xv0 = Vector3::load(v);
        lm128 xv1 = Vector3::load(l);
        xv1 = _mm_sqrt_ps(xv1);
        lm128 xv2 = _mm_div_ps(xv0, xv1);
        return Vector3::store(xv2);
#else
        //l = lmath::rsqrt(l);
        l = 1.0f/ lmath::sqrt(l);
        return Vector3(v.x_*l, v.y_*l, v.z_*l);
#endif
    }

    Vector3&& normalize(const Vector3& v, f32 lengthSqr)
    {
#if defined(LMATH_USE_SSE)
        lm128 xv0 = Vector3::load(v);
        lm128 xv1 = Vector3::load(lengthSqr);
        xv1 = _mm_sqrt_ps(xv1);
        lm128 xv2 = _mm_div_ps(xv0, xv1);
        return Vector3::store(xv2);
#else
        f32 l = 1.0f/ lmath::sqrt(lengthSqr);
        return Vector3(v.x_*l, v.y_*l, v.z_*l);
#endif
    }

    Vector3&& normalizeChecked(const Vector3& v)
    {
        f32 l = v.lengthSqr();
        if(lmath::isZeroPositive(l)){
            return lcore::move(Vector3::zero());
        }else{
            return normalize(v, l);
        }
    }

    Vector3 absolute(const Vector3& v)
    {
        return {lcore::absolute(v.x_), lcore::absolute(v.y_), lcore::absolute(v.z_)};
    }

    f32 distanceSqr(const Vector3& v0, const Vector3& v1)
    {
        const f32 dx = v0.x_ - v1.x_;
        const f32 dy = v0.y_ - v1.y_;
        const f32 dz = v0.z_ - v1.z_;
        return (dx*dx + dy*dy + dz*dz);
    }

    Vector3 cross(const Vector3& v0, const Vector3& v1)
    {
//#if defined(LMATH_USE_SSE)
#if 0
        static const u32 RotMaskR = 201;
        static const u32 RotMaskL = 210;

        lm128 xv0 = Vector3::load(v0.y_, v0.z_, v0.x_);
        lm128 xv1 = Vector3::load(v1.z_, v1.x_, v1.y_);
        lm128 xv2 = _mm_mul_ps(xv0, xv1);

        xv0 = _mm_shuffle_ps(xv0, xv0, RotMaskR);
        xv1 = _mm_shuffle_ps(xv1, xv1, RotMaskL);

        lm128 xv3 = _mm_mul_ps(xv0, xv1);

        xv3 = _mm_sub_ps(xv2, xv3);
        Vector3 v;
        Vector3::store(v, xv3);
        return v;

#else
        f32 x = v0.y_ * v1.z_ - v0.z_ * v1.y_;
        f32 y = v0.z_ * v1.x_ - v0.x_ * v1.z_;
        f32 z = v0.x_ * v1.y_ - v0.y_ * v1.x_;
        return {x,y,z};
#endif
    }

    Vector3&& lerp(const Vector3& v0, const Vector3& v1, f32 t)
    {
        Vector3 tmp = {v1.x_-v0.x_, v1.y_-v0.y_, v1.z_-v0.z_};
        tmp.x_ = tmp.x_*t + v0.x_;
        tmp.y_ = tmp.y_*t + v0.y_;
        tmp.z_ = tmp.z_*t + v0.z_;
        return lcore::move(tmp);
    }

    Vector3 lerp(const Vector3& v0, const Vector3& v1, f32 t0, f32 t1)
    {
        Vector3 tmp0 = {v0.x_*t1, v0.y_*t1, v0.z_*t1};
        Vector3 tmp1 = {v1.x_*t0, v1.y_*t0, v1.z_*t0};
        return {tmp0.x_+tmp1.x_, tmp0.y_+tmp1.y_, tmp0.z_+tmp1.z_};
    }


    Vector3&& mul(const Matrix34& m, const Vector3& v)
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

        return Vector3::store(tm0);
#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2] + m.m_[0][3];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2] + m.m_[1][3];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2] + m.m_[2][3];
        return Vector3(x,y,z);
#endif
    }

    Vector3&& mul(const Vector3& v, const Matrix34& m)
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

        return Vector3::store(tm0);
#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0] + m.m_[0][3];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1] + m.m_[1][3];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2] + m.m_[2][3];
        return Vector3(x,y,z);
#endif
    }

    Vector3&& mul33(const Matrix34& m, const Vector3& v)
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

        return Vector3::store(tm0);

#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2];
        return Vector3(x,y,z);
#endif
    }

    Vector3&& mul33(const Vector3& v, const Matrix34& m)
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

        return Vector3::store(tm0);

#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2];
        return Vector3(x,y,z);
#endif
    }

    Vector3&& mul33(const Matrix44& m, const Vector3& v)
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

        return Vector3::store(tm0);

#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[0][1] + v.z_ * m.m_[0][2];
        y = v.x_ * m.m_[1][0] + v.y_ * m.m_[1][1] + v.z_ * m.m_[1][2];
        z = v.x_ * m.m_[2][0] + v.y_ * m.m_[2][1] + v.z_ * m.m_[2][2];
        return Vector3(x,y,z);
#endif
    }

    Vector3&& mul33(const Vector3& v, const Matrix44& m)
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

        return Vector3::store(tm0);

#else
        f32 x,y,z;
        x = v.x_ * m.m_[0][0] + v.y_ * m.m_[1][0] + v.z_ * m.m_[2][0];
        y = v.x_ * m.m_[0][1] + v.y_ * m.m_[1][1] + v.z_ * m.m_[2][1];
        z = v.x_ * m.m_[0][2] + v.y_ * m.m_[1][2] + v.z_ * m.m_[2][2];
        return Vector3(x,y,z);
#endif
    }

    Vector3 rotate(const Vector3& v, const Quaternion& rotation)
    {
        //conjugate(Q) x V x Q
        Quaternion conj = conjugate(rotation);
        Quaternion rot = mul(conj, v);
        rot = mul(rot, rotation);

        return {rot.x_, rot.y_, rot.z_};
    }

    Vector3 rotate(const Quaternion& rotation, const Vector3& v)
    {
        //conjugate(Q) x V x Q
        Quaternion conj = conjugate(rotation);
        Quaternion rot = mul(conj, v);
        rot = mul(rot, rotation);

        return {rot.x_, rot.y_, rot.z_};
    }

    Vector3 mul(const Vector3& v0, const Vector3& v1)
    {
        return {v0.x_ * v1.x_, v0.y_ * v1.y_, v0.z_ * v1.z_};
    }

    Vector3&& div(const Vector3& v0, const Vector3& v1)
    {
        LASSERT(!lmath::isZero(v1.x_));
        LASSERT(!lmath::isZero(v1.y_));
        LASSERT(!lmath::isZero(v1.z_));

        lm128 xv0 = Vector3::load(v0);
        lm128 xv1 = Vector3::load(v1);
        lm128 xv2 = _mm_div_ps(xv0, xv1);
        return Vector3::store(xv2);
    }

    // v0*v1 + v2
    Vector3&& muladd(const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        lm128 xv0 = Vector3::load(v0);
        lm128 xv1 = Vector3::load(v1);
        lm128 xv2 = Vector3::load(v2);
        lm128 xv3 = _mm_add_ps(_mm_mul_ps(xv0, xv1), xv2);
        return Vector3::store(xv3);
    }

    // a*v1 + v2
    Vector3&& muladd(f32 a, const Vector3& v0, const Vector3& v1)
    {
        lm128 xv0 = Vector3::load(a);
        lm128 xv1 = Vector3::load(v0);
        lm128 xv2 = Vector3::load(v1);
        lm128 xv3 = _mm_add_ps(_mm_mul_ps(xv0, xv1), xv2);
        return Vector3::store(xv3);
    }
}

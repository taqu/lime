/**
@file Quaternion.cpp
@author t-sakai
@date 2009/09/21
*/
#include <lcore/lcore.h>
#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix34.h"

namespace lmath
{
    f32 Quaternion::length() const
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

    void Quaternion::setRotateX(f32 radian)
    {
        f32 over2 = radian * 0.5f;

        w_ = lmath::cosf(over2);
        x_ = lmath::sinf(over2);
        y_ = 0.0f;
        z_ = 0.0f;
    }

    void Quaternion::setRotateY(f32 radian)
    {
        f32 over2 = radian * 0.5f;

        w_ = lmath::cosf(over2);
        x_ = 0.0f;
        y_ = lmath::sinf(over2);
        z_ = 0.0f;
    }

    void Quaternion::setRotateZ(f32 radian)
    {
        f32 over2 = radian * 0.5f;

        w_ = lmath::cosf(over2);
        x_ = 0.0f;
        y_ = 0.0f;
        z_ = lmath::sinf(over2);
    }

    void Quaternion::setRotateAxis(f32 x, f32 y, f32 z, f32 radian)
    {
        //LASSERT( lmath::isEqual(axis.lengthSqr(), 1.0f) );

        f32 over2 = radian * 0.5f;
        f32 sinOver2 = lmath::sinf(over2);

        w_ = lmath::cosf(over2);
        x_ = x * sinOver2;
        y_ = y * sinOver2;
        z_ = z * sinOver2;
    }

    Quaternion& Quaternion::invert()
    {
        f32 lenSqr = lengthSqr();
        LASSERT(lmath::isEqual(lenSqr, 0.0f) == false);

        f32 oneOver = 1.0f / lenSqr;
        w_ *= oneOver;
        oneOver = -oneOver;
        x_*=oneOver;
        y_*=oneOver;
        z_*=oneOver;
        return *this;
    }

    void Quaternion::normalize()
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = _mm_mul_ps(r0, r0);
        r1 = _mm_add_ps( _mm_shuffle_ps(r1, r1, 0x4E), r1);
        r1 = _mm_add_ps( _mm_shuffle_ps(r1, r1, 0xB1), r1);

        r1 = _mm_sqrt_ss(r1);
        r1 = _mm_shuffle_ps(r1, r1, 0);
        r0 = _mm_div_ps(r0, r1);

        store(*this, r0);
#else
        f32 mag = lengthSqr();
        if(lmath::isEqual(mag, 0.0f)){
            LASSERT(false && "Quaternion::normalize magnitude is zero");
            identity();
            return;
        }
        f32 magOver = 1.0f / lmath::sqrt(mag);
        w_ *= magOver;
        x_ *= magOver;
        y_ *= magOver;
        z_ *= magOver;
#endif
    }

    f32 Quaternion::dot(const Quaternion& q) const
    {
        f32 ret = w_*q.w_ + x_*q.x_ + y_*q.y_ + z_*q.z_;
        return ret;
    }

    Quaternion& Quaternion::operator+=(const Quaternion& q)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = load(q);
        r0 = _mm_add_ps(r0, r1);
        store(*this, r0);

#else
        w_ += q.w_;
        x_ += q.x_;
        y_ += q.y_;
        z_ += q.z_;
#endif
        return *this;
    }

    Quaternion& Quaternion::operator-=(const Quaternion& q)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = load(q);
        r0 = _mm_sub_ps(r0, r1);
        store(*this, r0);

#else
        w_ -= q.w_;
        x_ -= q.x_;
        y_ -= q.y_;
        z_ -= q.z_;
#endif
        return *this;
    }


    Quaternion& Quaternion::operator*=(f32 f)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = _mm_load1_ps(&f);
        r0 = _mm_mul_ps(r0, r1);
        store(*this, r0);

#else
        w_ *= f;
        x_ *= f;
        y_ *= f;
        z_ *= f;
#endif
        return *this;
    }

    f32 Quaternion::getRotationAngle() const
    {
        f32 thetaOver2 = lmath::acos(w_);
        return 2.0f * thetaOver2;
    }

    void Quaternion::getRotationAxis(lmath::Vector3& axis) const
    {
        f32 thetaOver2Sqr = 1.0f - w_*w_;

        if(thetaOver2Sqr <= F32_EPSILON){
            axis.set(1.0f, 0.0f, 0.0f);
        }

        f32 oneOverTheta = 1.0f / lmath::sqrt(thetaOver2Sqr);

        axis.set(x_ * oneOverTheta,
            y_ * oneOverTheta,
            z_ * oneOverTheta);

    }

    void Quaternion::getMatrix(lmath::Matrix34& mat) const
    {
#if defined(LMATH_USE_SSE)

        lm128 t0 = load(*this);
        lm128 t1 = _mm_mul_ps(t0, t0);
        t1 = _mm_add_ps(t1, t1);

        lm128 t2 = _mm_shuffle_ps(t0, t0, 0);
        t2 = _mm_mul_ps(t2, t0);
        t2 = _mm_add_ps(t2, t2);

        lm128 t3 = _mm_shuffle_ps(t0, t0, 0x78);
        t3 = _mm_mul_ps(t3, t0);
        t3 = _mm_add_ps(t3, t3);


        mat.set(
            1.0f - t1.m128_f32[2] - t1.m128_f32[3],
            t3.m128_f32[1] - t2.m128_f32[3],
            t3.m128_f32[3] + t2.m128_f32[2],
            0.0f,
            
            t3.m128_f32[1] + t2.m128_f32[3],
            1.0f - t1.m128_f32[1] - t1.m128_f32[3],
            t3.m128_f32[2] - t2.m128_f32[1],
            0.0f,
            
            t3.m128_f32[3] - t2.m128_f32[2],
            t3.m128_f32[2] + t2.m128_f32[1],
            1.0f - t1.m128_f32[1] - t1.m128_f32[2],
            0.0f);


#else
        f32 x2 = x_ * x_; x2 += x2;
        f32 y2 = y_ * y_; y2 += y2;
        f32 z2 = z_ * z_; z2 += z2;

        f32 wx = w_ * x_; wx += wx;
        f32 wy = w_ * y_; wy += wy;
        f32 wz = w_ * z_; wz += wz;

        f32 xy = x_ * y_; xy += xy;
        f32 xz = x_ * z_; xz += xz;
        f32 yz = y_ * z_; yz += yz;

#if 0
        mat.set(1.0f-y2-z2, xy+wz,      xz-wy,      0.0f,
                xy-wz,      1.0f-x2-z2, yz+wx,      0.0f,
                xz+wy,      yz-wx,      1.0f-x2-y2, 0.0f);

#else
        mat.set(1.0f-y2-z2, xy-wz,      xz+wy,      0.0f,
                xy+wz,      1.0f-x2-z2, yz-wx,      0.0f,
                xz-wy,      yz+wx,      1.0f-x2-y2, 0.0f);
#endif
#endif
    }


    void Quaternion::exp(f32 exponent)
    {
        if(lmath::isEqual(w_, 1.0f)){
            return;
        }

        f32 old = lmath::acos(w_);

        f32 theta = old * exponent;

        w_ = lmath::cosf(theta);

        f32 t = lmath::sinf(theta)/lmath::sinf(old);
        x_ *= t;
        y_ *= t;
        z_ *= t;
    }

    Quaternion& Quaternion::operator*=(const Quaternion& q)
    {
#if defined(LMATH_USE_SSE)
        f32 f;
        *((u32*)&f) = 0x80000000U;
        lm128 mask = _mm_load_ss(&f);

        lm128 tw = _mm_load1_ps(&w_);

        lm128 tx = _mm_load1_ps(&x_);
        tx = _mm_xor_ps(tx, _mm_shuffle_ps(mask, mask, 0x14));

        lm128 ty = _mm_load1_ps(&y_);
        ty = _mm_xor_ps(ty, _mm_shuffle_ps(mask, mask, 0x50));

        lm128 tz = _mm_load1_ps(&z_);
        tz = _mm_xor_ps(tz, _mm_shuffle_ps(mask, mask, 0x44));

        lm128 t0 = load(q);
        lm128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
        lm128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
        lm128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

        t0 = _mm_mul_ps(t0, tw);
        t0 = _mm_add_ps(t0, _mm_mul_ps(tx, t1));
        t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
        t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
        store(*this, t0);
#else
#if 0
        f32 w = w_ * q.w_ - x_ * q.x_ - y_ * q.y_ - z_ * q.z_;
        f32 x = w_ * q.x_ + x_ * q.w_ + y_ * q.z_ - z_ * q.y_;
        f32 y = w_ * q.y_ - x_ * q.z_ + y_ * q.w_ + z_ * q.x_;
        f32 z = w_ * q.z_ + x_ * q.y_ - y_ * q.x_ + z_ * q.w_;
#else
        f32 w = w_ * q.w_ - x_ * q.x_ - y_ * q.y_ - z_ * q.z_;
        f32 x = w_ * q.x_ + x_ * q.w_ - y_ * q.z_ + z_ * q.y_;
        f32 y = w_ * q.y_ + x_ * q.z_ + y_ * q.w_ - z_ * q.x_;
        f32 z = w_ * q.z_ - x_ * q.y_ + y_ * q.x_ + z_ * q.w_;
#endif
        w_ = w;
        x_ = x;
        y_ = y;
        z_ = z;
#endif
        return *this;
    }

    void Quaternion::mul(const Quaternion& q0, const Quaternion& q1)
    {
#if defined(LMATH_USE_SSE)
        f32 f;
        *((u32*)&f) = 0x80000000U;
        lm128 mask = _mm_load_ss(&f);

        lm128 tw = _mm_load1_ps(&q0.w_);

        lm128 tx = _mm_load1_ps(&q0.x_);
        tx = _mm_xor_ps(tx, _mm_shuffle_ps(mask, mask, 0x14));

        lm128 ty = _mm_load1_ps(&q0.y_);
        ty = _mm_xor_ps(ty, _mm_shuffle_ps(mask, mask, 0x50));

        lm128 tz = _mm_load1_ps(&q0.z_);
        tz = _mm_xor_ps(tz, _mm_shuffle_ps(mask, mask, 0x44));

        lm128 t0 = load(q1);
        lm128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
        lm128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
        lm128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

        t0 = _mm_mul_ps(t0, tw);
        t0 = _mm_add_ps(t0, _mm_mul_ps(tx, t1));
        t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
        t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
        store(*this, t0);
#else

#if 0
        f32 w = q0.w_ * q1.w_ - q0.x_ * q1.x_ - q0.y_ * q1.y_ - q0.z_ * q1.z_;
        f32 x = q0.w_ * q1.x_ + q0.x_ * q1.w_ + q0.y_ * q1.z_ - q0.z_ * q1.y_;
        f32 y = q0.w_ * q1.y_ - q0.x_ * q1.z_ + q0.y_ * q1.w_ + q0.z_ * q1.x_;
        f32 z = q0.w_ * q1.z_ + q0.x_ * q1.y_ - q0.y_ * q1.x_ + q0.z_ * q1.w_;
#else
        f32 w = q0.w_ * q1.w_ - q0.x_ * q1.x_ - q0.y_ * q1.y_ - q0.z_ * q1.z_;
        f32 x = q0.w_ * q1.x_ + q0.x_ * q1.w_ - q0.y_ * q1.z_ + q0.z_ * q1.y_;
        f32 y = q0.w_ * q1.y_ + q0.x_ * q1.z_ + q0.y_ * q1.w_ - q0.z_ * q1.x_;
        f32 z = q0.w_ * q1.z_ - q0.x_ * q1.y_ + q0.y_ * q1.x_ + q0.z_ * q1.w_;
#endif
        w_ = w;
        x_ = x;
        y_ = y;
        z_ = z;
#endif
    }

    void Quaternion::mul(const Vector3& v, const Quaternion& q)
    {
//#if defined(LMATH_USE_SSE)
#if 0
        f32 f;
        *((u32*)&f) = 0x80000000U;
        lm128 mask = _mm_load_ss(&f);

        lm128 tx = _mm_load1_ps(&v.x_);
        tx = _mm_xor_ps(tx, _mm_shuffle_ps(mask, mask, 0x14));

        lm128 ty = _mm_load1_ps(&v.y_);
        ty = _mm_xor_ps(ty, _mm_shuffle_ps(mask, mask, 0x50));

        lm128 tz = _mm_load1_ps(&v.z_);
        tz = _mm_xor_ps(tz, _mm_shuffle_ps(mask, mask, 0x44));

        lm128 t0 = load(q);
        lm128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
        lm128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
        lm128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

        t0 = _mm_mul_ps(tx, t1);
        t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
        t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
        store(*this, t0);
#else

#if 0
        f32 w = - v.x_ * q.x_ - v.y_ * q.y_ - v.z_ * q.z_;
        f32 x = + v.x_ * q.w_ + v.y_ * q.z_ - v.z_ * q.y_;
        f32 y = - v.x_ * q.z_ + v.y_ * q.w_ + v.z_ * q.x_;
        f32 z = + v.x_ * q.y_ - v.y_ * q.x_ + v.z_ * q.w_;
#else
        f32 w = - v.x_ * q.x_ - v.y_ * q.y_ - v.z_ * q.z_;
        f32 x = + v.x_ * q.w_ - v.y_ * q.z_ + v.z_ * q.y_;
        f32 y = + v.x_ * q.z_ + v.y_ * q.w_ - v.z_ * q.x_;
        f32 z = - v.x_ * q.y_ + v.y_ * q.x_ + v.z_ * q.w_;
#endif
        w_ = w;
        x_ = x;
        y_ = y;
        z_ = z;
#endif
    }

    void Quaternion::mul(const Quaternion& q, const Vector3& v)
    {
//#if defined(LMATH_USE_SSE)
#if 0
        f32 f;
        *((u32*)&f) = 0x80000000U;
        lm128 mask = _mm_load_ss(&f);

        lm128 tw = _mm_load1_ps(&q.w_);

        lm128 tx = _mm_load1_ps(&q.x_);
        tx = _mm_xor_ps(tx, _mm_shuffle_ps(mask, mask, 0x14));

        lm128 ty = _mm_load1_ps(&q.y_);
        ty = _mm_xor_ps(ty, _mm_shuffle_ps(mask, mask, 0x50));

        lm128 tz = _mm_load1_ps(&q.z_);
        tz = _mm_xor_ps(tz, _mm_shuffle_ps(mask, mask, 0x44));

        lm128 t0 = _mm_set_ps(v.z_, v.y_, v.x_, 0.0f);
        lm128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
        lm128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
        lm128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

        t0 = _mm_mul_ps(t0, tw);
        t0 = _mm_add_ps(t0, _mm_mul_ps(tx, t1));
        t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
        t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
        store(*this, t0);
#else

#if 0
        f32 w =             - q.x_ * v.x_ - q.y_ * v.y_ - q.z_ * v.z_;
        f32 x = q.w_ * v.x_               + q.y_ * v.z_ - q.z_ * v.y_;
        f32 y = q.w_ * v.y_ - q.x_ * v.z_               + q.z_ * v.x_;
        f32 z = q.w_ * v.z_ + q.x_ * v.y_ - q.y_ * v.x_;
#else
        
        f32 w =             - q.x_ * v.x_ - q.y_ * v.y_ - q.z_ * v.z_;
        f32 x = q.w_ * v.x_               - q.y_ * v.z_ + q.z_ * v.y_;
        f32 y = q.w_ * v.y_ + q.x_ * v.z_               - q.z_ * v.x_;
        f32 z = q.w_ * v.z_ - q.x_ * v.y_ + q.y_ * v.x_;
#endif
        w_ = w;
        x_ = x;
        y_ = y;
        z_ = z;
#endif
    }


    Quaternion& Quaternion::lerp(const Quaternion& q0, const Quaternion& q1, f32 t)
    {
#if defined(LMATH_USE_SSE)

        lm128 tmp0 = load(q0);
        lm128 tmp1 = load(q1);
        lm128 tmp2 = _mm_load1_ps(&t);

        tmp1 = _mm_sub_ps(tmp1, tmp0);
        tmp1 = _mm_mul_ps(tmp1, tmp2);
        tmp1 = _mm_add_ps(tmp1, tmp0);

        store(*this, tmp1);

#else
        Quaternion ret = q0;
        ret *= 1.0f - t;
        Quaternion q2 = q1;
            q2 *= t;
        ret += q2;
        *this = ret;
#endif
        return *this;
    }

    Quaternion& Quaternion::slerp(const Quaternion& q0, const Quaternion& q1, f32 t)
    {
#if defined(LMATH_USE_SSE)
        if(t <= 0.0f){
            *this = q0;
            return *this;
        }
        if(t >= 1.0f){
            *this = q1;
            return *this;
        }

        f32 cosOmega = q0.dot(q1);
        f32 d = (cosOmega < 0.0f)? -cosOmega : cosOmega;

        LASSERT(d <= 1.01f);

        lm128 k0, k1;
        if(d > 0.999f){
            //非常に近い場合は線形補間する
            k1 = _mm_load1_ps(&t);
            t = 1.0f - t;
            k0 = _mm_load1_ps(&t);
        }else{
            f32 sinOmega = lmath::sqrt(1.0f - d * d);

            f32 omega = lmath::atan2(sinOmega, d);

            f32 s = lmath::sinf((1.0f-t) * omega);
            k0 = _mm_load1_ps(&s);

            s = lmath::sinf(t * omega);
            k1 = _mm_load1_ps(&s);

            lm128 r = _mm_load1_ps(&sinOmega);

            k0 = _mm_div_ps(k0, r);
            k1 = _mm_div_ps(k1, r);
        }

        lm128 t0 = load(q0);
        lm128 t1 = load(q1);
        t0 = _mm_mul_ps(k0, t0);
        t1 = _mm_mul_ps(k1, t1);

        if(cosOmega < 0.0f){
            t0 = _mm_sub_ps(t0, t1);

        }else{
            t0 = _mm_add_ps(t0, t1);
        }

        store(*this, t0);

#else
        if(t <= 0.0f){
            *this = q0;
            return *this;
        }
        if(t >= 1.0f){
            *this = q1;
            return *this;
        }

        f32 cosOmega = q0.dot(q1);

        f32 q1w;
        f32 q1x;
        f32 q1y;
        f32 q1z;
        if(cosOmega < 0.0f){
            cosOmega = -cosOmega;
            q1w = -q1w;
            q1x = -q1x;
            q1y = -q1y;
            q1z = -q1z;
        }else{
            q1w = q1.w_;
            q1x = q1.x_;
            q1y = q1.y_;
            q1z = q1.z_;
        }

        LASSERT(cosOmega <= 1.01f);

        f32 k0, k1;

        if(cosOmega > 0.999f){
            //非常に近い場合は線形補間する
            k0 = 1.0f - t;
            k1 = t;
        }else{
            f32 sinOmega = lmath::sqrt(1.0f - cosOmega * cosOmega);

            f32 omega = lmath::atan2(sinOmega, cosOmega);

            f32 oneOverSinOmega = 1.0f / sinOmega;

            k0 = lmath::sinf((1.0f-t) * omega) * oneOverSinOmega;
            k1 = lmath::sinf(t * omega) * oneOverSinOmega;
        }

        w_ = k0 * q0.w_ + k1 * q1w;
        x_ = k0 * q0.x_ + k1 * q1x;
        y_ = k0 * q0.y_ + k1 * q1y;
        z_ = k0 * q0.z_ + k1 * q1z;
#endif

        return *this;
    }
}

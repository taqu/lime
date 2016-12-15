/**
@file Quaternion.cpp
@author t-sakai
@date 2009/09/21
*/
#include <lcore/lcore.h>
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix34.h"
#include "Matrix44.h"

namespace lmath
{
#if defined(LMATH_USE_SSE)
    //LALIGN16 u32 QuaternionConjugateMask_[4] =
    //{
    //    0x00000000U,
    //    0x80000000U,
    //    0x80000000U,
    //    0x80000000U,
    //};

    LALIGN16 u32 QuaternionMulMask0_[4] =
    {
        0x80000000U,
        0x00000000U,
        0x00000000U,
        0x80000000U,
    };

    LALIGN16 u32 QuaternionMulMask1_[4] =
    {
        0x80000000U,
        0x80000000U,
        0x00000000U,
        0x00000000U,
    };

    LALIGN16 u32 QuaternionMulMask2_[4] =
    {
        0x80000000U,
        0x00000000U,
        0x80000000U,
        0x00000000U,
    };
#endif

    void Quaternion::set(const Vector4& v)
    {
        w_ = v.w_; x_ = v.x_; y_ = v.y_; z_ = v.z_;
    }

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

    void Quaternion::setRotateXYZ(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);

        *this = rotX;
        *this *= rotY;
        *this *= rotZ;
    }

    void Quaternion::setRotateXZY(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);

        *this = rotX;
        *this *= rotZ;
        *this *= rotY;
    }

    void Quaternion::setRotateYZX(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);

        *this = rotY;
        *this *= rotZ;
        *this *= rotX;
    }

    void Quaternion::setRotateYXZ(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);

        *this = rotY;
        *this *= rotX;
        *this *= rotZ;
    }

    void Quaternion::setRotateZXY(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);

        *this = rotZ;
        *this *= rotX;
        *this *= rotY;
    }

    void Quaternion::setRotateZYX(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);

        *this = rotZ;
        *this *= rotY;
        *this *= rotX;
    }

    void Quaternion::setRotateAxis(const lmath::Vector3& axis, f32 radian)
    {
        setRotateAxis(axis.x_, axis.y_, axis.z_, radian);
    }

    void Quaternion::setRotateAxis(const lmath::Vector4& axis, f32 radian)
    {
        setRotateAxis(axis.x_, axis.y_, axis.z_, radian);
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

    void Quaternion::lookAt(const Vector3& eye, const Vector3& at)
    {
        LASSERT(!eye.isEqual(at));
        lookAt(lmath::normalize(at-eye));
    }

    void Quaternion::lookAt(const Vector3& dir)
    {
        LASSERT(!lmath::isZero(dir.length()));
        f32 d = dot(dir, Vector3::Forward);
        if(lmath::isEqual(d, -1.0f, LMATH_F32_DOT_EPSILON)){
            setRotateAxis(Vector3::Up.x_, Vector3::Up.y_, Vector3::Up.z_, PI);
            return;
        }

        if(lmath::isEqual(d, 1.0f, LMATH_F32_DOT_EPSILON)){
            identity();
            return;
        }

        f32 radian = lmath::acos(d);
        lmath::Vector3 axis = normalize(cross(Vector3::Forward, dir));
        setRotateAxis(axis.x_, axis.y_, axis.z_, radian);
    }

    void Quaternion::lookAt(const Vector4& eye, const Vector4& at)
    {
        LASSERT(!eye.isEqual(at));
        lookAt(normalize(at-eye));
    }

    void Quaternion::lookAt(const Vector4& dir)
    {
        LASSERT(!lmath::isZero(dir.length()));

        f32 d = dot(dir, Vector4::Forward);
        if(lmath::isEqual(d, -1.0f, LMATH_F32_DOT_EPSILON)){
            setRotateAxis(Vector4::Up.x_, Vector4::Up.y_, Vector4::Up.z_, PI);
            return;
        }

        if(lmath::isEqual(d, 1.0f, LMATH_F32_DOT_EPSILON)){
            identity();
            return;
        }

        f32 radian = lmath::acos(d);
        lmath::Vector4 axis = normalize(cross3(Vector4::Forward, dir));
        setRotateAxis(axis.x_, axis.y_, axis.z_, radian);
    }

    void Quaternion::getDireciton(Vector4& dir) const
    {
        f32 x = z_ * x_ + y_ * w_ + x_ * z_ + w_ * y_;
        f32 y = z_ * y_ + y_ * z_ - x_ * w_ - w_ * x_;
        f32 z = z_ * z_ - y_ * y_ - x_ * x_ + w_ * w_;

        dir.x_ = x;
        dir.y_ = y;
        dir.z_ = z;
        dir.w_ = 0.0f;
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
        lm128 r1 = _mm_set1_ps(f);
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

        if(thetaOver2Sqr <= LMATH_F32_EPSILON){
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
        t1 = _mm_add_ps(t1, t1); // 2*(ww, xx, yy, zz)

        lm128 t2 = _mm_shuffle_ps(t0, t0, 0);
        t2 = _mm_mul_ps(t2, t0);
        t2 = _mm_add_ps(t2, t2); // 2*(ww, wx, wy, wz)

        lm128 t3 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 3, 2, 0));
        t3 = _mm_mul_ps(t3, t0);
        t3 = _mm_add_ps(t3, t3); // 2*(ww, xy, yz, zx)


        lm128 r0 = _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(1, 3, 2, 0));
        r0 = _mm_add_ps(r0, t1);
        r0 = _mm_sub_ps(_mm_set1_ps(1.0f), r0); //(0, 1-x2-y2, 1-y2-z2, 1-z2-x2)

        lm128 t = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2, 1, 3, 0)); //(ww, wz, wx, wy)
        lm128 r1 = _mm_add_ps(t3, t); //(2ww, xy+wz, yz+wx, zx+wy)
        lm128 r2 = _mm_sub_ps(t3, t); //(0, xy-wz, yz-wx, zx-wy)

        lm128 zero = _mm_setzero_ps();
        r0 = _mm_move_ss(r0, zero);
        r1 = _mm_move_ss(r1, zero);
        r2 = _mm_move_ss(r2, zero);

        //1-y2-z2, 1-x2-z2, xy+wz, yz+wx
        t0 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(2, 1, 3, 2));

        //xy+wz, 1-x2-z2,
        t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 2, 1, 2));
        _mm_storel_pi((lm64*)&mat.m_[1][0], t1);

        //1-y2-z2, 1-x2-z2, xy-wz, yz-wx
        t0 = _mm_shuffle_ps(r0, r2, _MM_SHUFFLE(2, 1, 3, 2));

        //1-y2-z2, xy-wz
        t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(2, 0, 2, 0));
        _mm_storel_pi((lm64*)&mat.m_[0][0], t1);

        //xz-wy, xz-wy, yz+wx, yz+wx
        t0 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(2, 2, 3, 3));

        //xz-wy, yz+wx
        t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(2, 0, 2, 0));
        _mm_storel_pi((lm64*)&mat.m_[2][0], t1);


        //xz+wy, 0
        t0 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(0, 3, 0, 3));
        _mm_storel_pi((lm64*)&mat.m_[0][2], t0);

        //yz-wx, 0
        t0 = _mm_shuffle_ps(r2, r2, _MM_SHUFFLE(0, 2, 0, 2));
        _mm_storel_pi((lm64*)&mat.m_[1][2], t0);

        //1-x2-y2, 0
        t0 = _mm_shuffle_ps(r0, r0, _MM_SHUFFLE(0, 1, 0, 1));
        _mm_storel_pi((lm64*)&mat.m_[2][2], t0);

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

    void Quaternion::getMatrix(lmath::Matrix44& mat) const
    {
#if defined(LMATH_USE_SSE)

        lm128 t0 = load(*this);
        lm128 t1 = _mm_mul_ps(t0, t0);
        t1 = _mm_add_ps(t1, t1); // 2*(ww, xx, yy, zz)

        lm128 t2 = _mm_shuffle_ps(t0, t0, 0);
        t2 = _mm_mul_ps(t2, t0);
        t2 = _mm_add_ps(t2, t2); // 2*(ww, wx, wy, wz)

        lm128 t3 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 3, 2, 0));
        t3 = _mm_mul_ps(t3, t0);
        t3 = _mm_add_ps(t3, t3); // 2*(ww, xy, yz, zx)


        lm128 r0 = _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(1, 3, 2, 0));
        r0 = _mm_add_ps(r0, t1);
        r0 = _mm_sub_ps(_mm_set1_ps(1.0f), r0); //(0, 1-x2-y2, 1-y2-z2, 1-z2-x2)

        lm128 t = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2, 1, 3, 0)); //(ww, wz, wx, wy)
        lm128 r1 = _mm_add_ps(t3, t); //(2ww, xy+wz, yz+wx, zx+wy)
        lm128 r2 = _mm_sub_ps(t3, t); //(0, xy-wz, yz-wx, zx-wy)

        lm128 zero = _mm_setzero_ps();
        _mm_storeu_ps(&mat.m_[3][0], zero);
        r0 = _mm_move_ss(r0, zero);
        r1 = _mm_move_ss(r1, zero);
        r2 = _mm_move_ss(r2, zero);

        //1-y2-z2, 1-x2-z2, xy+wz, yz+wx
        t0 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(2, 1, 3, 2));

        //xy+wz, 1-x2-z2,
        t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 2, 1, 2));
        _mm_storel_pi((lm64*)&mat.m_[1][0], t1);

        //1-y2-z2, 1-x2-z2, xy-wz, yz-wx
        t0 = _mm_shuffle_ps(r0, r2, _MM_SHUFFLE(2, 1, 3, 2));

        //1-y2-z2, xy-wz
        t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(2, 0, 2, 0));
        _mm_storel_pi((lm64*)&mat.m_[0][0], t1);

        //xz-wy, xz-wy, yz+wx, yz+wx
        t0 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(2, 2, 3, 3));

        //xz-wy, yz+wx
        t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(2, 0, 2, 0));
        _mm_storel_pi((lm64*)&mat.m_[2][0], t1);


        //xz+wy, 0
        t0 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(0, 3, 0, 3));
        _mm_storel_pi((lm64*)&mat.m_[0][2], t0);

        //yz-wx, 0
        t0 = _mm_shuffle_ps(r2, r2, _MM_SHUFFLE(0, 2, 0, 2));
        _mm_storel_pi((lm64*)&mat.m_[1][2], t0);

        //1-x2-y2, 0
        t0 = _mm_shuffle_ps(r0, r0, _MM_SHUFFLE(0, 1, 0, 1));
        _mm_storel_pi((lm64*)&mat.m_[2][2], t0);

        mat.m_[3][3] = 1.0f;

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
                xz+wy,      yz-wx,      1.0f-x2-y2, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);

#else
        mat.set(1.0f-y2-z2, xy-wz,      xz+wy,      0.0f,
                xy+wz,      1.0f-x2-z2, yz-wx,      0.0f,
                xz-wy,      yz+wx,      1.0f-x2-y2, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
#endif
#endif
    }

    void Quaternion::getEulerAngles(f32& x, f32& y, f32& z)
    {
        f32 xx = x_ * x_;
        f32 yy = y_ * y_;
        f32 zz = z_ * z_;
        f32 ww = w_ * w_;

        f32 r11 = ww + xx - yy - zz;
        f32 r21 = 2.0f*(x_*y_ + w_*z_);
        f32 r31 = 2.0f*(x_*z_ - w_*y_);
        f32 r32 = 2.0f*(y_*z_ + w_*x_);
        f32 r33 = ww - xx - yy - zz;

        f32 tmp = lcore::absolute(r31);
        if(0.9999f<tmp){
            f32 r12 = 2.0f*(x_*y_ - w_*z_);
            f32 r13 = 2.0f*(x_*z_ + w_*y_);

            x = -PI_2 * r31/tmp;
            y = lmath::atan2(-r12, -r31*r13);
            z = 0.0f;
            return;
        }

        x = lmath::asin(-r31);
        y = lmath::atan2(r21, r11);
        z = lmath::atan2(r32, r33);
    }

    Quaternion& Quaternion::operator*=(const Quaternion& q)
    {
#if defined(LMATH_USE_SSE)
        lm128 mask;

        lm128 tw = _mm_set1_ps(w_);

        lm128 tx = _mm_set1_ps(x_);
        mask = _mm_load_ps((f32*)QuaternionMulMask0_);
        tx = _mm_xor_ps(tx, mask);

        lm128 ty = _mm_set1_ps(y_);
        mask = _mm_load_ps((f32*)QuaternionMulMask1_);
        ty = _mm_xor_ps(ty, mask);

        lm128 tz = _mm_set1_ps(z_);
        mask = _mm_load_ps((f32*)QuaternionMulMask2_);
        tz = _mm_xor_ps(tz, mask);

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


    Quaternion Quaternion::rotateX(f32 radian)
    {
        f32 over2 = radian * 0.5f;
        return Quaternion(lmath::cosf(over2), lmath::sinf(over2), 0.0f, 0.0f);
    }

    Quaternion Quaternion::rotateY(f32 radian)
    {
        f32 over2 = radian * 0.5f;

        return Quaternion(lmath::cosf(over2), 0.0f, lmath::sinf(over2), 0.0f);
    }

    Quaternion Quaternion::rotateZ(f32 radian)
    {
        f32 over2 = radian * 0.5f;

        return Quaternion(lmath::cosf(over2), 0.0f, 0.0f, lmath::sinf(over2));
    }

    Quaternion Quaternion::rotateXYZ(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);
        return mul(mul(rotX, rotY), rotZ);
    }

    Quaternion Quaternion::rotateXZY(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);
        return mul(mul(rotX, rotZ), rotY);
    }

    Quaternion Quaternion::rotateYZX(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);

        return mul(mul(rotY, rotZ), rotX);
    }

    Quaternion Quaternion::rotateYXZ(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);

        return mul(mul(rotY, rotX), rotZ);
    }

    Quaternion Quaternion::rotateZXY(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);

        return mul(mul(rotZ, rotX), rotY);
    }

    Quaternion Quaternion::rotateZYX(f32 radx, f32 rady, f32 radz)
    {
        lmath::Quaternion rotX, rotY, rotZ;
        rotX.setRotateX(radx);
        rotY.setRotateY(rady);
        rotZ.setRotateZ(radz);

        return mul(mul(rotZ, rotY), rotX);
    }

    Quaternion Quaternion::rotateAxis(const lmath::Vector3& axis, f32 radian)
    {
        return rotateAxis(axis.x_, axis.y_, axis.z_, radian);
    }

    Quaternion Quaternion::rotateAxis(const lmath::Vector4& axis, f32 radian)
    {
        return rotateAxis(axis.x_, axis.y_, axis.z_, radian);
    }

    Quaternion Quaternion::rotateAxis(f32 x, f32 y, f32 z, f32 radian)
    {
        //LASSERT( lmath::isEqual(axis.lengthSqr(), 1.0f) );

        f32 over2 = radian * 0.5f;
        f32 sinOver2 = lmath::sinf(over2);

        return Quaternion(lmath::cosf(over2), x*sinOver2, y*sinOver2, z*sinOver2);
    }

    Quaternion invert(const Quaternion& q)
    {
        f32 lenSqr = q.lengthSqr();
        LASSERT(lmath::isEqual(lenSqr, 0.0f) == false);

        lm128 r0 = Quaternion::load(q);
        lm128 r1 = _mm_mul_ps(r0, r0);
        r1 = _mm_add_ps( _mm_shuffle_ps(r1, r1, 0x4E), r1);
        r1 = _mm_add_ps( _mm_shuffle_ps(r1, r1, 0xB1), r1);

        r1 = _mm_sqrt_ss(r1);
        r1 = _mm_shuffle_ps(r1, r1, 0);
        r0 = _mm_div_ps(r0, r1);
        return conjugate(Quaternion::store(r0));
    }

    Quaternion normalize(const Quaternion& q)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Quaternion::load(q);
        lm128 r1 = _mm_mul_ps(r0, r0);
        r1 = _mm_add_ps( _mm_shuffle_ps(r1, r1, 0x4E), r1);
        r1 = _mm_add_ps( _mm_shuffle_ps(r1, r1, 0xB1), r1);

        r1 = _mm_sqrt_ss(r1);
        r1 = _mm_shuffle_ps(r1, r1, 0);
        r0 = _mm_div_ps(r0, r1);
        return Quaternion::store(r0);
#else
        f32 mag = lengthSqr();
        if(lmath::isEqual(mag, 0.0f)){
            LASSERT(false && "Quaternion::normalize magnitude is zero");
            identity();
            return;
        }
        f32 magOver = 1.0f / lmath::sqrt(mag);
        f32 w = q.w_ * magOver;
        f32 x = q.x_ * magOver;
        f32 y = q.y_ * magOver;
        f32 z = q.z_ * magOver;
        return Quaternion(x,y,z,w);
#endif
    }

    Quaternion normalize(const Quaternion& q, f32 squaredLength)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Quaternion::load(q);
        lm128 r1 = _mm_set1_ps(squaredLength);
        r1 = _mm_sqrt_ss(r1);
        r1 = _mm_shuffle_ps(r1, r1, 0);
        r0 = _mm_div_ps(r0, r1);
        return Quaternion::store(r0);
#else
        f32 magOver = 1.0f / lmath::sqrt(squaredLength);
        f32 w = q.w_ * magOver;
        f32 x = q.x_ * magOver;
        f32 y = q.y_ * magOver;
        f32 z = q.z_ * magOver;
        return Quaternion(x,y,z,w);;
#endif
    }

    f32 dot(const Quaternion& q0, const Quaternion& q1)
    {
        f32 ret = q0.w_*q1.w_ + q0.x_*q1.x_ + q0.y_*q1.y_ + q0.z_*q1.z_;
        return ret;
    }

    Quaternion exp(const Quaternion& q, f32 exponent)
    {
        if(0.9999f < lcore::absolute(q.w_)){
            return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
        }

        f32 old = lmath::acos(q.w_);

        f32 theta = old * exponent;
        f32 t = lmath::sinf(theta)/lmath::sinf(old);
        return Quaternion(lmath::cosf(theta), q.x_*t, q.y_*t, q.z_*t);
    }

    Quaternion mul(const Quaternion& q0, const Quaternion& q1)
    {
#if defined(LMATH_USE_SSE)
        lm128 mask;

        lm128 tw = _mm_set1_ps(q0.w_);

        lm128 tx = _mm_set1_ps(q0.x_);
        mask = _mm_load_ps((f32*)QuaternionMulMask0_);
        tx = _mm_xor_ps(tx, mask);

        lm128 ty = _mm_set1_ps(q0.y_);
        mask = _mm_load_ps((f32*)QuaternionMulMask1_);
        ty = _mm_xor_ps(ty, mask);

        lm128 tz = _mm_set1_ps(q0.z_);
        mask = _mm_load_ps((f32*)QuaternionMulMask2_);
        tz = _mm_xor_ps(tz, mask);


        lm128 t0 = Quaternion::load(q1);
        lm128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
        lm128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
        lm128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

        t0 = _mm_mul_ps(t0, tw);
        t0 = _mm_add_ps(t0, _mm_mul_ps(tx, t1));
        t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
        t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
        return Quaternion::store(t0);

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
        return Quaternion(w, x, y, z);
#endif
    }

    Quaternion mul(f32 a, const Quaternion& q)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = _mm_set1_ps(a);
        lm128 r1 = Quaternion::load(q);
        r0 = _mm_mul_ps(r0, r1);
        return Quaternion::store(r0);

#else
        return Quaternion(a*q.w_, a*q.x_, a*q.y_, a*q.z_);
#endif
    }

    Quaternion mul(const Vector3& v, const Quaternion& q)
    {
#if defined(LMATH_USE_SSE)
//#if 0
        lm128 mask;

        lm128 tx = _mm_set1_ps(v.x_);
        mask = _mm_load_ps((f32*)QuaternionMulMask0_);
        tx = _mm_xor_ps(tx, mask);

        lm128 ty = _mm_set1_ps(v.y_);
        mask = _mm_load_ps((f32*)QuaternionMulMask1_);
        ty = _mm_xor_ps(ty, mask);

        lm128 tz = _mm_set1_ps(v.z_);
        mask = _mm_load_ps((f32*)QuaternionMulMask2_);
        tz = _mm_xor_ps(tz, mask);


        lm128 t0 = Quaternion::load(q);
        lm128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
        lm128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
        lm128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

        t0 = _mm_mul_ps(tx, t1);
        t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
        t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
        return Quaternion::store(t0);

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
        return Quaternion(w, x, y, z);
#endif
    }

    Quaternion mul(const Quaternion& q, const Vector3& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 mask;

        lm128 tw = _mm_set1_ps(q.w_);

        lm128 tx = _mm_set1_ps(q.x_);
        mask = _mm_load_ps((f32*)QuaternionMulMask0_);
        tx = _mm_xor_ps(tx, mask);

        lm128 ty = _mm_set1_ps(q.y_);
        mask = _mm_load_ps((f32*)QuaternionMulMask1_);
        ty = _mm_xor_ps(ty, mask);

        lm128 tz = _mm_set1_ps(q.z_);
        mask = _mm_load_ps((f32*)QuaternionMulMask2_);
        tz = _mm_xor_ps(tz, mask);

        lm128 t0 = _mm_load_ss(&v.x_);
        lm128 t = _mm_loadl_pi(t0, reinterpret_cast<const __m64*>(&v.y_));
        t0 = _mm_shuffle_ps(t0, t, _MM_SHUFFLE(1, 0, 0, 1));


        lm128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
        lm128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
        lm128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

        t0 = _mm_mul_ps(t0, tw);
        t0 = _mm_add_ps(t0, _mm_mul_ps(tx, t1));
        t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
        t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
        return Quaternion::store(t0);

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
        return Quaternion(w, x, y, z);
#endif
    }


    Quaternion mul(const Vector4& v, const Quaternion& q)
    {
#if defined(LMATH_USE_SSE)
//#if 0
        lm128 mask;

        lm128 tx = _mm_set1_ps(v.x_);
        mask = _mm_load_ps((f32*)QuaternionMulMask0_);
        tx = _mm_xor_ps(tx, mask);

        lm128 ty = _mm_set1_ps(v.y_);
        mask = _mm_load_ps((f32*)QuaternionMulMask1_);
        ty = _mm_xor_ps(ty, mask);

        lm128 tz = _mm_set1_ps(v.z_);
        mask = _mm_load_ps((f32*)QuaternionMulMask2_);
        tz = _mm_xor_ps(tz, mask);

        lm128 t0 = Quaternion::load(q);
        lm128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
        lm128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
        lm128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

        t0 = _mm_mul_ps(tx, t1);
        t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
        t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
        return Quaternion::store(t0);

#else

        f32 w = - v.x_ * q.x_ - v.y_ * q.y_ - v.z_ * q.z_;
        f32 x = + v.x_ * q.w_ - v.y_ * q.z_ + v.z_ * q.y_;
        f32 y = + v.x_ * q.z_ + v.y_ * q.w_ - v.z_ * q.x_;
        f32 z = - v.x_ * q.y_ + v.y_ * q.x_ + v.z_ * q.w_;

        return Quaternion(w, x, y, z);
#endif
    }

    Quaternion mul(const Quaternion& q, const Vector4& v)
    {
#if defined(LMATH_USE_SSE)
//#if 0
        lm128 mask;

        lm128 tw = _mm_set1_ps(q.w_);

        lm128 tx = _mm_set1_ps(q.x_);
        mask = _mm_load_ps((f32*)QuaternionMulMask0_);
        tx = _mm_xor_ps(tx, mask);

        lm128 ty = _mm_set1_ps(q.y_);
        mask = _mm_load_ps((f32*)QuaternionMulMask1_);
        ty = _mm_xor_ps(ty, mask);

        lm128 tz = _mm_set1_ps(q.z_);
        mask = _mm_load_ps((f32*)QuaternionMulMask2_);
        tz = _mm_xor_ps(tz, mask);
        
        lm128 t0 = _mm_load_ss(&v.x_);
        t0 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(0, 1, 0, 1));
        t0 = _mm_loadh_pi(t0, reinterpret_cast<const __m64*>(&v.y_));

        lm128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
        lm128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
        lm128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

        t0 = _mm_mul_ps(t0, tw);
        t0 = _mm_add_ps(t0, _mm_mul_ps(tx, t1));
        t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
        t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
        return Quaternion::store(t0);
#else

        
        f32 w =             - q.x_ * v.x_ - q.y_ * v.y_ - q.z_ * v.z_;
        f32 x = q.w_ * v.x_               - q.y_ * v.z_ + q.z_ * v.y_;
        f32 y = q.w_ * v.y_ + q.x_ * v.z_               - q.z_ * v.x_;
        f32 z = q.w_ * v.z_ - q.x_ * v.y_ + q.y_ * v.x_;

        return Quaternion(w, x, y, z);
#endif
    }

    Quaternion rotateToward(const Vector4& from, const Vector4& to)
    {
        f32 cosine = dot(from, to);
        if(LMATH_F32_ANGLE_LIMIT1<lcore::absolute(cosine)){
            return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
        }
        lmath::Vector4 axis=cross3(from, to);
        axis.w_ = 0.0f;
        return Quaternion::rotateAxis(normalize(axis), lmath::acos(cosine));
    }

    Quaternion lerp(const Quaternion& q0, const Quaternion& q1, f32 t)
    {
#if defined(LMATH_USE_SSE)
        f32 cosOmega = dot(q0, q1);

        lm128 tmp0 = Quaternion::load(q0);
        lm128 tmp1 = (cosOmega<0.0f)? Quaternion::load(-q1) : Quaternion::load(q1);
        lm128 tmp2 = _mm_set1_ps(t);

        tmp1 = _mm_sub_ps(tmp1, tmp0);
        tmp1 = _mm_mul_ps(tmp1, tmp2);
        tmp1 = _mm_add_ps(tmp1, tmp0);


        //正規化
        lm128 r1 = _mm_mul_ps(tmp1, tmp1);
        r1 = _mm_add_ps( _mm_shuffle_ps(r1, r1, 0x4E), r1);
        r1 = _mm_add_ps( _mm_shuffle_ps(r1, r1, 0xB1), r1);

        r1 = _mm_sqrt_ss(r1);
        r1 = _mm_shuffle_ps(r1, r1, 0);
        tmp1 = _mm_div_ps(tmp1, r1);

        return Quaternion::store(tmp1);

#else
        f32 cosOmega = dot(q0, q1);
        Quaternion q2 = (cosOmega<0.0f)? -q1 : q1;

        Quaternion ret = q0;
        ret *= 1.0f - t;

        q2 *= t;
        ret += q2;
        return normalize(ret);
#endif
    }

    Quaternion slerp(const Quaternion& q0, const Quaternion& q1, f32 t)
    {
#if defined(LMATH_USE_SSE)
        LASSERT(0.0f<=t && t<=1.0f);

        //if(t <= 0.0f){
        //    *this = q0;
        //    return *this;
        //}
        //if(t >= 1.0f){
        //    *this = q1;
        //    return *this;
        //}

        f32 cosOmega = dot(q0, q1);
        f32 d = (cosOmega < 0.0f)? -cosOmega : cosOmega;

        LASSERT(d <= 1.01f);

        lm128 k0, k1;
        if(d > 0.99f){
            //非常に近い場合は線形補間する
            k1 = _mm_set1_ps(t);
            k0 = _mm_set1_ps(1.0f-t);
        }else{
            f32 sinOmega = lmath::sqrt(1.0f - d * d);

            f32 omega = lmath::atan2(sinOmega, d);

            f32 s = lmath::sinf((1.0f-t) * omega);
            k0 = _mm_set1_ps(s);

            s = lmath::sinf(t * omega);
            k1 = _mm_set1_ps(s);

            lm128 r = _mm_set1_ps(sinOmega);

            k0 = _mm_div_ps(k0, r);
            k1 = _mm_div_ps(k1, r);
        }

        lm128 t0 = Quaternion::load(q0);
        Quaternion q2 = (cosOmega < 0.0f)? -q1 : q1;
        lm128 t1 = Quaternion::load(q2);
        t0 = _mm_mul_ps(k0, t0);
        t1 = _mm_mul_ps(k1, t1);

        t0 = _mm_add_ps(t0, t1);

        return Quaternion::store(t0);

#else
        LASSERT(0.0f<=t);
        LASSERT(t<=1.0f);
        //if(t <= 0.0f){
        //    *this = q0;
        //    return *this;
        //}
        //if(t >= 1.0f){
        //    *this = q1;
        //    return *this;
        //}

        f32 cosOmega = dot(q0, q1);

        f32 q1w;
        f32 q1x;
        f32 q1y;
        f32 q1z;
        if(cosOmega < 0.0f){
            cosOmega = -cosOmega;
            q1w = -q1.w_;
            q1x = -q1.x_;
            q1y = -q1.y_;
            q1z = -q1.z_;
        }else{
            q1w = q1.w_;
            q1x = q1.x_;
            q1y = q1.y_;
            q1z = q1.z_;
        }

        LASSERT(cosOmega <= 1.01f);

        f32 k0, k1;

        if(cosOmega > 0.99f){
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

        f32 w = k0 * q0.w_ + k1 * q1w;
        f32 x = k0 * q0.x_ + k1 * q1x;
        f32 y = k0 * q0.y_ + k1 * q1y;
        f32 z = k0 * q0.z_ + k1 * q1z;
        return Quaternion(w,x,y,z);
#endif
    }

namespace
{
    Quaternion slerpNoCheck(const Quaternion& q0, const Quaternion& q1, f32 t)
    {
#if defined(LMATH_USE_SSE)
        LASSERT(0.0f<=t);
        LASSERT(t<=1.0f);

        f32 cosOmega = dot(q0, q1);

        lm128 k0, k1;
        if(cosOmega < -0.99f || 0.99f < cosOmega){
            //非常に近い場合は線形補間する
            k1 = _mm_set1_ps(t);
            k0 = _mm_set1_ps(1.0f-t);

        }else{
            f32 sinOmega = lmath::sqrt(1.0f - cosOmega * cosOmega);
            f32 oneOverSinOmega = 1.0f / sinOmega;

            f32 omega = lmath::atan2(sinOmega, cosOmega);

            f32 s0 = lmath::sinf((1.0f-t) * omega) * oneOverSinOmega;
            k0 = _mm_set1_ps(s0);

            f32 s1 = lmath::sinf(t * omega) * oneOverSinOmega;
            k1 = _mm_set1_ps(s1);
        }

        lm128 t0 = _mm_loadu_ps(&q0.w_);
        lm128 t1 = _mm_loadu_ps(&q1.w_);
        t0 = _mm_mul_ps(k0, t0);
        t1 = _mm_mul_ps(k1, t1);

        t0 = _mm_add_ps(t0, t1);

        return Quaternion::store(t0);

#else
        f32 cosOmega = dot(q0, q1);

        LASSERT(cosOmega <= 1.01f);

        f32 k0, k1;

        if( cosOmega < -0.99f || 0.99f < cosOmega){
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

        f32 w = k0 * q0.w_ + k1 * q1.w_;
        f32 x = k0 * q0.x_ + k1 * q1.x_;
        f32 y = k0 * q0.y_ + k1 * q1.y_;
        f32 z = k0 * q0.z_ + k1 * q1.z_;
        return Quaternion(w,x,y,z);
#endif
    }
}

    Quaternion squad(const Quaternion& q0, const Quaternion& q1, const Quaternion& a, const Quaternion& b, f32 t)
    {
        Quaternion t0 = slerpNoCheck(q0, q1, t);
        Quaternion t1 = slerpNoCheck(a, b, t);

        return slerpNoCheck(t0, t1, 2.0f*t*(1.0f-t));
    }
}

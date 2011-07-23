#ifndef INC_LMATH_QUATERNION_H__
#define INC_LMATH_QUATERNION_H__
/**
@file Quaternion.h
@author t-sakai
@date 2009/09/21 create
*/
#include "lmathcore.h"
#include "Vector3.h"

namespace lmath
{
    class Vector3;
    class Matrix34;

    class Quaternion
    {
    public:
        Quaternion()
        {
        }

        Quaternion(f32 w, f32 x, f32 y, f32 z)
            :w_(w),
            x_(x),
            y_(y),
            z_(z)
        {
        }

        ~Quaternion()
        {
        }

        inline void set(f32 w, f32 x, f32 y, f32 z)
        {
            w_ = w; x_ = x; y_ = y; z_ = z;
        }

        inline f32 operator[](s32 index) const;
        inline f32& operator[](s32 index);

        /// 単位四元数
        inline void identity();

        void setRotateX(f32 radian);
        void setRotateY(f32 radian);
        void setRotateZ(f32 radian);
        inline void setRotateAxis(const lmath::Vector3& axis, f32 radian);
        void setRotateAxis(f32 x, f32 y, f32 z, f32 radian);

        inline Quaternion conjugate() const;

        Quaternion& inverse();

        inline f32 length() const;
        inline f32 lengthSqr() const;

        void normalize();

        f32 dot(const Quaternion& q) const;

        inline Quaternion operator-() const;

        Quaternion& operator+=(const Quaternion& q);
        Quaternion& operator-=(const Quaternion& q);

        Quaternion& operator*=(f32 f);
        //
        Quaternion& operator*=(const Quaternion& q);

        f32 getRotationAngle() const;

        void getRotationAxis(lmath::Vector3& axis) const;
        void getMatrix(lmath::Matrix34& mat) const;

        void exp(f32 exponent);

        void mul(const Quaternion& q0, const Quaternion& q1);

        void mul(const Vector3& v, const Quaternion& q);
        void mul(const Quaternion& q, const Vector3& v);

        /**
        @brief 線形補間。q = (1-t)*q1 + t*q2
        @param q0 ... 
        @param q1 ... 
        @param t ... 補間比
        */
        Quaternion& lerp(const Quaternion& q0, const Quaternion& q1, f32 t);

        /**
        @brief 球面線形補間。q = (1-t)*q1 + t*q2
        @param q0 ... 
        @param q1 ... 
        @param t ... 補間比
        */
        Quaternion& slerp(const Quaternion& q0, const Quaternion& q1, f32 t);

        inline bool isNan() const;

        f32 w_, x_, y_, z_;
    };

    inline Quaternion Quaternion::operator-() const
    {
        return Quaternion(-w_, -x_, -y_, -z_);
    }

    inline f32 Quaternion::operator[](s32 index) const
    {
        LASSERT(0<=index && index < 4);
        return (&w_)[index];
    }

    inline f32& Quaternion::operator[](s32 index)
    {
        LASSERT(0<=index && index < 4);
        return (&w_)[index];
    }

    //---------------------------------------
    // 単位四元数
    inline void Quaternion::identity()
    {
        w_ = 1.0f;
        x_ = y_ = z_ = 0.0f;
    }

    inline void Quaternion::setRotateAxis(const lmath::Vector3& axis, f32 radian)
    {
        setRotateAxis(axis.x_, axis.y_, axis.z_, radian);
    }

    inline f32 Quaternion::length() const
    {
        return lmath::sqrt( lengthSqr() );
    }

    inline f32 Quaternion::lengthSqr() const
    {
        return ( w_*w_ + x_*x_ + y_*y_ + z_*z_);
    }

    inline Quaternion Quaternion::conjugate() const
    {
        return Quaternion(w_, -x_, -y_, -z_);
    }

    inline bool Quaternion::isNan() const
    {
        return (lcore::isNan(w_) || lcore::isNan(x_) || lcore::isNan(y_) || lcore::isNan(z_));
    }

}

#endif //INC_LMATH_QUATERNION_H__

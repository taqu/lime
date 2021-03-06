﻿#ifndef INC_LMATH_VECTOR3_H__
#define INC_LMATH_VECTOR3_H__
/**
@file Vector3.h
@author t-sakai
@date 2009/01/17 create
*/
#include "lmath.h"

namespace lmath
{
    //--------------------------------------------
    //---
    //--- Vector3
    //---
    //--------------------------------------------
    class Vector3
    {
    public:
        static const Vector3 Forward;
        static const Vector3 Backward;
        static const Vector3 Up;
        static const Vector3 Down;
        static const Vector3 Right;
        static const Vector3 Left;

        inline static Vector3 construct(f32 xyz);
        inline static Vector3 construct(f32 x, f32 y, f32 z);
        inline static Vector3 zero();
        inline static Vector3 one();
        static Vector3 construct(const Vector4& v);

        inline void set(f32 x, f32 y, f32 z);

        void set(const Vector4& v);

        inline f32 operator[](s32 index) const;
        inline f32& operator[](s32 index);
        inline Vector3 operator-() const;

        inline Vector3& operator+=(const Vector3& v);
        inline Vector3& operator-=(const Vector3& v);

        inline Vector3& operator*=(f32 f);
        Vector3& operator/=(f32 f);

        Vector3& operator*=(const Vector3& v);
        Vector3& operator/=(const Vector3& v);

        inline bool isEqual(const Vector3& v) const;
        inline bool isEqual(const Vector3& v, f32 epsilon) const;

        inline bool operator==(const Vector3& v) const;
        inline bool operator!=(const Vector3& v) const;

        inline f32 length() const;
        inline f32 lengthSqr() const;
        
        inline void swap(Vector3& rhs);

        inline bool isNan() const;

        inline static lm128 load(const Vector3& v)
        {
            return load3(&v.x_);
        }

        inline static lm128 load(f32 x, f32 y, f32 z)
        {
            return set_m128(x, y, z, 1.0f);
        }

        inline static lm128 load(f32 v)
        {
            return _mm_set1_ps(v);
        }

        inline static void store(Vector3& v, const lm128& r)
        {
            store3(&v.x_, r);
        }

        inline static Vector3&& store(const lm128& r)
        {
            Vector3 v;
            store3(&v.x_, r);
            return lcore::move(v);
        }

        //--- Friend functions
        //--------------------------------------------------
        friend Vector3 operator+(const Vector3& v0, const Vector3& v1);
        friend Vector3 operator-(const Vector3& v0, const Vector3& v1);
        friend Vector3 operator*(f32 f, const Vector3& v);
        friend Vector3 operator*(const Vector3& v, f32 f);
        friend Vector3 operator*(const Vector3& v0, const Vector3& v1);
        friend Vector3&& operator/(const Vector3& v, f32 f);
        friend Vector3&& operator/(const Vector3& v0, const Vector3& v1);
        friend Vector3&& normalize(const Vector3& v);
        friend Vector3&& normalize(const Vector3& v, f32 lengthSqr);
        friend Vector3&& normalizeChecked(const Vector3& v);
        friend Vector3 absolute(const Vector3& v);

        friend f32 dot(const Vector3& v0, const Vector3& v1);
        friend f32 distanceSqr(const Vector3& v0, const Vector3& v1);
        friend f32 distance(const Vector3& v0, const Vector3& v1);

        friend Vector3 cross(const Vector3& v0, const Vector3& v1);

        /**
        @brief 線形補間。v = (1-t)*v0 + t*v1
        @param v0 ... 
        @param v1 ... 
        */
        friend Vector3&& lerp(const Vector3& v0, const Vector3& v1, f32 t);

        /**
        @brief 線形補間。v = t1*v0 + t0*v1
        @param v0 ... 
        @param v1 ... 
        */
        friend Vector3 lerp(const Vector3& v0, const Vector3& v1, f32 t0, f32 t1);

        friend Vector3 mul(f32 f, const Vector3& v);
        friend Vector3&& mul(const Vector3& v, f32 f);

        friend Vector3&& mul(const Matrix34& m, const Vector3& v);
        friend Vector3&& mul(const Vector3& v, const Matrix34& m);

        friend Vector3&& mul33(const Matrix34& m, const Vector3& v);
        friend Vector3&& mul33(const Vector3& v, const Matrix34& m);

        friend Vector3&& mul33(const Matrix44& m, const Vector3& v);
        friend Vector3&& mul33(const Vector3& v, const Matrix44& m);

        friend Vector3 rotate(const Vector3& v, const Quaternion& rotation);
        friend Vector3 rotate(const Quaternion& rotation, const Vector3& v);

        friend Vector3 add(const Vector3& v0, const Vector3& v1);
        friend Vector3 sub(const Vector3& v0, const Vector3& v1);
        friend Vector3 mul(const Vector3& v0, const Vector3& v1);
        friend Vector3&& div(const Vector3& v0, const Vector3& v1);

        friend Vector3 minimum(const Vector3& v0, const Vector3& v1);
        friend Vector3 maximum(const Vector3& v0, const Vector3& v1);

        friend f32 minimum(const Vector3& v);
        friend f32 maximum(const Vector3& v);

        /**
        @brief v0*v1 + v2
        */
        friend Vector3&& muladd(const Vector3& v0, const Vector3& v1, const Vector3& v2);

        /**
        @brief a*v1 + v2
        */
        friend Vector3&& muladd(f32 a, const Vector3& v0, const Vector3& v1);

        //--------------------------------------------------
        f32 x_;
        f32 y_;
        f32 z_;
    };

    //--------------------------------------------
    //---
    //--- Vector3
    //---
    //--------------------------------------------
    static_assert(std::is_trivially_copyable<Vector3>::value, "Vector3 must be trivially copyable");

    inline Vector3 Vector3::construct(f32 xyz)
    {
        return {xyz, xyz, xyz};
    }

    inline Vector3 Vector3::construct(f32 x, f32 y, f32 z)
    {
        return {x, y, z};
    }

    inline Vector3 Vector3::zero()
    {
        return {0.0f, 0.0f, 0.0f};
    }

    inline Vector3 Vector3::one()
    {
        return {1.0f, 1.0f, 1.0f};
    }

    inline void Vector3::set(f32 x, f32 y, f32 z)
    {
        x_ = x; y_ = y; z_ = z;
    }

    inline f32 Vector3::operator[](s32 index) const
    {
        LASSERT(0<=index
            && index < 3);
        return (&x_)[index];
    }

    inline f32& Vector3::operator[](s32 index)
    {
        LASSERT(0<=index
            && index < 3);
        return (&x_)[index];
    }

    inline Vector3 Vector3::operator-() const
    {
        return {-x_, -y_, -z_};
    }

    inline Vector3& Vector3::operator+=(const Vector3& v)
    {
        x_ += v.x_;
        y_ += v.y_;
        z_ += v.z_;
        return *this;
    }

    inline Vector3& Vector3::operator-=(const Vector3& v)
    {
        x_ -= v.x_;
        y_ -= v.y_;
        z_ -= v.z_;
        return *this;
    }

    inline Vector3& Vector3::operator*=(f32 f)
    {
        x_ *= f;
        y_ *= f;
        z_ *= f;
        return *this;
    }

    inline bool Vector3::isEqual(const Vector3& v) const
    {
        return ( lmath::isEqual(x_, v.x_)
            && lmath::isEqual(y_, v.y_)
            && lmath::isEqual(z_, v.z_) );
    }

    inline bool Vector3::isEqual(const Vector3& v, f32 epsilon) const
    {
        return ( lmath::isEqual(x_, v.x_, epsilon)
            && lmath::isEqual(y_, v.y_, epsilon)
            && lmath::isEqual(z_, v.z_, epsilon) );
    }

    inline bool Vector3::operator==(const Vector3& v) const
    {
        return isEqual(v);
    }

    inline bool Vector3::operator!=(const Vector3& v) const
    {
        return !isEqual(v);
    }

    inline f32 Vector3::length() const
    {
        return lmath::sqrt( lengthSqr() );
    }

    inline f32 Vector3::lengthSqr() const
    {
        return ( x_ * x_ + y_ * y_ + z_ * z_);
    }

    inline void Vector3::swap(Vector3& rhs)
    {
        lcore::swap(x_, rhs.x_);
        lcore::swap(y_, rhs.y_);
        lcore::swap(z_, rhs.z_);
    }

    inline bool Vector3::isNan() const
    {
        return (lcore::isNan(x_) || lcore::isNan(y_) || lcore::isNan(z_));
    }
    
    Vector3 operator+(const Vector3& v0, const Vector3& v1);
    Vector3 operator-(const Vector3& v0, const Vector3& v1);
    Vector3 operator*(f32 f, const Vector3& v);
    Vector3 operator*(const Vector3& v, f32 f);
    Vector3 operator*(const Vector3& v0, const Vector3& v1);
    Vector3&& operator/(const Vector3& v, f32 f);
    Vector3&& operator/(const Vector3& v0, const Vector3& v1);

    Vector3&& normalize(const Vector3& v);
    Vector3&& normalize(const Vector3& v, f32 lengthSqr);
    Vector3&& normalizeChecked(const Vector3& v);

    Vector3 absolute(const Vector3& v);

    inline f32 dot(const Vector3& v0, const Vector3& v1)
    {
        return (v0.x_*v1.x_ + v0.y_*v1.y_ + v0.z_*v1.z_);
    }

    f32 distanceSqr(const Vector3& v0, const Vector3& v1);
    inline f32 distance(const Vector3& v0, const Vector3& v1)
    {
        return lmath::sqrt(distanceSqr(v0,v1));
    }

    Vector3 cross(const Vector3& v0, const Vector3& v1);

    /**
    @brief 線形補間。v = (1-t)*v0 + t*v1
    @param v0 ...
    @param v1 ...
    */
    Vector3&& lerp(const Vector3& v0, const Vector3& v1, f32 t);

    /**
    @brief 線形補間。v = t1*v0 + t0*v1
    @param v0 ...
    @param v1 ...
    */
    Vector3 lerp(const Vector3& v0, const Vector3& v1, f32 t0, f32 t1);

    inline Vector3 mul(f32 f, const Vector3& v)
    {
        return f*v;
    }

    inline Vector3&& mul(const Vector3& v, f32 f)
    {
        return v*f;
    }

    Vector3&& mul(const Matrix34& m, const Vector3& v);
    Vector3&& mul(const Vector3& v, const Matrix34& m);

    Vector3&& mul33(const Matrix34& m, const Vector3& v);
    Vector3&& mul33(const Vector3& v, const Matrix34& m);

    Vector3&& mul33(const Matrix44& m, const Vector3& v);
    Vector3&& mul33(const Vector3& v, const Matrix44& m);

    Vector3 rotate(const Vector3& v, const Quaternion& rotation);
    Vector3 rotate(const Quaternion& rotation, const Vector3& v);


    inline Vector3 add(const Vector3& v0, const Vector3& v1)
    {
        return v0+v1;
    }

    inline Vector3 sub(const Vector3& v0, const Vector3& v1)
    {
        return v0-v1;
    }

    Vector3 mul(const Vector3& v0, const Vector3& v1);
    Vector3&& div(const Vector3& v0, const Vector3& v1);


    inline Vector3 minimum(const Vector3& v0, const Vector3& v1)
    {
        return {lcore::minimum(v0.x_, v1.x_),
            lcore::minimum(v0.y_, v1.y_),
            lcore::minimum(v0.z_, v1.z_)};
    }

    inline Vector3 maximum(const Vector3& v0, const Vector3& v1)
    {
        return {lcore::maximum(v0.x_, v1.x_),
            lcore::maximum(v0.y_, v1.y_),
            lcore::maximum(v0.z_, v1.z_)};
    }

    inline f32 minimum(const Vector3& v)
    {
        return lcore::minimum(lcore::minimum(v.x_, v.y_), v.z_);
    }

    inline f32 maximum(const Vector3& v)
    {
        return lcore::maximum(lcore::maximum(v.x_, v.y_), v.z_);
    }

    // v0*v1 + v2
    Vector3&& muladd(const Vector3& v0, const Vector3& v1, const Vector3& v2);

    // a*v1 + v2
    Vector3&& muladd(f32 a, const Vector3& v0, const Vector3& v1);
}

#endif //INC_LMATH_VECTOR3_H__

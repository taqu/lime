#ifndef INC_LMATH_VECTOR3_H__
#define INC_LMATH_VECTOR3_H__
/**
@file Vector3.h
@author t-sakai
@date 2009/01/17 create
*/
#include "lmathcore.h"

namespace lmath
{
    class Matrix43;
    class Matrix44;
    class Quaternion;

    //--------------------------------------------
    //---
    //--- Vector3
    //---
    //--------------------------------------------
    class Vector3
    {
    public:
        Vector3()
        {}

        Vector3(f32 x, f32 y, f32 z)
            :_x(x), _y(y), _z(z)
        {}

        inline void set(f32 x, f32 y, f32 z)
        {
            _x = x; _y = y; _z = z;
        }

        inline f32 operator[](s32 index) const;
        inline f32& operator[](s32 index);
        inline Vector3 operator-() const;

        inline Vector3 operator*(f32 f) const;
        inline Vector3 operator/(f32 f) const;

        inline Vector3& operator+=(const Vector3& v);
        inline Vector3& operator-=(const Vector3& v);

        inline Vector3& operator*=(f32 f);
        inline Vector3& operator/=(f32 f);

        inline bool isEqual(const Vector3& v) const;
        inline bool isEqual(const Vector3& v, f32 epsilon) const;

        inline bool operator==(const Vector3& v) const;
        inline bool operator!=(const Vector3& v) const;

        inline f32 length() const;
        inline f32 lengthSqr() const;
        inline void normalize();
        inline f32 dot(const Vector3& v) const;
        f32 distanceSqr(const Vector3& v) const;
        inline f32 distance(const Vector3& v) const;

        inline const Vector3& cross(const Vector3& v1, const Vector3& v2);

        /**
        @brief 線形補間。v = (1-t)*v0 + t*v1
        @param v0 ... 
        @param v1 ... 
        */
        Vector3& lerp(const Vector3& v0, const Vector3& v1, f32 f);

        /**
        @brief 線形補間。v = f1*v0 + f0*v1
        @param v0 ... 
        @param v1 ... 
        */
        Vector3& lerp(const Vector3& v0, const Vector3& v1, f32 f0, f32 f1);

        void mul(const Matrix43& matrix, const Vector3& vector);
        void mul(const Vector3& v, const Matrix43& m);

        void mul33(const Vector3& vector, const Matrix43& matrix);
        void mul33(const Matrix43& matrix, const Vector3& vector);
        void mul33(const Vector3& vector, const Matrix44& matrix);
        void mul33(const Matrix44& matrix, const Vector3& vector);

        void rotate(const Quaternion& rotation);

        inline void swap(Vector3& rhs);

        inline bool isNan() const;

        f32 _x, _y, _z;
    };

    //--------------------------------------------
    //---
    //--- 実装
    //---
    //--------------------------------------------
    inline const Vector3& Vector3::cross(const Vector3& v1, const Vector3& v2)
    {
        _x = v1._y * v2._z - v1._z * v2._y;
        _y = v1._z * v2._x - v1._x * v2._z;
        _z = v1._x * v2._y - v1._y * v2._x;
        return *this;
    }

    inline f32 Vector3::operator[](s32 index) const
    {
        LASSERT(0<=index
            && index < 3);
        return (&_x)[index];
    }

    inline f32& Vector3::operator[](s32 index)
    {
        LASSERT(0<=index
            && index < 3);
        return (&_x)[index];
    }

    inline Vector3 Vector3::operator-() const
    {
        return Vector3(-_x, -_y, -_z);
    }

    inline Vector3 Vector3::operator*(f32 f) const
    {
        return Vector3(f*_x, f*_y, f*_z);
    }

    inline Vector3 Vector3::operator/(f32 f) const
    {
        f32 inv = 1.0f/f;
        return Vector3(inv*_x, inv*_y, inv*_z);
    }

    inline f32 Vector3::dot(const Vector3& v) const
    {
        return ( _x * v._x + _y * v._y + _z * v._z);
    }

    inline Vector3& Vector3::operator+=(const Vector3& v)
    {
        _x += v._x;
        _y += v._y;
        _z += v._z;
        return *this;
    }

    inline Vector3& Vector3::operator-=(const Vector3& v)
    {
        _x -= v._x;
        _y -= v._y;
        _z -= v._z;
        return *this;
    }

    inline Vector3& Vector3::operator*=(f32 f)
    {
        _x *= f;
        _y *= f;
        _z *= f;
        return *this;
    }

    inline Vector3& Vector3::operator/=(f32 f)
    {
        LASSERT(f != 0.0f);
        f = 1.0f / f;
        _x *= f;
        _y *= f;
        _z *= f;
        return *this;
    }

    inline bool Vector3::isEqual(const Vector3& v) const
    {
        return ( lmath::isEqual(_x, v._x)
            && lmath::isEqual(_y, v._y)
            && lmath::isEqual(_z, v._z) );
    }

    inline bool Vector3::isEqual(const Vector3& v, f32 epsilon) const
    {
        return ( lmath::isEqual(_x, v._x, epsilon)
            && lmath::isEqual(_y, v._y, epsilon)
            && lmath::isEqual(_z, v._z, epsilon) );
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
        return ( _x * _x + _y * _y + _z * _z);
    }

    inline void Vector3::normalize()
    {
        f32 l = lengthSqr();
        LASSERT( !(lmath::isEqual(l, 0.0f)) );
        //l = lmath::rsqrt(l);
        l = 1.0f/ lmath::sqrt(l);
        *this *= l;
    }

    inline f32 Vector3::distance(const Vector3& v) const
    {
        return lmath::sqrt( distanceSqr(v) );
    }

    inline void Vector3::swap(Vector3& rhs)
    {
        lcore::swap(_x, rhs._x);
        lcore::swap(_y, rhs._y);
        lcore::swap(_z, rhs._z);
    }

    inline bool Vector3::isNan() const
    {
        return (lcore::isNan(_x) || lcore::isNan(_y) || lcore::isNan(_z));
    }

}

#endif //INC_LMATH_VECTOR3_H__

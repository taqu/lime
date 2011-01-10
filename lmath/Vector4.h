#ifndef INC_LMATH_VECTOR4_H__
#define INC_LMATH_VECTOR4_H__
/**
@file Vector4.h
@author t-sakai
@date 2009/01/17 create
*/
#include "lmathcore.h"

namespace lmath
{
    class Matrix44;

    //--------------------------------------------
    //---
    //--- Vector4
    //---
    //--------------------------------------------
    class Vector4
    {
    public:
        Vector4()
        {}

        Vector4(f32 x, f32 y, f32 z, f32 w)
            :_x(x), _y(y), _z(z), _w(w)
        {}

        inline void set(f32 x, f32 y, f32 z, f32 w)
        {
            _x = x; _y = y; _z = z; _w = w;
        }

        inline f32 operator[](s32 index) const;
        inline f32& operator[](s32 index);
        inline Vector4 operator-() const;

        inline Vector4& operator+=(const Vector4& v);
        inline Vector4& operator-=(const Vector4& v);

        inline Vector4& operator*=(f32 f);
        inline Vector4& operator/=(f32 f);

        bool isEqual(const Vector4& v) const;
        bool isEqual(const Vector4& v, f32 epsilon) const;

        inline bool operator==(const Vector4& v) const;
        inline bool operator!=(const Vector4& v) const;

        inline f32 length() const;
        inline f32 lengthSqr() const;
        inline void normalize();
        inline f32 dot(const Vector4& v) const;
        inline f32 distance(const Vector4& v) const;
        f32 distanceSqr(const Vector4& v) const;

        void setLerp(const Vector4& v1, const Vector4& v2, f32 f);

        void mul(const Vector4& vector, const Matrix44& matrix);
        void mul(const Matrix44& matrix, const Vector4& vector);

        inline void swap(Vector4& rhs);

        inline bool isNan() const;

        f32 _x, _y, _z, _w;
    };

    //--------------------------------------------
    //---
    //--- Vector4
    //---
    //--------------------------------------------
    inline f32 Vector4::operator[](s32 index) const
    {
        LASSERT(0<=index
            && index < 4);

        return (&_x)[index];
    }

    inline f32& Vector4::operator[](s32 index)
    {
        LASSERT(0<=index
            && index < 4);

        return (&_x)[index];
    }

    inline Vector4 Vector4::operator-() const
    {
        return Vector4(-_x, -_y, -_z, -_w);
    }

    inline Vector4& Vector4::operator+=(const Vector4& v)
    {
        _x += v._x;
        _y += v._y;
        _z += v._z;
        _w += v._w;
        return *this;
    }

    inline Vector4& Vector4::operator-=(const Vector4& v)
    {
        _x -= v._x;
        _y -= v._y;
        _z -= v._z;
        _w -= v._w;
        return *this;
    }

    inline Vector4& Vector4::operator*=(f32 f)
    {
        _x *= f;
        _y *= f;
        _z *= f;
        _w *= f;
        return *this;
    }

    inline Vector4& Vector4::operator/=(f32 f)
    {
        LASSERT(!lmath::isEqual(f, 0.0f));
        f = 1.0f / f;
        _x *= f;
        _y *= f;
        _z *= f;
        _w *= f;
        return *this;
    }


    inline bool Vector4::operator==(const Vector4& v) const
    {
        return isEqual(v);
    }

    inline bool Vector4::operator!=(const Vector4& v) const
    {
        return !isEqual(v);
    }

    inline f32 Vector4::length() const
    {
        return lmath::sqrt( lengthSqr() );
    }

    inline f32 Vector4::lengthSqr() const
    {
        return ( _x * _x + _y * _y + _z * _z + _w * _w);
    }

    inline f32 Vector4::distance(const Vector4& v) const
    {
        return lmath::sqrt( distanceSqr(v) );
    }


    inline void Vector4::normalize()
    {
        f32 l = lengthSqr();
        LASSERT( !(lmath::isEqual(l, 0.0f)) );
        //l = lmath::rsqrt(l);
        l = 1.0f/ lmath::sqrt(l);
        *this *= l;
    }

    inline void Vector4::swap(Vector4& rhs)
    {
        lcore::swap(_x, rhs._x);
        lcore::swap(_y, rhs._y);
        lcore::swap(_z, rhs._z);
        lcore::swap(_w, rhs._w);
    }

    inline bool Vector4::isNan() const
    {
        return (lcore::isNan(_x) || lcore::isNan(_y) || lcore::isNan(_z) || lcore::isNan(_w));
    }
}

#endif //INC_LMATH_VECTOR4_H__

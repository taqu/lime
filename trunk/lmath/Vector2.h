#ifndef INC_LMATH_VECTOR2_H__
#define INC_LMATH_VECTOR2_H__
/**
@file Vector2.h
@author t-sakai
@date 2009/01/17 create
*/
#include "lmathcore.h"

namespace lmath
{
    //--------------------------------------------
    //---
    //--- Vector2
    //---
    //--------------------------------------------
    class Vector2
    {
    public:
        Vector2()
        {}

        Vector2(f32 x, f32 y)
            :_x(x), _y(y)
        {}

        Vector2(const Vector2& v)
            :_x(v._x), _y(v._y)
        {}

        void set(f32 x, f32 y)
        {
            _x = x;
            _y = y;
        }

        inline explicit Vector2(const f32 *xy);

        inline f32 operator[](s32 index) const;
        inline f32& operator[](s32 index);
        inline Vector2 operator-() const;

        inline Vector2& operator+=(const Vector2& v);
        inline Vector2& operator-=(const Vector2& v);

        inline Vector2& operator*=(f32 f);
        inline Vector2& operator/=(f32 f);

        inline bool isEqual(const Vector2& v) const;
        inline bool isEqual(const Vector2& v, f32 epsilon) const;

        inline bool operator==(const Vector2& v) const;
        inline bool operator!=(const Vector2& v) const;

        inline f32 length() const;
        inline f32 lengthSqr() const;
        inline void normalize();

        inline f32 dot(const Vector2& v) const;

        inline f32 distance(const Vector2& v) const;
        inline f32 distanceSqr(const Vector2& v) const;


        void setLerp(const Vector2& v1, const Vector2& v2, f32 f);

        inline bool isNan() const;

        f32 _x, _y;
    };

    //--------------------------------------------
    //---
    //--- 実装
    //---
    //--------------------------------------------
    inline f32 Vector2::operator[](s32 index) const
    {
        LASSERT(0<=index
            && index < 2);
        return (&_x)[index];
    }

    inline f32& Vector2::operator[](s32 index)
    {
        LASSERT(0<=index
            && index < 2);
        return (&_x)[index];
    }

    inline Vector2 Vector2::operator-() const
    {
        return Vector2(-_x, -_y);
    }

    inline Vector2& Vector2::operator+=(const Vector2& v)
    {
        _x += v._x;
        _y += v._y;
        return *this;
    }

    inline Vector2& Vector2::operator-=(const Vector2& v)
    {
        _x -= v._x;
        _y -= v._y;
        return *this;
    }

    inline Vector2& Vector2::operator*=(f32 f)
    {
        _x *= f;
        _y *= f;
        return *this;
    }

    inline Vector2& Vector2::operator/=(f32 f)
    {
        LASSERT(f != 0.0f);
        f = 1.0f/f;
        _x *= f;
        _y *= f;
        return *this;
    }

    inline bool Vector2::isEqual(const Vector2& v) const
    {
        return ( lmath::isEqual(_x, v._x)
            && lmath::isEqual(_y, v._y) );
    }

    inline bool Vector2::isEqual(const Vector2& v, f32 epsilon) const
    {
        return ( lmath::isEqual(_x, v._x, epsilon)
            && lmath::isEqual(_y, v._y, epsilon) );
    }

    inline bool Vector2::operator==(const Vector2& v) const
    {
        return isEqual(v);
    }

    inline bool Vector2::operator!=(const Vector2& v) const
    {
        return !isEqual(v);
    }

    inline f32 Vector2::length() const
    {
        return lmath::sqrt( lengthSqr() );
    }

    inline f32 Vector2::lengthSqr() const
    {
        return ( _x * _x + _y * _y );
    }

    inline void Vector2::normalize()
    {
        f32 l = lengthSqr();
        LASSERT( !(lmath::isEqual(l, 0.0f)) );
        l = lmath::sqrt(l);
        *this /= l;
    }

    inline f32 Vector2::dot(const Vector2& v) const
    {
        return ( _x * v._x + _y * v._y );
    }

    inline f32 Vector2::distance(const Vector2& v) const
    {
        return lmath::sqrt(distanceSqr(v));
    }

    inline f32 Vector2::distanceSqr(const Vector2& v) const
    {
        f32 dx = v._x - _x;
        f32 dy = v._y - _y;
        return (dx*dx + dy*dy); 
    }

    inline bool Vector2::isNan() const
    {
        return (lcore::isNan(_x) || lcore::isNan(_y));
    }

}

#endif //INC_LMATH_VECTOR2_H__

#ifndef INC_LMATH_VECTOR4_H__
#define INC_LMATH_VECTOR4_H__
/**
@file Vector4.h
@author t-sakai
@date 2009/01/17 create
*/
#include "lmathcore.h"
#include <lcore/utility.h>

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
            :x_(x), y_(y), z_(z), w_(w)
        {}

        inline void set(f32 x, f32 y, f32 z, f32 w)
        {
            x_ = x; y_ = y; z_ = z; w_ = w;
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

        void mul(const Matrix44& m, const Vector4& v);
        void mul(const Vector4& v, const Matrix44& m);

        inline void swap(Vector4& rhs);

        inline bool isNan() const;

        f32 x_, y_, z_, w_;
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

        return (&x_)[index];
    }

    inline f32& Vector4::operator[](s32 index)
    {
        LASSERT(0<=index
            && index < 4);

        return (&x_)[index];
    }

    inline Vector4 Vector4::operator-() const
    {
        return Vector4(-x_, -y_, -z_, -w_);
    }

    inline Vector4& Vector4::operator+=(const Vector4& v)
    {
        x_ += v.x_;
        y_ += v.y_;
        z_ += v.z_;
        w_ += v.w_;
        return *this;
    }

    inline Vector4& Vector4::operator-=(const Vector4& v)
    {
        x_ -= v.x_;
        y_ -= v.y_;
        z_ -= v.z_;
        w_ -= v.w_;
        return *this;
    }

    inline Vector4& Vector4::operator*=(f32 f)
    {
        x_ *= f;
        y_ *= f;
        z_ *= f;
        w_ *= f;
        return *this;
    }

    inline Vector4& Vector4::operator/=(f32 f)
    {
        LASSERT(!lmath::isEqual(f, 0.0f));
        f = 1.0f / f;
        x_ *= f;
        y_ *= f;
        z_ *= f;
        w_ *= f;
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
        return ( x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
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
        lcore::swap(x_, rhs.x_);
        lcore::swap(y_, rhs.y_);
        lcore::swap(z_, rhs.z_);
        lcore::swap(w_, rhs.w_);
    }

    inline bool Vector4::isNan() const
    {
        return (lcore::isNan(x_) || lcore::isNan(y_) || lcore::isNan(z_) || lcore::isNan(w_));
    }
}

#endif //INC_LMATH_VECTOR4_H__

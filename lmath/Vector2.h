#ifndef INC_LMATH_VECTOR2_H__
#define INC_LMATH_VECTOR2_H__
/**
@file Vector2.h
@author t-sakai
@date 2009/01/17 create
*/
#include "lmath.h"

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
        static Vector2 construct(f32 xy)
        {
            return {xy, xy};
        }

        static Vector2 construct(f32 x, f32 y)
        {
            return {x, y};
        }

        static Vector2 zero()
        {
            return {0.0f, 0.0f};
        }

        static Vector2 one()
        {
            return {1.0f, 1.0f};
        }

        void set(f32 x, f32 y)
        {
            x_ = x;
            y_ = y;
        }

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

        inline bool isNan() const;

        friend Vector2 normalize(const Vector2& v);
        friend Vector2 normalizeCheced(const Vector2& v);
        friend Vector2 operator+(const Vector2& v0, const Vector2& v1);
        friend Vector2 operator-(const Vector2& v0, const Vector2& v1);
        friend Vector2 operator*(f32 f, const Vector2& v);
        friend Vector2 operator*(const Vector2& v, f32 f);
        friend Vector2 operator/(const Vector2& v, f32 f);
        friend f32 dot(const Vector2& v0, const Vector2& v1);
        friend f32 distanceSqr(const Vector2& v0, const Vector2& v1);
        friend f32 distance(const Vector2& v0, const Vector2& v1);
        friend Vector2 lerp(const Vector2& v0, const Vector2& v1, f32 t);

        friend Vector2 add(const Vector2& v0, const Vector2& v1);
        friend Vector2 sub(const Vector2& v0, const Vector2& v1);
        friend Vector2 mul(f32 f, const Vector2& v);
        friend Vector2 mul(const Vector2& v, f32 f);
        friend Vector2 muladd(f32 f, const Vector2& v0, const Vector2& v1);

        f32 x_, y_;
    };

    //--------------------------------------------
    //---
    //--- 実装
    //---
    //--------------------------------------------
    inline f32 Vector2::operator[](s32 index) const
    {
        LASSERT(0<=index && index < 2);
        return (&x_)[index];
    }

    inline f32& Vector2::operator[](s32 index)
    {
        LASSERT(0<=index && index < 2);
        return (&x_)[index];
    }

    inline Vector2 Vector2::operator-() const
    {
        return {-x_, -y_};
    }

    inline Vector2& Vector2::operator+=(const Vector2& v)
    {
        x_ += v.x_;
        y_ += v.y_;
        return *this;
    }

    inline Vector2& Vector2::operator-=(const Vector2& v)
    {
        x_ -= v.x_;
        y_ -= v.y_;
        return *this;
    }

    inline Vector2& Vector2::operator*=(f32 f)
    {
        x_ *= f;
        y_ *= f;
        return *this;
    }

    inline Vector2& Vector2::operator/=(f32 f)
    {
        LASSERT(f != 0.0f);
        f = 1.0f/f;
        x_ *= f;
        y_ *= f;
        return *this;
    }

    inline bool Vector2::isEqual(const Vector2& v) const
    {
        return ( lmath::isEqual(x_, v.x_)
            && lmath::isEqual(y_, v.y_) );
    }

    inline bool Vector2::isEqual(const Vector2& v, f32 epsilon) const
    {
        return ( lmath::isEqual(x_, v.x_, epsilon)
            && lmath::isEqual(y_, v.y_, epsilon) );
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
        return ( x_ * x_ + y_ * y_ );
    }

    inline bool Vector2::isNan() const
    {
        return (lcore::isNan(x_) || lcore::isNan(y_));
    }

    Vector2 normalize(const Vector2& v);

    Vector2 normalizeChecked(const Vector2& v);

    inline Vector2 operator+(const Vector2& v0, const Vector2& v1)
    {
        return {v0.x_+v1.x_, v0.y_+v1.y_};
    }

    inline Vector2 operator-(const Vector2& v0, const Vector2& v1)
    {
        return {v0.x_-v1.x_, v0.y_-v1.y_};
    }

    inline Vector2 operator*(f32 f, const Vector2& v)
    {
        return {f*v.x_, f*v.y_};
    }

    inline Vector2 operator*(const Vector2& v, f32 f)
    {
        return {v.x_*f, v.y_*f};
    }

    inline Vector2 operator/(const Vector2& v, f32 f)
    {
        f32 inv = 1.0f/f;
        return {v.x_*inv, v.y_*inv};
    }

    inline f32 dot(const Vector2& v0, const Vector2& v1)
    {
        return (v0.x_*v1.x_ + v0.y_*v1.y_);
    }

    inline f32 distanceSqr(const Vector2& v0, const Vector2& v1)
    {
        f32 dx = v0.x_ - v1.x_;
        f32 dy = v0.y_ - v1.y_;
        return (dx*dx + dy*dy); 
    }

    inline f32 distance(const Vector2& v0, const Vector2& v1)
    {
        return lmath::sqrt(distanceSqr(v0, v1));
    }

    Vector2 lerp(const Vector2& v0, const Vector2& v1, f32 t);


    inline Vector2 add(const Vector2& v0, const Vector2& v1)
    {
        return {v0.x_+v1.x_, v0.y_+v1.y_};
    }

    inline Vector2 sub(const Vector2& v0, const Vector2& v1)
    {
        return {v0.x_-v1.x_, v0.y_-v1.y_};
    }

    inline Vector2 mul(f32 f, const Vector2& v)
    {
        return {f*v.x_, f*v.y_};
    }

    inline Vector2 mul(const Vector2& v, f32 f)
    {
        return {v.x_*f, v.y_*f};
    }

    Vector2 muladd(f32 f, const Vector2& v0, const Vector2& v1);
}

#endif //INC_LMATH_VECTOR2_H__

#ifndef INC_LMATH_VECTOR4_H__
#define INC_LMATH_VECTOR4_H__
/**
@file Vector4.h
@author t-sakai
@date 2009/01/17 create
*/
#include "lmath.h"

namespace lmath
{
#if defined(LMATH_USE_SSE)
    typedef lm128 return_type_vec4;
#else
    typedef Vector4 return_type_vec4;
#endif

    //--------------------------------------------
    //---
    //--- Vector4
    //---
    //--------------------------------------------
    class Vector4
    {
    public:
        static const LALIGN16 f32 One[4];
        static const LALIGN16 f32 Identity[4];
        static const Vector4 Forward;
        static const Vector4 Backward;
        static const Vector4 Up;
        static const Vector4 Down;
        static const Vector4 Right;
        static const Vector4 Left;

        inline static Vector4 construct(f32 xyzw);
        inline static Vector4 construct(f32 x, f32 y, f32 z);
        inline static Vector4 construct(f32 x, f32 y, f32 z, f32 w);
        inline static Vector4 zero();
        inline static Vector4 one();
        inline static Vector4 identity();
        inline static Vector4 construct(const Vector4& v);
        inline static Vector4 construct(const lm128& v);
        static Vector4 construct(const Vector3& v);
        static Vector4 construct(const Vector3& v, f32 w);

        inline operator lm128() const;

        inline void set(f32 x, f32 y, f32 z, f32 w);
        void set(const Vector3& v);
        void set(const Vector3& v, f32 w);
        inline void set(f32 v);

        inline f32 operator[](s32 index) const;
        inline f32& operator[](s32 index);
        inline Vector4 operator-() const;

        inline Vector4& operator+=(const Vector4& v);
        inline Vector4& operator-=(const Vector4& v);

        inline Vector4& operator*=(f32 f);
        inline Vector4& operator/=(f32 f);

        inline Vector4& operator*=(const Vector4& v);
        inline Vector4& operator/=(const Vector4& v);

        bool isEqual(const Vector4& v) const;
        bool isEqual(const Vector4& v, f32 epsilon) const;

        inline bool operator==(const Vector4& v) const;
        inline bool operator!=(const Vector4& v) const;

        void setLength();
        f32 length() const;
        f32 lengthSqr() const;

        inline void swap(Vector4& rhs);

        inline bool isNan() const;
        inline bool isZero() const;

        inline Vector4 getParallelComponent(const Vector4& basis) const
        {
            f32 cs = dot(*this, basis);
            return construct(basis*cs);
        }

        inline Vector4 getPerpendicularComponent(const Vector4& basis) const
        {
            return construct(*this - getParallelComponent(basis));
        }

        friend return_type_vec4 normalize(const Vector4& v);
        friend return_type_vec4 normalize(const Vector4& v, f32 lengthSqr);
        friend return_type_vec4 normalizeChecked(const Vector4& v);
        friend return_type_vec4 absolute(const Vector4& v);

        friend f32 dot(const Vector4& v0, const Vector4& v1);
        friend return_type_vec4 cross3(const Vector4& v0, const Vector4& v1);
        friend f32 distanceSqr(const Vector4& v0, const Vector4& v1);
        friend f32 distance(const Vector4& v0, const Vector4& v1);
        friend f32 manhattanDistance(const Vector4& v0, const Vector4& v1);

        friend f32 distanceSqr3(const Vector3& v0, const Vector4& v1);
        friend f32 distanceSqr3(const Vector4& v0, const Vector3& v1);
        friend f32 distanceSqr3(const Vector4& v0, const Vector4& v1);
        friend f32 distance3(const Vector3& v0, const Vector4& v1);
        friend f32 distance3(const Vector4& v0, const Vector3& v1);
        friend f32 distance3(const Vector4& v0, const Vector4& v1);
        friend f32 manhattanDistance3(const Vector3& v0, const Vector4& v1);
        friend f32 manhattanDistance3(const Vector4& v0, const Vector3& v1);
        friend f32 manhattanDistance3(const Vector4& v0, const Vector4& v1);

        friend return_type_vec4 mul(const Matrix34& m, const Vector4& v);
        friend return_type_vec4 mul(const Vector4& v, const Matrix34& m);

        friend return_type_vec4 mul(const Matrix44& m, const Vector4& v);
        friend return_type_vec4 mul(const Vector4& v, const Matrix44& m);

        friend Vector4 mulPoint(const Matrix44& m, const Vector4& v);
        friend Vector4 mulVector(const Matrix44& m, const Vector4& v);

        friend Vector4 rotate(const Vector4& v, const Quaternion& rotation);
        friend Vector4 rotate(const Quaternion& rotation, const Vector4& v);

        friend return_type_vec4 operator+(const Vector4& v0, const Vector4& v1);
        friend return_type_vec4 operator-(const Vector4& v0, const Vector4& v1);
        friend return_type_vec4 operator*(f32 f, const Vector4& v);
        friend return_type_vec4 operator*(const Vector4& v, f32 f);
        friend return_type_vec4 operator*(const Vector4& v0, const Vector4& v1);
        friend return_type_vec4 operator/(const Vector4& v, f32 f);
        friend return_type_vec4 operator/(const Vector4& v0, const Vector4& v1);
        friend return_type_vec4 add(const Vector4& v0, const Vector4& v1);
        friend return_type_vec4 sub(const Vector4& v0, const Vector4& v1);
        friend return_type_vec4 mul(const Vector4& v0, const Vector4& v1);
        friend return_type_vec4 mul(f32 f, const Vector4& v);
        friend return_type_vec4 mul(const Vector4& v0, f32 f);
        friend return_type_vec4 div(const Vector4& v0, const Vector4& v1);
        friend return_type_vec4 div(const Vector4& v0, f32 f);

        friend return_type_vec4 add(const Vector4& v, f32 f);
        friend return_type_vec4 sub(const Vector4& v, f32 f);

        friend return_type_vec4 minimum(const Vector4& v0, const Vector4& v1);
        friend return_type_vec4 maximum(const Vector4& v0, const Vector4& v1);

        friend f32 minimum(const Vector4& v);
        friend f32 maximum(const Vector4& v);

        /**
        @brief v0*v1 + v2
        */
        friend return_type_vec4 muladd(const Vector4& v0, const Vector4& v1, const Vector4& v2);

        /**
        @brief a*v0 + v1
        */
        friend return_type_vec4 muladd(f32 a, const Vector4& v0, const Vector4& v1);

        friend return_type_vec4 floor(const Vector4& v);
        friend return_type_vec4 ceil(const Vector4& v);
        friend return_type_vec4 invert(const Vector4& v);

        friend return_type_vec4 sqrt(const Vector4& v);

        /**
        @brief v0 * (1-t) + v1 * t
        */
        friend return_type_vec4 lerp(const lmath::Vector4& v0, const lmath::Vector4& v1, f32 t);

        /**
        @brief v0 * (1-t) + v1 * t
        */
        friend return_type_vec4 slerp(const lmath::Vector4& v0, const lmath::Vector4& v1, f32 t);
        /**
        @brief v0 * (1-t) + v1 * t
        */
        friend return_type_vec4 slerp(const lmath::Vector4& v0, const lmath::Vector4& v1, f32 t, f32 cosine);


        inline static lm128 load(const Vector4& v)
        {
            return _mm_loadu_ps(&v.x_);
        }

        inline static void store(Vector4& v, lm128& r)
        {
            _mm_storeu_ps(&v.x_, r);
        }

        inline static Vector4 store(lm128& r)
        {
            Vector4 v;
            _mm_storeu_ps(&v.x_, r);
            return v;
        }

        f32 x_, y_, z_, w_;
    };

    //--------------------------------------------
    //---
    //--- Vector4
    //---
    //--------------------------------------------
    inline Vector4 Vector4::construct(f32 xyzw)
    {
        Vector4 v;
        _mm_storeu_ps(&v.x_, _mm_set1_ps(xyzw));
        return v;
    }

    inline Vector4 Vector4::construct(f32 x, f32 y, f32 z)
    {
        return {x,y,z,0.0f};
    }

    inline Vector4 Vector4::construct(f32 x, f32 y, f32 z, f32 w)
    {
        return {x,y,z,w};
    }

    inline Vector4 Vector4::zero()
    {
        Vector4 v;
        _mm_storeu_ps(&v.x_, _mm_setzero_ps());
        return v;
    }

    inline Vector4 Vector4::one()
    {
        Vector4 v;
        _mm_storeu_ps(&v.x_, _mm_load_ps(One));
        return v;
    }

    inline Vector4 Vector4::identity()
    {
        Vector4 v;
        _mm_storeu_ps(&v.x_, _mm_load_ps(Identity));
        return v;
    }

    inline Vector4 Vector4::construct(const Vector4& v)
    {
        Vector4 r;
        _mm_storeu_ps(&r.x_, _mm_loadu_ps(&v.x_));
        return r;
    }

    inline Vector4 Vector4::construct(const lm128& v)
    {
        Vector4 r;
        _mm_storeu_ps(&r.x_, v);
        return r;
    }

    inline Vector4::operator lm128() const
    {
        return _mm_loadu_ps(&x_);
    }

    inline void Vector4::set(f32 x, f32 y, f32 z, f32 w)
    {
        x_ = x; y_ = y; z_ = z; w_ = w;
    }

    inline void Vector4::set(f32 v)
    {
#if defined(LMATH_USE_SSE)
        lm128 t = _mm_set1_ps(v);
        store(*this, t);
#else
        x_ = y_ = z_ = w_ = v;
#endif
    }

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
#if defined(LMATH_USE_SSE)
        f32 f;
        *((u32*)&f) = 0x80000000U;
        lm128 mask = _mm_set1_ps(f);
        lm128 r0 = load(*this);
        r0 = _mm_xor_ps(r0, mask);

        Vector4 ret;
        store(ret, r0);
        return ret;
#else
        return Vector4(-x_, -y_, -z_, -w_);
#endif
    }

    inline Vector4& Vector4::operator+=(const Vector4& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = load(v);
        r0 = _mm_add_ps(r0, r1);
        store(*this, r0);

#else
        x_ += v.x_;
        y_ += v.y_;
        z_ += v.z_;
        w_ += v.w_;
#endif
        return *this;
    }

    inline Vector4& Vector4::operator-=(const Vector4& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = load(v);
        r0 = _mm_sub_ps(r0, r1);
        store(*this, r0);

#else
        x_ -= v.x_;
        y_ -= v.y_;
        z_ -= v.z_;
        w_ -= v.w_;
#endif
        return *this;
    }

    inline Vector4& Vector4::operator*=(f32 f)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = _mm_set1_ps(f);
        r0 = _mm_mul_ps(r0, r1);
        store(*this, r0);

#else
        x_ *= f;
        y_ *= f;
        z_ *= f;
        w_ *= f;
#endif
        return *this;
    }

    inline Vector4& Vector4::operator/=(f32 f)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = _mm_set1_ps(f);
        r0 = _mm_div_ps(r0, r1);
        store(*this, r0);

#else
        LASSERT(!lmath::isEqual(f, 0.0f));
        f = 1.0f / f;
        x_ *= f;
        y_ *= f;
        z_ *= f;
        w_ *= f;
#endif
        return *this;
    }

    inline Vector4& Vector4::operator*=(const Vector4& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = load(v);
        r0 = _mm_mul_ps(r0, r1);
        store(*this, r0);
#else
        x_ *= v.x_;
        y_ *= v.y_;
        z_ *= v.z_;
        w_ *= v.w_;
#endif
        return *this;
    }

    inline Vector4& Vector4::operator/=(const Vector4& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = load(*this);
        lm128 r1 = load(v);
        r0 = _mm_div_ps(r0, r1);
        store(*this, r0);
#else
        x_ /= v.x_;
        y_ /= v.y_;
        z_ /= v.z_;
        w_ /= v.w_;
#endif
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

    inline void Vector4::swap(Vector4& rhs)
    {
#if defined(LMATH_USE_SSE)
        lm128 t0 = load(*this);
        lm128 t1 = load(rhs);

        store(*this, t1);
        store(rhs, t0);
#else
        lcore::swap(x_, rhs.x_);
        lcore::swap(y_, rhs.y_);
        lcore::swap(z_, rhs.z_);
        lcore::swap(w_, rhs.w_);
#endif
    }

    inline bool Vector4::isNan() const
    {
        return (lcore::isNan(x_) || lcore::isNan(y_) || lcore::isNan(z_) || lcore::isNan(w_));
    }

    inline bool Vector4::isZero() const
    {
        return (lmath::isZero(x_) && lmath::isZero(y_) && lmath::isZero(z_) && lmath::isZero(w_));
    }

    return_type_vec4 normalize(const Vector4& v);
    return_type_vec4 normalize(const Vector4& v, f32 lengthSqr);
    return_type_vec4 normalizeChecked(const Vector4& v);

    return_type_vec4 absolute(const Vector4& v);

    f32 dot(const Vector4& v0, const Vector4& v1);
    return_type_vec4 cross3(const Vector4& v0, const Vector4& v1);
    f32 distanceSqr(const Vector4& v0, const Vector4& v1);
    f32 distance(const Vector4& v0, const Vector4& v1);
    f32 manhattanDistance(const Vector4& v0, const Vector4& v1);

    f32 distanceSqr3(const Vector3& v0, const Vector4& v1);
    inline f32 distanceSqr3(const Vector4& v0, const Vector3& v1)
    {
        return distanceSqr3(v1, v0);
    }
    f32 distanceSqr3(const Vector4& v0, const Vector4& v1);
    f32 distance3(const Vector3& v0, const Vector4& v1);
    inline f32 distance3(const Vector4& v0, const Vector3& v1)
    {
        return distance3(v1, v0);
    }
    f32 distance3(const Vector4& v0, const Vector4& v1);
    f32 manhattanDistance3(const Vector3& v0, const Vector4& v1);
    inline f32 manhattanDistance3(const Vector4& v0, const Vector3& v1)
    {
        return manhattanDistance3(v1, v0);
    }
    f32 manhattanDistance3(const Vector4& v0, const Vector4& v1);

    Vector4 mulPoint(const Matrix44& m, const Vector4& v);
    Vector4 mulVector(const Matrix44& m, const Vector4& v);

    return_type_vec4 mul(const Matrix34& m, const Vector4& v);
    return_type_vec4 mul(const Vector4& v, const Matrix34& m);

    return_type_vec4 mul(const Matrix44& m, const Vector4& v);
    return_type_vec4 mul(const Vector4& v, const Matrix44& m);    

    Vector4 rotate(const Vector4& v, const Quaternion& rotation);
    Vector4 rotate(const Quaternion& rotation, const Vector4& v);

    inline return_type_vec4 operator+(const Vector4& v0, const Vector4& v1)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v0);
        lm128 r1 = Vector4::load(v1);
        return _mm_add_ps(r0, r1);

#else
        f32 x = v0.x_ + v1.x_;
        f32 y = v0.y_ + v1.y_;
        f32 z = v0.z_ + v1.z_;
        f32 w = v0.w_ + v1.w_;
        return Vector4(x,y,z,w);
#endif
    }

    inline return_type_vec4 operator-(const Vector4& v0, const Vector4& v1)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v0);
        lm128 r1 = Vector4::load(v1);
        return _mm_sub_ps(r0, r1);

#else
        f32 x = v0.x_ - v1.x_;
        f32 y = v0.y_ - v1.y_;
        f32 z = v0.z_ - v1.z_;
        f32 w = v0.w_ - v1.w_;
        return Vector4(x,y,z,w);
#endif
    }

    inline return_type_vec4 operator*(f32 f, const Vector4& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = _mm_set1_ps(f);
        lm128 r1 = Vector4::load(v);
        return _mm_mul_ps(r0, r1);
#else
        f32 x = f * v.x_;
        f32 y = f * v.y_;
        f32 z = f * v.z_;
        f32 w = f * v.w_;
        return Vector4(x,y,z,w);
#endif
    }

    inline return_type_vec4 operator*(const Vector4& v, f32 f)
    {
        return f*v;
    }

    inline return_type_vec4 operator*(const Vector4& v0, const Vector4& v1)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v0);
        lm128 r1 = Vector4::load(v1);
        return _mm_mul_ps(r0, r1);
#else
        return {v0.x_*v1.x_, v0.y_*v1.y_, v0.z_*v1.z_, v0.w_*v1.w_};
#endif
    }

    inline return_type_vec4 operator/(const Vector4& v, f32 f)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v);
        lm128 r1 = _mm_set1_ps(f);
        return _mm_div_ps(r0, r1);
#else
        f = 1.0f/f;
        return v*f;
#endif
    }

    inline return_type_vec4 operator/(const Vector4& v0, const Vector4& v1)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v0);
        lm128 r1 = Vector4::load(v1);
        return _mm_div_ps(r0, r1);
#else
        return {v0.x_/v1.x_, v0.y_/v1.y_, v0.z_/v1.z_, v0.w_/v1.w_};
#endif
    }

    inline return_type_vec4 add(const Vector4& v0, const Vector4& v1)
    {
        return v0+v1;
    }

    inline return_type_vec4 sub(const Vector4& v0, const Vector4& v1)
    {
        return v0-v1;
    }

    inline return_type_vec4 mul(const Vector4& v0, const Vector4& v1)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v0);
        lm128 r1 = Vector4::load(v1);
        return _mm_mul_ps(r0, r1);
#else
        f32 x = v0.x_ * v1.x_;
        f32 y = v0.y_ * v1.y_;
        f32 z = v0.z_ * v1.z_;
        f32 w = v0.w_ * v1.w_;
        return {x,y,z,w};
#endif
    }

    inline return_type_vec4 mul(f32 f, const Vector4& v)
    {
        return f*v;
    }

    inline return_type_vec4 mul(const Vector4& v, f32 f)
    {
        return v*f;
    }

    inline return_type_vec4 div(const Vector4& v0, const Vector4& v1)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v0);
        lm128 r1 = Vector4::load(v1);
        return _mm_div_ps(r0, r1);
#else
        f32 x = v0.x_ / v1.x_;
        f32 y = v0.y_ / v1.y_;
        f32 z = v0.z_ / v1.z_;
        f32 w = v0.w_ / v1.w_;
        return {x,y,z,w};
#endif
    }

    inline return_type_vec4 div(const Vector4& v0, f32 f)
    {
        return v0/f;
    }


    inline return_type_vec4 add(const Vector4& v, f32 f)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v);
        lm128 r1 = _mm_set1_ps(f);
        return _mm_add_ps(r0, r1);
#else
        return {v.x_+f, v.y_+f, v.z_+f, v.w_+f};
#endif
    }

    inline return_type_vec4 sub(const Vector4& v, f32 f)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v);
        lm128 r1 = _mm_set1_ps(f);
        return _mm_sub_ps(r0, r1);
#else
        return {v.x_-f, v.y_-f, v.z_-f, v.w_0f};
#endif
    }

    inline return_type_vec4 minimum(const Vector4& v0, const Vector4& v1)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v0);
        lm128 r1 = Vector4::load(v1);
        return _mm_min_ps(r0, r1);

#else
        f32 x = lcore::minimum(v0.x_, v1.x_);
        f32 y = lcore::minimum(v0.y_, v1.y_);
        f32 z = lcore::minimum(v0.z_, v1.z_);
        f32 w = lcore::minimum(v0.w_, v1.w_);
        return {x,y,z,w};
#endif
    }

    inline return_type_vec4 maximum(const Vector4& v0, const Vector4& v1)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v0);
        lm128 r1 = Vector4::load(v1);
        return _mm_max_ps(r0, r1);

#else
        f32 x = lcore::maximum(v0.x_, v1.x_);
        f32 y = lcore::maximum(v0.y_, v1.y_);
        f32 z = lcore::maximum(v0.z_, v1.z_);
        f32 w = lcore::maximum(v0.w_, v1.w_);
        return {x,y,z,w};
#endif
    }

    inline f32 minimum(const Vector4& v)
    {
        return lcore::minimum(lcore::minimum(v.x_, v.y_), lcore::minimum(v.z_, v.w_));
    }

    inline f32 maximum(const Vector4& v)
    {
        return lcore::maximum(lcore::maximum(v.x_, v.y_), lcore::maximum(v.z_, v.w_));
    }

    /**
    @brief v0*v1 + v2
    */
    inline return_type_vec4 muladd(const Vector4& v0, const Vector4& v1, const Vector4& v2)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v0);
        lm128 r1 = Vector4::load(v1);
        lm128 r2 = Vector4::load(v2);
        r0 = _mm_mul_ps(r0, r1);
        r0 = _mm_add_ps(r0, r2);
        return r0;

#else
        f32 x = v0.x_ * v1.x_ + v2.x_;
        f32 y = v0.y_ * v1.y_ + v2.y_;
        f32 z = v0.z_ * v1.z_ + v2.z_;
        f32 w = v0.w_ * v1.w_ + v2.w_;
        return {x,y,z,w};
#endif
    }

    /**
    @brief a*v0 + v1
    */
    inline return_type_vec4 muladd(f32 a, const Vector4& v0, const Vector4& v1)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = _mm_set1_ps(a);
        lm128 r1 = Vector4::load(v0);
        lm128 r2 = Vector4::load(v1);
        r0 = _mm_mul_ps(r0, r1);
        r0 = _mm_add_ps(r0, r2);
        return r0;

#else
        f32 x = a * v0.x_ + v1.x_;
        f32 y = a * v0.y_ + v1.y_;
        f32 z = a * v0.z_ + v1.z_;
        f32 w = a * v0.w_ + v1.w_;
        return {x,y,z,w};
#endif
    }

    return_type_vec4 floor(const Vector4& v);
    return_type_vec4 ceil(const Vector4& v);

    inline return_type_vec4 invert(const Vector4& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = _mm_set1_ps(1.0f);
        lm128 r1 = Vector4::load(v);
        return _mm_div_ps(r0, r1);
#else
        f32 x = 1.0f/x_;
        f32 y = 1.0f/y_;
        f32 z = 1.0f/z_;
        f32 w = 1.0f/w_;
        return {x,y,z,w};
#endif
    }

    inline return_type_vec4 sqrt(const Vector4& v)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = Vector4::load(v);
        return _mm_sqrt_ps(r0);
#else
        f32 x = lmath::sqrt(x_);
        f32 y = lmath::sqrt(y_);
        f32 z = lmath::sqrt(z_);
        f32 w = lmath::sqrt(w_);
        return {x,y,z,w};
#endif
    }

    /**
    @brief v0 * (1-t) + v1 * t
    */
    return_type_vec4 lerp(const lmath::Vector4& v0, const lmath::Vector4& v1, f32 t);

    /**
    @brief v0 * (1-t) + v1 * t
    */
    return_type_vec4 slerp(const lmath::Vector4& v0, const lmath::Vector4& v1, f32 t);

    /**
    @brief v0 * (1-t) + v1 * t
    */
    return_type_vec4 slerp(const lmath::Vector4& v0, const lmath::Vector4& v1, f32 t, f32 cosine);
}

#endif //INC_LMATH_VECTOR4_H__

#ifndef INC_LMATH_SPHERE_H__
#define INC_LMATH_SPHERE_H__
/**
@file Sphere.h
@author t-sakai
@date 2009/12/20 create
*/
#include "../lmath.h"
#include "../Vector3.h"
#include "../Vector4.h"

namespace lmath
{
    /// 球
    class Sphere
    {
    public:
        Sphere()
        {
        }

        Sphere(const Vector3& position, f32 radius)
            :x_(position.x_)
            ,y_(position.y_)
            ,z_(position.z_)
            ,r_(radius)
        {
        }

        Sphere(f32 x, f32 y, f32 z, f32 radius)
            :x_(x)
            ,y_(y)
            ,z_(z)
            ,r_(radius)
        {
        }

        void set(f32 x, f32 y, f32 z, f32 radius)
        {
            x_ = x;
            y_ = y;
            z_ = z;
            r_ = radius;
        }

        void zero()
        {
            _mm_storeu_ps(&x_, _mm_setzero_ps());
        }

        void setPosition(f32 x, f32 y, f32 z)
        {
            x_ = x;
            y_ = y;
            z_ = z;
        }

        void setPosition(const lmath::Vector3& position)
        {
            x_ = position.x_;
            y_ = position.y_;
            z_ = position.z_;
        }

        void setPosition(const lmath::Vector4& position)
        {
            x_ = position.x_;
            y_ = position.y_;
            z_ = position.z_;
        }

        void setRadius(f32 radius)
        {
            r_ = radius;
        }

        void translate(const lmath::Vector3& position);
        void translate(const lmath::Vector4& position);

        /**
        @brief 点に外接する球を計算
        */
        static Sphere circumscribed(const Vector3& p0, const Vector3& p1);

        /**
        @brief 点に外接する球を計算
        */
        static Sphere circumscribed(const Vector3& p0, const Vector3& p1, const Vector3& p2);

        /**
        @brief 点に外接する球を計算
        */
        static Sphere circumscribed(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3);


        static Sphere calcMiniSphere(const Vector3* points, u32 numPoints);

        void combine(const Sphere& s0, const Sphere& s1);
        void add(const Sphere& s1){ combine(*this, s1);}
        void add(const Vector4& s1){ combine(*this, reinterpret_cast<const Sphere&>(s1));}

        f32 signedDistanceSqr(const Vector3& p) const;

        f32 distance(const Sphere& rhs) const;

        void swap(Sphere& rhs)
        {
            lcore::swap(x_, rhs.x_);
            lcore::swap(y_, rhs.y_);
            lcore::swap(z_, rhs.z_);
            lcore::swap(r_, rhs.r_);
        }

        const Vector3& position() const
        {
            return *((const Vector3*)&x_);
        }

        Vector3& position()
        {
            return *((Vector3*)&x_);
        }

        const f32& radius() const
        {
            return r_;
        }

        f32& radius()
        {
            return r_;
        }

        operator const Vector4&() const
        {
            return *((const Vector4*)&x_);
        }
        operator Vector4&()
        {
            return *((Vector4*)&x_);
        }

        void getAABB(lmath::lm128& bmin, lmath::lm128& bmax) const;

        f32 x_;
        f32 y_;
        f32 z_;
        f32 r_; //Radius
    };
}
#endif //INC_LMATH_SPHERE_H__

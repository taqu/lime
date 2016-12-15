#ifndef INC_LMATH_PLANE_H__
#define INC_LMATH_PLANE_H__
/**
@file Plane.h
@author t-sakai
@date 2009/12/20 create
*/
#include "../lmath.h"
#include "../Vector3.h"
#include "../Vector4.h"

namespace lmath
{
    /// 平面
    class Plane
    {
    public:
        Plane()
        {}

        Plane(f32 nx, f32 ny, f32 nz, f32 d)
            :nx_(nx)
            ,ny_(ny)
            ,nz_(nz)
            ,d_(d)
        {}


        Plane(const Vector3& point, const Vector3& normal)
            :nx_(normal.x_)
            ,ny_(normal.y_)
            ,nz_(normal.z_)
            ,d_(-lmath::dot(normal, point))
        {}

        f32 dot(const lmath::Vector3& p) const
        {
            return (nx_*p.x_ + ny_*p.y_ + nz_*p.z_ + d_);
        }

        f32 dot(const lmath::Vector4& p) const
        {
            return lmath::dot((const Vector4&)*this, p);
        }

        Vector3 normal() const
        {
            return Vector3(nx_, ny_, nz_);
        }

        f32 d() const
        {
            return d_;
        }

        void normalize();

        void translate(f32 x, f32 y, f32 z);
        inline void translate(const lmath::Vector3& v){ translate(v.x_, v.y_, v.z_);}
        inline void translate(const lmath::Vector4& v){ translate(v.x_, v.y_, v.z_);}

        operator const Vector4&() const
        {
            return *((const Vector4*)&nx_);
        }
        operator Vector4&()
        {
            return *((Vector4*)&nx_);
        }

        f32 nx_; /// 平面の法線
        f32 ny_; /// 平面の法線
        f32 nz_; /// 平面の法線
        f32 d_; /// 原点から平面までの距離
    };
}

#endif //INC_LMATH_PLANE_H__

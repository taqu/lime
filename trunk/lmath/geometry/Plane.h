#ifndef INC_LMATH_PLANE_H__
#define INC_LMATH_PLANE_H__
/**
@file Plane.h
@author t-sakai
@date 2009/12/20 create
*/
#include "../lmathcore.h"
#include "../Vector3.h"

namespace lmath
{
    /// 平面
    class Plane
    {
    public:
        Plane()
        {
        }

        Plane(const Vector3& normal, f32 d)
            :normal_(normal)
            ,d_(d)
        {
        }

        Plane(const Vector3& point,
            const Vector3& normal)
            :normal_(normal)
        {
            d_ = normal_.dot(point);
        }

        f32 dot(const lmath::Vector3& p) const
        {
            return (normal_.dot(p) - d_);
        }


        Vector3 normal_; /// 平面の法線
        f32 d_; /// 原点から平面までの距離
    };
}

#endif //INC_LMATH_PLANE_H__

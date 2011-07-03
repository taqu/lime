#ifndef INC_LMATH_SPHERE_H__
#define INC_LMATH_SPHERE_H__
/**
@file Sphere.h
@author t-sakai
@date 2009/12/20 create
*/
#include "../lmathcore.h"
#include "../Vector3.h"

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
            :position_(position)
            ,radius_(radius)
        {
        }

        Vector3 position_; /// 中心
        f32 radius_; /// 半径
    };
}
#endif //INC_LMATH_SPHERE_H__

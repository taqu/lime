#ifndef INC_LMATH_RAY_H__
#define INC_LMATH_RAY_H__
/**
@file Ray.h
@author t-sakai
@date 2009/12/21 create
*/
#include "../lmathcore.h"
#include "../Vector3.h"

namespace lmath
{
    class Ray
    {
    public:
        Ray()
        {
        }

        Ray(const Vector3& origin,
            const Vector3& direction,
            f32 t)
            :origin_(origin)
            ,direction_(direction)
            ,t_(t)
        {
        }

        Vector3 origin_;
        Vector3 direction_;
        f32 t_;
    };
}

#endif //INC_LMATH_RAY_H__

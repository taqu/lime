﻿#ifndef INC_LMATH_RAY_H__
#define INC_LMATH_RAY_H__
/**
@file Ray.h
@author t-sakai
@date 2009/12/21 create
*/
#include "../lmath.h"
#include "../Vector3.h"

namespace lmath
{
    class Ray
    {
    public:
        Ray()
        {}

        Ray(const Vector3& origin,
            const Vector3& direction,
            f32 t);

        void reset();
        void invertDirection();

        void setDirection(const lmath::Vector3& direction);
        void setDirection(const lmath::Vector4& direction);
        void setDirection(const lmath::Vector3& direction, const lmath::Vector3& invDirection);
        void setDirection(const lmath::Vector4& direction, const lmath::Vector4& invDirection);

        Vector3 origin_;
        Vector3 direction_;
        Vector3 invDirection_;
        f32 t_;
    };
}

#endif //INC_LMATH_RAY_H__

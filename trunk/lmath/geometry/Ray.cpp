/**
@file Ray.cpp
@author t-sakai
@date 2013/05/10 create
*/
#include "Ray.h"

namespace lmath
{
    Ray::Ray(const Vector3& origin,
        const Vector3& direction,
        f32 t)
        :origin_(origin)
        ,direction_(direction)
        ,t_(t)
    {
        invertDirection();
    }

    void Ray::invertDirection()
    {
        for(s32 i=0; i<3; ++i){
            if(isPositive(direction_[i])){
                invDirection_[i] = (lmath::isZeroPositive(direction_[i]))? lcore::numeric_limits<f32>::maximum() : 1.0f/direction_[i];
            }else{
                invDirection_[i] = (lmath::isZero(direction_[i]))? lcore::numeric_limits<f32>::minimum() : 1.0f/direction_[i];
            }
        }
    }

    void Ray::setDirection(const lmath::Vector3& direction)
    {
        direction_ = direction;
        invertDirection();
    }

    void Ray::setDirection(const lmath::Vector3& direction, const lmath::Vector3& invDirection)
    {
        direction_ = direction;
        invDirection_ = invDirection;
    }

}

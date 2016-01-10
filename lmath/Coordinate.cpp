/**
@file Coordinate.cpp
@author t-sakai
@date 2015/12/20 create
*/
#include "Coordinate.h"

namespace lmath
{
    Vector3 worldToLocal(const Vector3& v, const Vector3& normal, const Vector3& binormal0, const Vector3& binormal1)
    {
        return Vector3(v.dot(binormal0), v.dot(binormal1), v.dot(normal));
    }

    Vector3 localToWorld(const Vector3& v, const Vector3& normal, const Vector3& binormal0, const Vector3& binormal1)
    {
        return Vector3(
            binormal0.x_*v.x_ + binormal1.x_*v.y_ + normal.x_*v.z_,
            binormal0.y_*v.x_ + binormal1.y_*v.y_ + normal.y_*v.z_,
            binormal0.z_*v.x_ + binormal1.z_*v.y_ + normal.z_*v.z_);
    }

    Vector3 Coordinate::worldToLocal(const Vector3& v) const
    {
        return Vector3(v.dot(binormal0_), v.dot(binormal1_), v.dot(normal_));
    }

    Vector3 Coordinate::localToWorld(const Vector3& v) const
    {
        return Vector3(
            binormal0_.x_*v.x_ + binormal1_.x_*v.y_ + normal_.x_*v.z_,
            binormal0_.y_*v.x_ + binormal1_.y_*v.y_ + normal_.y_*v.z_,
            binormal0_.z_*v.x_ + binormal1_.z_*v.y_ + normal_.z_*v.z_);
    }
}

/**
@file Coordinate.cpp
@author t-sakai
@date 2015/09/24 create
*/
#include "core/Coordinate.h"

namespace lrender
{
    Vector3 Coordinate::worldToLocal(const Vector3& v) const
    {
        return Vector3(dot(v, binormal_), dot(v, tangent_), dot(v, normal_));
    }

    Vector3 Coordinate::localToWorld(const Vector3& v) const
    {
        return Vector3(
            binormal_.x_*v.x_ + tangent_.x_*v.y_ + normal_.x_*v.z_,
            binormal_.y_*v.x_ + tangent_.y_*v.y_ + normal_.y_*v.z_,
            binormal_.z_*v.x_ + tangent_.z_*v.y_ + normal_.z_*v.z_);
    }
}

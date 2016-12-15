/**
@file Visibility.cpp
@author t-sakai
@date 2015/09/24 create
*/
#include "core/Visibility.h"
#include "scene/Scene.h"

namespace lrender
{
    void Visibility::setSegment(
        const Vector3& p0, f32 eps0,
        const Vector3& p1, f32 eps1)
    {
        Vector3 d = p0-p1;
        f32 distance = d.length();
        d /= distance;
        ray_ = Ray(p0, d, eps0, distance*(1.0f-eps1), RAY_EPSILON);
    }

    void Visibility::setRay(
        const Vector3& p, f32 eps, f32 tmax,
        const Vector3& w)
    {
        ray_ = Ray(p, w, eps, tmax, RAY_EPSILON);
    }

    bool Visibility::unoccluded(const Scene& scene) const
    {
        return !scene.intersect(ray_);
    }
}

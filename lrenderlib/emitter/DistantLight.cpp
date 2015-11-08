/**
@file DistantLight.cpp
@author t-sakai
@date 2015/09/24 create
*/
#include "DistantLight.h"

#include <lmath/geometry/Sphere.h>
#include "core/Visibility.h"
#include "scene/Scene.h"

namespace lrender
{
    DistantLight::DistantLight(const Vector3& direction, const Color3& radiance)
        :direction_(direction)
        ,radiance_(radiance)
    {
        direction_.normalize();
        localToWorld_.lookAt(direction);
        worldToLocal_.getTranspose(localToWorld_);
    }

    DistantLight::~DistantLight()
    {
    }

    Color3 DistantLight::sample_L(
        const Vector3& p,
        const Vector3& ng,
        const Vector3& ns,
        f32 tmax,
        const EmitterSample& sample,
        Vector3& wiw,
        f32& pdf,
        Visibility& visibility) const
    {
        wiw = direction_;
        pdf = 1.0f;

        Vector3 o;
        o.muladd(SHADOW_EPSILON, ng, p);
        visibility.setRay(o, 0.0f, tmax, wiw);
        return radiance_;
    }

    Color3 DistantLight::sample_L(
        const Scene& scene,
        f32 u0, f32 u1,
        Ray& ray,
        Vector3& Ns,
        f32& pdf) const
    {
        lmath::Sphere bsphere;
        scene.getWorldBound().getBSphere(bsphere);

        Vector3 b0,b1;
        lmath::orthonormalBasis(b0, b1, direction_);

        f32 d0,d1;
        lmath::randomDiskConcentric(d0, d1, u0, u1);
        b0 *= d0;
        b1 *= d1;
        Vector3 dir;
        dir.add(b0, b1);
        dir += direction_;
        dir *= bsphere.getRadius();

        Vector3 point;
        bsphere.getPosition(point);
        point += dir;

        ray = Ray(point, direction_, 0.0f, scene.getWorldMaxSize(), RAY_EPSILON);
        Ns = ray.direction_;
        pdf = 1.0f/(PI*bsphere.getRadius()*bsphere.getRadius());
        return radiance_;
    }

    Color3 DistantLight::sample_L(
        const Vector3& p,
        const Vector3& n,
        f32 tmax,
        const EmitterSample& sample,
        Vector3& wiw,
        f32& pdf) const
    {
        wiw = direction_;
        pdf = 1.0f;
        return radiance_;
    }

    Color3 DistantLight::power(const Scene& scene) const
    {
        lmath::Sphere bsphere;
        scene.getWorldBound().getBSphere(bsphere);
        return radiance_ * PI * bsphere.getRadius() * bsphere.getRadius();
    }

    bool DistantLight::isDeltaLight() const
    {
        return true;
    }
}
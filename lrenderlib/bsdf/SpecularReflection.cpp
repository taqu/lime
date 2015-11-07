/**
@file SpecularReflection.cpp
@author t-sakai
@date 2015/09/24 create
*/
#include "SpecularReflection.h"
#include "core/Intersection.h"
#include "core/Coordinate.h"

namespace lrender
{
    SpecularReflection::SpecularReflection(const Color3& reflectionScale, const Fresnel& fresnel)
        :BSDF(BSDF::Type_Reflection|BSDF::Type_Specular)
        ,reflectionScale_(reflectionScale)
        ,fresnel_(fresnel)
    {
    }

    SpecularReflection::~SpecularReflection()
    {
    }

    Color3 SpecularReflection::f(const Vector3& wo, const Vector3& wi) const
    {
        return Color3::black();
    }

    Color3 SpecularReflection::sample_f(const Vector3& wo, Vector3& wi, f32& pdf, f32 u0, f32 u1) const
    {
        wi.set(-wo.x_, -wo.y_, wo.z_);
        pdf = 1.0f;
        return fresnel_.evaluate(LocalCoordinate::cosTheta(wo)) * reflectionScale_/lcore::absolute(LocalCoordinate::cosTheta(wi));
    }

    f32 SpecularReflection::getPdf(const Vector3& wo, const Vector3& wi) const
    {
        return 0.0f;
    }
}

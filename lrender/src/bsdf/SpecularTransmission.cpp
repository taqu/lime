/**
@file SpecularTransmission.cpp
@author t-sakai
@date 2015/09/24 create
*/
#include "bsdf/SpecularTransmission.h"
#include "core/Intersection.h"
#include "core/Coordinate.h"

namespace lrender
{
    SpecularTransmission::SpecularTransmission(const Color3& transmissionScale, f32 eta_i, f32 eta_t)
        :BSDF(BSDF::Type_Transmission|BSDF::Type_Specular)
        ,transmissionScale_(transmissionScale)
        ,eta_i_(eta_i)
        ,eta_t_(eta_t)
        ,fresnel_(eta_i, eta_t)
    {
    }

    SpecularTransmission::~SpecularTransmission()
    {
    }

    Color3 SpecularTransmission::f(const Vector3& wo, const Vector3& wi) const
    {
        return Color3::black();
    }

    Color3 SpecularTransmission::sample_f(const Vector3& wo, Vector3& wi, f32& pdf, f32 u0, f32 u1) const
    {
        wi.set(-wo.x_, -wo.y_, wo.z_);

        bool entering = 0.0f<LocalCoordinate::cosTheta(wo);
        f32 ei, et;
        if(entering){
            ei = eta_i_;
            et = eta_t_;
        }else{
            ei = eta_t_;
            et = eta_i_;
        }


        f32 ei2 = ei*ei;
        f32 et2 = et*et;
        f32 sini2 = LocalCoordinate::sinTheta2(wo);
        f32 eta = ei/et;
        f32 eta2 = eta*eta;
        f32 sint2 = eta2*sini2;

        if(0.99999f<=sint2){
            return Color3::black();
        }

        f32 cost = lmath::sqrt(lcore::maximum(1.0f-sint2, 0.0f));
        if(entering){
            cost = -cost;
        }
        wi.set(eta * -wo.x_, eta * -wo.y_, cost);

        pdf = 1.0f;
        Color3 F = fresnel_.evaluate(LocalCoordinate::cosTheta(wo));
        return et2/ei2 * (Color3::white()-F) * transmissionScale_ / lcore::absolute(LocalCoordinate::cosTheta(wi));
    }

    f32 SpecularTransmission::getPdf(const Vector3& wo, const Vector3& wi) const
    {
        return 0.0f;
    }
}

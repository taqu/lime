#ifndef INC_LRENDER_SPECULARTRANSMISSION_H__
#define INC_LRENDER_SPECULARTRANSMISSION_H__
/**
@file SpecularTransmission.h
@author t-sakai
@date 2015/09/24 create
*/
#include "BSDF.h"

namespace lrender
{
    class SpecularTransmission : public BSDF
    {
    public:
        SpecularTransmission(const Color3& transmissionScale, f32 eta_i, f32 eta_t);
        virtual ~SpecularTransmission();

        virtual Color3 f(const Vector3& wo, const Vector3& wi) const;
        virtual Color3 sample_f(const Vector3& wo, Vector3& wi, f32& pdf, f32 u0, f32 u1) const;

        virtual f32 getPdf(const Vector3& wo, const Vector3& wi) const;
    protected:
        Color3 transmissionScale_;
        f32 eta_i_;
        f32 eta_t_;
        Fresnel fresnel_;
    };
}
#endif //INC_LRENDER_SPECULARTRANSMISSION_H__

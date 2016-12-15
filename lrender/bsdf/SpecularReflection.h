#ifndef INC_LRENDER_SPECULARREFLECTION_H__
#define INC_LRENDER_SPECULARREFLECTION_H__
/**
@file SpecularReflection.h
@author t-sakai
@date 2015/09/24 create
*/
#include "BSDF.h"

namespace lrender
{
    class SpecularReflection : public BSDF
    {
    public:
        SpecularReflection(const Color3& reflectionScale, const Fresnel& fresnel);
        virtual ~SpecularReflection();

        virtual Color3 f(const Vector3& wo, const Vector3& wi) const;
        virtual Color3 sample_f(const Vector3& wo, Vector3& wi, f32& pdf, f32 u0, f32 u1) const;

        virtual f32 getPdf(const Vector3& wo, const Vector3& wi) const;
    protected:
        Color3 reflectionScale_;
        Fresnel fresnel_;
    };
}
#endif //INC_LRENDER_SPECULARREFLECTION_H__

#ifndef INC_LRENDER_MICROFACET_H__
#define INC_LRENDER_MICROFACET_H__
/**
@file Microfacet.h
@author t-sakai
@date 2015/09/15 create
*/
#include "BSDF.h"
#include "MicrofacetDistribution.h"
#include "MicrofacetTransmittance.h"
#include "texture/Texture.h"

namespace lrender
{
    class Microfacet : public BSDF
    {
    public:
        Microfacet();
        virtual ~Microfacet();

        virtual void initialize();

        virtual Color3 albedo(const Intersection& intersection) const;
        virtual Color3 f(const Vector3& wo, const Vector3& wi, const Intersection& intersection) const;

        virtual Color3 sample_f(const Vector3& wo, Vector3& wi, f32& pdf, const BSDFSample& sample, Type& sampledType, const Intersection& intersection) const;
        virtual f32 getPdf(const Vector3& wo, const Vector3& wi, Type type) const;
    protected:
        Microfacet(const Microfacet&);
        Microfacet& operator=(const Microfacet&);

        Color3 f_diffuse(const Vector3& wo, const Vector3& wi, const Intersection& intersection) const;
        f32 getPdfDiffuse(const Vector3& wo, const Vector3& wi) const;

        Color3 f_specular(const Vector3& wo, const Vector3& wi, const Intersection& intersection) const;

        inline f32 calcFresnelDierecticSchlick(f32 cosWiWh) const;
    public:
        MicrofacetDistribution distribution_;
        MicrofacetTransmittance transmittance_;

        Fresnel fresnel_;
        Texture::pointer diffuseScale_;
        Texture::pointer specularScale_;
        //Texture::pointer transmittanceScale_;
        f32 F0_;
        f32 specularSamplingWeight_;
    };

    inline f32 Microfacet::calcFresnelDierecticSchlick(f32 cosWiWh) const
    {
        f32 F1 = 1.0f-F0_;
        return F0_ + F1*lmath::pow(1.0f-cosWiWh, 5.0f);
    }
}
#endif //INC_LRENDER_MICROFACET_H__

#ifndef INC_LRENDER_MICROFACETTRANCEMITTANCE_H__
#define INC_LRENDER_MICROFACETTRANCEMITTANCE_H__
/**
@file MicrofacetTransmittance.h
@author t-sakai
@date 2015/10/12 create
*/
#include "../lrender.h"


namespace lrender
{
    class Fresnel;
    class MicrofacetDistribution;

    class MicrofacetTransmittance
    {
    public:
        static const s32 PhiResolution = 128;
        static const s32 PhiSamples = 256;

        MicrofacetTransmittance();
        ~MicrofacetTransmittance();

        void create(const Fresnel& fresnel, const MicrofacetDistribution& distribution);

        f32 evaluateProbability(f32 coso) const;

        f32 evaluate(const Vector3& two, const Vector3& wi, const Fresnel& fresnel, f32 alpha) const;

        MicrofacetTransmittance(const MicrofacetTransmittance&);
        MicrofacetTransmittance& operator=(const MicrofacetTransmittance&);

        f32 D(f32 cosH, f32 alpha) const;
        inline f32 G(f32 cosWo, f32 cosWi, f32 cosWoH, f32 cosWiH, f32 alpha) const;

        f32 smithG1(f32 cosV, f32 cosVH, f32 alpha) const;

        inline f32 calcFresnelDierecticSchlick(f32 cosWiWh, f32 F0) const;

        f32 invPhiStep_;
        f32 transmitProbability_[PhiResolution];
    };

    inline f32 MicrofacetTransmittance::G(f32 cosWo, f32 cosWi, f32 cosWoH, f32 cosWiH, f32 alpha) const
    {
        return smithG1(cosWo, cosWoH, alpha)*smithG1(cosWi, cosWiH, alpha);
    }

    inline f32 MicrofacetTransmittance::calcFresnelDierecticSchlick(f32 cosWiWh, f32 F0) const
    {
        f32 F1 = 1.0f-F0;
        return F0 + F1*lmath::pow(1.0f-cosWiWh, 5.0f);
    }
}
#endif //INC_LRENDER_MICROFACETTRANCEMITTANCE_H__

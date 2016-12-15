/**
@file MicrofacetTransmittance.cpp
@author t-sakai
@date 2015/10/12 create
*/
#include "bsdf/MicrofacetTransmittance.h"
#include "core/Coordinate.h"
#include "bsdf/MicrofacetDistribution.h"
#include "bsdf/BSDF.h"

namespace lrender
{
    MicrofacetTransmittance::MicrofacetTransmittance()
        :invPhiStep_(1.0f)
    {
    }

    MicrofacetTransmittance::~MicrofacetTransmittance()
    {
    }

    void MicrofacetTransmittance::create(const Fresnel& fresnel, const MicrofacetDistribution& distribution)
    {
        f32 phiStep = PI_2/PhiResolution;
        invPhiStep_ = 1.0f/phiStep;

        f32 sampleStep = PI_2/PhiSamples;

        f32 F0 = lcore::calcFresnelTerm(fresnel.getEtaI(), fresnel.getEtaT());
#define TRANSMITTANCE (1)

#if TRANSMITTANCE
        f32 maxTransmittance = 0.0f;
#else
        f32 maxSpecular = 0.0f;
#endif
        for(s32 i=0; i<PhiResolution; ++i){
            f32 cosPhi = lmath::cosf(phiStep*i);
            f32 sinPhi = lmath::sqrt(1.0f-cosPhi*cosPhi);
            Vector3 wo(sinPhi, 0.0f, cosPhi);

#if TRANSMITTANCE
            f32 transmittance = 0.0f;
#else
            f32 specular = 0.0f;
#endif
            for(s32 j=0; j < PhiSamples; ++j){
                f32 cosPhi2 = lmath::cosf(sampleStep*j);
                f32 sinPhi2 = lmath::sqrt(1.0f - cosPhi2*cosPhi2);

                Vector3 wi(-sinPhi2, 0.0f, cosPhi2);

#if TRANSMITTANCE
                transmittance += evaluate(wo, wi, fresnel, distribution.getAlpha()) * cosPhi2;
#else
                Vector3 wh;
                wh.add(wo, wi);
                wh.normalizeChecked();

                f32 D = lcore::maximum(distribution.D(wh), 0.0f);

                f32 cosWiWh = wi.dot(wh);
                f32 F = calcFresnelDierecticSchlick(cosWiWh, F0);
                f32 G = distribution.G(wo, wi, wh);

                f32 ggx = F*D*G / (4.0f*cosPhi*cosPhi2);

                specular += ggx * cosPhi2;
#endif
            }
#if TRANSMITTANCE
            transmittance *= (1.0f/PhiSamples);
            //transmitProbability_[i] = transmittance;
            transmitProbability_[i] = lcore::clamp01(transmittance);
            maxTransmittance = lcore::maximum(maxTransmittance, transmittance);
#else
            specular *= (1.0f/PhiSamples);
            maxSpecular = lcore::maximum(maxSpecular, specular);
            //transmitProbability_[i] = specular;
            transmitProbability_[i] = lcore::clamp01(1.0f-specular);
#endif
        }

        //if(maxTransmittance < PDF_EPSILON){
        //    for(s32 i = 0; i < PhiResolution; ++i){
        //        transmitProbability_[i] = 0.0f;
        //    }
        //} else{
        //    f32 invMax = 1.0f / maxTransmittance;
        //    for(s32 i = 0; i < PhiResolution; ++i){
        //        transmitProbability_[i] *= invMax;
        //    }
        //}

        //if(maxTransmittance < PDF_EPSILON){
        //    for(s32 i = 0; i < PhiResolution; ++i){
        //        transmitProbability_[i] = 0.0f;
        //    }
        //} else{
        //    f32 invMax = 1.0f / maxTransmittance;
        //    for(s32 i = 0; i < PhiResolution; ++i){
        //        transmitProbability_[i] *= invMax;
        //    }
        //}
    }

    f32 MicrofacetTransmittance::evaluateProbability(f32 coso) const
    {
        f32 theta = lmath::acos(coso);
        s32 index = lcore::clamp(lmath::float2S32(theta*invPhiStep_), 0, PhiResolution-1);
        return transmitProbability_[index];
    }

    f32 MicrofacetTransmittance::evaluate(const Vector3& two, const Vector3& wi, const Fresnel& fresnel, f32 alpha) const
    {
        Vector3 wo = two;
        //if(LocalCoordinate::isSameHemisphere(two, wi)){
        //    wo.z_ = -wo.z_;
        //}

        f32 etai=fresnel.getEtaI();
        f32 etao=fresnel.getEtaT();
        f32 F0 = lcore::calcFresnelTerm(etai, etao);

        f32 cosWi = wi.z_;
        //f32 sinWi = lmath::sqrt(1.0f-cosWi*cosWi);

        f32 cosWo = wo.z_;
        //f32 sinWo = lmath::sqrt(1.0f - cosWo*cosWo);

        Vector3 wo2 = etao*wo;
        Vector3 wi2 = etai*wi;

        Vector3 wh = normalizeChecked(wo2+wi2);

        f32 cosH = lcore::absolute(wh.z_);
        f32 cosWoH = dot(wo, wh);
        f32 cosWiH = dot(wi, wh);


        f32 denom = (etai*cosWiH + etao*cosWoH);
        f32 t = (lcore::absolute(cosWiH) * lcore::absolute(cosWoH) *etao*etao) / (lcore::absolute(cosWi)*lcore::absolute(cosWo) * denom * denom);

        f32 f = 1.0f-calcFresnelDierecticSchlick(cosWiH, F0);
        f32 g = G(cosWi, cosWo, cosWiH, cosWoH, alpha);
        f32 d = D(cosH, alpha);
        f32 value = f*g*d;
        
        return value * t;
    }


    f32 MicrofacetTransmittance::D(f32 cosH, f32 alpha) const
    {
        //GGX Distribution
        if(cosH<=0.0f){
            return 0.0f;
        }

        f32 cosTheta2 = cosH*cosH;
#if 0
        f32 t = (wh.x_*wh.x_)/(alphaU_*alphaU_) + (wh.y_*wh.y_)/(alphaV_*alphaV_) + cosTheta2;
        f32 ret = 1.0f/(PI*alphaU_*alphaV_ * t*t);
#else
        f32 alpha2 = alpha*alpha;
        f32 denom = cosTheta2*(alpha2 - 1.0f) + 1.0f;
        f32 ret = alpha2 / (PI*denom*denom);
#endif
        return (ret*cosH<1.0e-6f)? 0.0f : ret;
    }

    f32 MicrofacetTransmittance::smithG1(f32 cosV, f32 cosVH, f32 alpha) const
    {
        f32 cosTheta = cosV;
        //if(cosVH * cosV <= 0.0f){
        //    return 0.0f;
        //}
        cosTheta = lcore::absolute(cosTheta);
        f32 k = alpha*0.5f;
        return 1.0f/(cosTheta*(1.0f-k)+k);
    }
}

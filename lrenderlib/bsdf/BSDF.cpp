/**
@file BSDF.cpp
@author t-sakai
@date 2015/09/15 create
*/
#include "BSDF.h"
#include "core/Spectrum.h"
#include "core/Coordinate.h"
#include "core/Intersection.h"

namespace lrender
{
    //---------------------------------------------------------------------
    //---
    //--- Fresnel
    //---
    //---------------------------------------------------------------------
    f32 fresnelDielectic(f32 cosi, f32 cost, const f32& etai, const f32& etat)
    {
        f32 etatcosi = etat*cosi;
        f32 etaicost = etai*cost;
        f32 etaicosi = etai*cosi;
        f32 etatcost = etat*cost;

        f32 rParl = (etatcosi - etaicost) / (etatcosi + etaicost);
        f32 rPerp = (etaicosi - etatcost) / (etaicosi + etatcost);
        return (rParl*rParl + rPerp*rPerp) * 0.5f;
    }

    Color3 fresnelConductor(f32 cosi, const Color3& eta, const Color3& k)
    {
        f32 cosi2 = cosi*cosi;
        Color3 tmp = (eta*eta + k*k);
        Color3 tmp2 = tmp * cosi2;
        Color3 etacos = eta*2.0f*cosi;

        Color3 one(1.0f);
        Color3 ccosi2(cosi2);

        Color3 rParl2 = (tmp2-etacos+one)/(tmp2+etacos+one);

        Color3 rPerp2 = (tmp - etacos + ccosi2)/(tmp + etacos + ccosi2);

        return (rParl2 + rPerp2) * 0.5f;
    }

    Color3 Fresnel::evaluate(f32 cosi) const
    {
        switch(type_)
        {
        case Type_Conductor:
            return fresnelConductor(cosi, eta_, k_);

        case Type_Dielectic:
            {
                return Color3(evalDielectic(cosi));
            }

        default:
            return Color3::white();
        }
    }

    Color3 Fresnel::evalConductor(f32 cosi) const
    {
        return fresnelConductor(cosi, eta_, k_);
    }

    f32 Fresnel::evalDielectic(f32 cosi) const
    {
        cosi = lcore::clamp(cosi, -1.0f, 1.0f);
        f32 ei = eta_i_;
        f32 et = eta_t_;
        if(0.0f<cosi){
            lcore::swap(ei, et);
        }
        f32 sint = ei/et * lmath::sqrt(lcore::maximum(1.0f-cosi*cosi, 0.0f)); //Snell's law
        if(1.0f<=sint){
            return 1.0f;
        } else{
            f32 cost = lmath::sqrt(lcore::maximum(1.0f-sint*sint, 1.0e-4f));
            return fresnelDielectic(lcore::absolute(cosi), cost, ei, et);
        }
    }

    f32 Fresnel::dielectricExt(f32 cosi, f32 eta)
    {
        if(lmath::isEqual(eta, 1.0f)){
            return 0.0f;
        }

        f32 scale = (1.0e-5f<cosi)? 1.0f/eta : eta;
        f32 sqrCost = 1.0f - (1.0f-cosi*cosi) * (scale * scale);
        if(sqrCost < 1.0e-5f){
            return 1.0f;
        }
        f32 cosThetaI = lcore::absolute(cosi);
        f32 cosThetaT = lmath::sqrt(sqrCost);

        f32 rs = (cosThetaI - eta * cosThetaT)/(cosThetaI + eta * cosThetaT);
        f32 rp = (eta * cosThetaI - cosThetaT)/(eta*cosThetaI + cosThetaT);
        return 0.5f * (rs*rs + rp*rp);
    }

    Color3 Fresnel::conductorExt(f32 cosi, const Color3& eta, const Color3& k)
    {
        return fresnelConductor(cosi, eta, k);
    }

    //---------------------------------------------------------------------
    //---
    //--- BSDF
    //---
    //---------------------------------------------------------------------
    Color3 BSDF::f(const Vector3& wow, const Vector3& wiw, Type type, const Intersection& intersection) const
    {
        Vector3 wo = intersection.worldToLocal(wow);
        Vector3 wi = intersection.worldToLocal(wiw);
        f32 descr = wiw.dot(intersection.geometricNormal_) * wow.dot(intersection.geometricNormal_);

        if(0.0f<descr){ //ジオメトリの法線に対して同じ側
            type = Type(type & ~Type_Transmission);
        }else{
            type = Type(type & ~Type_Reflection);
        }

        Color3 l = Color3::black();
        if(checkType(type)){
            l += f(wo, wi, intersection);
        }
        return l;
    }

    Color3 BSDF::sample_f(const Vector3& wo, Vector3& wi, f32& pdf, const BSDFSample& sample, Type& sampledType, const Intersection& intersection) const
    {
        lmath::cosineWeightedRandomOnHemisphere(wi.x_, wi.y_, wi.z_, sample.u0_, sample.u1_);
        if(wo.z_<0.0f){
            wi.z_ = -wi.z_;
        }
        sampledType = Type_Diffuse;
        pdf = getPdf(wo, wi, sampledType);
        return f(wo, wi, intersection);
    }

    Color3 BSDF::sample_f(const Vector3& wow, Vector3& wi, Vector3& wiw, const BSDFSample& sample, f32& pdf, Type& sampledType, const Intersection& intersection) const
    {
        Vector3 wo = intersection.worldToLocal(wow);

        Type type = sampledType;
        Color3 l = sample_f(wo, wi, pdf, sample, type, intersection);
        wiw = intersection.localToWorld(wi);

        if(lmath::isZeroPositive(pdf)){
            return Color3::black();
        }
        sampledType = type;
        return l;
    }

    Color3 BSDF::rho(const Vector3& wo, s32 numSamples, const BSDFSample* samples, Type type, const Intersection& intersection) const
    {
        Color3 r = Color3::black();
        for(s32 i=0; i<numSamples; ++i){
            Vector3 wi;
            f32 pdf;
            Color3 l = sample_f(wo, wi, pdf, samples[i], type, intersection);
            if(0.0f<pdf){
                r += l*LocalCoordinate::absCosTheta(wi)/pdf;
            }
        }
        return r/numSamples;
    }

    Color3 BSDF::rho(s32 numSamples, const f32* samples0, const BSDFSample* samples1, Type type, const Intersection& intersection) const
    {
        Color3 r = Color3::black();
        for(s32 i=0; i<numSamples; ++i){
            Vector3 wo, wi;
            lmath::randomOnHemisphere(wo.x_, wo.y_, wo.z_, samples0[2*i+0], samples0[2*i+1]);

            f32 pdfo = INV_PI2, pdfi;
            Color3 l = sample_f(wo, wi, pdfi, samples1[i], type, intersection);
            if(0.0f<pdfi){
                r += l*LocalCoordinate::absCosTheta(wo)*LocalCoordinate::absCosTheta(wi)/(pdfo*pdfi);
            }
        }
        return r/(PI*numSamples);
    }

    f32 BSDF::getPdf(const Vector3& wo, const Vector3& wi, Type type) const
    {
        return (LocalCoordinate::isSameHemisphere(wo, wi))
            ? LocalCoordinate::absCosTheta(wi)*INV_PI
            : 0.0f;
    }
}

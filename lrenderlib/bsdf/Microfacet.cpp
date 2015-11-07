/**
@file Microfacet.cpp
@author t-sakai
@date 2015/09/15 create
*/
#include "Microfacet.h"
#include "core/Intersection.h"
#include "shape/Shape.h"

namespace lrender
{
    Microfacet::Microfacet()
        :BSDF(BSDF::Type_Specular|Type_Diffuse)
        ,distribution_(0.9f)
        ,F0_(0.0f)
        ,specularSamplingWeight_(0.5f)
    {
    }

    Microfacet::~Microfacet()
    {
    }

    void Microfacet::initialize()
    {
        if(NULL == diffuseScale_){
            diffuseScale_ = LIME_NEW TextureConstant(Color3::white());
        }
        if(NULL == specularScale_){
            specularScale_ = LIME_NEW TextureConstant(Color3::white());
        }

        F0_ = lcore::calcFresnelTerm(fresnel_.getEtaI(), fresnel_.getEtaT());

        //f32 diffuseScale = diffuseScale_->avarage().getLuminance() * (1.0f-F0_);
        //f32 specularScale = specularScale_->avarage().getLuminance();

        //diffuseScale = diffuseScale*diffuseScale;
        //specularScale = specularScale*specularScale;

        //if(diffuseScale<1.0e-4f){
        //    unsetType(Type_Diffuse);
        //}
        //if(specularScale<1.0e-4f){
        //    unsetType(Type_Specular);
        //}

        //f32 totalScale = diffuseScale + specularScale;
        //if(totalScale<1.0e-4f){
        //    specularSamplingWeight_ = 0.5f;
        //}else{
        //    specularSamplingWeight_ = specularScale/totalScale;
        //}

        //transmittance_.create(fresnel_, distribution_);

        specularSamplingWeight_ = 0.0f;
    }

    Color3 Microfacet::f(const Vector3& wo, const Vector3& wi, const Intersection& intersection) const
    {
        f32 cosThetaO = LocalCoordinate::cosTheta(wo);
        f32 cosThetaI = LocalCoordinate::cosTheta(wi);
        if(cosThetaO<=DOT_EPSILON || cosThetaI<=DOT_EPSILON){
            return Color3::black();
        }


        Vector3 wh;
        wh.add(wo, wi);
        wh.normalizeChecked();

        f32 D = lcore::maximum(distribution_.D(wh), 0.0f);

        f32 cosWiWh = wi.dot(wh);
        f32 F = calcFresnelDierecticSchlick(cosWiWh);//fresnel_.evalDielectic(cosWiWh);
        f32 G = distribution_.G(wo, wi, wh);
        f32 normalizeFactor = lcore::clamp01(1.0f/(4.0f*cosThetaO*cosThetaI));
        f32 ggx = (F*D*G)*normalizeFactor;

        Color3 specReflectance = specularScale_->sample(intersection) * ggx;

        //f32 trans = transmittance_.evaluate(wo, wi, fresnel_, distribution_.getAlpha());
        Color3 diffReflectance = diffuseScale_->sample(intersection) * INV_PI;

        return (specReflectance + diffReflectance);
    }

    Color3 Microfacet::sample_f(const Vector3& wo, Vector3& wi, f32& pdf, const BSDFSample& sample, Type& sampledType, const Intersection& intersection) const
    {
        pdf = 0.0f;

        f32 cosThetaO = LocalCoordinate::cosTheta(wo);
        if(cosThetaO<=DOT_EPSILON
            || !checkType(sampledType))
        {
            return Color3::black();
        }

        bool typeDiffuse = checkType(sampledType, Type_Diffuse);
        bool typeSpecular = checkType(sampledType, Type_Specular);

        if(typeDiffuse && typeSpecular){
            //f32 diffuseProbability = transmittance_.evaluateProbability(cosThetaO);
            //f32 specularProbability = 1.0f - diffuseProbability;
            //f32 specularWeight = specularProbability*specularSamplingWeight_;
            //f32 diffuseWeight =  diffuseProbability*(1.0f - specularSamplingWeight_);

            //specularProbability = specularWeight/(specularWeight+diffuseWeight);
            f32 specularProbability = specularSamplingWeight_;
            if(sample.component_ < specularProbability){
                typeDiffuse = false;
            }else{
                typeSpecular = false;
            }
        }

        if(typeDiffuse){
            //Lambert        
            lmath::cosineWeightedRandomOnHemisphere(wi.x_, wi.y_, wi.z_, sample.u0_, sample.u1_);
            if(wo.z_<0){
                wi.z_ = -wi.z_;
            }
#if 1
            
            pdf = getPdfDiffuse(wo, wi);
            sampledType = Type_Diffuse;
            return f_diffuse(wo, wi, intersection);
#else
            pdf = getPdf(wo, wi, sampledType);
            sampledType = Type_Diffuse;
            return f(wo, wi, intersection);
#endif
        }

        if(typeSpecular){
            //GGX
            wi = distribution_.sample_f(wo, sample.u0_, sample.u1_);
            if(wi.isNan()){
                return Color3::black();
            }
            if(LocalCoordinate::cosTheta(wi)<=0.0f){
                return Color3::black();
            }
#if 1
            
            pdf = distribution_.getPdf(wo, wi);
            sampledType = Type_Specular;
            return f_specular(wo, wi, intersection);
#else
            pdf = getPdf(wo, wi, sampledType);
            sampledType = Type_Specular;
            return f(wo, wi, intersection);
#endif
        }
        return Color3::black();
    }

    f32 Microfacet::getPdf(const Vector3& wo, const Vector3& wi, Type type) const
    {
        f32 cosThetaO = LocalCoordinate::cosTheta(wo);
        f32 cosThetaI = LocalCoordinate::cosTheta(wi);
        if(cosThetaO<=DOT_EPSILON || cosThetaI<=DOT_EPSILON){
            return 0.0f;
        }

        if(checkType(type, Type_Diffuse) && checkType(type, Type_Specular)){

            //f32 diffuseProbability = transmittance_.evaluateProbability(cosThetaO);
            //f32 specularProbability = 1.0f - diffuseProbability;
            //f32 specularWeight = specularProbability*specularSamplingWeight_;
            //f32 diffuseWeight =  diffuseProbability*(1.0f - specularSamplingWeight_);

            //specularProbability = specularWeight/(specularWeight+diffuseWeight);
            //diffuseProbability = 1.0f - specularProbability;

            f32 specularProbability = specularSamplingWeight_;
            f32 diffuseProbability = 1.0f - specularSamplingWeight_;

            f32 diffPdf = getPdfDiffuse(wo, wi);
            f32 specPdf = distribution_.getPdf(wo, wi);
            return diffPdf*diffuseProbability + specPdf*specularProbability;
        }

        if(checkType(type, Type_Diffuse)){
            return getPdfDiffuse(wo, wi);
        }

        if(checkType(type, Type_Specular)){
            return distribution_.getPdf(wo, wi);
        }
        return 0.0f;
    }

    Color3 Microfacet::f_diffuse(const Vector3& wo, const Vector3& wi, const Intersection& intersection) const
    {
        return diffuseScale_->sample(intersection) * INV_PI;
    }

    f32 Microfacet::getPdfDiffuse(const Vector3& wo, const Vector3& wi) const
    {
        return LocalCoordinate::isSameHemisphere(wo, wi)
            ? lmath::cosineWeightedHemispherePdf(LocalCoordinate::absCosTheta(wi))
            : 0.0f;
    }

    Color3 Microfacet::f_specular(const Vector3& wo, const Vector3& wi, const Intersection& intersection) const
    {
        f32 cosThetaO = LocalCoordinate::cosTheta(wo);
        f32 cosThetaI = LocalCoordinate::cosTheta(wi);
        if(cosThetaO<=DOT_EPSILON || cosThetaI<=DOT_EPSILON){
            return Color3::black();
        }

        Vector3 wh;
        wh.add(wo, wi);
        wh.normalizeChecked();

        f32 D = lcore::maximum(distribution_.D(wh), 0.0f);

        f32 cosWiWh = wi.dot(wh);
        f32 F = calcFresnelDierecticSchlick(cosWiWh);//fresnel_.evalDielectic(cosWiWh);
        f32 G = distribution_.G(wo, wi, wh);
        
        f32 ggx = F*D*G/(4.0f*cosThetaO*cosThetaI);
        Color3 specReflectance = specularScale_->sample(intersection) * ggx;

        return (specReflectance);
    }
}

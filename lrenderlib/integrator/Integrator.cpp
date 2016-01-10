/**
@file Integrator.cpp
@author t-sakai
@date 2015/09/28 create
*/
#include "Integrator.h"
#include "core/Intersection.h"
#include "scene/Scene.h"
#include "emitter/Emitter.h"
#include "bsdf/BSDF.h"
#include "core/Visibility.h"
#include "core/Coordinate.h"

namespace lrender
{
    void IntegratorQuery::clear()
    {
        intersection_.shape_ = NULL;
        depth_ = 0;
    }

    bool IntegratorQuery::intersect(const Ray& ray)
    {
        intersection_.shape_ = NULL;
        return scene_->intersect(intersection_, ray);
    }

    Color3 IntegratorQuery::sampleEmitterDirect(
        const Vector3& wow,
        const EmitterSample& emitterSample,
        const BSDFSample& bsdfSample)
    {
        const Emitter::EmitterVector& emitters = scene_->getEmitters();
        if(emitters.size()<=0){
            return Color3::black();
        }

        s32 index = lmath::floorS32(sampler_->get1D() * emitters.size());
        index = lcore::minimum(index, emitters.size() - 1);

        const Emitter& emitter = *emitters[index];

        return estimateDirect(
            emitter,
            intersection_.point_,
            intersection_.shadingNormal_,
            intersection_.geometricNormal_,
            wow,
            emitterSample,
            bsdfSample,
            BSDF::Type_All_Emitter);
    }

    Color3 IntegratorQuery::estimateDirect(
        const Emitter& emitter,
        const Vector3& p,
        const Vector3& ns,
        const Vector3& ng,
        const Vector3& wow,
        const EmitterSample& emitterSample,
        const BSDFSample& /*bsdfSample*/,
        s32 bsdfType) const
    {
        const BSDF::pointer& bsdf = intersection_.shape_->getBSDF();

        Color3 Ld = Color3::black();

        Vector3 wiw;
        f32 lightPdf, bsdfPdf;
        Visibility visibility;
        Color3 Li = emitter.sample_L(p, ng, ns, scene_->getWorldMaxSize(), emitterSample, wiw, lightPdf, visibility);

        if(0.0f<lightPdf && !Li.isZero()){
            Color3 f = bsdf->f(wow, wiw, (BSDF::Type)bsdfType, intersection_);

            if(!f.isZero()
                && visibility.unoccluded(*scene_)){

                if(emitter.isDeltaLight()){
                    Ld += f * Li * (lcore::absolute(wiw.dot(ns))/lightPdf);
                }else{
                    Vector3 wo = intersection_.worldToLocal(wow);
                    Vector3 wi = intersection_.worldToLocal(wiw);
                    bsdfPdf = bsdf->getPdf(wo, wi, (BSDF::Type)bsdfType);
                    f32 weight = powerHeuristic(1, lightPdf, 1, bsdfPdf);
                    Ld += f * Li * (lcore::absolute(wi.dot(ns)*weight)/lightPdf);
                }
            }
        }

        //TODO:
        if(!emitter.isDeltaLight()){

        }
        return Ld;
    }

    Color3 Integrator::specularReflectDirect(const Ray& /*ray*/, const Intersection& /*intersection*/, IntegratorQuery& /*query*/)
    {
        //const BSDF::pointer& bsdf = intersection.shape_->getBSDF();

        //Vector3 wo = -ray.direction_;
        //Vector3 wi;
        //f32 pdf;
        //BSDF::Type type = BSDF::Type(BSDF::Type_All_Reflection|BSDF::Type_Specular);
        //Color3 f = bsdf->sample_f(wo, wi, BSDFSample(query.getSampler().next2D()), pdf, type, intersection);

        //const Vector3& geometricNormal = intersection.geometricNormal_;
        //const Vector3& shadingNormal = intersection.shadingNormal_;
        //f32 wi_ns = lcore::absolute(wi.dot(shadingNormal));
        //if(pdf<=PDF_EPSILON || f.isZero() || lcore::absolute(wi.dot(geometricNormal)<=DOT_EPSILON ){
        //    return Color3(0.0f);
        //}
        //Ray rd = intersection.nextRay(wi);

        //Intersection nextIntersection;

        ////if(query.getScene().intersect(nextIntersection, rd)){
        ////    return Color3::black();
        ////}

        //Color3 Li = Color3::black();

        //Vector3 wo = -ray.direction_;

        //const BSDF::pointer& bsdf = intersection.shape_->getBSDF();
        //const Emitter::EmitterVector& emitters = query.getScene().getEmitters();
        //if(0 < emitters.size()){
        //    for(s32 i = 0; i < numLightSamples_; ++i){
        //        const Emitter::EmitterVector& emitters = query.getScene().getEmitters();
        //        s32 index = lmath::floorS32(lightSamples1D[i] * emitters.size());
        //        index = lcore::minimum(index, emitters.size() - 1);

        //        EmitterSample emitterSample(lightSamples2D[i].x_, lightSamples2D[i].y_);
        //        BSDFSample bsdfSample(bsdfSamples2D[i].x_, bsdfSamples2D[i].y_);
        //        Li += query.estimateDirect(
        //            *emitters[index],
        //            intersection.point_,
        //            intersection.shadingNormal_,
        //            wo,
        //            emitterSample,
        //            bsdfSample,
        //            BSDF::Type_AllEmitter,
        //            intersection);

        //    }
        //}

        //return f * Li(rd, query) * wi_ns/pdf;
        return Color3(0.0f);
    }
}

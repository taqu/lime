/**
@file PathTracer.cpp
@author t-sakai
@date 2015/09/16 create
*/
#include "PathTracer.h"
#include "core/Ray.h"
#include "core/Intersection.h"
#include "sampler/Sampler.h"
#include "scene/Scene.h"
#include "bsdf/Microfacet.h"
#include "core/Visibility.h"

namespace lrender
{
    PathTracer::PathTracer(s32 russianRouletteDepth, s32 maxDepth)
        :russianRouletteDepth_(russianRouletteDepth)
        ,maxDepth_(maxDepth)
        ,strictNormal_(false)
    {
    }

    PathTracer::~PathTracer()
    {
    }

    void PathTracer::requestSamples(Sampler* /*sampler*/)
    {
    }

    Color3 PathTracer::Li(const Ray& r, IntegratorQuery& query)
    {
        Ray ray(r);
        query.intersect(ray);

        Color3 L = Color3::black();
        Color3 beta = Color3(1.0f);
        Intersection& insect = query.intersection_;

        EmitterSample emitterSample;
        BSDFSample emitterBsdfSample;
        BSDFSample bsdfBsdfSample;
        while(query.depth_<=maxDepth_ || maxDepth_<0){

            if(!insect.isIntersect()){
                break;
            }

            emitterSample = query.sampler_->next2D();
            emitterBsdfSample = BSDFSample(query.sampler_->next2D(), query.sampler_->next1D());
            bsdfBsdfSample = BSDFSample(query.sampler_->next2D(), query.sampler_->next1D());

            Vector3 wow = -ray.direction_;
            Vector3 wo = insect.worldToLocal(wow);
            const BSDF::pointer& bsdf = insect.shape_->getBSDF();

            if(strictNormal_ && 0.0f<=ray.direction_.dot(insect.geometricNormal_) * LocalCoordinate::cosTheta(wo)){
                break;
            }
            
            L += query.sampleEmitterDirect(wow, emitterSample, emitterBsdfSample) * beta;

            Vector3 wi, wiw;
            f32 bsdfPdf;
            BSDF::Type sampledBsdfType = BSDF::Type_All;
            Color3 f = bsdf->sample_f(wow, wi, wiw, bsdfBsdfSample, bsdfPdf, sampledBsdfType, query.intersection_);
            if(f.isZero() || bsdfPdf<=PDF_EPSILON){
                break;
            }
            if(strictNormal_ && insect.geometricNormal_.dot(wiw)*LocalCoordinate::cosTheta(wi)<=0.0f){
                break;
            }
            
            beta *= f * lcore::absolute(wiw.dot(insect.shadingNormal_))/bsdfPdf;
            ray = insect.nextRay(wiw, query.scene_->getWorldMaxSize());

            ++query.depth_;

            if(russianRouletteDepth_<=query.depth_){
                f32 continueProbability = lcore::minimum(beta.getLuminance(), 0.9f);
                if(continueProbability <= query.sampler_->get1D()){
                    break;
                }
                beta /= continueProbability;
            }
            query.intersect(ray);
        }
        
        return L;
    }

    Color4 PathTracer::E(IntegratorQuery& /*query*/)
    {
        return Color4::black();
    }
}

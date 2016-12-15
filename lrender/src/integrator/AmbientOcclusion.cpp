/**
@file AmbientOcclusion.cpp
@author t-sakai
@date 2015/10/06 create
*/
#include "integrator/AmbientOcclusion.h"
#include "scene/Scene.h"

namespace lrender
{
    AmbientOcclusion::AmbientOcclusion(s32 numSamples, f32 rayLength)
        :numSamples_(numSamples)
        ,rayLength_(rayLength)
    {
    }

    AmbientOcclusion::~AmbientOcclusion()
    {
    }

    void AmbientOcclusion::requestSamples(Sampler* sampler)
    {
        sampler->request2DArray(numSamples_);
    }

    Color3 AmbientOcclusion::Li(const Ray& r, IntegratorQuery& query)
    {
        Ray ray(r);
        query.intersect(ray);

        Color3 L = Color3::black();
        if(!query.intersect(ray)){
            return Color3::white();
        }

        Sampler& sampler = *query.sampler_;

        Sample2D* sample2D = sampler.next2DArray(numSamples_);

        f32 tmax = (rayLength_<0.0f)? query.scene_->getWorldBound().getBSphereRadius() : rayLength_;
        for(s32 i=0; i<numSamples_; ++i){
            
            Vector3 d;
            lmath::cosineWeightedRandomOnHemisphere(d.x_, d.y_, d.z_, sample2D[i].x_, sample2D[i].y_);
            Ray nextRay = query.intersection_.nextRay(d, tmax);
            if(!query.intersect(nextRay)){
                L += Color3::white();
            }
        }
        L /= static_cast<f32>(numSamples_);
        return L;
    }

    Color4 AmbientOcclusion::E(IntegratorQuery& query)
    {
        Color3 L(0.0f);

        Sampler& sampler = *query.sampler_;

        Sample2D* sample2D = sampler.next2DArray(numSamples_);

        f32 tmax = (rayLength_<0.0f)? query.scene_->getWorldBound().getBSphereRadius() : rayLength_;
        for(s32 i=0; i<numSamples_; ++i){
            
            Ray r = query.intersection_.nextCosineWeightedHemisphere(sample2D[i].x_, sample2D[i].y_, tmax);
            if(!query.intersect(r)){
                L += Color3::white();
            }
        }
        L /= static_cast<f32>(numSamples_);
        return Color4(L[0], L[1], L[2], 1.0f);
    }
}

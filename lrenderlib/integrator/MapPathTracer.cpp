/**
@file MapPathTracer.cpp
@author t-sakai
@date 2015/10/07 create
*/
#include "MapPathTracer.h"
#include "core/Ray.h"
#include "core/Intersection.h"
#include "sampler/Sampler.h"
#include "scene/Scene.h"
#include "bsdf/Microfacet.h"
#include "core/Visibility.h"

namespace lrender
{
    MapPathTracer::MapPathTracer(bool handleIndirect, s32 russianRouletteDepth, s32 maxDepth)
        :PathTracer(russianRouletteDepth, maxDepth)
        ,handleIndirect_(handleIndirect)
    {
    }

    MapPathTracer::~MapPathTracer()
    {
    }

    void MapPathTracer::requestSamples(Sampler* sampler)
    {
        PathTracer::requestSamples(sampler);
    }

    Color4 MapPathTracer::E(IntegratorQuery& query)
    {
        RayDifferential ray;

        Sampler& sampler = *query.sampler_;
        Color3 irradiance = Color3::black();

        EmitterSample emitterSample;
        BSDFSample emitterBsdfSample;
        BSDFSample bsdfBsdfSample;

        Sample2D sample;
        sampler.generate();
        for(s32 i=0; i<sampler.getNumSamples(); ++i){
            emitterSample = sampler.next2D();
            emitterBsdfSample = BSDFSample(sampler.next2D(), sampler.next1D());

            sample = sampler.next2D();
            ray = query.intersection_.nextHemisphere(sample.x_, sample.y_, query.scene_->getWorldMaxSize());

            Color3 L = query.sampleEmitterDirect(ray.direction_, emitterSample, emitterBsdfSample);
            if(!L.isZero()){
                irradiance += L;
            }

            if(handleIndirect_){
                IntegratorQuery indirectQuery = query;
                indirectQuery.clear();
                sample = sampler.next2D();
                ray = indirectQuery.intersection_.nextCosineWeightedHemisphere(sample.x_, sample.y_, indirectQuery.scene_->getWorldMaxSize());
                ++indirectQuery.depth_;
                irradiance += Li(ray, indirectQuery) * PI;
            }
            sampler.advance();
        }
        f32 invN = 1.0f/sampler.getNumSamples();
        irradiance *= invN;
        return Color4(irradiance[0], irradiance[1], irradiance[2], 1.0f);
    }
}

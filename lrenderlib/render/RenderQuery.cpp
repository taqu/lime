/**
@file RenderQuery.cpp
@author t-sakai
@date 2015/09/16 create
*/
#include "RenderQuery.h"
#include "core/Ray.h"
#include "sampler/Sampler.h"
#include "scene/Scene.h"
#include "scene/Screen.h"
#include "integrator/Integrator.h"

namespace lrender
{
    RenderQuery::RenderQuery(const Scene* scene, Sampler* sampler, Screen* screen, Integrator* integrator)
        :scene_(scene)
        ,sampler_(sampler)
        ,screen_(screen)
        ,integrator_(integrator)
    {
        LASSERT(NULL != scene_);
        LASSERT(NULL != sampler_);
        LASSERT(NULL != screen_);
        LASSERT(NULL != integrator_);
    }

    RenderQuery::~RenderQuery()
    {
    }

    void RenderQuery::initialize()
    {
        integrator_->requestSamples(sampler_);
    }

    void RenderQuery::render()
    {
        IntegratorQuery integratorQuery(scene_, sampler_);

        RayDifferential ray;
        screen_->clearWeights();
        for(s32 y=0; y<screen_->getHeight(); ++y){
            for(s32 x=0; x<screen_->getWidth(); ++x){
                sampler_->generate();

                for(s32 i=0; i<sampler_->getNumSamples(); ++i){
                    Sample2D cameraSample = sampler_->next2D();
                    cameraSample.x_ += x;
                    cameraSample.y_ += y;
                    scene_->getCamera().generateRayDifferential(ray, cameraSample.x_, cameraSample.y_);
                    integratorQuery.clear();
                    Color3 color = integrator_->Li(ray, integratorQuery);
                    screen_->setFiltered(cameraSample.x_, cameraSample.y_, color);
                    sampler_->advance();
                }
            }
        }
        screen_->divWeights();
    }
}

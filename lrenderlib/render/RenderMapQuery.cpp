/**
@file RenderMapQuery.cpp
@author t-sakai
@date 2015/10/06 create
*/
#include "RenderMapQuery.h"
#include "core/Ray.h"
#include "sampler/Sampler.h"
#include "scene/Scene.h"
#include "integrator/Integrator.h"
#include "core/ShadingGeometry.h"

namespace lrender
{
    RenderMapQuery::RenderMapQuery(const Scene* scene, Sampler* sampler, Image* image, Integrator* integrator)
        :scene_(scene)
        ,sampler_(sampler)
        ,image_(image)
        ,integrator_(integrator)
    {
        LASSERT(NULL != scene_);
        LASSERT(NULL != sampler_);
        LASSERT(NULL != image_);
        LASSERT(NULL != integrator_);
    }

    RenderMapQuery::~RenderMapQuery()
    {
    }

    void RenderMapQuery::initialize()
    {
        integrator_->requestSamples(sampler_);
    }

    void RenderMapQuery::render(Shape::ShapeVector& shapes)
    {
        IntegratorQuery integratorQuery(scene_, sampler_);

        RayDifferential ray;
        f32 invN = 1.0f/sampler_->getNumSamples();

        Mapper::ConservativeSampler mapperSampler;
        PrimitiveSample primitiveSample;
        for(s32 i=0; i<shapes.size(); ++i){
            for(s32 j=0; j<shapes[i]->getPrimitiveCount(); ++j){
                shapes[i]->getPrimitive(primitiveSample, j);
                if(!mapper_.generateSampler(
                    mapperSampler,
                    image_->getWidth(),
                    image_->getHeight(),
                    primitiveSample.uvs_[0],
                    primitiveSample.uvs_[1],
                    primitiveSample.uvs_[2]))
                {
                    continue;
                }

                MapperSample mapperSample;
                while(mapperSampler.next(mapperSample)){
                    sampler_->generate();

                    ShadingGeometry sg(primitiveSample, mapperSample);
                    Color3 Li = integrator_->E(sg, integratorQuery);
                    //Color3 Li = Color3(mapperSample.b0_, mapperSample.b1_, mapperSample.b2_);
                    //Color3 Li = Color3(lcore::absolute(sg.shadingNormal_.x_), lcore::absolute(sg.shadingNormal_.y_), lcore::absolute(sg.shadingNormal_.z_));
                    image_->set(Li[0], Li[1], Li[2], mapperSample.texcoordu_, mapperSample.texcoordv_);
                }

            }//for(s32 j=0; j<shapes_[i]->getPrimitiveCount()
        }//for(s32 i=0; i<shapes_.size()
    }
}

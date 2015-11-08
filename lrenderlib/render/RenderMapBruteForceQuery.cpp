/**
@file RenderMapBruteForceQuery.cpp
@author t-sakai
@date 2015/10/07 create
*/
#include "RenderMapBruteForceQuery.h"
#include "core/Ray.h"
#include "core/Geometry.h"
#include "sampler/Sampler.h"
#include "scene/Scene.h"
#include "integrator/Integrator.h"
#include "core/ShadingGeometry.h"
#include "image/ImageProcessing.h"

namespace lrender
{
    RenderMapBruteForceQuery::RenderMapBruteForceQuery(const Scene* scene, Sampler* sampler, Image* image, Integrator* integrator)
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

    RenderMapBruteForceQuery::~RenderMapBruteForceQuery()
    {
    }

    void RenderMapBruteForceQuery::initialize()
    {
        integrator_->requestSamples(sampler_);
    }

    void RenderMapBruteForceQuery::render(Shape::ShapeVector& shapes)
    {
        accelerator_.clearShapes();

        for(s32 i=0; i<shapes.size(); ++i){
            shapes[i]->setID(i);
            accelerator_.addShape(shapes[i]);
        }
        accelerator_.build();

        IntegratorQuery integratorQuery(scene_, sampler_);

        RayDifferential ray;
        f32 invN = 1.0f/sampler_->getNumSamples();

        f32 invWidth = 1.0f/image_->getWidth();
        f32 invHeight = 1.0f/image_->getHeight();

        ShadingGeometry shadingGeometry;
        Vector2 texcoord;
        for(s32 y=0; y<image_->getHeight(); ++y){
            for(s32 x=0; x<image_->getWidth(); ++x){
                texcoord.x_ = (0.5f+x)*invWidth;
                texcoord.y_ = (0.5f+y)*invHeight;
                if(!testTexcoordInPrimitive(shadingGeometry, shapes, texcoord)){
                    continue;
                }

                sampler_->generate();
                Color3 Li = integrator_->E(shadingGeometry, integratorQuery);
                //Color3 Li = Color3(mapperSample.b0_, mapperSample.b1_, mapperSample.b2_);
                //Color3 Li = Color3(lcore::absolute(sg.shadingNormal_.x_), lcore::absolute(sg.shadingNormal_.y_), lcore::absolute(sg.shadingNormal_.z_));
                //Color3 Li = Color3(shadingGeometry.uv_.x_, shadingGeometry.uv_.y_, 0.0f);
                image_->set(Li[0], Li[1], Li[2], 1.0f, x, image_->getHeight()-y-1);
            }
        }

        MidBVH2D tmpBVH;
        accelerator_.swap(tmpBVH);

        Image image(image_->getWidth(), image_->getHeight());
        ImageProcessing::dilate(image, *image_);
        image_->swap(image);
    }

    bool RenderMapBruteForceQuery::testTexcoordInPrimitive(ShadingGeometry& shadingGeometry, Shape::ShapeVector& shapes, const Vector2& p)
    {
#if 1
        MidBVH2D::HitRecord hitRecord;

        if(accelerator_.intersect(hitRecord, p)){
            PrimitiveSample primitiveSample;
            hitRecord.shape_->getPrimitive(primitiveSample, hitRecord.primitive_);
            shadingGeometry = ShadingGeometry(primitiveSample, hitRecord.b0_, hitRecord.b1_, hitRecord.b2_);
            return true;
        }
#else
        Vector2 uvs[3];
        f32 b0,b1,b2;
        for(s32 i=0; i<shapes.size(); ++i){
            for(s32 j=0; j<shapes[i]->getPrimitiveCount(); ++j){
                shapes[i]->getTexcoord(uvs, j);

                if(testPointInTriangle(b0, b1, b2, p, uvs[0], uvs[1], uvs[2])){

                    PrimitiveSample primitiveSample;
                    shapes[i]->getPrimitive(primitiveSample, j);
                    shadingGeometry = ShadingGeometry(primitiveSample, b0, b1, b2);
                    return true;
                }

            }//for(s32 j=0; j<shapes_[i]->getPrimitiveCount()
        }//for(s32 i=0; i<shapes_.size()
#endif
        return false;
    }
}

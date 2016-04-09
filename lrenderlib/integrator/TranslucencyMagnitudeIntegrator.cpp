/**
@file TranslucencyMagnitudeIntegrator.cpp
@author t-sakai
@date 2015/12/18 create
*/
#include "TranslucencyMagnitudeIntegrator.h"
#include "core/Ray.h"
#include "core/Intersection.h"
#include "sampler/Sampler.h"
#include "scene/Scene.h"
#include "bsdf/BSDF.h"
#include "bsdf/Microfacet.h"
#include "core/Visibility.h"

#include <render/RenderSurfacePointQuery.h>

namespace lrender
{

    //--------------------------------------------------------------
    //--- TranslucencyMagnitudeIntegrator
    //--------------------------------------------------------------
    TranslucencyMagnitudeIntegrator::TranslucencyMagnitudeIntegrator(
        const Vector3& sigmaS,
        const Vector3& sigmaA,
        const Vector3& g,
        f32 eta,
        s32 russianRouletteDepth,
        s32 maxDepth,
        f32 maxSolidAngle,
        f32 minSampleDistance,
        f32 maxSampleDistance,
        s32 maxSamples)
        :russianRouletteDepth_(russianRouletteDepth)
        ,maxDepth_(maxDepth)
        ,maxSolidAngle_(maxSolidAngle)
        ,minSampleDistance_(minSampleDistance)
        ,maxSampleDistance_(maxSampleDistance)
        ,maxSamples_(maxSamples)
        ,octreeNodeAllocator_(sizeof(PointOctree))
        ,octree_(NULL)
    {
        diffusion_.set(sigmaS, sigmaA, g, eta);
    }

    TranslucencyMagnitudeIntegrator::~TranslucencyMagnitudeIntegrator()
    {
    }

    void TranslucencyMagnitudeIntegrator::set(
            const Vector3& sigmaS,
            const Vector3& sigmaA,
            const Vector3& g,
            f32 eta,
            s32 russianRouletteDepth,
            s32 maxDepth,
            f32 maxSolidAngle,
            f32 minSampleDistance,
            f32 maxSampleDistance,
            s32 maxSamples)
    {
        diffusion_.set(sigmaS, sigmaA, g, eta);

        russianRouletteDepth_ = russianRouletteDepth;
        maxDepth_ = maxDepth;
        maxSolidAngle_ = maxSolidAngle;
        minSampleDistance_ = minSampleDistance;
        maxSampleDistance_ = maxSampleDistance;
        maxSamples_ = maxSamples;
    }

    void TranslucencyMagnitudeIntegrator::requestSamples(Sampler* /*sampler*/)
    {
    }

    void TranslucencyMagnitudeIntegrator::preprocess(
        const Scene& scene,
        s32 numOcclusionSamples,
        f32 occlusionGamma,
        f32 occlusionRayLength)
    {
        numOcclusionSamples = lcore::maximum(numOcclusionSamples, 1);

        const AABB& worldBound = scene.getWorldBound();
        lrender::Vector3 worldCenter = worldBound.center();

        f32 sampleDistance = diffusion_.radius_;

        sampleDistance = lcore::clamp(sampleDistance, minSampleDistance_, maxSampleDistance_);
        lrender::RenderSurfacePointQuery renderSurfacePointQuery(&scene, worldCenter, sampleDistance, maxSamples_);

        renderSurfacePointQuery.initialize();
        renderSurfacePointQuery.render();

        lcore::RandomWELL random(lcore::getDefaultSeed());
        f32 tmax = (occlusionRayLength<0.0f)? scene.getWorldBound().getBSphereRadius() : occlusionRayLength;

        const lrender::RenderSurfacePointQuery::SurfacePointVector& surfacePoints = renderSurfacePointQuery.getSurfacePoints();
        irradiancePoints_.reserve(surfacePoints.size());
        for(s32 i=0; i<surfacePoints.size(); ++i){
            const SurfacePoint& sp = surfacePoints[i];

            Intersection intersection;
            intersection.create(sp.point_, sp.normal_, Vector2(0.0f, 0.0f));

            u32 scramble[2] ={random.rand(), random.rand()};

            
            f32 ao = 0.0f;
            for(s32 j=0; j<numOcclusionSamples; ++j){
                f32 u0, u1;
                lcore::sobol02(u0, u1, j, scramble[0], scramble[1]);

                lrender::Ray ray = intersection.nextHemisphere(u0, u1, tmax);
                if(scene.intersect(intersection, ray)){
                    ao += 1.0f;
                }
            } //for(s32 j=0;
            ao /= numOcclusionSamples;
            ao = 1.0f - lcore::clamp01(lmath::pow(ao, occlusionGamma));

            irradiancePoints_.push_back(IrradiancePoint(sp.point_, sp.normal_, ao, sp.area_));
        } //for(s32 i=0;

        //lcore::ofstream filePoints("points.ply");
        //filePoints.print(
        //    "ply\n"
        //    "format ascii 1.0\n"
        //    "element vertex %d\n"
        //    "property double x\n"
        //    "property double y\n"
        //    "property double z\n"
        //    "property uchar red\n"
        //    "property uchar green\n"
        //    "property uchar blue\n"
        //    "end_header\n\n", surfacePoints.size());

        //for(lrender::s32 i=0; i<irradiancePoints.size(); ++i){
        //    u8 r = static_cast<u8>(lcore::clamp01(irradiancePoints[i].occlusion_)*255);
        //    u8 g = static_cast<u8>(lcore::clamp01(irradiancePoints[i].occlusion_)*255);
        //    u8 b = static_cast<u8>(lcore::clamp01(irradiancePoints[i].occlusion_)*255);
        //    filePoints.print("%f %f %f %d %d %d\n", irradiancePoints[i].point_.x_, irradiancePoints[i].point_.y_, irradiancePoints[i].point_.z_, r, g, b);
        //}
        //filePoints.close();

        octreeNodeAllocator_.clear();
        octree_ = lcore::construct<PointOctree>(octreeNodeAllocator_.allocate());
        
        octreeBound_.setInvalid();
        for(s32 i=0; i<irradiancePoints_.size(); ++i){
            octreeBound_.extend(irradiancePoints_[i].point_);
        }
        for(s32 i=0; i<irradiancePoints_.size(); ++i){
            octree_->insert(octreeBound_, &irradiancePoints_[i], octreeNodeAllocator_);
        }
        octree_->initialize();
    }

    Color3 TranslucencyMagnitudeIntegrator::Li(const Ray& r, IntegratorQuery& query)
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

            if(0.0f<=ray.direction_.dot(insect.geometricNormal_) * LocalCoordinate::cosTheta(wo)){
                break;
            }

            {
                Color3 mo = octree_->Mo(octreeBound_, insect.point_, diffusion_, maxSolidAngle_);

                f32 Ft = 1.0f - Fresnel::dielectricExt( lcore::absolute(wow.dot(insect.shadingNormal_)), diffusion_.eta_);
                L += INV_PI * Ft * mo;
            }

            L += query.sampleEmitterDirect(wow, emitterSample, emitterBsdfSample) * beta;

            Vector3 wi, wiw;
            f32 bsdfPdf;
            BSDF::Type sampledBsdfType = BSDF::Type_All;
            Color3 f = bsdf->sample_f(wow, wi, wiw, bsdfBsdfSample, bsdfPdf, sampledBsdfType, query.intersection_);
            if(f.isZero() || bsdfPdf<=PDF_EPSILON){
                break;
            }
            if(insect.geometricNormal_.dot(wiw)*LocalCoordinate::cosTheta(wi)<=0.0f){
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

    Color4 TranslucencyMagnitudeIntegrator::E(IntegratorQuery& query)
    {
        Intersection& insect = query.intersection_;
        const BSDF::pointer& bsdf = insect.shape_->getBSDF();

        Color3 mo = octree_->Mo(octreeBound_, insect.point_, diffusion_, maxSolidAngle_);
        mo *= INV_PI;
        return Color4(mo[0], mo[1], mo[2], 1.0f);
    }
}

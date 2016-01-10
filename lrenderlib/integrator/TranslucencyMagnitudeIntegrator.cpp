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
    void TranslucencyMagnitudeIntegrator::DiffusionReflectance::set(f32 internalIOR, f32 externalIOR, const Vector3& diffuseMeanFreePath)
    {
        Vector3 ld;
        ld.x_ = lcore::maximum(diffuseMeanFreePath.x_, 1.0e-5f);
        ld.y_ = lcore::maximum(diffuseMeanFreePath.y_, 1.0e-5f);
        ld.z_ = lcore::maximum(diffuseMeanFreePath.z_, 1.0e-5f);
        sigmaTr_ = Vector3(1.0f/ld.x_, 1.0f/ld.y_, 1.0f/ld.z_);

        eta_ = internalIOR/externalIOR;
        Fdr_ = lrender::fresnelDiffuseReflectance(eta_);
        Fdt_ = 1.0f - Fdr_;
        A_ = (1.0f+Fdr_)/Fdt_;

        alphaPrimeTable_.create<ClacDiffuseReflectance>(128, ClacDiffuseReflectance(A_), 0.0f, 1.0f, 1.0e-5f);
    }

    void TranslucencyMagnitudeIntegrator::DiffusionReflectance::setVertexParameter(
        const Vector3& position,
        const Color3& albedo,
        const Vector3& diffuseMeanFreePath)
    {
        position_ = position;
        mo_.zero();

        Vector3 rd;
        rd.x_ = lcore::minimum(albedo[0], 0.999f);
        rd.y_ = lcore::minimum(albedo[1], 0.999f);
        rd.z_ = lcore::minimum(albedo[2], 0.999f);

        rd.set(0.77f, 0.62f, 0.21f);

        Vector3 alphaPrime;
        alphaPrime.x_ = alphaPrimeTable_.interpolateLinear(rd.x_);
        alphaPrime.y_ = alphaPrimeTable_.interpolateLinear(rd.y_);
        alphaPrime.z_ = alphaPrimeTable_.interpolateLinear(rd.z_);

        for(s32 i=0; i<3; ++i){
            meanFreePath_[i] = diffuseMeanFreePath[i] * lmath::sqrt(3.0f*(1.0f-alphaPrime[i]));

            f32 sigmaTPrime = 1.0f/meanFreePath_[i];
            f32 sigmaSPrime = alphaPrime[i] * sigmaTPrime;
            f32 sigmaA = sigmaTPrime - sigmaSPrime;

            sigmaTrPrime_[i] = lmath::sqrt(3.0f*sigmaA*sigmaTPrime);
        }

        zr_ = meanFreePath_;
        zv_.mul(1.0f+4.0f/3.0f*A_, zr_);
    }

    Color3 TranslucencyMagnitudeIntegrator::DiffusionReflectance::operator()(f32 sqrDistance) const
    {
        //f32 sqrDmpf = meanFreePath_.lengthSqr();
        //sqrDistance = (sqrDistance<sqrDmpf)? sqrDmpf : sqrDistance;

        Color3 dr, dv, idr, idv;
        for(s32 i=0; i<3; ++i){
            dr[i] = lmath::sqrt(sqrDistance + zr_[i]*zr_[i]);
            dv[i] = lmath::sqrt(sqrDistance + zv_[i]*zv_[i]);

            idr[i] = 1.0f/dr[i];
            idv[i] = 1.0f/dv[i];
        }

        Color3 Rd;
        for(s32 i=0; i<3; ++i){
            f32 c0 = zr_[i] * (sigmaTrPrime_[i] + idr[i]) * lmath::exp(-sigmaTrPrime_[i] * dr[i]) * (idr[i] * idr[i]);
            f32 c1 = zv_[i] * (sigmaTrPrime_[i] + idv[i]) * lmath::exp(-sigmaTrPrime_[i] * dv[i]) * (idv[i] * idv[i]);
            Rd[i] = lcore::maximum(1.0f/(4.0f*PI)*(c0+c1), 0.0f);
        }
        return Rd;
    }

    //--------------------------------------------------------------
    //--- DipoleIntegrator::PointOctree
    //--------------------------------------------------------------
    const f32 TranslucencyMagnitudeIntegrator::PointOctree::Epsilon = 1.0e-6f;

    TranslucencyMagnitudeIntegrator::PointOctree::PointOctree()
    {
        clear();
    }

    TranslucencyMagnitudeIntegrator::PointOctree::PointOctree(IrradiancePointVector& values)
    {
        clear();
        values_.swap(values);
        build();
        initialize();
    }

    TranslucencyMagnitudeIntegrator::PointOctree::~PointOctree()
    {
    }

    const TranslucencyMagnitudeIntegrator::IrradiancePointVector&
        TranslucencyMagnitudeIntegrator::PointOctree::getValues() const
    {
        return values_;
    }

    void TranslucencyMagnitudeIntegrator::PointOctree::clear()
    {
        nodes_.clear();
        Node root;
        root.num_ = 0;
        root.offset_ = 0;
        nodes_.push_back(root);
    }

    void TranslucencyMagnitudeIntegrator::PointOctree::swap(PointOctree& rhs)
    {
        lcore::swap(bbox_, rhs.bbox_);
        values_.swap(rhs.values_);
        nodes_.swap(rhs.nodes_);
    }

    void TranslucencyMagnitudeIntegrator::PointOctree::initialize()
    {
        initialize(0);
    }

    Color3 TranslucencyMagnitudeIntegrator::PointOctree::Mo(const Vector3& point, const DiffusionReflectance& diffusion, f32 maxSolidAngle) const
    {
        return Mo(0, bbox_, point, diffusion, maxSolidAngle);
    }

    void TranslucencyMagnitudeIntegrator::PointOctree::build()
    {
        if(128 < values_.size()){
            nodes_.reserve(values_.size() >> 1);
            nodes_.setIncSize(values_.size() >> 2);
        } else{
            nodes_.setIncSize(128);
        }

        bbox_.setInvalid();
        for(s32 i=0; i<values_.size(); ++i){
            bbox_.extend(values_[i].getPosition());
        }
        Node& root = nodes_[0];
        root.offset_ = 0;
        root.setNum(values_.size());
        split(0);
    }

    void TranslucencyMagnitudeIntegrator::PointOctree::initialize(s32 nodeIndex)
    {
        Node& node = nodes_[nodeIndex];
        node.point_.zero();
        node.occlusion_ = 0.0f;
        node.area_ = 0.0f;

        f32 sumWeight = 0.0f;

        if(node.isLeaf()){
            s32 start = node.getOffset();
            s32 num = node.getNum();
            s32 end = start + num;
            for(s32 i=start; i<end; ++i){
                const IrradiancePoint& sp = values_[i];
                node.occlusion_ += sp.occlusion_ * sp.area_;
                node.area_ += sp.area_;

                f32 weight = sp.occlusion_ * sp.area_;
                node.point_.muladd(weight, sp.point_, node.point_);
                sumWeight += weight;
            }

        } else{
            for(s32 i=0; i<8; ++i){
                if(0 == node.children_[i]){
                    continue;
                }

                initialize(node.children_[i]);
                Node& childNode = nodes_[node.children_[i]];

                node.occlusion_ += childNode.occlusion_ * childNode.area_;
                node.area_ += childNode.area_;

                f32 weight = childNode.occlusion_ * childNode.area_;
                node.point_.muladd(weight, childNode.point_, node.point_);

                sumWeight += weight;
            }
        }

        if(PDF_EPSILON<sumWeight){
            node.point_ /= sumWeight;
        }
        if(PDF_EPSILON<node.area_){
            node.occlusion_ /= node.area_;
        }
    }

    void TranslucencyMagnitudeIntegrator::PointOctree::split(s32 nodeIndex)
    {
        Node& node = nodes_[nodeIndex];
        node.clear();

        s32 num = node.getNum();
        s32 offset = node.offset_;

        s32 end = offset + num;
        AABB bbox;
        bbox.setInvalid();
        for(s32 i=offset; i<end; ++i){
            bbox.extend(values_[i].getPosition());
        }

        Vector3 extent = bbox.extent();
        bool degenerate = (extent.x_ < Epsilon && extent.y_ < Epsilon && extent.z_ < Epsilon);
        bbox.expand(Epsilon);

        if(num <= MaxNumNodePoints || degenerate){
            node.setLeaf(true);
            return;
        }
        node.setLeaf(false);
        node.setNum(0);

        s32 numPoints[8];
        s32 offsets[8];
        Vector3 pivot = bbox.center();
        value_type* values = &values_[0];

        offsets[0] = offset;
        split(numPoints[0], numPoints[4], offsets[4], offsets[0], num, values, 0, pivot);
        split(numPoints[0], numPoints[2], offsets[2], offsets[0], numPoints[0], values, 1, pivot);
        split(numPoints[0], numPoints[1], offsets[1], offsets[0], numPoints[0], values, 2, pivot);
        split(numPoints[2], numPoints[3], offsets[3], offsets[2], numPoints[2], values, 2, pivot);

        split(numPoints[4], numPoints[6], offsets[6], offsets[4], numPoints[4], values, 1, pivot);
        split(numPoints[4], numPoints[5], offsets[5], offsets[4], numPoints[4], values, 2, pivot);
        split(numPoints[6], numPoints[7], offsets[7], offsets[6], numPoints[6], values, 2, pivot);

        for(s32 i = 0; i < 8; ++i){
            if(numPoints[i] <= 0){
                continue;
            }
            Node child;
            child.setOffset(offsets[i]);
            child.setNum(numPoints[i]);

            nodes_[nodeIndex].children_[i] = nodes_.size();
            nodes_.push_back(child);
        }
        for(s32 i=0; i<8; ++i){
            if(numPoints[i]<=0){
                continue;
            }
            split(nodes_[nodeIndex].children_[i]);
        }
    }

    void TranslucencyMagnitudeIntegrator::PointOctree::split(
        s32& numLeft,
        s32& numRight,
        s32& offsetRight,
        s32 offset,
        s32 num,
        value_type* values,
        s32 axis,
        const Vector3& pivot)
    {
        values += offset;
        s32 i0 = 0;
        s32 i1 = num - 1;

        for(;;){
            while(i0 < num && values[i0].getPosition()[axis] < pivot[axis]){
                ++i0;
            }

            while(0 <= i1 && pivot[axis] < values[i1].getPosition()[axis]){
                --i1;
            }

            if(i1 <= i0){
                break;
            }
            lcore::swap(values[i0], values[i1]);
            ++i0;
            --i1;
        }

        numLeft = lcore::minimum(i0, num);
        numRight = num - numLeft;
        offsetRight = offset + numLeft;
    }

    Color3 TranslucencyMagnitudeIntegrator::PointOctree::Mo(s32 nodeIndex, const AABB& bbox, const Vector3& point, const DiffusionReflectance& diffusion, f32 maxSolidAngle) const
    {
        const Node& node = nodes_[nodeIndex];

        f32 sqrDistance = point.distanceSqr(node.point_);
        f32 solidAngle = node.area_ / sqrDistance;

        if(!bbox.contains(point) && solidAngle<maxSolidAngle){
            return diffusion(sqrDistance) * node.occlusion_ * node.area_;
        }

        Color3 mo = Color3::black();
        if(node.isLeaf()){
            s32 start = node.offset_;
            s32 end = start + node.getNum();
            for(s32 i=start; i<end; ++i){
                mo += diffusion(point.distanceSqr(values_[i].point_)) * values_[i].occlusion_ * values_[i].area_;
            }

        } else{
            Vector3 pivot = bbox.center();
            for(s32 i=0; i<8; ++i){
                if(0 == node.children_[i]){
                    continue;
                }
                AABB childBBox = octreeChildBound(i, pivot, bbox);
                mo += Mo(node.children_[i], childBBox, point, diffusion, maxSolidAngle);
            }
        }
        return mo;
    }

    TranslucencyMagnitudeIntegrator::TranslucencyMagnitudeIntegrator(
        const Vector3& diffuseMeanFreePath,
        const Vector3& g,
        s32 russianRouletteDepth,
        s32 maxDepth,
        f32 maxSolidAngle,
        f32 minSampleDistance,
        f32 maxSampleDistance,
        s32 maxSamples)
        :diffuseMeanFreePath_(diffuseMeanFreePath)
        ,g_(g)
        ,russianRouletteDepth_(russianRouletteDepth)
        ,maxDepth_(maxDepth)
        ,maxSolidAngle_(maxSolidAngle)
        ,minSampleDistance_(minSampleDistance)
        ,maxSampleDistance_(maxSampleDistance)
        ,maxSamples_(maxSamples)
    {
    }

    TranslucencyMagnitudeIntegrator::~TranslucencyMagnitudeIntegrator()
    {
    }

    void TranslucencyMagnitudeIntegrator::set(
            const Vector3& diffuseMeanFreePath,
            const Vector3& g,
            s32 russianRouletteDepth,
            s32 maxDepth,
            f32 maxSolidAngle,
            f32 minSampleDistance,
            f32 maxSampleDistance,
            s32 maxSamples)
    {
        diffuseMeanFreePath_ = diffuseMeanFreePath;
        g_ = g;
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
        f32 internalIOR,
        f32 externalIOR,
        s32 numOcclusionSamples,
        f32 occlusionGamma,
        f32 occlusionRayLength)
    {
        numOcclusionSamples = lcore::maximum(numOcclusionSamples, 1);

        diffusion_.set(internalIOR, externalIOR, diffuseMeanFreePath_);

        const AABB& worldBound = scene.getWorldBound();
        lrender::Vector3 worldCenter = worldBound.center();

        f32 sampleDistance = lcore::numeric_limits<f32>::maximum();
        for(s32 i=0; i<3; ++i){
            //sampleDistance = lcore::minimum(sampleDistance, diffuseMeanFreePath_[i] * 0.70710678118f);
            sampleDistance = lcore::minimum(sampleDistance, diffuseMeanFreePath_[i]);
        }
        sampleDistance = lcore::clamp(sampleDistance, minSampleDistance_, maxSampleDistance_);
        lrender::RenderSurfacePointQuery renderSurfacePointQuery(&scene, worldCenter, sampleDistance, maxSamples_);

        renderSurfacePointQuery.initialize();
        renderSurfacePointQuery.render();

        lcore::RandomWELL random(lcore::getDefaultSeed());
        f32 tmax = (occlusionRayLength<0.0f)? scene.getWorldBound().getBSphereRadius() : occlusionRayLength;

        const lrender::RenderSurfacePointQuery::SurfacePointVector& surfacePoints = renderSurfacePointQuery.getSurfacePoints();
        IrradiancePointVector irradiancePoints;
        irradiancePoints.reserve(surfacePoints.size());
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


            irradiancePoints.push_back(IrradiancePoint(sp.point_, sp.normal_, ao, sp.area_));
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

        PointOctree ocree(irradiancePoints);

        octree_.swap(ocree);
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
                diffusion_.setVertexParameter(insect.point_, bsdf->albedo(insect), diffuseMeanFreePath_);
                Color3 mo = octree_.Mo(insect.point_, diffusion_, maxSolidAngle_);

                //Fresnel fresnel(1.0f, diffusion_.eta_);
                //Color3 Ft = Color3::white() - fresnel.evaluate(lcore::absolute(wow.dot(insect.shadingNormal_)));
                f32 Fdt = 1.0f - diffusion_.Fdr_;

                //L += (INV_PI * Ft * Fdt * mo);
                L += Fdt * mo;
                //L += mo;
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
        diffusion_.setVertexParameter(insect.point_, bsdf->albedo(insect), diffuseMeanFreePath_);

        Color3 mo = octree_.Mo(insect.point_, diffusion_, maxSolidAngle_);
        f32 Fdt = 1.0f - diffusion_.Fdr_;
        mo *= Fdt;
        return Color4(mo[0], mo[1], mo[2], 1.0f);
    }
}

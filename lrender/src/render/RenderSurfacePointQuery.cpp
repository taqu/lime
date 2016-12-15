/**
@file RenderSurfacePointQuery.cpp
@author t-sakai
@date 2015/12/01 create
*/
#include "render/RenderSurfacePointQuery.h"
#include <lcore/CPU.h>
#include <lcore/SyncObject.h>
#include <lcore/Thread.h>
#include <lmath/geometry/Sphere.h>

#include "scene/Scene.h"
#include "sampler/Sampler.h"
#include "integrator/Integrator.h"
#include "core/DynamicPointOctree.h"

namespace lrender
{
namespace
{
    typedef DynamicPointOctree<SurfacePoint> SurfacePointOctree;

    bool testRaySphereBack(Intersection& intersection, const Ray& ray, const lmath::Sphere& sphere)
    {
        f32 t;
        if(!testRaySphere(t, ray, sphere)){
            return false;
        }

        Vector3 center = sphere.position();
        intersection.point_ = muladd(t, ray.direction_, ray.origin_);

        f32 l = distanceSqr(center, intersection.point_);
        if(l<RAY_EPSILON){
            return true;
        }
        intersection.geometricNormal_ = normalize(center - intersection.point_, l);
        intersection.shadingNormal_ = intersection.geometricNormal_;
        return true;
    }

    struct JobStatistics
    {
        void incRepeatedFails()
        {
            ++repeatedFails_;
            maxRepeatedFails_ = lcore::maximum(repeatedFails_, maxRepeatedFails_);
        }

        s32 repeatedFails_;
        s32 maxRepeatedFails_;
        s32 totalPathsTraced_;
        s32 totalRaysTraced_;
        s32 numPointsAdded_;
    };

    struct JobConstants
    {
        lmath::Sphere bsphere_;
        s32 maxSamples_;
        s32 maxRaySamples_;
        s32 maxFails_;
        s32 maxPathsTraced_;
        Vector3 origin_;
        f32 minimumSampleDistance_;
    };

    struct JobContext
    {
        s32 jobIndex_;
        lcore::ReadersWriterLock* rwlock_;
        const Scene* scene_;
        const JobConstants* constants_;
        DynamicPointOctree<SurfacePoint>* octree_;
        lcore::Array<SurfacePoint>* surfacePoints_;
        JobStatistics* statistics_;
    };

    class PoissonCheckQuery : public SurfacePointOctree::QueryBase
    {
    public:
        PoissonCheckQuery(f32 maxDistance, const Vector3& point)
            :sqrMaxDistance_(maxDistance*maxDistance)
            ,point_(point)
            ,result_(false)
        {
        }

        virtual bool operator()(const SurfacePoint& value)
        {
            if(distanceSqr(value.point_, point_)<sqrMaxDistance_){
                result_ = true;
                return false;
            }
            return true;
        }

        bool getResult() const
        {
            return result_;
        }
    private:
        f32 sqrMaxDistance_;
        Vector3 point_;
        bool result_;
    };

    void job_proc(u32 threadId, void* data);
}

    RenderSurfacePointQuery::RenderSurfacePointQuery(const Scene* scene, const Vector3& origin, f32 minimumSampleDistance, s32 maxSamples)
        :scene_(scene)
        ,origin_(origin)
        ,minimumSampleDistance_(minimumSampleDistance)
        ,maxSamples_(maxSamples)
    {
    }

    RenderSurfacePointQuery::~RenderSurfacePointQuery()
    {
    }

    void RenderSurfacePointQuery::initialize()
    {
    }

    void RenderSurfacePointQuery::render()
    {
        surfacePoints_.clear();

        static const u32 MaxCPUs = 32;
        s32 numThreads = lcore::clamp(lcore::getLogicalCPUCount(), 1U, MaxCPUs);

        lcore::ThreadRaw threads[MaxCPUs];

        AABB worldBound = scene_->getWorldBound();
        worldBound.expand(minimumSampleDistance_);

        lmath::Sphere bsphere;
        worldBound.getBSphere(bsphere);
        SurfacePointOctree octree(worldBound);

        JobConstants jobConstants;

        jobConstants.maxSamples_ = maxSamples_;
        jobConstants.maxRaySamples_ = 20000;
        jobConstants.maxFails_ = 2000;
        jobConstants.maxPathsTraced_ = 50000;

        jobConstants.bsphere_ = bsphere;
        jobConstants.minimumSampleDistance_ = minimumSampleDistance_;
        jobConstants.origin_ = origin_;

        JobStatistics jobStatistics;
        jobStatistics.repeatedFails_ = 0;
        jobStatistics.maxRepeatedFails_ = 0;
        jobStatistics.totalPathsTraced_ = 0;
        jobStatistics.totalRaysTraced_ = 0;
        jobStatistics.numPointsAdded_ = 0;

        lcore::MultipleWait multiWait;
        lcore::ReadersWriterLock* RWLock = LNEW lcore::ReadersWriterLock();
        JobContext jobContext[MaxCPUs];
        for(s32 i=0; i<numThreads; ++i){
            jobContext[i].jobIndex_ = i;
            jobContext[i].rwlock_ = RWLock;
            jobContext[i].scene_ = scene_;
            jobContext[i].constants_ = &jobConstants;
            jobContext[i].octree_ = &octree;
            jobContext[i].surfacePoints_ = &surfacePoints_;
            jobContext[i].statistics_ = &jobStatistics;

            threads[i].create(job_proc, &jobContext[i], false);
            multiWait.set(i, &threads[i]);
        }
        for(;;){
            u32 status = multiWait.join(numThreads, 1000);
            if(lcore::MultipleWait::WaitStatus::Wait_Timeout != status){
                break;
            }
        }
        LDELETE(RWLock);
    }

namespace
{
    void job_proc(u32 threadId, void* data)
    {
        lcore::RandWELL random(lcore::getTime()*(threadId+1));
        JobContext& context = *(JobContext*)data;
        const JobConstants& constants = *(context.constants_);
        JobStatistics& statistics = *(context.statistics_);

        f32 area = PI * (constants.minimumSampleDistance_*0.5f) * (constants.minimumSampleDistance_*0.5f);
        RenderSurfacePointQuery::SurfacePointVector candidates;
        lcore::Array<bool> candidateRejected;

        Intersection intersection;
        for(;;){
            candidates.clear();
            candidateRejected.clear();

            s32 raysTraced = 0;
            s32 pathsTraced;
            for(pathsTraced=0; pathsTraced<constants.maxRaySamples_; ++pathsTraced){
                Vector3 dir;
                lmath::randomOnSphere(dir, random.frand(), random.frand());
                Ray ray(constants.origin_, dir, 0.0f, LRENDER_INFINITY, RAY_EPSILON);
                s32 depth = 0;
                while(depth<30){
                    ++raysTraced;
                    bool hitOnSphere = false;
                    if(context.scene_->intersect(intersection, ray)){
                        intersection.geometricNormal_ = faceForward(intersection.geometricNormal_, -ray.direction_);
                    }else{
                        if(!testRaySphereBack(intersection, ray, constants.bsphere_)){
                            break;
                        }
                        hitOnSphere = true;
                    }

                    if(!hitOnSphere && 3<=depth){
                        candidates.push_back(SurfacePoint(intersection.point_, intersection.geometricNormal_, area));
                    }

                    lmath::randomOnSphere(dir, random.frand(), random.frand());
                    dir = faceForward(dir, intersection.geometricNormal_);
                    ++depth;
                    ray = Ray(intersection.point_, dir, RAY_EPSILON, LRENDER_INFINITY, RAY_EPSILON);
                } //while(depth<30)
            } //for(pathsTraced=0;

            candidateRejected.reserve(candidates.size());
            context.rwlock_->enterReader();
            for(s32 i=0; i<candidates.size(); ++i){
                PoissonCheckQuery query(constants.minimumSampleDistance_, candidates[i].point_);
                context.octree_->traverse(candidates[i].point_, query);
                candidateRejected.push_back(query.getResult());
            }
            context.rwlock_->leaveReader();

            lcore::ScopedWriterLock writerLock(*context.rwlock_);
            if(constants.maxFails_<=statistics.repeatedFails_){
                return;
            }
            statistics.totalPathsTraced_ += pathsTraced;
            statistics.totalRaysTraced_ += raysTraced;
            s32 oldMaxRepeatedFails = statistics.maxRepeatedFails_;
            for(s32 i=0; i<candidates.size(); ++i){
                if(candidateRejected[i]){
                    statistics.incRepeatedFails();
                    if(constants.maxFails_<=statistics.repeatedFails_){
                        return;
                    }

                }else{
                    SurfacePoint& surfacePoint = candidates[i];
                    PoissonCheckQuery query(constants.minimumSampleDistance_, surfacePoint.point_);
                    context.octree_->traverse(surfacePoint.point_, query);
                    if(query.getResult()){
                        statistics.incRepeatedFails();
                        if(constants.maxFails_ <= statistics.repeatedFails_){
                            return;
                        }

                    }else if(context.octree_->add(surfacePoint)){
                        ++statistics.numPointsAdded_;
                        statistics.repeatedFails_ = 0;
                        context.surfacePoints_->push_back(surfacePoint);
                    }
                } //if(candidateRejected[i])
            } //for(s32 i=0;

            if(constants.maxPathsTraced_<=statistics.totalPathsTraced_
                && statistics.numPointsAdded_<=0)
            {
                return;
            }
            if(constants.maxSamples_<=statistics.numPointsAdded_){
                return;
            }
        } //for(;;){
    }
}
}

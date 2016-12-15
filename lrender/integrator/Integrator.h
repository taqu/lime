#ifndef INC_LRENDER_INTEGRATOR_H__
#define INC_LRENDER_INTEGRATOR_H__
/**
@file Integrator.h
@author t-sakai
@date 2015/09/16 create
*/
#include "../lrender.h"
#include "../core/Spectrum.h"
#include "../core/Intersection.h"
#include "../sampler/Sampler.h"

namespace lrender
{
    class Scene;
    class Sampler;
    class Ray;
    class Emitter;
    struct EmitterSample;
    class BSDF;
    struct BSDFSample;


    class IntegratorQuery
    {
    public:
        inline IntegratorQuery(const Scene* scene, Sampler* sampler);
        inline ~IntegratorQuery();

        void clear();

        bool intersect(const Ray& ray);

        Color3 sampleEmitterDirect(
            const Vector3& wow,
            const EmitterSample& emitterSample,
            const BSDFSample& bsdfSample);

        Color3 estimateDirect(
            const Emitter& emitter,
            const Vector3& p,
            const Vector3& ns,
            const Vector3& ng,
            const Vector3& wow,
            const EmitterSample& emitterSample,
            const BSDFSample& bsdfSample,
            s32 bsdfType) const;

        inline f32 get1D();
        inline Sample2D get2D();

        inline f32 next1D();
        inline Sample2D next2D();

        const Scene* scene_;
        Sampler* sampler_;
        Intersection intersection_;
        s32 depth_;
    };

    inline IntegratorQuery::IntegratorQuery(const Scene* scene, Sampler* sampler)
        :scene_(scene)
        ,sampler_(sampler)
        ,depth_(0)
    {
        LASSERT(NULL != scene_);
        LASSERT(NULL != sampler_);
    }

    inline IntegratorQuery::~IntegratorQuery()
    {
    }

    inline f32 IntegratorQuery::get1D()
    {
        return sampler_->get1D();
    }

    inline Sample2D IntegratorQuery::get2D()
    {
        return sampler_->get2D();
    }

    inline f32 IntegratorQuery::next1D()
    {
        return sampler_->next1D();
    }

    inline Sample2D IntegratorQuery::next2D()
    {
        return sampler_->next2D();
    }


    class Integrator
    {
    public:
        virtual ~Integrator()
        {}

        virtual void requestSamples(Sampler* sampler) =0;
        virtual Color3 Li(const Ray& ray, IntegratorQuery& query) =0;
        virtual Color4 E(IntegratorQuery& query) =0;
    protected:
        Integrator()
        {}

        Color3 specularReflectDirect(const Ray& ray, const Intersection& intersection, IntegratorQuery& query);
    };
}
#endif //INC_LRENDER_INTEGRATOR_H__

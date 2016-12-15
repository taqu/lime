#ifndef INC_LRENDER_PATHTRACER_H__
#define INC_LRENDER_PATHTRACER_H__
/**
@file PathTracer.h
@author t-sakai
@date 2015/09/16 create
*/
#include "Integrator.h"

namespace lrender
{
    class PathTracer : public Integrator
    {
    public:
        PathTracer(s32 russianRouletteDepth, s32 maxDepth=-1);
        virtual ~PathTracer();

        virtual void requestSamples(Sampler* sampler);

        virtual Color3 Li(const Ray& ray, IntegratorQuery& query);
        virtual Color4 E(IntegratorQuery& query);
    protected:
        PathTracer(const PathTracer&);
        PathTracer& operator=(const PathTracer&);

        s32 russianRouletteDepth_;
        s32 maxDepth_;
        bool strictNormal_;
    };
}
#endif //INC_LRENDER_PATHTRACER_H__

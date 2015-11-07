#ifndef INC_LRENDER_MAPPATHTRACER_H__
#define INC_LRENDER_MAPPATHTRACER_H__
/**
@file MapPathTracer.h
@author t-sakai
@date 2015/10/07 create
*/
#include "PathTracer.h"

namespace lrender
{
    class MapPathTracer : public PathTracer
    {
    public:
        MapPathTracer(s32 russianRouletteDepth=5, s32 maxDepth=-1);
        virtual ~MapPathTracer();

        virtual void requestSamples(Sampler* sampler);

        //virtual Color3 Li(const Ray& ray, IntegratorQuery& query);
        virtual Color3 E(const ShadingGeometry& shadingGeometry, IntegratorQuery& query);
    protected:
        MapPathTracer(const MapPathTracer&);
        MapPathTracer& operator=(const MapPathTracer&);
    };
}
#endif //INC_LRENDER_MAPPATHTRACER_H__

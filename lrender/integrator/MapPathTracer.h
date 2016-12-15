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
        MapPathTracer(bool handleIndirect, s32 russianRouletteDepth=5, s32 maxDepth=-1);
        virtual ~MapPathTracer();

        virtual void requestSamples(Sampler* sampler);

        virtual Color4 E(IntegratorQuery& query);
    protected:
        MapPathTracer(const MapPathTracer&);
        MapPathTracer& operator=(const MapPathTracer&);
        bool handleIndirect_;
    };
}
#endif //INC_LRENDER_MAPPATHTRACER_H__

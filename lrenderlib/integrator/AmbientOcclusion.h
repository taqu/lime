#ifndef INC_LRENDER_AMBIENTOCCLUSION_H__
#define INC_LRENDER_AMBIENTOCCLUSION_H__
/**
@file AmbientOcclusion.h
@author t-sakai
@date 2015/10/06 create
*/
#include "Integrator.h"

namespace lrender
{
    class AmbientOcclusion : public Integrator
    {
    public:
        AmbientOcclusion(s32 numSamples, f32 rayLength=-1.0f);
        virtual ~AmbientOcclusion();

        virtual void requestSamples(Sampler* sampler);

        virtual Color3 Li(const Ray& ray, IntegratorQuery& query);
        virtual Color3 E(const ShadingGeometry& shadingGeometry, IntegratorQuery& query);
    protected:
        AmbientOcclusion(const AmbientOcclusion&);
        AmbientOcclusion& operator=(const AmbientOcclusion&);

        s32 numSamples_;
        f32 rayLength_;
    };
}
#endif //INC_LRENDER_AMBIENTOCCLUSION_H__

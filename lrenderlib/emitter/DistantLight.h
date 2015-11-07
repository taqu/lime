#ifndef INC_LRENDER_DISTANTLIGHT_H__
#define INC_LRENDER_DISTANTLIGHT_H__
/**
@file DistantLight.h
@author t-sakai
@date 2015/09/24 create
*/
#include "Emitter.h"
#include "core/Spectrum.h"

namespace lrender
{
    class DistantLight : public Emitter
    {
    public:
        DistantLight(const Vector3& direction, const Color3& radiance);
        virtual ~DistantLight();

        virtual Color3 sample_L(
            const Vector3& p,
            const Vector3& ng,
            f32 epsilon,
            const EmitterSample& sample,
            Vector3& wiw,
            f32& pdf,
            Visibility& visibility) const;

        virtual Color3 sample_L(
            const Scene& scene,
            f32 u0, f32 u1,
            Ray& ray,
            Vector3& Ns,
            f32& pdf) const;

        virtual Color3 sample_L(
            const Vector3& p,
            const Vector3& n,
            f32 epsilon,
            const EmitterSample& sample,
            Vector3& wi,
            f32& pdf) const;

        virtual Color3 power(const Scene& scene) const;
        virtual bool isDeltaLight() const;

    protected:
        DistantLight(const DistantLight&);
        DistantLight& operator=(const DistantLight&);

        Vector3 direction_;
        Color3 radiance_;
    };
}

#endif //INC_LRENDER_DISTANTLIGHT_H__

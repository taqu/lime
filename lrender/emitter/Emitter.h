#ifndef INC_LRENDER_EMITTER_H__
#define INC_LRENDER_EMITTER_H__
/**
@file Emitter.h
@author t-sakai
@date 2015/09/24 create
*/
#include "../lrender.h"
#include <lcore/Array.h>
#include <lmath/Matrix44.h>

namespace lrender
{
    struct EmitterSample;
    class Visibility;
    class Scene;
    class Ray;

    class Emitter : public ReferenceCounted
    {
    public:
        typedef lcore::intrusive_ptr<Emitter> pointer;
        typedef lcore::Array<Emitter::pointer> EmitterVector;

        virtual ~Emitter();

        inline s32 getNumSamples() const;

        virtual Color3 sample_L(
            const Vector3& p,
            const Vector3& ng,
            const Vector3& ns,
            f32 tmax,
            const EmitterSample& sample,
            Vector3& wi,
            f32& pdf,
            Visibility& visibility) const=0;

        virtual Color3 sample_L(
            const Scene& scene,
            f32 u0, f32 u1,
            Ray& ray,
            Vector3& Ns,
            f32& pdf) const=0;

        virtual Color3 sample_L(
            const Vector3& p,
            const Vector3& n,
            f32 tmax,
            const EmitterSample& sample,
            Vector3& wi,
            f32& pdf) const=0;

        virtual Color3 power(const Scene& scene) const=0;
        virtual bool isDeltaLight() const=0;
    protected:
        Emitter(const Emitter&);
        Emitter& operator=(const Emitter&);

        Emitter();
        Emitter(s32 numSamples, const lmath::Matrix44& localToWorld);

        s32 numSamples_;
        lmath::Matrix44 localToWorld_;
        lmath::Matrix44 worldToLocal_;
    };

    inline s32 Emitter::getNumSamples() const
    {
        return numSamples_;
    }
}

#endif //INC_LRENDER_EMITTER_H__

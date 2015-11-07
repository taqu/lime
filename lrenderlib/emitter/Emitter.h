#ifndef INC_LRENDER_EMITTER_H__
#define INC_LRENDER_EMITTER_H__
/**
@file Emitter.h
@author t-sakai
@date 2015/09/24 create
*/
#include "../lrender.h"
#include <lcore/Vector.h>

namespace lrender
{
    class EmitterSample;
    class Visibility;
    class Scene;
    class Ray;

    class Emitter : public ReferenceCounted
    {
    public:
        typedef lcore::intrusive_ptr<Emitter> pointer;
        typedef lcore::vector_arena<Emitter::pointer> EmitterVector;

        virtual ~Emitter();

        virtual Color3 sample_L(
            const Vector3& p,
            const Vector3& n,
            f32 epsilon,
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
            f32 epsilon,
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
}

#endif //INC_LRENDER_EMITTER_H__
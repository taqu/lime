#ifndef INC_LRENDER_SAMPLER_H__
#define INC_LRENDER_SAMPLER_H__
/**
@file Sampler.h
@author t-sakai
@date 2013/05/24 create
*/
#include <lcore/Random.h>
#include <lcore/intrusive_ptr.h>
#include <lcore/Array.h>

#include "../lrender.h"

namespace lrender
{
    //----------------------------------
    //---
    //--- Sampler
    //---
    //----------------------------------
    class Sampler : public ReferenceCounted
    {
    public:
        typedef lcore::intrusive_ptr<Sampler> pointer;

        virtual ~Sampler();

        inline f32 get1D();
        inline Sample2D get2D();
        inline u32 get1DU();

        virtual f32 next1D() =0;
        virtual Sample2D next2D() =0;
        virtual void advance() =0;
        virtual void generate() =0;
        virtual Sampler* clone() =0;

        inline s32 getNumSamples() const;

        f32* next1DArray(s32 size);
        Sample2D* next2DArray(s32 size);

        virtual void request1DArray(s32 size);
        virtual void request2DArray(s32 size);
    protected:
        Sampler(const Sampler&);
        Sampler& operator=(const Sampler&);

        explicit Sampler(u32 seed);
        Sampler(s32 numSamples, u32 seed);

        lcore::RandWELL random_;

        s32 numSamples_;
        s32 index_;

        s32 indexArray1D_;
        s32 indexArray2D_;
        lcore::Array<s32> req1D_, req2D_;
        lcore::Array<f32*> array1D_;
        lcore::Array<Sample2D*> array2D_;
    };

    inline f32 Sampler::get1D()
    {
        return random_.frand2();
    }

    inline Sample2D Sampler::get2D()
    {
        return Sample2D(random_.frand2(), random_.frand2());
    }

    inline u32 Sampler::get1DU()
    {
        return random_.rand();
    }

    inline s32 Sampler::getNumSamples() const
    {
        return numSamples_;
    }
}
#endif //INC_LRENDER_SAMPLER_H__

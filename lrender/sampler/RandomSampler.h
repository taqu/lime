#ifndef INC_LRENDER_RANDOMSAMPLER_H__
#define INC_LRENDER_RANDOMSAMPLER_H__
/**
@file RandomSampler.h
@author t-sakai
@date 2015/10/28 create
*/
#include "Sampler.h"

namespace lrender
{
    class RandomSampler : public Sampler
    {
    public:
        static const s32 MaxDimensions = 16;

        RandomSampler(s32 numSamples, u32 seed, s32 numDimensions);
        virtual ~RandomSampler();

        virtual f32 next1D();
        virtual Sample2D next2D();
        virtual void advance();
        virtual void generate();
        virtual Sampler* clone();

    private:
        RandomSampler(const RandomSampler&);
        RandomSampler& operator=(const RandomSampler&);

        void generate1D(lcore::RandWELL& random, f32* samples, s32 size);
        void generate1D(lcore::RandWELL& random, f32* samples, s32 numSamples, s32 size);
        void generate2D(lcore::RandWELL& random, Sample2D* samples, s32 size);
        void generate2D(lcore::RandWELL& random, Sample2D* samples, s32 numSamples, s32 size);

        s32 numDimensions_;
        s32 dim1D_;
        s32 dim2D_;
        f32* table1D_[MaxDimensions];
        Sample2D* table2D_[MaxDimensions];
    };
}
#endif //INC_LRENDER_RANDOMSAMPLER_H__

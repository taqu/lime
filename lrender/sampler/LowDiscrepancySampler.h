#ifndef INC_LRENDER_LOWDISCREPANCYSAMPLER_H__
#define INC_LRENDER_LOWDISCREPANCYSAMPLER_H__
/**
@file LowDiscrepancySampler.h
@author t-sakai
@date 2015/09/22 create
*/
#include "Sampler.h"

namespace lrender
{
    class LowDiscrepancySampler : public Sampler
    {
    public:
        static const s32 MaxDimensions = 16;

        LowDiscrepancySampler(s32 numSamples, u32 seed, s32 numDimensions);
        virtual ~LowDiscrepancySampler();

        virtual f32 next1D();
        virtual Sample2D next2D();
        virtual void advance();
        virtual void generate();
        virtual Sampler* clone();

    private:
        LowDiscrepancySampler(const LowDiscrepancySampler&);
        LowDiscrepancySampler& operator=(const LowDiscrepancySampler&);

        void generate1D(lcore::RandWELL& random, f32* samples, s32 size);
        void generate2D(lcore::RandWELL& random, Sample2D* samples, s32 size);

        s32 numDimensions_;
        s32 dim1D_;
        s32 dim2D_;
        f32* table1D_[MaxDimensions];
        Sample2D* table2D_[MaxDimensions];
    };
}
#endif //INC_LRENDER_LOWDISCREPANCYSAMPLER_H__

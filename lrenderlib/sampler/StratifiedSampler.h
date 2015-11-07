#ifndef INC_LRENDER_STRATIFIEDSAMPLER_H__
#define INC_LRENDER_STRATIFIEDSAMPLER_H__
/**
@file StratifiedSampler.h
@author t-sakai
@date 2015/09/16 create
*/
#include "Sampler.h"

namespace lrender
{
    class StratifiedSampler : public Sampler
    {
    public:
        static const s32 NumTables = 4;

        StratifiedSampler(s32 numSamples, u32 seed);
        virtual ~StratifiedSampler();

        virtual f32 next1D();
        virtual Sample2D next2D();
        virtual void advance();
        virtual void generate();
        virtual Sampler* clone();

    private:
        StratifiedSampler(const StratifiedSampler&);
        StratifiedSampler& operator=(const StratifiedSampler&);

        s32 dimension1D_;
        s32 dimension2D_;
        f32* table1D_[NumTables];
        Sample2D* table2D_[NumTables];
    };
}
#endif //INC_LRENDER_STRATIFIEDSAMPLER_H__

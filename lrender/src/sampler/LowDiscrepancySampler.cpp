/**
@file LowDiscrepancySampler.cpp
@author t-sakai
@date 2015/09/22 create
*/
#include "sampler/LowDiscrepancySampler.h"

namespace lrender
{
    LowDiscrepancySampler::LowDiscrepancySampler(s32 numSamples, u32 seed, s32 numDimensions)
        :Sampler( lcore::roundUpPow2(static_cast<u32>(numSamples)), seed)
        ,numDimensions_(lcore::minimum(numDimensions, MaxDimensions))
        ,dim1D_(0)
        ,dim2D_(0)
    {
        f32* v1D = LNEW f32[numSamples_*numDimensions_];
        Sample2D* v2D = LNEW Sample2D[numSamples_*numDimensions_];

        for(size_t i = 0; i < numDimensions_; ++i){
            table1D_[i] = v1D + i*numSamples_;
            table2D_[i] = v2D + i*numSamples_;
            generate1D(random_, table1D_[i], numSamples_);
            generate2D(random_, table2D_[i], numSamples_);
        }
    }

    LowDiscrepancySampler::~LowDiscrepancySampler()
    {
        LDELETE_ARRAY(table2D_[0]);
        LDELETE_ARRAY(table1D_[0]);
    }

    f32 LowDiscrepancySampler::next1D()
    {
        LASSERT(index_<numSamples_);
        if(dim1D_<numDimensions_){
            return table1D_[dim1D_++][index_];
        }else{
            return random_.frand2();
        }
    }

    Sample2D LowDiscrepancySampler::next2D()
    {
        LASSERT(index_<numSamples_);
        if(dim2D_<numDimensions_){
            return table2D_[dim2D_++][index_];
        }else{
            return Sample2D(random_.frand2(), random_.frand2());
        }
    }

    void LowDiscrepancySampler::advance()
    {
        indexArray1D_ = 0;
        indexArray2D_ = 0;
        dim1D_ = 0;
        dim2D_ = 0;
        ++index_;
    }

    void LowDiscrepancySampler::generate1D(lcore::RandWELL& random, f32* samples, s32 size)
    {
        u32 scramble1D = random.rand();

        for(s32 j = 0; j < size; ++j){
            samples[j] = lcore::radicalInverseVanDerCorput(j, scramble1D);
        }

        lcore::random::shuffle(random, samples, samples+size);
    }

    void LowDiscrepancySampler::generate2D(lcore::RandWELL& random, Sample2D* samples, s32 size)
    {
        u32 scramble2D[2] = {random.rand(), random.rand()};
        for(s32 i = 0; i < size; ++i){
            lcore::sobol02(samples[i].x_, samples[i].y_, i, scramble2D[0], scramble2D[1]);
        }
        lcore::random::shuffle(random, samples, samples+size);
    }

    void LowDiscrepancySampler::generate()
    {
        for(size_t i = 0; i < numDimensions_; ++i){
            generate1D(random_, table1D_[i], numSamples_);
            generate2D(random_, table2D_[i], numSamples_);
        }

        for(size_t i = 0; i < req1D_.size(); ++i){
            generate1D(random_, array1D_[i], numSamples_*req1D_[i]);
        }

        for(size_t i = 0; i < req2D_.size(); ++i){
            generate2D(random_, array2D_[i], numSamples_*req2D_[i]);
        }

        indexArray1D_ = 0;
        indexArray2D_ = 0;
        dim1D_ = 0;
        dim2D_ = 0;
        index_ = 0;
    }

    Sampler* LowDiscrepancySampler::clone()
    {
        LowDiscrepancySampler* sampler = LNEW LowDiscrepancySampler(numSamples_, random_.rand(), numDimensions_);

        for (size_t i=0; i<req1D_.size(); ++i){
			sampler->request1DArray(req1D_[i]);
        }
		for (size_t i=0; i<req2D_.size(); ++i){
			sampler->request2DArray(req2D_[i]);
        }
        return sampler;
    }
}

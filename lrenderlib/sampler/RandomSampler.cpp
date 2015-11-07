/**
@file RandomSampler.cpp
@author t-sakai
@date 2015/10/28 create
*/
#include "RandomSampler.h"

namespace lrender
{
    RandomSampler::RandomSampler(s32 numSamples, u32 seed, s32 numDimensions)
        :Sampler( lcore::roundUpPow2(static_cast<u32>(numSamples)), seed)
        ,numDimensions_(numDimensions)
        ,dim1D_(0)
        ,dim2D_(0)
    {
        f32* v1D = LIME_NEW f32[numSamples_*numDimensions_];
        Sample2D* v2D = LIME_NEW Sample2D[numSamples_*numDimensions_];

        for(size_t i = 0; i < numDimensions_; ++i){
            table1D_[i] = v1D + i*numSamples_;
            table2D_[i] = v2D + i*numSamples_;
            generate1D(random_, table1D_[i], numSamples_);
            generate2D(random_, table2D_[i], numSamples_);
        }
    }

    RandomSampler::~RandomSampler()
    {
        LIME_DELETE_ARRAY(table2D_[0]);
        LIME_DELETE_ARRAY(table1D_[0]);
    }

    f32 RandomSampler::next1D()
    {
        LASSERT(index_<numSamples_);
        if(dim1D_<numDimensions_){
            return table1D_[dim1D_++][index_];
        }else{
            return random_.frand2();
        }
    }

    Sample2D RandomSampler::next2D()
    {
        LASSERT(index_<numSamples_);
        if(dim2D_<numDimensions_){
            return table2D_[dim2D_++][index_];
        }else{
            return Sample2D(random_.frand2(), random_.frand2());
        }
    }

    void RandomSampler::advance()
    {
        indexArray1D_ = 0;
        indexArray2D_ = 0;
        dim1D_ = 0;
        dim2D_ = 0;
        ++index_;
    }

    void RandomSampler::generate1D(lcore::RandomWELL& random, f32* samples, s32 size)
    {
        for(s32 j = 0; j < size; ++j){
            samples[j] = random.frand2();
        }
    }

    void RandomSampler::generate1D(lcore::RandomWELL& random, f32* samples, s32 numSamples, s32 size)
    {
        for(s32 i=0; i<size; ++i){
            for(s32 j = 0; j < numSamples; ++j){
                samples[j*size] = random.frand2();
            }
            ++samples;
        }
    }

    void RandomSampler::generate2D(lcore::RandomWELL& random, Sample2D* samples, s32 size)
    {
        for(s32 j = 0; j < size; ++j){
            samples[j].x_ = random.frand2();
            samples[j].y_ = random.frand2();
        }
    }

    void RandomSampler::generate2D(lcore::RandomWELL& random, Sample2D* samples, s32 numSamples, s32 size)
    {
        for(s32 i=0; i<size; ++i){
            for(s32 j = 0; j < numSamples; ++j){
                samples[j*size].x_ = random.frand2();
                samples[j*size].y_ = random.frand2();
            }
            ++samples;
        }
    }

    void RandomSampler::generate()
    {
        for(size_t i = 0; i < numDimensions_; ++i){
            generate1D(random_, table1D_[i], numSamples_);
            generate2D(random_, table2D_[i], numSamples_);
        }

        for(size_t i = 0; i < req1D_.size(); ++i){
            generate1D(random_, array1D_[i], numSamples_, req1D_[i]);
        }

        for(size_t i = 0; i < req2D_.size(); ++i){
            generate2D(random_, array2D_[i], numSamples_, req2D_[i]);
        }

        indexArray1D_ = 0;
        indexArray2D_ = 0;
        dim1D_ = 0;
        dim2D_ = 0;
        index_ = 0;
    }

    Sampler* RandomSampler::clone()
    {
        RandomSampler* sampler = LIME_NEW RandomSampler(numSamples_, random_.rand(), numDimensions_);

        for (size_t i=0; i<req1D_.size(); ++i){
			sampler->request1DArray(req1D_[i]);
        }
		for (size_t i=0; i<req2D_.size(); ++i){
			sampler->request2DArray(req2D_[i]);
        }
        return sampler;
    }
}

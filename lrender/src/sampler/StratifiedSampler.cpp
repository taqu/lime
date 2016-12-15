/**
@file StratifiedSampler.cpp
@author t-sakai
@date 2015/09/16 create
*/
#include "sampler/StratifiedSampler.h"

namespace lrender
{
namespace
{
    void generateStratified(f32* values, s32 numSamples, lcore::RandWELL& random)
    {
        f32 inv = 1.0f/numSamples;
        for(s32 i=0; i<numSamples; ++i){
            values[i] = inv*(random.frand()+i);
        }
    }

    void generateStratified(Sample2D* values, s32 numSamples, lcore::RandWELL& random)
    {
        f32 inv = 1.0f/numSamples;
        for(s32 i=0; i<numSamples; ++i){
            values[i].x_ = inv*(random.frand()+i);
            values[i].y_ = inv*(random.frand()+i);
        }
    }
}

    StratifiedSampler::StratifiedSampler(s32 numSamples, u32 seed)
        :Sampler(numSamples, seed)
        ,dimension1D_(0)
        ,dimension2D_(0)
    {
        f32* values1D = LNEW f32[numSamples_ * NumTables];
        Sample2D* values2D = LNEW Sample2D[numSamples_ * NumTables];

        for(s32 i=0; i<NumTables; ++i){
            table1D_[i] = values1D + numSamples_*i;
            table2D_[i] = values2D + numSamples_*i;
        }
    }

    StratifiedSampler::~StratifiedSampler()
    {
        LDELETE_ARRAY(table2D_[0]);
        LDELETE_ARRAY(table1D_[0]);
    }

    f32 StratifiedSampler::next1D()
    {
        LASSERT(index_<numSamples_);
        s32 dim = dimension1D_++;
        if(NumTables<=dimension1D_){
            dimension1D_ = 0;
        }
        return table1D_[dim][index_];
    }

    Sample2D StratifiedSampler::next2D()
    {
        LASSERT(index_<numSamples_);
        s32 dim = dimension2D_++;
        if(NumTables<=dimension2D_){
            dimension2D_ = 0;
        }
        return table2D_[dim][index_];
    }

    void StratifiedSampler::advance()
    {
        dimension1D_ = 0;
        dimension2D_ = 0;
        ++index_;
    }

    void StratifiedSampler::generate()
    {
        for(s32 i=0; i<NumTables; ++i){
            generateStratified(table1D_[i], numSamples_, random_);
            generateStratified(table2D_[i], numSamples_, random_);
        }
    }

    Sampler* StratifiedSampler::clone()
    {
        StratifiedSampler* sampler = LNEW StratifiedSampler(numSamples_, random_.rand());
        lcore::memcpy(sampler->table1D_[0], table1D_[0], sizeof(f32)*numSamples_*NumTables);
        lcore::memcpy(sampler->table2D_[0], table2D_[0], sizeof(Sample2D)*numSamples_*NumTables);
        sampler->dimension1D_ = dimension1D_;
        sampler->dimension2D_ = dimension2D_;
        sampler->index_ = index_;
        return sampler;
    }
}

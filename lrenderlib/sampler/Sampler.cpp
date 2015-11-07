/**
@file Sampler.cpp
@author t-sakai
@date 2013/05/24 create
*/
#include "Sampler.h"

namespace lrender
{
    //----------------------------------
    //---
    //--- Sampler
    //---
    //----------------------------------
    Sampler::Sampler(u32 seed)
        :numSamples_(0)
        ,index_(0)
        ,indexArray1D_(0)
        ,indexArray2D_(0)
    {
        random_.srand(seed);
    }

    Sampler::Sampler(s32 numSamples, u32 seed)
        :numSamples_(numSamples)
        ,index_(0)
        ,indexArray1D_(0)
        ,indexArray2D_(0)
    {
        random_.srand(seed);
    }

    Sampler::~Sampler()
    {
        for(s32 i=0; i<req1D_.size(); ++i){
            LIME_DELETE_ARRAY(array1D_[i]);
        }
        for(s32 i=0; i<req2D_.size(); ++i){
            LIME_DELETE_ARRAY(array2D_[i]);
        }
    }

    void Sampler::request1DArray(s32 size)
    {
        req1D_.push_back(size);
        array1D_.push_back(LIME_NEW f32[numSamples_*size]);
    }

    void Sampler::request2DArray(s32 size)
    {
        req2D_.push_back(size);
        array2D_.push_back(LIME_NEW Sample2D[numSamples_*size]);
    }

    f32* Sampler::next1DArray(s32 size)
    {
        LASSERT(indexArray1D_<req1D_.size());
        LASSERT(req1D_[indexArray1D_] == size);

        f32* ret = array1D_[indexArray1D_] + index_ * size;
        ++indexArray1D_;
        return ret;
    }

    Sample2D* Sampler::next2DArray(s32 size)
    {
        LASSERT(indexArray2D_<req2D_.size());
        LASSERT(req2D_[indexArray2D_] == size);

        Sample2D* ret = array2D_[indexArray2D_] + index_ * size;
        ++indexArray2D_;
        return ret;
    }
}

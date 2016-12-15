/**
@file Filter.cpp
@author t-sakai
@date 2015/09/11 create
*/
#include "filter/Filter.h"

namespace lrender
{
    void Filter::initialize()
    {
        f32 total = 0.0f;
        f32 step = radius_/FilterSize;
        for(s32 i=0; i<FilterSize; ++i){
            values_[i] = evaluate(step*i);
            total += values_[i];
        }
        total *= 2.0f * step;

        values_[FilterSize] = 0.0f;
        scale_ = static_cast<f32>(FilterSize)/radius_;
        
        f32 invTotal = 1.0f/total;
        for(s32 i=0; i<FilterSize; ++i){
            values_[i] *= invTotal;
        }
    }
}

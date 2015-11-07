/**
@file load_geometry.cpp
@author t-sakai
@date 2015/01/05 create
*/
#include "load_geometry.h"

namespace lscene
{
namespace lload
{
    bool Bone::hasEmpty() const
    {
        return count_<WorkDimBone;
    }

    void Bone::setInvalid()
    {
        count_ = 0;
        for(s32 i=0; i<WorkDimBone; ++i){
            indices_[i] = 0xFFFFU;
            weights_[i] = 0.0f;
        }
    }

    void Bone::setZero()
    {
        for(s32 i=0; i<WorkDimBone; ++i){
            if(0xFFFFU == indices_[i]){
                indices_[i] = 0;
            }
        }
    }

    void Bone::correct()
    {
        for(s32 i=1; i<count_; ++i){
            for(s32 j=i; 0<j; --j){
                if(weights_[j-1]<weights_[j]){
                    lcore::swap(indices_[j-1], indices_[j]);
                    lcore::swap(weights_[j-1], weights_[j]);
                }else{
                    break;
                }
            }
        }

        s32 count = (DimBone<count_)? DimBone : count_;
        f32 total = 0.0f;
        for(s32 i=0; i<count; ++i){
            total += weights_[i];
        }
        if(total<0.0001f){
            return;
        }

        f32 inv = 1.0f/total;
        for(s32 i=0; i<count; ++i){
            weights_[i] *= inv;
        }
    }
}
}

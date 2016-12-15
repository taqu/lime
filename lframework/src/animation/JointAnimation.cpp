/**
@file JointAnimation.cpp
@author t-sakai
@date 2016/11/24 create
*/
#include "animation/JointAnimation.h"
#include "animation/JointPose.h"

namespace lfw
{
    s32 JointAnimation::binarySearchIndex(f32 time) const
    {
        LASSERT(0<numPoses_);
        s32 mid = 0;
        s32 left = 0;
        s32 right = numPoses_;
        while(left < right){
            mid = (left + right) >> 1;
            if(poses_[mid].time_ <= time){
                left = mid + 1;
            }else{
                right = mid;
            }
        }
        LASSERT(0<=left && left<=numPoses_);
        if(left>0){
            left -= 1;
        }
        return left;
    }
}

/**
@file JointAnimation.cpp
@author t-sakai
@date 2010/11/12 create

*/
#include "JointAnimation.h"
#include "JointPose.h"

namespace lanim
{
    s32 JointAnimation::binarySearchIndex(u32 frame) const
    {
        LASSERT(0<numPoses_);
        s32 mid = 0;
        s32 left = 0;
        s32 right = numPoses_;
        while(left < right){
            mid = (left + right) >> 1;
            if(poses_[mid].frameNo_ <= frame){
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

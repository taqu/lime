/**
@file AnimationControler.cpp
@author t-sakai
@date 2010/11/16 create

*/
#include "AnimationControler.h"
namespace lanim
{
    void AnimationControlerResource::setSkeleton(Skeleton::pointer& skeleton)
    {
        skeleton_ = skeleton;
        if(skeleton_ == NULL){
            SkeletonPose empty;
            skeletonPose_.swap(empty);
            return;
        }
        SkeletonPose tmp(skeleton_->getNumJoints());
        skeletonPose_.swap(tmp);
    }
}

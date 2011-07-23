/**
@file SkeletonPose.cpp
@author t-sakai
@date 2010/11/12 create

*/
#include "SkeletonPose.h"
#include "JointPose.h"
#include <lmath/Matrix34.h>

namespace lanim
{
    SkeletonPose::SkeletonPose()
        :numJoints_(0)
        ,localPoses_(NULL)
        ,matrix_(NULL)
    {
    }


    SkeletonPose::SkeletonPose(u32 numJoints)
        :numJoints_(numJoints)
    {
        LASSERT(numJoints_>=0);
        localPoses_ = LIME_NEW JointPose[numJoints_];
        matrix_ = LIME_NEW lmath::Matrix34[numJoints_];
        for(u32 i=0; i<numJoints_; ++i){
            matrix_[i].identity();
        }
    }

    SkeletonPose::~SkeletonPose()
    {
        LIME_DELETE_ARRAY(matrix_);
        LIME_DELETE_ARRAY(localPoses_);
    }

    // スワップ
    void SkeletonPose::swap(SkeletonPose& rhs)
    {
        lcore::swap(numJoints_, rhs.numJoints_);
        lcore::swap(localPoses_, rhs.localPoses_);
        lcore::swap(matrix_, rhs.matrix_);
    }
}

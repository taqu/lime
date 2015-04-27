/**
@file SkeletonPose.cpp
@author t-sakai
@date 2010/11/12 create

*/
#include "SkeletonPose.h"
#include "JointPose.h"
#include <lmath/Matrix34.h>

namespace lscene
{
namespace lanim
{
    SkeletonPose::SkeletonPose()
        :numJoints_(0)
        ,localPoses_(NULL)
        ,matrix_(NULL)
    {
    }


    SkeletonPose::SkeletonPose(s32 numJoints)
        :numJoints_(numJoints)
    {
        LASSERT(0<=numJoints_);

        u32 total = (sizeof(JointPose) + sizeof(lmath::Matrix34)) * numJoints_;
        u8* buffer = reinterpret_cast<u8*>(LSCENE_MALLOC(total));

        u32 offset = 0;
        localPoses_ = reinterpret_cast<JointPose*>(buffer);
        offset += sizeof(JointPose) * numJoints_;

        matrix_ = reinterpret_cast<lmath::Matrix34*>(buffer + offset);
        offset += sizeof(lmath::Matrix34) * numJoints_;

        for(s32 i=0; i<numJoints_; ++i){
            matrix_[i].identity();
        }
    }

    SkeletonPose::~SkeletonPose()
    {
        for(s32 i=0; i<numJoints_; ++i){
            localPoses_[i].~JointPose();
            matrix_[i].~Matrix34();
        }
        numJoints_ = 0;
        LSCENE_FREE(localPoses_);
        matrix_ = NULL;
    }

    // スワップ
    void SkeletonPose::swap(SkeletonPose& rhs)
    {
        lcore::swap(numJoints_, rhs.numJoints_);
        lcore::swap(localPoses_, rhs.localPoses_);
        lcore::swap(matrix_, rhs.matrix_);
    }
}
}

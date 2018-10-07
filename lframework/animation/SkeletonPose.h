#ifndef INC_LFRAMEWORK_SKELETONPOSE_H_
#define INC_LFRAMEWORK_SKELETONPOSE_H_
/**
@file SkeletonPose.h
@author t-sakai
@date 2016/11/24 create
*/
#include "lanim.h"
namespace lmath
{
    class Matrix34;
}

namespace lfw
{
    struct JointPose;

    class SkeletonPose
    {
    public:
        SkeletonPose();
        explicit SkeletonPose(s32 numJoints);
        ~SkeletonPose();

        /// ジョイント数取得
        inline s32 getNumJoints() const;

        inline JointPose* getPoses();
        inline const JointPose* getPoses() const;

        inline lmath::Matrix34* getMatrices();
        inline const lmath::Matrix34* getMatrices() const;


        /// スワップ
        void swap(SkeletonPose& rhs);
    private:
        SkeletonPose(const SkeletonPose&);
        SkeletonPose& operator=(const SkeletonPose&);

        s32 numJoints_;
        JointPose* localPoses_;
        lmath::Matrix34* matrix_;
    };

    // ジョイント数取得
    inline s32 SkeletonPose::getNumJoints() const
    {
        return numJoints_;
    }

    inline JointPose* SkeletonPose::getPoses()
    {
        return localPoses_;
    }

    inline const JointPose* SkeletonPose::getPoses() const
    {
        return localPoses_;
    }

    inline lmath::Matrix34* SkeletonPose::getMatrices()
    {
        return matrix_;
    }

    inline const lmath::Matrix34* SkeletonPose::getMatrices() const
    {
        return matrix_;
    }
}
#endif //INC_LFRAMEWORK_SKELETONPOSE_H_

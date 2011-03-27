#ifndef INC_ANIMATIONCONTROLERIK_H__
#define INC_ANIMATIONCONTROLERIK_H__
/**
@file AnimationControlerIK.h
@author t-sakai
@date 2010/12/30 create
*/
#include <lcore/Vector.h>
#include <lframework/anim/AnimationControler.h>
#include <lframework/anim/IKPack.h>

namespace lmath
{
    class Quaternion;
}

namespace lanim
{
    //--------------------------------------------------------------
    //---
    //--- AnimationControlerIK
    //---
    //--------------------------------------------------------------
    /**
    */
    class AnimationControlerIK
    {
    public:
        static f32 IKEpsilon; //=1.0e-5f;
        
        enum Flag
        {
        };

        AnimationControlerIK();
        ~AnimationControlerIK();

        inline void setSkeleton(Skeleton::pointer& skeleton);

        /**
        @brief IK登録
        @param iks ... IKパック
        */
        bool setIKPack(IKPack::pointer& iks)
        {
            ikPack_ = iks;
            return true;
        }

        void blendPose(SkeletonPose& pose);

    protected:
        AnimationControlerIK(const AnimationControlerIK&);
        AnimationControlerIK& operator=(const AnimationControlerIK&);

        void updateMatrix(const JointPose& jointPose, lmath::Matrix43* matrices, u8 jointIndex);

        void updateMatrices(SkeletonPose& pose, u8 childIndex);

        void updateChildMatrices(SkeletonPose& pose, u8 jointIndex);

        inline void updateMatrices(SkeletonPose& pose, const IKEntry& entry, u16 ikIndex);

        Skeleton::pointer skeleton_;
        IKPack::pointer ikPack_;
    };

    // スケルトンセット
    inline void AnimationControlerIK::setSkeleton(Skeleton::pointer& skeleton)
    {
        skeleton_ = skeleton;
    }

}
#endif //INC_ANIMATIONCONTROLERIK_H__

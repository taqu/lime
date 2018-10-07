/**
@file AnimationClip.cpp
@author t-sakai
@date 2016/11/24 create
*/
#include "animation/AnimationClip.h"
#include "animation/JointAnimation.h"
#include <lcore/File.h>

namespace lfw
{
    AnimationClip::AnimationClip()
        :referenceCount_(0)
        ,lastTime_(0.0f)
        ,numJoints_(0)
        ,jointAnims_(NULL)
    {
    }

    AnimationClip::AnimationClip(s32 numJoints)
        :referenceCount_(0)
        ,lastTime_(0.0f)
        ,numJoints_(numJoints)
    {
        LASSERT(0<=numJoints_);
        jointAnims_ = LNEW JointAnimation[numJoints];
    }

    AnimationClip::~AnimationClip()
    {
        LDELETE_ARRAY(jointAnims_);
    }

    bool AnimationClip::serialize(lcore::File& os, AnimationClip* anim)
    {
        LASSERT(NULL != anim);

        write(os, anim->getName());
        os.write(anim->getLastTime());
        os.write(anim->getNumJoints());

        for(u32 i=0; i<anim->getNumJoints(); ++i){
            const JointAnimation& jointAnim = anim->getJointAnimation(i);
            const Name& jointName = jointAnim.getName();
            write(os, jointName);
        }

        for(u32 i=0; i<anim->getNumJoints(); ++i){
            const JointAnimation& jointAnim = anim->getJointAnimation(i);
            os.write(jointAnim.getNumPoses());
            
            for(s32 j=0; j<jointAnim.getNumPoses(); ++j){
                const JointPoseWithTime& pose = jointAnim.getPose(j);
                os.write(pose.time_);
                os.write(pose.translation_);
                os.write(pose.rotation_);
            }
        }
        return true;
    }

    bool AnimationClip::deserialize(AnimationClip** ppAnim, lcore::File& is)
    {
        Name animName;
        read(animName, is);

        f32 lastTime = 0.0f;
        is.read(lastTime);

        u32 numJoints = 0;
        is.read(numJoints);

        *ppAnim = LNEW AnimationClip(numJoints);
        AnimationClip* anim = *ppAnim;
        anim->setName(animName);
        anim->setLastTime(lastTime);

        for(u32 i=0; i<anim->getNumJoints(); ++i){
            JointAnimation& jointAnim = anim->getJointAnimation(i);
            Name jointName;
            read(jointName, is);

            jointAnim.setName(jointName);
        }

        for(u32 i=0; i<anim->getNumJoints(); ++i){
            JointAnimation& jointAnim = anim->getJointAnimation(i);
            u32 numPoses = 0;
            is.read(numPoses);
            
            jointAnim.setNumPoses(numPoses);

            for(s32 j=0; j<jointAnim.getNumPoses(); ++j){
                JointPoseWithTime& pose = jointAnim.getPose(j);
                is.read(pose.time_);
                is.read(pose.translation_);
                is.read(pose.rotation_);
            }
        }
        return true;
    }
}

/**
@file AnimationClip.cpp
@author t-sakai
@date 2010/11/12 create

*/
#include "AnimationClip.h"
#include "JointAnimation.h"
#include <lcore/liostream.h>

namespace lanim
{
    AnimationClip::AnimationClip()
        :refCount_(0)
        ,lastFrame_(0.0f)
        ,numJoints_(0)
        ,jointAnims_(NULL)
    {
    }

    AnimationClip::AnimationClip(u32 numJoints)
        :refCount_(0)
        ,lastFrame_(0.0f)
        ,numJoints_(numJoints)
    {
        LASSERT(numJoints_>=0);
        jointAnims_ = LIME_NEW JointAnimation[numJoints];
    }

    AnimationClip::~AnimationClip()
    {
        LIME_DELETE_ARRAY(jointAnims_);
    }

    bool AnimationClip::serialize(lcore::ostream& os, AnimationClip::pointer& anim)
    {
        write(os, anim->getName());
        lcore::io::write(os, anim->getLastFrame());
        lcore::io::write(os, anim->getNumJoints());

        for(u32 i=0; i<anim->getNumJoints(); ++i){
            const JointAnimation& jointAnim = anim->getJointAnimation(i);
            const Name& jointName = jointAnim.getName();
            write(os, jointName);
        }

        for(u32 i=0; i<anim->getNumJoints(); ++i){
            const JointAnimation& jointAnim = anim->getJointAnimation(i);
            lcore::io::write(os, jointAnim.getNumPoses());
            
            for(s32 j=0; j<jointAnim.getNumPoses(); ++j){
                const JointPoseWithFrame& frame = jointAnim.getPose(j);
                lcore::io::write(os, frame.frameNo_);
                lcore::io::write(os, frame.translation_);
                lcore::io::write(os, frame.rotation_);
            }
        }
        return true;
    }

    bool AnimationClip::deserialize(AnimationClip::pointer& anim, lcore::istream& is)
    {
        Name animName;
        read(animName, is);

        f32 lastFrame = 0.0f;
        lcore::io::read(is, lastFrame);

        u32 numJoints = 0;
        lcore::io::read(is, numJoints);

        anim = LIME_NEW lanim::AnimationClip(numJoints);
        anim->setName(animName);
        anim->setLastFrame(lastFrame);

        for(u32 i=0; i<anim->getNumJoints(); ++i){
            JointAnimation& jointAnim = anim->getJointAnimation(i);
            Name jointName;
            read(jointName, is);

            jointAnim.setName(jointName);
        }

        for(u32 i=0; i<anim->getNumJoints(); ++i){
            JointAnimation& jointAnim = anim->getJointAnimation(i);
            u32 numPoses = 0;
            lcore::io::read(is, numPoses);
            
            jointAnim.setNumPoses(numPoses);

            for(s32 j=0; j<jointAnim.getNumPoses(); ++j){
                JointPoseWithFrame& frame = jointAnim.getPose(j);
                lcore::io::read(is, frame.frameNo_);
                lcore::io::read(is, frame.translation_);
                lcore::io::read(is, frame.rotation_);
            }
        }
        return true;
    }
}

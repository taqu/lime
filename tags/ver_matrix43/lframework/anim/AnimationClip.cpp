/**
@file AnimationClip.cpp
@author t-sakai
@date 2010/11/12 create

*/
#include "AnimationClip.h"
#include "JointAnimation.h"

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
}

/**
@file AnimObject.cpp
@author t-sakai
@date 2010/04/02 create
*/
#include "AnimObject.h"

#include <lframework/anim/SkeletonPose.h>

namespace lscene
{
    AnimObject::AnimObject()
        :skeletonPose_(NULL)
    {
    }


    AnimObject::AnimObject(
        u32 numGeometries,
        u32 numMaterials)
        :lrender::Drawable(numGeometries, numMaterials)
        ,skeletonPose_(NULL)
    {
    }

    AnimObject::~AnimObject()
    {
    }

    void AnimObject::swap(AnimObject& rhs)
    {
        lrender::Drawable::swap(rhs);
        skeleton_.swap(rhs.skeleton_);
        lcore::swap(skeletonPose_, rhs.skeletonPose_);
        lcore::swap(ikPack_, rhs.ikPack_);

    }


    u32 AnimObject::getNumJointPoses() const
    {
        return (skeletonPose_ != NULL)? skeletonPose_->getNumJoints() : 0;
    }

    const lmath::Matrix34* AnimObject::getGlobalJointPoses() const
    {
        LASSERT(skeletonPose_ != NULL);
        return skeletonPose_->getMatrices();
    }
}

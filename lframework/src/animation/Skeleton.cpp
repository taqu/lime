/**
@file Skeleton.cpp
@author t-sakai
@date 2016/11/24 create
*/
#include "animation/Skeleton.h"
#include "animation/Joint.h"
#include <lcore/liostream.h>

namespace lfw
{
    Skeleton::Skeleton()
        :numJoints_(0)
        ,joints_(NULL)
        ,jointNames_(NULL)
    {
    }

    Skeleton::Skeleton(s32 numJoints)
        :numJoints_(numJoints)
    {
        u32 total = (sizeof(Joint) + sizeof(Name)) * numJoints_;

        u8* buffer = static_cast<u8*>(LMALLOC(total));

        u32 offset = 0;
        joints_ = reinterpret_cast<Joint*>(buffer);
        offset += sizeof(Joint) * numJoints_;

        jointNames_ = reinterpret_cast<Name*>(buffer + offset);
        offset += sizeof(Name) * numJoints_;

        for(s32 i=0; i<numJoints_; ++i){
            LPLACEMENT_NEW(&joints_[i]) Joint;
            LPLACEMENT_NEW(&jointNames_[i]) Name;
        }
    }

    Skeleton::~Skeleton()
    {
        for(s32 i=0; i<numJoints_; ++i){
            jointNames_[i].~Name();
            joints_[i].~Joint();
        }
        numJoints_ = 0;
        LFREE(joints_);
        jointNames_ = NULL;
    }

    // 名前からジョイント検索
    const Joint* Skeleton::findJoint(const Name& name) const
    {
        for(s32 i=0; i<numJoints_; ++i){
            if(jointNames_[i] == name){
                return &joints_[i];
            }
        }
        return NULL;
    }

    // 名前からジョイント検索
    const Joint* Skeleton::findJoint(const Char* name) const
    {
        LASSERT(NULL != name);
        //u32 hash = lcore::calc_hash_string(static_cast<const u8*>(name));

        for(s32 i=0; i<numJoints_; ++i){
            if(jointNames_[i] == name){
                return &joints_[i];
            }
        }
        return NULL;
    }

    // スワップ
    void Skeleton::swap(Skeleton& rhs)
    {
        lcore::swap(numJoints_, rhs.numJoints_);
        lcore::swap(joints_, rhs.joints_);
        lcore::swap(jointNames_, rhs.jointNames_);
    }

    void Skeleton::copyTo(Skeleton& dst)
    {
        Skeleton tmp(numJoints_);
        dst.swap(tmp);
        dst.name_ = name_;
        for(s32 i=0; i<numJoints_; ++i){
            dst.joints_[i] = joints_[i];
            dst.jointNames_[i] = jointNames_[i];
        }
    }

    bool Skeleton::serialize(lcore::ostream& os, Skeleton& skeleton)
    {
        write(os, skeleton.getName());
        lcore::io::write(os, skeleton.getNumJoints());
        for(s32 i=0; i<skeleton.getNumJoints(); ++i){
            write(os, skeleton.getJointName(i));
        }

        for(s32 i=0; i<skeleton.getNumJoints(); ++i){
            const Joint& joint = skeleton.getJoint(i);
            lcore::io::write(os, joint);
        }
        return true;
    }

    bool Skeleton::deserialize(Skeleton& skeleton, lcore::istream& is)
    {
        Name name;
        read(name, is);
        
        s32 numJoints = 0;
        lcore::io::read(is, numJoints);
        Skeleton tmpSkeleton(numJoints);
        tmpSkeleton.setName(name);

        for(s32 i=0; i<numJoints; ++i){
            read(name, is);
            tmpSkeleton.setJointName(i, name);
        }
        for(s32 i=0; i<numJoints; ++i){
            Joint& joint = tmpSkeleton.getJoint(i);
            lcore::io::read(is, joint);
        }
        skeleton.swap(tmpSkeleton);
        return true;
    }
}

/**
@file Skeleton.cpp
@author t-sakai
@date 2010/11/12 create

*/
#include "Skeleton.h"
#include "Joint.h"
#include <lcore/liostream.h>

namespace lscene
{
namespace lanim
{
    Skeleton::Skeleton()
        :refCount_(0)
        ,numJoints_(0)
        ,joints_(NULL)
        ,jointNames_(NULL)
    {
    }

    Skeleton::Skeleton(s32 numJoints)
        :refCount_(0)
        ,numJoints_(numJoints)
    {
        u32 total = (sizeof(Joint) + sizeof(Name)) * numJoints_;

        u8* buffer = reinterpret_cast<u8*>(LSCENE_MALLOC(total));

        u32 offset = 0;
        joints_ = reinterpret_cast<Joint*>(buffer);
        offset += sizeof(Joint) * numJoints_;

        jointNames_ = reinterpret_cast<Name*>(buffer + offset);
        offset += sizeof(Name) * numJoints_;

        for(s32 i=0; i<numJoints_; ++i){
            LSCENE_PLACEMENT_NEW(&joints_[i]) Joint;
            LSCENE_PLACEMENT_NEW(&jointNames_[i]) Name;
        }
    }

    Skeleton::~Skeleton()
    {
        refCount_ = 0;

        for(s32 i=0; i<numJoints_; ++i){
            jointNames_[i].~Name();
            joints_[i].~Joint();
        }
        numJoints_ = 0;
        LSCENE_FREE(joints_);
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
        u32 hash = lcore::calc_hash_string(reinterpret_cast<const u8*>(name));

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

    bool Skeleton::serialize(lcore::ostream& os, Skeleton::pointer& skeleton)
    {
        write(os, skeleton->getName());
        lcore::io::write(os, skeleton->getNumJoints());
        for(s32 i=0; i<skeleton->getNumJoints(); ++i){
            write(os, skeleton->getJointName(i));
        }

        for(s32 i=0; i<skeleton->getNumJoints(); ++i){
            const Joint& joint = skeleton->getJoint(i);
            lcore::io::write(os, joint);
        }
        return true;
    }

    bool Skeleton::deserialize(Skeleton::pointer& skeleton, lcore::istream& is)
    {
        Name name;
        read(name, is);
        
        s32 numJoints = 0;
        lcore::io::read(is, numJoints);
        skeleton = LSCENE_NEW Skeleton(numJoints);
        skeleton->setName(name);

        for(s32 i=0; i<skeleton->getNumJoints(); ++i){
            read(name, is);
            skeleton->setJointName(i, name);
        }
        for(s32 i=0; i<skeleton->getNumJoints(); ++i){
            Joint& joint = skeleton->getJoint(i);
            lcore::io::read(is, joint);
        }
        return true;
    }
}
}

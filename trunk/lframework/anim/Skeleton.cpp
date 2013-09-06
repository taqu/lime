/**
@file Skeleton.cpp
@author t-sakai
@date 2010/11/12 create

*/
#include "Skeleton.h"
#include "Joint.h"
#include <lcore/Hash.h>
#include <lcore/liostream.h>

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
        ,nameJointIndexMap_(numJoints<<1)
    {
        u32 size[] = 
        {
            sizeof(Joint) * numJoints_,
            sizeof(Name) * numJoints_,
        };
        u32 num = sizeof(size) / sizeof(u32);
        u32 total = 0;
        for(u32 i=0; i<num; ++i){
            total += size[i];
        }

        resourceBuffer_.resize(total);

        u32 offset = 0;
        joints_ = resourceBuffer_.allocate<Joint>(offset, numJoints_);
        offset += size[0];

        jointNames_ = resourceBuffer_.allocate<Name>(offset, numJoints_);
        offset += size[1];

    }

    Skeleton::~Skeleton()
    {
        refCount_ = 0;
        resourceBuffer_.destruct<Name>(jointNames_, numJoints_);
        resourceBuffer_.destruct<Joint>(joints_, numJoints_);
    }

    // 名前からジョイント検索
    const Joint* Skeleton::findJoint(const Name& name) const
    {
        NameJointIndexMap::size_type pos = nameJointIndexMap_.find(name.c_str(), name.size());
        if(pos == nameJointIndexMap_.end()){
            return NULL;
        }

        s32 index = nameJointIndexMap_.getValue(pos);
        LASSERT(0<=index && index<numJoints_);
        return &(joints_[index]);
    }

    // 名前からジョイント検索
    const Joint* Skeleton::findJoint(const char* name) const
    {
        LASSERT(name != NULL);

        NameJointIndexMap::size_type pos = nameJointIndexMap_.find(name);
        if(pos == nameJointIndexMap_.end()){
            return NULL;
        }

        s32 index = nameJointIndexMap_.getValue(pos);
        LASSERT(0<=index && index<numJoints_);
        return &(joints_[index]);
    }

    // スワップ
    void Skeleton::swap(Skeleton& rhs)
    {
        lcore::swap(numJoints_, rhs.numJoints_);
        lcore::swap(joints_, rhs.joints_);
        lcore::swap(jointNames_, rhs.jointNames_);

        nameJointIndexMap_.swap(rhs.nameJointIndexMap_);
        resourceBuffer_.swap(rhs.resourceBuffer_);
    }

    void Skeleton::recalcHash()
    {
        for(s32 i=0; i<numJoints_; ++i){
            nameJointIndexMap_.insert(jointNames_[i].c_str(), i);
        }
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
        skeleton = LIME_NEW Skeleton(numJoints);
        skeleton->setName(name);

        for(s32 i=0; i<skeleton->getNumJoints(); ++i){
            read(name, is);
            skeleton->setJointName(i, name);
        }
        for(s32 i=0; i<skeleton->getNumJoints(); ++i){
            Joint& joint = skeleton->getJoint(i);
            lcore::io::read(is, joint);
        }
        skeleton->recalcHash();
        return true;
    }
}

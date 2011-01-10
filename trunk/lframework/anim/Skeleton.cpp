/**
@file Skeleton.cpp
@author t-sakai
@date 2010/11/12 create

*/
#include "Skeleton.h"
#include "Joint.h"
#include <lcore/Hash.h>

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

        resourceBuffer_.reset(total);

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

    //bool Skeleton::createFromMemory(const s8* data, u32 bytes)
    //{
    //    return false;
    //}

    //u32 Skeleton::calcSizeToSave() const
    //{
    //    return 0;
    //}

    //void Skeleton::saveToMemory(s8* data) const
    //{
    //}
}

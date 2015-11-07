﻿#ifndef INC_LSCENE_LANIM_SKELETON_H__
#define INC_LSCENE_LANIM_SKELETON_H__
/**
@file Skeleton.h
@author t-sakai
@date 2010/11/12 create

*/
#include "lanim.h"
#include <lcore/smart_ptr/intrusive_ptr.h>
#include <lcore/Buffer.h>
#include "Joint.h"

namespace lscene
{
namespace lanim
{
    /**
    @brief スケルトン
    */
    class Skeleton : public SkeletonAllocator
    {
    public:
        typedef lcore::intrusive_ptr<Skeleton> pointer;

        Skeleton();
        explicit Skeleton(s32 numJoints);
        ~Skeleton();

        /// ジョイント数取得
        inline s32 getNumJoints() const;

        /// ジョイント取得
        inline const Joint& getJoint(s32 index) const;

        /// ジョイント取得
        inline Joint& getJoint(s32 index);

        /// ジョイント名取得
        inline const Name& getJointName(s32 index) const;

        /// ジョイント名セット
        inline void setJointName(s32 index, const Name& name);

        /// ジョイント名セット
        inline void setJointName(s32 index, const Char* name);

        /// ジョイント名セット
        inline void setJointName(s32 index, const Char* name, u32 length);

        /// 名前取得
        inline const Name& getName() const;

        /// 名前セット
        inline void setName(const Name& name);

        /// 名前セット
        inline void setName(const Char* name);

        /// 名前セット
        inline void setName(const Char* name, u32 length);

        /// 名前からジョイント検索
        const Joint* findJoint(const Name& name) const;

        /// 名前からジョイント検索
        const Joint* findJoint(const Char* name) const;

        /// ジョイントインデックス計算
        u8 calcJointIndex(const Joint* joint) const;

        /// スワップ
        void swap(Skeleton& rhs);

        static bool serialize(lcore::ostream& os, Skeleton::pointer& anim);
        static bool deserialize(Skeleton::pointer& anim, lcore::istream& is);
    private:
        friend inline void intrusive_ptr_addref(Skeleton* ptr);
        friend inline void intrusive_ptr_release(Skeleton* ptr);

        /// 参照カウント
        void addRef()
        {
            ++refCount_;
        }

        /// 参照カウント開放
        void release()
        {
            if(--refCount_ == 0){
                LSCENE_DELETE_NO_NULL(this);
            }
        }

        s32 refCount_;
        Name name_;
        s32 numJoints_;
        Joint* joints_;
        Name* jointNames_;
    };

    // ジョイント数取得
    inline s32 Skeleton::getNumJoints() const
    {
        return numJoints_;
    }

    // ジョイント取得
    inline const Joint& Skeleton::getJoint(s32 index) const
    {
        LASSERT(0<=index && index<numJoints_);
        return joints_[index];
    }

    // ジョイント取得
    inline Joint& Skeleton::getJoint(s32 index)
    {
        LASSERT(0<=index && index<numJoints_);
        return joints_[index];
    }

    // ジョイント名取得
    inline const Name& Skeleton::getJointName(s32 index) const
    {
        LASSERT(0<=index && index<numJoints_);
        return jointNames_[index];
    }

    // ジョイント名セット
    inline void Skeleton::setJointName(s32 index, const Name& name)
    {
        LASSERT(0<=index && index<numJoints_);
        jointNames_[index] = name;
    }

    // ジョイント名セット
    inline void Skeleton::setJointName(s32 index, const Char* name)
    {
        LASSERT(0<=index && index<numJoints_);
        jointNames_[index].assign(name);
    }

    // ジョイント名セット
    inline void Skeleton::setJointName(s32 index, const Char* name, u32 length)
    {
        LASSERT(0<=index && index<numJoints_);
        jointNames_[index].assign(name, length);
    }

    // 名前取得
    inline const Name& Skeleton::getName() const
    {
        return name_;
    }

    // 名前セット
    inline void Skeleton::setName(const Name& name)
    {
        name_ = name;
    }

    // 名前セット
    inline void Skeleton::setName(const Char* name)
    {
        LASSERT(NULL != name);
        name_.assign(name);
    }

    // 名前セット
    inline void Skeleton::setName(const Char* name, u32 length)
    {
        LASSERT(NULL != name);
        name_.assign(name, length);
    }

    // ジョイントインデックス計算
    inline u8 Skeleton::calcJointIndex(const Joint* joint) const
    {
        LASSERT(NULL != joint);
        LASSERT(NULL != joints_);

        ptrdiff_t diff = joint - joints_;
        LASSERT(0<=diff && diff<numJoints_);

        return static_cast<u8>(diff);
    }

    inline void intrusive_ptr_addref(Skeleton* ptr)
    {
        ptr->addRef();
    }

    inline void intrusive_ptr_release(Skeleton* ptr)
    {
        ptr->release();
    }
}
}
#endif //INC_LSCENE_LANIM_SKELETON_H__

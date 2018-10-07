#ifndef INC_LFRAMEWORK_IKPACK_H_
#define INC_LFRAMEWORK_IKPACK_H_
/**
@file IKPack.h
@author t-sakai
@date 2016/11/24 create
*/
#include <lcore/intrusive_ptr.h>
#include "lanim.h"

namespace lfw
{
    struct IKEntry
    {
        IKEntry()
            :targetJoint_(0)
            ,effectorJoint_(0)
            ,chainLength_(0)
            ,numIterations_(0)
            ,limitAngle_(0.0f)
            ,children_(NULL)
        {}

        ~IKEntry()
        {
            LFREE(children_);
        }

        u16 targetJoint_;
        u16 effectorJoint_;
        u16 chainLength_;
        u16 numIterations_;
        f32 limitAngle_;
        u8* children_;
    };

    class IKPack
    {
    public:
        typedef lcore::intrusive_ptr<IKPack> pointer;

        IKPack();
        explicit IKPack(s32 numIKs);
        ~IKPack();

        inline s32 getNumIKs() const;
        inline IKEntry& get(s32 index);
        inline const IKEntry& get(s32 index) const;

        void swap(IKPack& rhs);
    private:
        IKPack(const IKPack&);
        IKPack& operator=(const IKPack&);

        friend inline void intrusive_ptr_addref(IKPack* ptr);
        friend inline void intrusive_ptr_release(IKPack* ptr);

        /// 参照カウント
        void addRef()
        {
            ++refCount_;
        }

        /// 参照カウント開放
        void release()
        {
            if(--refCount_ == 0){
                LDELETE_RAW(this);
            }
        }

        s32 refCount_;

        s32 numIKs_;
        IKEntry* iks_;
    };

    inline s32 IKPack::getNumIKs() const
    {
        return numIKs_;
    }

    inline IKEntry& IKPack::get(s32 index)
    {
        LASSERT(0<=index && index<numIKs_);
        return iks_[index];
    }

    inline const IKEntry& IKPack::get(s32 index) const
    {
        LASSERT(0<=index && index<numIKs_);
        return iks_[index];
    }

    inline void intrusive_ptr_addref(IKPack* ptr)
    {
        ptr->addRef();
    }

    inline void intrusive_ptr_release(IKPack* ptr)
    {
        ptr->release();
    }
}
#endif //INC_LFRAMEWORK_IKPACK_H_

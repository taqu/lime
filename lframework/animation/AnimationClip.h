#ifndef INC_LFRAMEWORK_ANIMATIONCLIP_H_
#define INC_LFRAMEWORK_ANIMATIONCLIP_H_
/**
@file AnimationClip.h
@author t-sakai
@date 2016/11/24 create
*/
#include "lanim.h"
#include <lcore/intrusive_ptr.h>
#include "JointAnimation.h"

namespace lfw
{
    class AnimationClip
    {
    public:
        typedef lcore::intrusive_ptr<AnimationClip> pointer;

        AnimationClip();
        explicit AnimationClip(s32 numJoints);
        ~AnimationClip();

        inline f32 getLastTime() const;
        inline void setLastTime(f32 time);

        inline u32 getNumJoints() const;
        inline const JointAnimation& getJointAnimation(s32 index) const;
        inline JointAnimation& getJointAnimation(s32 index);

        /// 名前取得
        inline const Name& getName() const;

        /// 名前セット
        inline void setName(const Name& name);

        /// 名前セット
        inline void setName(const Char* name);

        /// 名前セット
        inline void setName(s32 length, const Char* name);

        static bool serialize(lcore::File& os, AnimationClip* anim);
        static bool deserialize(AnimationClip** ppAnim, lcore::File& is);
    private:
        AnimationClip(const AnimationClip&);
        AnimationClip& operator=(const AnimationClip&);

        friend inline void intrusive_ptr_addref(AnimationClip* ptr);
        friend inline void intrusive_ptr_release(AnimationClip* ptr);

        s32 referenceCount_;
        Name name_;
        f32 lastTime_;
        s32 numJoints_;
        JointAnimation *jointAnims_;
    };

    inline f32 AnimationClip::getLastTime() const
    {
        return lastTime_;
    }

    inline void AnimationClip::setLastTime(f32 time)
    {
        lastTime_ = time;
    }

    inline u32 AnimationClip::getNumJoints() const
    {
        return numJoints_;
    }

    inline const JointAnimation& AnimationClip::getJointAnimation(s32 index) const
    {
        LASSERT(0<=index && index<numJoints_);
        return jointAnims_[index];
    }

    inline JointAnimation& AnimationClip::getJointAnimation(s32 index)
    {
        LASSERT(0<=index && index<numJoints_);
        return jointAnims_[index];
    }


    // 名前取得
    inline const Name& AnimationClip::getName() const
    {
        return name_;
    }

    // 名前セット
    inline void AnimationClip::setName(const Name& name)
    {
        name_ = name;
    }

    // 名前セット
    inline void AnimationClip::setName(const Char* name)
    {
        LASSERT(NULL != name);
        name_.assign(lcore::strlen_s32(name), name);
    }

    // 名前セット
    inline void AnimationClip::setName(s32 length, const Char* name)
    {
        LASSERT(NULL != name);
        name_.assign(length, name);
    }

    inline void intrusive_ptr_addref(AnimationClip* ptr)
    {
        ++ptr->referenceCount_;
    }

    inline void intrusive_ptr_release(AnimationClip* ptr)
    {
        if(--ptr->referenceCount_ == 0){
            LDELETE_RAW(ptr);
        }
    }
}
#endif //INC_LFRAMEWORK_ANIMATIONCLIP_H_

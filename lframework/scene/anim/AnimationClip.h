#ifndef INC_LSCENE_LANIM_ANIMATIONCLIP_H__
#define INC_LSCENE_LANIM_ANIMATIONCLIP_H__
/**
@file AnimationClip.h
@author t-sakai
@date 2010/11/12 create

*/
#include "lanim.h"
#include "../Resource.h"
#include "JointAnimation.h"

namespace lscene
{
namespace lanim
{
    class AnimationClip : public ResourceTyped<ResourceType_AnimClip>
    {
    public:
        AnimationClip();
        explicit AnimationClip(u32 numJoints);
        ~AnimationClip();

        inline f32 getLastTime() const;
        inline void setLastTime(f32 time);

        inline u32 getNumJoints() const;
        inline const JointAnimation& getJointAnimation(u32 index) const;
        inline JointAnimation& getJointAnimation(u32 index);

        /// 名前取得
        inline const Name& getName() const;

        /// 名前セット
        inline void setName(const Name& name);

        /// 名前セット
        inline void setName(const Char* name);

        /// 名前セット
        inline void setName(const Char* name, u32 length);

        static bool serialize(lcore::ostream& os, AnimationClip* anim);
        static bool deserialize(AnimationClip** ppAnim, lcore::istream& is);
    private:
        AnimationClip(const AnimationClip&);
        AnimationClip& operator=(const AnimationClip&);

        Name name_;
        f32 lastTime_;
        u32 numJoints_;
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

    inline const JointAnimation& AnimationClip::getJointAnimation(u32 index) const
    {
        LASSERT(0<=index && index<numJoints_);
        return jointAnims_[index];
    }

    inline JointAnimation& AnimationClip::getJointAnimation(u32 index)
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
        name_.assign(name, lcore::strlen(name));
    }

    // 名前セット
    inline void AnimationClip::setName(const Char* name, u32 length)
    {
        LASSERT(NULL != name);
        name_.assign(name, length);
    }
}
}
#endif //INC_LSCENE_LANIM_ANIMATIONCLIP_H__

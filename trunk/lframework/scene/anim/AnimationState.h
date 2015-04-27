#ifndef INC_LSCENE_LANIM_ANIMATIONSTATE_H__
#define INC_LSCENE_LANIM_ANIMATIONSTATE_H__
/**
@file Animation.h
@author t-sakai
@date 2013/08/02 create
*/
#include "lanim.h"
#include "Skeleton.h"

namespace lscene
{
namespace lanim
{
    class AnimationClip;

    struct Transform;

    class AnimationState : public AnimationStateAllocator
    {
    public:
        AnimationState();
        ~AnimationState();

        void setSkeleton(Skeleton::pointer& skeleton);
        void setClip(AnimationClip* clip);

        inline Skeleton::pointer& getSkeleton();
        inline AnimationClip* getClip();

        inline bool valid() const;
        inline bool isLoop() const;
        inline void setLoop(bool enable);

        inline bool isPlay() const;
        inline bool checkFlag(AnimationFlag flag) const;
        inline void setFlag(AnimationFlag flag);
        inline void unsetFlag(AnimationFlag flag);

        inline BlendMode getBlendMode() const;
        inline void setBlendMode(BlendMode mode);

        bool isEnd() const;

        inline f32 getTime() const;
        void setTime(f32 time);
        inline f32 getSpeed() const;
        inline void setSpeed(f32 speed);
        inline f32 getWeight() const;
        inline void setWeight(f32 weight);

        f32 getFrameCount() const;
        void updateTime();

        inline void update(Transform* transforms);
    private:
        AnimationState(const AnimationState&);
        AnimationState& operator=(const AnimationState&);

        void updateBlendNone(Transform* transforms);
        void updateBlendMix(Transform* transforms);
        void updateBlendAdd(Transform* transforms);

        u16 flags_;
        u16 blendMode_;
        f32 time_;
        f32 speed_;
        f32 totalTime_;
        f32 weight_;
        Skeleton::pointer skeleton_;
        AnimationClip* clip_;
        u8* animationToJointIndex_;
    };

    inline Skeleton::pointer& AnimationState::getSkeleton()
    {
        return skeleton_;
    }

    inline AnimationClip* AnimationState::getClip()
    {
        return clip_;
    }

    inline bool AnimationState::valid() const
    {
        return NULL != clip_;
    }

    inline bool AnimationState::isLoop() const
    {
        return 0 != (flags_ & AnimationFlag_Loop);
    }

    inline void AnimationState::setLoop(bool enable)
    {
        if(enable){
            setFlag(AnimationFlag_Loop);
        }else{
            unsetFlag(AnimationFlag_Loop);
        }
    }

    inline bool AnimationState::isPlay() const
    {
        return 0 != (flags_ & AnimationFlag_Play);
    }

    inline bool AnimationState::checkFlag(AnimationFlag flag) const
    {
        return 0 != (flags_ & flag);
    }

    inline void AnimationState::setFlag(AnimationFlag flag)
    {
        flags_ |= flag;
    }

    inline void AnimationState::unsetFlag(AnimationFlag flag)
    {
        flags_ &= ~flag;
    }

    inline BlendMode AnimationState::getBlendMode() const
    {
        return static_cast<BlendMode>(blendMode_);
    }

    inline void AnimationState::setBlendMode(BlendMode blendMode)
    {
        blendMode_ = static_cast<u16>(blendMode);
    }

    inline f32 AnimationState::getTime() const
    {
        return time_;
    }

    inline f32 AnimationState::getSpeed() const
    {
        return speed_;
    }

    inline void AnimationState::setSpeed(f32 speed)
    {
        speed_ = speed;
    }

    inline f32 AnimationState::getWeight() const
    {
        return weight_;
    }

    inline void AnimationState::setWeight(f32 weight)
    {
        weight_ = lcore::clamp01(weight);
    }

    inline void AnimationState::update(Transform* transforms)
    {
        switch(blendMode_)
        {
        case BlendMode_None:
            updateBlendNone(transforms);
            break;
        case BlendMode_Mix:
            updateBlendMix(transforms);
            break;
        case BlendMode_Add:
            updateBlendAdd(transforms);
            break;
        }
    }
}
}
#endif //INC_LSCENE_LANIM_ANIMATIONSTATE_H__

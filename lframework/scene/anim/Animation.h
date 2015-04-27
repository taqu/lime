#ifndef INC_LSCENE_LANIM_ANIMATION_H__
#define INC_LSCENE_LANIM_ANIMATION_H__
/**
@file Animation.h
@author t-sakai
@date 2013/08/01 create
*/
#include <lmath/Quaternion.h>
#include <lmath/Vector4.h>

#include "lanim.h"
#include "AnimationState.h"

namespace lmath
{
    class DualQuaternion;
}

namespace lscene
{
namespace lanim
{
    class AnimationClip;

    struct Transform
    {
        lmath::Vector4 translation_;
        lmath::Quaternion rotation_;
    };

    class Animation
    {
    public:
        static const s16 MaxClips = 2048;
        static const s32 MaxPlay = 4;
        static const s32 MaxDelay = 4;

        Animation();
        ~Animation();

        void create(s16 numClips);
        void setSkeleton(Skeleton::pointer skeleton);

        void setClip(s16 index, AnimationClip* clip);

        inline const AnimationState& getState(s16 index) const;
        inline AnimationState& getState(s16 index);

        bool isInPlay(s16 clip) const;
        void play(s16 clip, f32 weight=1.0f);
        void playDelay(s16 clip, f32 delay, f32 weight=1.0f);
        void stop(s16 clip);
        void stopDelay(s16 clip, f32 delay);
        void stopAll();

        void fadeIn(s16 clip, f32 time);
        void fadeOut(s16 clip, f32 time);

        void crossFrade(s16 clip, f32 time);

        void update();
        void getMatrices(lmath::Matrix34* matrices);
        void getMatricesAligned16(lmath::Matrix34* matrices);
        void getDualQuaternion(lmath::DualQuaternion* dq);
    private:
        Animation(const Animation&);
        Animation& operator=(const Animation&);

        static const s16 Mode_PlayDelay = 0;
        static const s16 Mode_StopDelay = 1;
        static const s16 Mode_FadeIn = 2;
        static const s16 Mode_FadeOut = 3;

        struct Entry
        {
            s16 mode_;
            s16 clip_;
            f32 count_;
            f32 time_;
        };

        void clearTransforms();

        void pushPlay(s16 clip);
        void popPlay(s16 clip);
        void pushDelay(s16 clip, s16 mode, f32 time);
        void popDelay(s16 clip);

        Skeleton::pointer skeleton_;

        s16 numPlays_;
        s16 numDelays_;
        s16 numStates_;
        s16 padding_;
        s16 play_[MaxPlay];
        Entry delay_[MaxDelay];
        AnimationState* states_;
        Transform* transforms_;
    };

    inline const AnimationState& Animation::getState(s16 index) const
    {
        LASSERT(0<=index && index<numStates_);
        return states_[index];
    }

    inline AnimationState& Animation::getState(s16 index)
    {
        LASSERT(0<=index && index<numStates_);
        return states_[index];
    }
}
}
#endif //INC_LSCENE_LANIM_ANIMATION_H__

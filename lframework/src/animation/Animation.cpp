/**
@file Animation.cpp
@author t-sakai
@date 2016/11/24 create
*/
#include "animation/Animation.h"
#include <lmath/lmath.h>
#include <lmath/DualQuaternion.h>
#include "animation/AnimationClip.h"

namespace lfw
{
    Animation::Animation()
        :numPlays_(0)
        ,numDelays_(0)
        ,numStates_(0)
        ,states_(NULL)
        ,transforms_(NULL)
    {
    }

    Animation::~Animation()
    {
        LFREE(transforms_);
        LDELETE_ARRAY(states_);
        skeleton_ = NULL;
    }

    void Animation::create(s16 numClips)
    {
        LASSERT(numClips<=MaxClips);
        LDELETE_ARRAY(states_);
        numStates_ = numClips;
        states_ = LNEW AnimationState[numStates_];
    }

    void Animation::setSkeleton(Skeleton* skeleton)
    {
        LASSERT(NULL != skeleton);

        s32 numTransforms = (NULL != skeleton_)? skeleton_->getNumJoints() : 0;
        if(numTransforms < skeleton->getNumJoints()){
            LFREE(transforms_);
            transforms_ = static_cast<Transform*>(LALIGNED_MALLOC(sizeof(Transform)*skeleton->getNumJoints(), 16));
        }
        skeleton_ = skeleton;
    }

    void Animation::setClip(s16 index, AnimationClip::pointer& clip)
    {
        LASSERT(0<=index && index<numStates_);
        states_[index].setSkeleton(skeleton_);
        states_[index].setClip(clip);
    }

    bool Animation::isInPlay(s16 clip) const
    {
        return states_[clip].checkFlag(AnimationFlag_Play);
    }

    void Animation::play(s16 clip, f32 weight)
    {
        LASSERT(0<=clip && clip<numStates_);
        if(!states_[clip].valid()){
            return;
        }
        states_[clip].setBlendMode(BlendMode_None);
        states_[clip].setWeight(weight);
        pushPlay(clip);
    }

    void Animation::playDelay(s16 clip, f32 delay, f32 weight)
    {
        LASSERT(0<=clip && clip<numStates_);
        LASSERT(LANIM_ANIMATION_EPSILON<=delay);

        if(!states_[clip].valid()){
            return;
        }
        states_[clip].setBlendMode(BlendMode_None);
        states_[clip].setWeight(weight);
        pushDelay(clip, Mode_PlayDelay, delay);
    }

    void Animation::stop(s16 clip)
    {
        LASSERT(0<=clip && clip<numStates_);
        popPlay(clip);
    }

    void Animation::stopDelay(s16 clip, f32 delay)
    {
        LASSERT(0<=clip && clip<numStates_);
        LASSERT(LANIM_ANIMATION_EPSILON<=delay);

        pushDelay(clip, Mode_StopDelay, delay);
    }

    void Animation::stopAll()
    {
        for(s16 i=0; i<numPlays_; ++i){
            states_[play_[i]].unsetFlag(AnimationFlag_Play);
        }
        numPlays_ = 0;
    }

    void Animation::fadeIn(s16 clip, f32 time)
    {
        LASSERT(0<=clip && clip<numStates_);
        LASSERT(LANIM_ANIMATION_EPSILON<=time);
        if(!states_[clip].valid()){
            return;
        }
        states_[clip].setBlendMode(BlendMode_Mix);
        states_[clip].setWeight(0.0f);
        pushDelay(clip, Mode_FadeIn, time);
    }

    void Animation::fadeOut(s16 clip, f32 time)
    {
        LASSERT(0<=clip && clip<numStates_);
        LASSERT(LANIM_ANIMATION_EPSILON<=time);
        if(!states_[clip].valid()){
            return;
        }
        states_[clip].setWeight(1.0f);
        pushDelay(clip, Mode_FadeOut, time);
    }

    void Animation::crossFrade(s16 clip, f32 time)
    {
        LASSERT(0<=clip && clip<numStates_);
        LASSERT(LANIM_ANIMATION_EPSILON<=time);
        if(!states_[clip].valid()){
            return;
        }
        if(states_[clip].checkFlag(AnimationFlag_Play)){
            popPlay(clip);
        }

        for(s32 i=0; i<numPlays_; ++i){
            states_[play_[i]].setWeight(1.0f);
            pushDelay(play_[i], Mode_FadeOut, time);
        }
        states_[clip].setBlendMode(BlendMode_Mix);
        states_[clip].setWeight(0.0f);
        states_[clip].setTime(0.0f);
        pushDelay(clip, Mode_FadeIn, time);
    }

    void Animation::update()
    {
        clearTransforms();

        for(s16 i=0; i<numPlays_;){
            AnimationState& animState = states_[play_[i]];
            if(animState.isEnd()){
                popPlay(play_[i]);
            }else{
                animState.update(transforms_);
                animState.updateTime();
                ++i;
            }
        }

        f32 unitTime = AnimationSystem::getUnitTime();
        for(s16 i=0; i<numDelays_;){
            delay_[i].count_ += unitTime;
            AnimationState& animState = states_[delay_[i].clip_];
            switch(delay_[i].mode_)
            {
            case Mode_PlayDelay:
                if(delay_[i].time_<=delay_[i].count_){
                    animState.update(transforms_);
                    popDelay(delay_[i].clip_);
                    pushPlay(delay_[i].clip_);
                    continue;
                }
                break;

            case Mode_StopDelay:
                if(delay_[i].time_<=delay_[i].count_){
                    popDelay(delay_[i].clip_);
                    popPlay(delay_[i].clip_);
                    continue;
                }
                break;

            case Mode_FadeIn:
                {
                    f32 weight = lcore::clamp01(delay_[i].count_/delay_[i].time_);
                    animState.setWeight(weight);
                    animState.update(transforms_);
                    animState.updateTime();
                    if(delay_[i].time_<=delay_[i].count_){
                        animState.setBlendMode(BlendMode_None);
                        popDelay(delay_[i].clip_);
                        pushPlay(delay_[i].clip_);
                        continue;
                    }
                }
                break;

            case Mode_FadeOut:
                {
                    f32 weight = lcore::clamp01(1.0f - delay_[i].count_/delay_[i].time_);
                    animState.setWeight(weight);
                    if(delay_[i].time_<=delay_[i].count_){
                        popDelay(delay_[i].clip_);
                        popPlay(delay_[i].clip_);
                        continue;
                    }
                }
                break;

            default:
                popDelay(delay_[i].clip_);
                continue;
            }
            ++i;
        }
    }

    void Animation::clearTransforms()
    {
//#if defined(LMATH_USE_SSE)
#if 0
        for(s32 i=0; i<skeleton_->getNumJoints(); ++i){
            const Joint& joint = skeleton_->getJoint(i);
            _mm_store_ps(&transforms_[i].translation_.x_, _mm_loadu_ps(&joint.getTranslation().x_));
            _mm_store_ps(&transforms_[i].rotation_.w_, _mm_loadu_ps(&joint.getRotation().w_));
        }

#elif defined(LMATH_USE_SSE)
        const f32 one = 1.0f;
        lmath::lm128 zero = _mm_setzero_ps();
        lmath::lm128 identity = _mm_load_ss(&one);
        for(s32 i=0; i<skeleton_->getNumJoints(); ++i){
            //const Joint& joint = skeleton_->getJoint(i);
            _mm_store_ps(&transforms_[i].translation_.x_, zero);
            _mm_store_ps(&transforms_[i].rotation_.w_, identity);
        }
#else
        for(s32 i=0; i<skeleton_->getNumJoints(); ++i){
            transforms_[i].translation_.zero();
            transforms_[i].rotation_.identity();
        }
#endif
    }


    void Animation::getMatrices(lmath::Matrix34* matrices)
    {
        for(s32 i=0; i<skeleton_->getNumJoints(); ++i){
            //lanim::Joint& joint = skeleton_->getJoint(i);

            transforms_[i].rotation_.getMatrix( matrices[i] );

            const lmath::Vector4& translate = transforms_[i].translation_;

            matrices[i].translate(translate.x_, translate.y_, translate.z_);

            //if(lanim::InvalidJointIndex == joint.getParentIndex()){
            //    continue;
            //}

            //const lmath::Matrix34& parentMat = matrices[joint.getParentIndex()];

            //matrices[i].mul(parentMat, matrices[i]);
        }
    }
    void Animation::getMatricesAligned16(lmath::Matrix34* matrices)
    {
        for(s32 i=0; i<skeleton_->getNumJoints(); ++i){
            Joint& joint = skeleton_->getJoint(i);

            transforms_[i].rotation_.getMatrix( matrices[i] );

            const lmath::Vector4& translate = transforms_[i].translation_;

            matrices[i].translate(translate.x_, translate.y_, translate.z_);
            matrices[i] *= joint.getInvInitialMatrix();
            matrices[i].mul(joint.getInitialMatrix(), matrices[i]);

            if(InvalidJointIndex == joint.getParentIndex()){
                continue;
            }

            const lmath::Matrix34& parentMat = matrices[joint.getParentIndex()];

            matrices[i].mul(parentMat, matrices[i]);
        }
    }

    void Animation::getDualQuaternion(lmath::DualQuaternion* dq)
    {
        for(s32 i=0; i<skeleton_->getNumJoints(); ++i){
            //lanim::Joint& joint = skeleton_->getJoint(i);
            dq[i].identity();

            dq[i].rotate(transforms_[i].rotation_);

            const lmath::Vector4& translate = transforms_[i].translation_;

            dq[i].translate(translate.x_, translate.y_, translate.z_);

            //if(lanim::InvalidJointIndex == joint.getParentIndex()){
            //    continue;
            //}

            //const lmath::DualQuaternion& parentDq = dq[joint.getParentIndex()];

            //dq[i].mul(dq[i], parentDq);
        }
    }

    void Animation::pushPlay(s16 clip)
    {
        if(states_[clip].isPlay()){
            popPlay(clip);
        }
        if(MaxPlay<=numPlays_){
            states_[play_[0]].unsetFlag(AnimationFlag_Play);
            for(s16 j=1; j<MaxPlay; ++j){
                play_[j-1] = play_[j];
            }
            --numPlays_;
        }
        play_[numPlays_] = clip;
        ++numPlays_;
        states_[clip].setFlag(AnimationFlag_Play);
    }

    void Animation::popPlay(s16 clip)
    {
        for(s16 i=0; i<numPlays_; ++i){
            if(clip == play_[i]){
                for(s16 j=i+1; j<numPlays_; ++j){
                    play_[j-1] = play_[j];
                }
                states_[clip].unsetFlag(AnimationFlag_Play);
                --numPlays_;
                return;
            }
        }
    }

    void Animation::pushDelay(s16 clip, s16 mode, f32 time)
    {
        if(states_[clip].checkFlag(AnimationFlag_Delay)){
            popDelay(clip);
        }
        if(MaxDelay<=numDelays_){
            states_[delay_[0].clip_].unsetFlag(AnimationFlag_Delay);
            for(s16 j=1; j<MaxDelay; ++j){
                delay_[j-1] = delay_[j];
            }
            --numDelays_;
        }
        delay_[numDelays_].mode_ = mode;
        delay_[numDelays_].clip_ = clip;
        delay_[numDelays_].count_ = 0.0f;
        delay_[numDelays_].time_ = time;
        ++numDelays_;
        states_[clip].setFlag(AnimationFlag_Delay);
    }

    void Animation::popDelay(s16 clip)
    {
        for(s16 i=0; i<numDelays_; ++i){
            if(clip == delay_[i].clip_){
                for(s16 j=i+1; j<numDelays_; ++j){
                    delay_[j-1] = delay_[j];
                }
                states_[clip].unsetFlag(AnimationFlag_Delay);
                --numDelays_;
                return;
            }
        }
    }
}

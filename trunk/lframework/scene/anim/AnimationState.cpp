/**
@file Animation.cpp
@author t-sakai
@date 2013/08/02 create
*/
#include "AnimationState.h"
#include "Animation.h"
#include "AnimationClip.h"

namespace lscene
{
namespace lanim
{
    AnimationState::AnimationState()
        :flags_(AnimationFlag_None)
        ,blendMode_(BlendMode_None)
        ,time_(0.0f)
        ,speed_(AnimationSystem::getAnimationSpeed())
        ,totalTime_(0.0f)
        ,weight_(1.0f)
        ,clip_(NULL)
        ,animationToJointIndex_(NULL)
    {
    }

    AnimationState::~AnimationState()
    {
        LSCENE_RELEASE(clip_);
        LSCENE_FREE(animationToJointIndex_);
    }

    void AnimationState::setSkeleton(Skeleton::pointer& skeleton)
    {
        skeleton_ = skeleton;
    }

    void AnimationState::setClip(AnimationClip* clip)
    {
        LASSERT(skeleton_);
        u32 prevNumAnim = (clip_)? clip_->getNumJoints() : 0;

        lscene::move(clip_, clip);

        if(!clip_){
            return;
        }
        if(prevNumAnim<clip_->getNumJoints()){
            LSCENE_FREE(animationToJointIndex_);
            animationToJointIndex_ = reinterpret_cast<u8*>( LSCENE_MALLOC(sizeof(u8)*clip_->getNumJoints()) );
        }

        for(u32 i=0; i<clip_->getNumJoints(); ++i){
            const lanim::Joint* joint = skeleton_->findJoint(clip->getJointAnimation(i).getName());
            if(NULL == joint){
                animationToJointIndex_[i] = LANIM_INVALID_SKINNING_MATRIX;
                continue;
            }
            animationToJointIndex_[i] = skeleton_->calcJointIndex(joint);
        }
        totalTime_ = clip_->getLastTime();
    }

    bool AnimationState::isEnd() const
    {
        if(NULL == clip_ || isLoop()){
            return false;
        }
        return (0.0f<=speed_)? (totalTime_<time_) : (time_<0.0f);
    }

    void AnimationState::setTime(f32 time)
    {
        if(isLoop()){
            time_ = lcore::clampRotate0(time, totalTime_);
        }else{
            time_ = lcore::clamp(time, 0.0f, totalTime_);
        }
    }

    f32 AnimationState::getFrameCount() const
    {
        return time_ * AnimationSystem::getInvUnitTime();
    }

    void AnimationState::updateTime()
    {
        time_ += speed_;
        if(isLoop()){
            time_ = lcore::clampRotate0(time_, totalTime_);

        }else{
            //time_ = lcore::clamp(time_, 0.0f, totalTime_);
        }
    }

namespace
{
    inline void lerp(lmath::lm128& dst, const lmath::Vector3& v0, const lmath::Vector3& v1, f32 t)
    {
        lmath::lm128 tv0 = _mm_loadu_ps(&v0.x_);
        lmath::lm128 tv1 = _mm_loadu_ps(&v1.x_);
        lmath::lm128 t0 = _mm_load1_ps(&t);

        tv1 = _mm_sub_ps(tv1, tv0);
        tv1 = _mm_mul_ps(tv1, t0);
        dst = _mm_add_ps(tv1, tv0);
    }

    inline void normalize(lmath::lm128& v)
    {
        lmath::lm128 l = _mm_mul_ps(v, v);
        l = _mm_add_ps( _mm_shuffle_ps(l, l, 0x4E), l);
        l = _mm_add_ps( _mm_shuffle_ps(l, l, 0xB1), l);

        l = _mm_sqrt_ss(l);
        l = _mm_shuffle_ps(l, l, 0);
        v = _mm_div_ps(v, l);
    }

    inline void lerp(lmath::lm128& dst, const lmath::Quaternion& q0, const lmath::Quaternion& q1, f32 t)
    {
        lmath::Quaternion q;
        q.lerp(q0, q1, t);
        dst = _mm_loadu_ps(&q.w_);
    }

    inline void slerp(lmath::lm128& dst, const lmath::Quaternion& q0, const lmath::Quaternion& q1, f32 t)
    {
        f32 cosOmega = q0.dot(q1);

        lmath::lm128 k0, k1;
        if(cosOmega < -0.8f || 0.8f < cosOmega){
            //”ñí‚É‹ß‚¢ê‡‚ÍüŒ`•âŠÔ‚·‚é
            k1 = _mm_load1_ps(&t);
            t = 1.0f - t;
            k0 = _mm_load1_ps(&t);

        }else{
            f32 sinOmega = lmath::sqrt(1.0f - cosOmega * cosOmega);
            f32 oneOverSinOmega = 1.0f / sinOmega;
            f32 omega = lmath::atan2(sinOmega, cosOmega);

            f32 s0 = lmath::sinf((1.0f-t) * omega) * oneOverSinOmega;
            k0 = _mm_load1_ps(&s0);

            f32 s1 = lmath::sinf(t * omega) * oneOverSinOmega;
            k1 = _mm_load1_ps(&s1);
        }

        lmath::lm128 t0 = _mm_loadu_ps(&q0.w_);
        lmath::lm128 t1 = _mm_loadu_ps(&q1.w_);
        t0 = _mm_mul_ps(k0, t0);
        t1 = _mm_mul_ps(k1, t1);

        dst = _mm_add_ps(t0, t1);
    }

    inline void blendMix(lmath::Vector4& dst, lmath::lm128& v, lmath::lm128& w0, lmath::lm128& w1)
    {
        lmath::lm128 v0 = _mm_load_ps(&dst.x_);
        lmath::lm128 t0 = _mm_mul_ps(v0, w0);
        lmath::lm128 t1 = _mm_mul_ps(v, w1);
        _mm_store_ps(&dst.x_, _mm_add_ps(t0, t1));
        dst.w_ = 0.0f;
    }

    inline void blendMix(lmath::Quaternion& dst, lmath::lm128& v, lmath::lm128& w0, lmath::lm128& w1)
    {
        lmath::lm128 v0 = _mm_load_ps(&dst.w_);
        lmath::lm128 t0 = _mm_mul_ps(v0, w0);
        lmath::lm128 t1 = _mm_mul_ps(v, w1);
        lmath::lm128 d = _mm_add_ps(t0, t1);
        normalize(d);
        _mm_store_ps(&dst.w_, d);
    }

    inline void blendAdd(lmath::Vector4& dst, lmath::lm128& v, lmath::lm128& weight)
    {
        lmath::lm128 v0 = _mm_load_ps(&dst.x_);
        lmath::lm128 t1 = _mm_mul_ps(v, weight);
        _mm_store_ps(&dst.x_, _mm_add_ps(v0, t1));
        dst.w_ = 0.0f;
    }

    inline void blendAdd(lmath::Quaternion& dst, lmath::lm128& v, lmath::lm128& weight)
    {
        lmath::lm128 v0 = _mm_load_ps(&dst.x_);
        lmath::lm128 t1 = _mm_mul_ps(v, weight);
        lmath::lm128 d = _mm_add_ps(v0, t1);
        normalize(d);
        _mm_store_ps(&dst.w_, d);
    }
}

    void AnimationState::updateBlendNone(Transform* transforms)
    {
        LASSERT(NULL != transforms);

        for(u32 i=0; i<clip_->getNumJoints(); ++i){
            u8 jointIndex = animationToJointIndex_[i];
            if(LANIM_INVALID_SKINNING_MATRIX == jointIndex){
                continue;
            }

            const lanim::JointAnimation& jointAnim = clip_->getJointAnimation(i);

            s32 poseIndex = jointAnim.binarySearchIndex(time_);
            s32 nextPoseIndex = jointAnim.getNextIndex(time_, poseIndex);

            const lanim::JointPoseWithTime& jointPose = jointAnim.getPose(poseIndex);
            const lanim::JointPoseWithTime& nextPose = jointAnim.getPose(nextPoseIndex);

            f32 d = nextPose.time_-jointPose.time_;
            f32 t = lmath::isZero(d, LANIM_ANIMATION_TIME_EPSILON)? 0.0f : (time_-jointPose.time_)/d;
            LASSERT(0.0f<=t && t<=1.0f);

#if defined(LMATH_USE_SSE)
            lmath::lm128 tmp;
            lerp(tmp, jointPose.translation_, nextPose.translation_, t);
            _mm_store_ps(&transforms[jointIndex].translation_.x_, tmp);
            transforms[jointIndex].translation_.w_ = 0.0f;

            slerp(tmp, jointPose.rotation_, nextPose.rotation_, t);
            normalize(tmp);
            _mm_store_ps(&transforms[jointIndex].rotation_.w_, tmp);
#else
            lmath::Vector3 tmp;
            tmp.lerp(jointPose.translation_, nextPose.translation_, t);
            transforms[jointIndex].translation_ = tmp;

            transforms[jointIndex].rotation_.slerp(jointPose.rotation_, nextPose.rotation_, t);
#endif
        }
    }

    void AnimationState::updateBlendMix(Transform* transforms)
    {
        LASSERT(NULL != transforms);
#if defined(LMATH_USE_SSE)
        f32 w0 = 1.0f-weight_;
        lmath::lm128 weight0 = _mm_load1_ps(&w0);
        lmath::lm128 weight1 = _mm_load1_ps(&weight_);
#endif

        for(u32 i=0; i<clip_->getNumJoints(); ++i){
            u8 jointIndex = animationToJointIndex_[i];
            if(LANIM_INVALID_SKINNING_MATRIX == jointIndex){
                continue;
            }

            const lanim::JointAnimation& jointAnim = clip_->getJointAnimation(i);

            s32 poseIndex = jointAnim.binarySearchIndex(time_);
            s32 nextPoseIndex = jointAnim.getNextIndex(time_, poseIndex);

            const lanim::JointPoseWithTime& jointPose = jointAnim.getPose(poseIndex);
            const lanim::JointPoseWithTime& nextPose = jointAnim.getPose(nextPoseIndex);

            f32 d = nextPose.time_ - jointPose.time_;
            f32 t = lmath::isZero(d, LANIM_ANIMATION_TIME_EPSILON)? 0.0f : (time_-jointPose.time_)/d;
            LASSERT(0.0f<=t && t<=1.0f);

#if defined(LMATH_USE_SSE)
            lmath::lm128 tmp;
            lerp(tmp, jointPose.translation_, nextPose.translation_, t);
            blendMix(transforms[jointIndex].translation_, tmp, weight0, weight1);

            slerp(tmp, jointPose.rotation_, nextPose.rotation_, t);
            blendMix(transforms[jointIndex].rotation_, tmp, weight0, weight1);
#else
            lmath::Vector3 tmp;
            tmp.lerp(jointPose.translation_, nextPose.translation_, t);
            transforms[jointIndex].translation_ = tmp;

            transforms[jointIndex].rotation_.slerp(jointPose.rotation_, nextPose.rotation_, t);
#endif
        }
    }

    void AnimationState::updateBlendAdd(Transform* transforms)
    {
        LASSERT(NULL != transforms);
#if defined(LMATH_USE_SSE)
        lmath::lm128 weight = _mm_load1_ps(&weight_);
#endif

        for(u32 i=0; i<clip_->getNumJoints(); ++i){
            u8 jointIndex = animationToJointIndex_[i];
            if(LANIM_INVALID_SKINNING_MATRIX == jointIndex){
                continue;
            }

            const lanim::JointAnimation& jointAnim = clip_->getJointAnimation(i);

            s32 poseIndex = jointAnim.binarySearchIndex(time_);
            s32 nextPoseIndex = jointAnim.getNextIndex(time_, poseIndex);

            const lanim::JointPoseWithTime& jointPose = jointAnim.getPose(poseIndex);
            const lanim::JointPoseWithTime& nextPose = jointAnim.getPose(nextPoseIndex);

            f32 d = nextPose.time_ - jointPose.time_;
            f32 t = lmath::isEqual(d, LANIM_ANIMATION_TIME_EPSILON)? 0.0f : (time_-jointPose.time_)/d;
            LASSERT(0.0f<=t && t<=1.0f);

#if defined(LMATH_USE_SSE)
            lmath::lm128 tmp;
            lerp(tmp, jointPose.translation_, nextPose.translation_, t);
            blendAdd(transforms[jointIndex].translation_, tmp, weight);

            slerp(tmp, jointPose.rotation_, nextPose.rotation_, t);
            blendAdd(transforms[jointIndex].rotation_, tmp, weight);
#else
            lmath::Vector3 tmp;
            tmp.lerp(jointPose.translation_, nextPose.translation_, t);
            transforms[jointIndex].translation_ = tmp;

            transforms[jointIndex].rotation_.slerp(jointPose.rotation_, nextPose.rotation_, t);
#endif
        }
    }
}
}

#ifndef INC_LANIM_ANIMATIONCONTROLERIMPL_H__
#define INC_LANIM_ANIMATIONCONTROLERIMPL_H__
/**
@file AnimationControlerImpl.h
@author t-sakai
@date 2010/12/15 create

*/
#include <lframework/anim/AnimationControler.h>
#include <lframework/anim/IKPack.h>

namespace lanim
{
    //-----------------------------------------------
    //---
    //--- AnimCtrlSimpleIK
    //---
    //-----------------------------------------------
    template<class Resource, class Ctrl, class CtrlIK>
    class AnimCtrlSimpleIK : public AnimationControler
    {
    public:
        typedef lframework::Flags Flags;

        typedef Resource resource_type;
        typedef Ctrl ctrl_type;
        typedef CtrlIK ctrl_ik_type;


        virtual ~AnimCtrlSimpleIK()
        {
        }

        virtual void initialize();
        virtual void update();
        virtual void reset(f32 frame);

        /// スケルトンセット
        inline void setSkeleton(Skeleton::pointer& skeleton)
        {
            resource_.setSkeleton(skeleton);
            controler_.setSkeleton(skeleton);
            controlerIK_.setSkeleton(skeleton);

            //更新対象のポーズをセット
            controler_.setSkeletonPose(resource_.getSkeletonPose());
        }

        /// クリップ登録
        inline bool setClip(AnimationClip::pointer& clip)
        {
            return controler_.setClip(clip);
        }

        /// クリップ登録
        inline bool resetClip()
        {
            return controler_.resetClip();
        }

        /// IK登録
        inline bool setIKPack(IKPack::pointer& ik)
        {
            return controlerIK_.setIKPack(ik);
        }

        /// ポーズ取得
        inline const SkeletonPose& getSkeletonPose() const
        {
            return resource_.getSkeletonPose();
        }

        /// フラグセット取得
        inline const Flags& getFlags() const
        {
            return controler_.getFlags();
        }

        /// フラグセット取得
        inline Flags& getFlags()
        {
            return controler_.getFlags();
        }

        resource_type resource_;
        ctrl_type controler_;
        ctrl_ik_type controlerIK_;
    };

    //-----------------------------------------------
    //--- AnimCtrlSimpleIK実装
    //-----------------------------------------------
    template<class Resource, class Ctrl, class CtrlIK>
    void AnimCtrlSimpleIK<Resource, Ctrl, CtrlIK>::initialize()
    {
        controler_.initialize();
        controler_.getPose();
        controler_.updateMatrix();
        controlerIK_.blendPose( resource_.getSkeletonPose() );
    }


    template<class Resource, class Ctrl, class CtrlIK>
    void AnimCtrlSimpleIK<Resource, Ctrl, CtrlIK>::update()
    {
        if(controler_.getClip() != NULL){
            controler_.updateFrame( resource_.getFrameDuration() );

            if(controler_.getFlags().checkFlag(AnimFlag_Loop)){
                controler_.getPoseLoop();
            }else{
                controler_.getPose();
            }

            controler_.updateMatrix();
            controlerIK_.blendPose( resource_.getSkeletonPose() );
        }
    }

    template<class Resource, class Ctrl, class CtrlIK>
    void AnimCtrlSimpleIK<Resource, Ctrl, CtrlIK>::reset(f32 frame)
    {
        if(controler_.getClip() != NULL){
            controler_.setFrame(frame);

            if(controler_.getFlags().checkFlag(AnimFlag_Loop)){
                controler_.getPoseLoop();
            }else{
                controler_.getPose();
            }

            controler_.updateMatrix();
            controlerIK_.blendPose( resource_.getSkeletonPose() );
        }
    }


    //-----------------------------------------------
    //---
    //--- AnimCtrlSimple
    //---
    //-----------------------------------------------
    template<class Resource, class Ctrl>
    class AnimCtrlSimple : public AnimationControler
    {
    public:
        typedef lframework::Flags Flags;

        typedef Resource resource_type;
        typedef Ctrl ctrl_type;


        virtual ~AnimCtrlSimple()
        {
        }

        virtual void initialize();
        virtual void update();
        virtual void reset(f32 frame);

        /// スケルトンセット
        inline void setSkeleton(Skeleton::pointer& skeleton)
        {
            resource_.setSkeleton(skeleton);
            controler_.setSkeleton(skeleton);

            //更新対象のポーズをセット
            controler_.setSkeletonPose(resource_.getSkeletonPose());
        }

        /// クリップ登録
        inline bool setClip(AnimationClip::pointer& clip)
        {
            return controler_.setClip(clip);
        }

        /// クリップ登録
        inline bool resetClip()
        {
            return controler_.resetClip();
        }

        /// ポーズ取得
        inline const SkeletonPose& getSkeletonPose() const
        {
            return resource_.getSkeletonPose();
        }

        /// フラグセット取得
        inline const Flags& getFlags() const
        {
            return controler_.getFlags();
        }

        /// フラグセット取得
        inline Flags& getFlags()
        {
            return controler_.getFlags();
        }

        resource_type resource_;
        ctrl_type controler_;
    };

    //-----------------------------------------------
    //--- AnimCtrlSimpleIK実装
    //-----------------------------------------------
    template<class Resource, class Ctrl>
    void AnimCtrlSimple<Resource, Ctrl>::initialize()
    {
        controler_.initialize();
        controler_.getPose();
        controler_.updateMatrix();
    }


    template<class Resource, class Ctrl>
    void AnimCtrlSimple<Resource, Ctrl>::update()
    {
        if(controler_.getClip() != NULL){
            controler_.updateFrame( resource_.getFrameDuration() );

            if(controler_.getFlags().checkFlag(AnimFlag_Loop)){
                controler_.getPoseLoop();
            }else{
                controler_.getPose();
            }

            controler_.updateMatrix();
        }
    }

    template<class Resource, class Ctrl>
    void AnimCtrlSimple<Resource, Ctrl>::reset(f32 frame)
    {
        if(controler_.getClip() != NULL){
            controler_.setFrame(frame);

            if(controler_.getFlags().checkFlag(AnimFlag_Loop)){
                controler_.getPoseLoop();
            }else{
                controler_.getPose();
            }

            controler_.updateMatrix();
        }
    }
}

#endif //INC_LANIM_ANIMATIONCONTROLERIMPL_H__

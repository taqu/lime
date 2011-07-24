/**
@file AnimationControlerPartial.cpp
@author t-sakai
@date 2010/11/17 create
@date 2011/04/07 更新単位を整数化。フレーム検索方法を汎用アルゴリズムから一時変更
*/
#include <lmath/lmath.h>
#include "AnimationControlerPartial.h"
#include "AnimationClip.h"

namespace lanim
{
    //--------------------------------------------------------------
    //---
    //--- AnimationControlerPartial
    //---
    //--------------------------------------------------------------
    AnimationControlerPartial::AnimationControlerPartial()
        :frame_(0.0f)
        ,blendRate_(1.0f)
        ,skeletonPose_(NULL)
    {
        flags_.set( AnimFlag_Active );
    }

    AnimationControlerPartial::AnimationControlerPartial(const AnimationControlerPartial& rhs)
        :flags_(rhs.flags_)
        ,skeleton_(rhs.skeleton_)
        ,frame_(rhs.frame_)
        ,blendRate_(rhs.blendRate_)
        ,clip_(rhs.clip_)
        ,jointMap_(rhs.jointMap_)
        ,skeletonPose_(NULL)
    {
    }

    AnimationControlerPartial::~AnimationControlerPartial()
    {
        skeletonPose_ = NULL;
        clip_ = NULL;
        skeleton_ = NULL;
    }

    AnimationControlerPartial& AnimationControlerPartial::operator=(const AnimationControlerPartial& rhs)
    {
        frame_ = rhs.frame_;
        blendRate_ = rhs.blendRate_;
        clip_ = rhs.clip_;
        flags_ = rhs.flags_;

        jointMap_.clear();
        jointMap_.reserve( rhs.jointMap_.size() );

        for(u32 i=0; i<rhs.jointMap_.size(); ++i){
            jointMap_.push_back(rhs.jointMap_[i]);
        }
        return *this;
    }

    // クリップ登録
    bool AnimationControlerPartial::resetClip()
    {
        LASSERT(skeleton_ != NULL);
        LASSERT(clip_ != NULL);

        s32 numClipJoints = clip_->getNumJoints();

        jointMap_.clear();
        jointMap_.setIncSize(skeleton_->getNumJoints());

        const Joint* joint = NULL;

        JointMap map;

        //クリップの間接とスケルトンの間接を対応させる
        for(s32 i=0; i<numClipJoints; ++i){
            const Name& name = clip_->getJointAnimation(i).getName();

            joint = skeleton_->findJoint(name);
            if(joint != NULL){
                map.frame_ = 0;
                map.joint_ = skeleton_->calcJointIndex(joint);
                map.clipJoint_ = i;
                jointMap_.push_back(map);
            }
        }
        return true;
    }

    // フレーム時間更新
    void AnimationControlerPartial::updateFrame(f32 duration)
    {
        frame_ += duration;
        u32 last = static_cast<u32>( clip_->getLastFrame() );
        if(frame_>last){
            if(flags_.checkFlag(AnimFlag_AutoDisactive)){
                frame_ = 0.0f;
                flags_.resetFlag(AnimFlag_Active);
                //ボーン初期化しておく
                initialize();
            }else{
                if(flags_.checkFlag(AnimFlag_Loop)){
                    last += 1;
                    if(frame_>=last){
                        frame_ -= last;
                        //ボーン初期化しておく
                        initialize();
                    }
                }else{
                    frame_ = last;
                }
            }
        }
    }

    void AnimationControlerPartial::initialize()
    {
        LASSERT(skeletonPose_ != NULL);
        SkeletonPose& pose = *skeletonPose_;

        JointPose* poses = pose.getPoses();
        lmath::Matrix34* matrices = pose.getMatrices();

        lmath::Vector3 tmp;
        for(u32 i=0; i<pose.getNumJoints(); ++i){
            Joint& joint = skeleton_->getJoint(i);

            JointPose& jointPose = poses[i];
            jointPose.rotation_.identity();
            jointPose.translation_.set(0.0f, 0.0f, 0.0f);

            u16 parentIndex = joint.getParentIndex();
            if(parentIndex != InvalidJointIndex){
                //親ジョイント有
                Joint& parentJoint = skeleton_->getJoint( joint.getParentIndex() );

                jointPose.offset_ = joint.getPosition();
                jointPose.offset_ -= parentJoint.getPosition();
                jointPose.offset_ += poses[parentIndex].offset_;
            }else{
                //親ジョイント無
                jointPose.offset_ = joint.getPosition();
            }

            //パレットの行列更新
            tmp = -joint.getPosition();
            tmp += jointPose.offset_;
            matrices[i].identity();
            matrices[i].translate(tmp);
        }
#if 0
        for(u32 i=0; i<jointMap_.size(); ++i){
            JointMap& map = jointMap_[i];

            LASSERT(0<=map.joint_ && map.joint_<skeleton_->getNumJoints());
            LASSERT(0<=map.clipJoint_ && map.clipJoint_<clip_->getNumJoints());

            JointAnimation& jointAnim = clip_->getJointAnimation( map.clipJoint_ );
            jointAnim.initialize();
        }
#endif
    }

    void AnimationControlerPartial::getPose()
    {
        LASSERT(skeletonPose_ != NULL);
        SkeletonPose& pose = *skeletonPose_;

        JointPose* poses = pose.getPoses();


        for(u32 i=0; i<jointMap_.size(); ++i){
            JointMap& map = jointMap_[i];

            LASSERT(0<=map.joint_ && map.joint_<skeleton_->getNumJoints());
            LASSERT(0<=map.clipJoint_ && map.clipJoint_<clip_->getNumJoints());

            JointAnimation& jointAnim = clip_->getJointAnimation( map.clipJoint_ );
            JointPose& jointPose = poses[ map.joint_ ];
            Joint& joint = skeleton_->getJoint( map.joint_ );


            if(joint.getType() == lanim::JointType_FreeRot){
                //先端からの捻りの影響を受ける
            }else{
                //普通にスキニング
                u32 frame = frame_;
                u32 animIndex = jointAnim.binarySearchIndex(frame);
                //u32 animIndex = jointAnim.getNextIndex(frame_);
                const JointPoseWithFrame& animPose = jointAnim.getPose(animIndex);

                if(animIndex == jointAnim.getNumPoses() - 1){
                    //ジョイントアニメーションの中で最後のフレーム
                    jointPose.translation_ = animPose.translation_;
                    jointPose.rotation_ = animPose.rotation_;
                }else{
                    //補間する

                    //次のフレームのポーズ
                    const JointPoseWithFrame& nextPose = jointAnim.getPose(animIndex+1);
                    LASSERT(nextPose.frameNo_ != animPose.frameNo_); //データコンバート、ロード時にチェックする

                    f32 blend = (frame_ - static_cast<f32>(animPose.frameNo_));
                    blend /= static_cast<f32>(nextPose.frameNo_ - animPose.frameNo_);

                    jointPose.translation_.lerp(animPose.translation_, nextPose.translation_, blend);

                    jointPose.rotation_.slerp(animPose.rotation_, nextPose.rotation_, blend);
                } //if(animIndex ==
            } //if(joint.getType()

            LASSERT(false == jointPose.translation_.isNan());
            LASSERT(false == jointPose.rotation_.isNan());
        } //for(u32 i=0;
    }

    void AnimationControlerPartial::getPoseLoop()
    {
    }

    void AnimationControlerPartial::blendPose()
    {
    }

    void AnimationControlerPartial::blendPoseLoop()
    {
    }

    void AnimationControlerPartial::updateMatrix()
    {
        LASSERT(skeletonPose_ != NULL);
        SkeletonPose& pose = *skeletonPose_;

        JointPose* poses = pose.getPoses();
        lmath::Matrix34* matrices = pose.getMatrices();

        for(u32 i=0; i<pose.getNumJoints(); ++i){

            JointPose& jointPose = poses[i];
            Joint& joint = skeleton_->getJoint(i);

            jointPose.rotation_.getMatrix( matrices[i] );

            matrices[i].preTranslate( -joint.getPosition() );
            matrices[i].translate( jointPose.translation_ );
            matrices[i].translate( jointPose.offset_ );

            if(joint.getParentIndex() != lanim::InvalidJointIndex){
                //親有
                matrices[i].mul( matrices[ joint.getParentIndex() ], matrices[i]);
            }

            LASSERT(false == matrices[i].isNan());
        }
    }
}

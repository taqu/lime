/**
@file AnimationControlerIK.cpp
@author t-sakai
@date 2010/12/30
*/
#include <lmath/lmath.h>
#include <lmath/utility.h>
#include <lframework/System.h>
#include "AnimationControlerIK.h"

#define LIME_ANIM_IK_DEBUG (1)

namespace lanim
{
    f32 AnimationControlerIK::IKEpsilon = 1.0e-5f;

#if defined(LIME_ANIM_IK_DEBUG)
    inline void drawDebugTriangle(const lmath::Vector3& pos, u32 color)
    {
        static const f32 unit_leng = 0.5f;

        lmath::Vector3 v1(pos._x-unit_leng, pos._y+unit_leng, pos._z);
        lmath::Vector3 v2(pos._x+unit_leng, pos._y+unit_leng, pos._z);
        lframework::System::debugDraw(pos, v1, v2, color);
    }
#endif

    AnimationControlerIK::AnimationControlerIK()
    {
    }

    AnimationControlerIK::~AnimationControlerIK()
    {
    }

    namespace
    {
        inline void transformPosition(lmath::Vector3& dst, const lmath::Vector3& src, lmath::Matrix43 *matrices, s32 index)
        {
            dst.mul(src, matrices[index]);
        }

        /**
        @brief 長さチェックして正規化
        @return 長さが0ならfalse
        */
        inline bool normalizeVec(lmath::Vector3& dst, const lmath::Vector3& src)
        {
            f32 l = src.lengthSqr();
            if(lmath::isEqual(l, 0.0f)){
                return false;
            }
            l = lmath::rsqrt_22bit(l);
            dst.set( src._x*l, src._y*l, src._z*l);
            return true;
        }
    }

    void AnimationControlerIK::blendPose(SkeletonPose& pose)
    {
        //TODO:物理演算対応
        if(ikPack_ == NULL){
            return;
        }

        JointPose* poses = pose.getPoses();
        lmath::Matrix43 *matrices = pose.getMatrices();
        lmath::Matrix43 matInv;
        lmath::Quaternion rotation;
        lmath::Vector3 targetPos, effectorPos;
        lmath::Vector3 localTargetPos, localEffectorPos;

        lmath::Vector3 localToTarget, localToEffector;
        lmath::Vector3 axis;

        f32 totalHeading = 0.0f;

        for(u32 i=0; i<ikPack_->getNumIKs(); ++i){
            IKEntry& entry = ikPack_->get(i);
#if 0
            if(entry.joint_ >= pose.getNumJoints()){
                continue;
            }
            if(entry.targetJoint_>=pose.getNumJoints()){
                continue;
            }
#endif

            Joint& target = skeleton_->getJoint( entry.joint_ );
            Joint& effector = skeleton_->getJoint( entry.targetJoint_ );

            //ターゲットの現在位置
            transformPosition(targetPos, target.getPosition(), matrices, entry.joint_);
            
#if defined(LIME_ANIM_IK_DEBUG)
            drawDebugTriangle(targetPos, 0xFF00FF00U);
#endif

            totalHeading = 0.0f; //曲げ角初期化
            for(u32 j=0; j<entry.numIterations_; ++j){ //イテレーション
                for(u32 k=0; k<entry.chainLength_; ++k){
                    LASSERT(entry.children_[k]<pose.getNumJoints());
#if 0
                    if(entry.children_[k]>=pose.getNumJoints()){
                        continue;
                    }
#endif
                    u8 childIndex = entry.children_[k];
                    Joint& child = skeleton_->getJoint(childIndex);

                    //エフェクタ現在の位置計算
                    transformPosition(effectorPos, effector.getPosition(), matrices, entry.targetJoint_);
#if 0
                    if(effectorPos.isNan() || targetPos.isNan()){
                        LASSERT(false);
                    }
#endif
                    //エフェクタに近ければなにもしない
                    f32 l = effectorPos.distance(targetPos);
                    if(l <= IKEpsilon){
                        j = entry.numIterations_; //イテレーションも終了
                        break;
                    }

                    //エフェクタ、ターゲットをジョイントローカルへ
                    matInv = matrices[childIndex];
                    matInv.preTranslate( (skeleton_->getJoint(childIndex).getPosition()) );
                    matInv.invert();

                    localTargetPos.mul(targetPos, matInv);
                    localEffectorPos.mul(effectorPos, matInv);

                    //原点からの距離が0ならなにもしない
                    if(false == normalizeVec(localToTarget, localTargetPos)){
                        continue;
                    }

                    if(false == normalizeVec(localToEffector, localEffectorPos)){
                        continue;
                    }

                    //回転軸計算
                    axis.cross(localToEffector, localToTarget);
                    if(false == normalizeVec(axis, axis)){
                        continue;
                    }

                    f32 dotProduct = localToTarget.dot(localToEffector);
                    f32 radian;

                    static const f32 IKThreshold = (1.0f-IKEpsilon);
                    if(dotProduct>IKThreshold){
                        radian = 0.0f;
                    }else if(dotProduct< -IKThreshold){
                        radian = lmath::acos(-IKThreshold);
                    }else{
                        radian = lmath::acos(dotProduct);
                    }

                    LASSERT(false == lcore::isNan(radian));

                    if(entry.limitAngle_ < radian){
                        radian = entry.limitAngle_;
                    }


                    if(child.getFlag() & lanim::JointFlag_IKLimitHeading){
                        rotation.setRotateAxis(axis, radian);

                        f32 head, pitch, bank;
                        lmath::getEulerObjectToInertial(head, pitch, bank, rotation);

                        if(totalHeading > -head){ //プラス方向に曲がらないように
                            head = -totalHeading;
                        }
                        rotation.setRotateAxis(1.0f, 0.0f, 0.0f, head);
                        totalHeading += head;
                    }else{
                        rotation.setRotateAxis(axis, radian);
                    }

                    LASSERT(false == rotation.isNan());
                    poses[childIndex].rotation_.mul(rotation, poses[childIndex].rotation_);
                    poses[childIndex].rotation_.normalize();


                    //TODO:再帰を無くすorスタック消費を抑える
                    updateMatrices(pose, childIndex);
                }// for(u32 k=0; k<entry.chainLength_
            }// for(u32 j=0; j<entry.numIterations_

#if defined(LIME_ANIM_IK_DEBUG)
            //ターゲットの現在位置
            transformPosition(targetPos, target.getPosition(), matrices, entry.targetJoint_);

            drawDebugTriangle(targetPos, 0xFFFF0000U);

            lmath::Vector3 childPos;
            for(u32 k=0; k<entry.chainLength_; ++k){
                u8 childIndex = entry.children_[k];
                Joint& child = skeleton_->getJoint(childIndex);

                transformPosition(childPos, child.getPosition(), matrices, childIndex);

                drawDebugTriangle(childPos, 0xFFFFFFFFU);
            }
#endif
        }// for(u32 i=0; i<ikPack_->getNumIKs()

    }


    inline void AnimationControlerIK::updateMatrices(SkeletonPose& pose, u8 childIndex)
    {
        JointPose* poses = pose.getPoses();
        lmath::Matrix43 *matrices = pose.getMatrices();

        JointPose& jointPose = poses[childIndex];
        Joint& joint = skeleton_->getJoint(childIndex);

        jointPose.rotation_.getMatrix( matrices[ childIndex ] );
        matrices[ childIndex ].translate( jointPose.translation_ );
        matrices[ childIndex ].translate( jointPose.offset_ );

        if(joint.getParentIndex() != lanim::InvalidJointIndex){
            //親有
            matrices[ childIndex ] *= matrices[ joint.getParentIndex() ];
        }

        //パレットの行列更新
        matrices[ childIndex ].preTranslate( -joint.getPosition() );

        //子も更新
        for(u32 i=childIndex+1; i<pose.getNumJoints(); ++i){
            Joint& joint = skeleton_->getJoint(i);
            if(joint.getParentIndex() == childIndex){
                updateMatrices(pose, i); //再帰
            }
        }
    }
}

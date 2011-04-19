/**
@file AnimationControlerIK.cpp
@author t-sakai
@date 2010/12/30
*/
#include <lmath/lmath.h>
#include <lmath/utility.h>
#include <lframework/System.h>
#include "AnimationControlerIK.h"

//#define LIME_ANIM_IK_DEBUG (1)

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
        inline bool normalizeVec(lmath::Vector3& dst, const lmath::Vector3& src, f32 length)
        {
            if(lmath::isEqual(length, 0.0f)){
                return false;
            }
            f32 l = lmath::rsqrt_22bit(length);
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
        f32 ltot, ltoe;

        f32 totalXRot = 0.0f;

        for(u32 i=0; i<ikPack_->getNumIKs(); ++i){
            IKEntry& entry = ikPack_->get(i);

            Joint& target = skeleton_->getJoint( entry.targetJoint_ );
            Joint& effector = skeleton_->getJoint( entry.effectorJoint_ );

            //const lanim::Name& effectorName = skeleton_->getJointName( entry.effectorJoint_);
            //const lanim::Name& targetName = skeleton_->getJointName( entry.targetJoint_);

            //ターゲットの現在位置
            transformPosition(targetPos, target.getPosition(), matrices, entry.targetJoint_);

            totalXRot = 0.0f; //曲げ角初期化
            for(u32 j=0; j<entry.numIterations_; ++j){ //イテレーション
                for(u16 k=0; k<entry.chainLength_; ++k){
                    LASSERT(entry.children_[k]<pose.getNumJoints());

                    u8 childIndex = entry.children_[k];
                    Joint& child = skeleton_->getJoint(childIndex);

                    //エフェクタ現在の位置計算
                    transformPosition(effectorPos, effector.getPosition(), matrices, entry.effectorJoint_);

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
                    ltot = localTargetPos.lengthSqr();
                    if(false == normalizeVec(localToTarget, localTargetPos, ltot)){
                        continue;
                    }
                    ltot = lmath::sqrt(ltot);

                    ltoe = localEffectorPos.lengthSqr();
                    if(false == normalizeVec(localToEffector, localEffectorPos, ltoe)){
                        continue;
                    }
                    ltoe = lmath::sqrt(ltoe);

                    //回転軸計算
                    axis.cross(localToEffector, localToTarget);
                    normalizeVec(axis, axis, axis.lengthSqr());

                    f32 dotProduct = localToTarget.dot(localToEffector);
                    f32 radian;

                    static const f32 IKThreshold = (1.0f-IKEpsilon);
                    if(dotProduct>IKThreshold){
                        continue;
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

                        //エフェクタの方が遠くにある場合、曲げをプラス
                        //if(ltot<ltoe){
                        //    f32 cs = ltot/ltoe;//ltot/ltoeは、およそcosになるのではないかな
                        //    pitch -= lmath::acos(cs);
                        //}

                        f32 sum = totalXRot + pitch;

                        if(sum > 0.0f){ //プラス方向に曲がらないように
                            pitch = -totalXRot;
                        }else if(sum<(-PI)){
                            pitch = -entry.limitAngle_ - totalXRot;
                        }

                        rotation.setRotateAxis(1.0f, 0.0f, 0.0f, pitch);

                        totalXRot += pitch;

                    }else{
                        rotation.setRotateAxis(axis, radian);
                    }

                    LASSERT(false == rotation.isNan());
                    poses[childIndex].rotation_.mul(rotation, poses[childIndex].rotation_);
                    poses[childIndex].rotation_.normalize();


                    updateMatrices(pose, entry, k);

                }// for(u32 k=0; k<entry.chainLength_
            }// for(u32 j=0; j<entry.numIterations_

            updateChildMatrices(pose, entry.effectorJoint_);
#if defined(LIME_ANIM_IK_DEBUG)
            //ターゲットの現在位置
            transformPosition(targetPos, target.getPosition(), matrices, entry.targetJoint_);

            drawDebugTriangle(targetPos, 0xFF0000FFU);

            transformPosition(effectorPos, effector.getPosition(), matrices, entry.effectorJoint_);

            drawDebugTriangle(effectorPos, 0x9000FF00U);

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


    void AnimationControlerIK::updateMatrix(const JointPose& jointPose, lmath::Matrix43* matrices, u8 jointIndex)
    {
        lmath::Matrix43& mat = matrices[ jointIndex ];

        jointPose.rotation_.getMatrix( mat );
        mat.translate( jointPose.translation_ );
        mat.translate( jointPose.offset_ );

        const Joint& joint = skeleton_->getJoint(jointIndex);

        //親有なら親のマトリックスかける
        if(joint.getParentIndex() != lanim::InvalidJointIndex){
            mat *= matrices[ joint.getParentIndex() ];
        }

        mat.preTranslate( -joint.getPosition() );
    }

    void AnimationControlerIK::updateMatrices(SkeletonPose& pose, u8 childIndex)
    {
        JointPose* poses = pose.getPoses();
        lmath::Matrix43 *matrices = pose.getMatrices();

        const JointPose& jointPose = poses[childIndex];

        //パレットの行列更新
        updateMatrix(jointPose, matrices, childIndex);

        //子も更新
        for(u8 i=childIndex+1; i<pose.getNumJoints(); ++i){
            Joint& joint = skeleton_->getJoint(i);
            if(joint.getParentIndex() == childIndex){
                updateMatrices(pose, i); //再帰
            }
        }
    }

    void AnimationControlerIK::updateChildMatrices(SkeletonPose& pose, u8 jointIndex)
    {
        //TODO:再帰を無くすorスタック消費を抑える
        JointPose* poses = pose.getPoses();
        lmath::Matrix43 *matrices = pose.getMatrices();

        //子更新
        for(u8 i=jointIndex+1; i<pose.getNumJoints(); ++i){
            Joint& joint = skeleton_->getJoint(i);

            if(joint.getParentIndex() == jointIndex){
                //パレットの行列更新
                updateMatrix(poses[i], matrices, i);

                updateChildMatrices(pose, i); //再帰
            }
        }
    }

    inline void AnimationControlerIK::updateMatrices(SkeletonPose& pose, const IKEntry& entry, u16 ikIndex)
    {
        JointPose* poses = pose.getPoses();
        lmath::Matrix43 *matrices = pose.getMatrices();


        for(s32 i=ikIndex; i>=0; --i){
            u8 childIndex = entry.children_[i];

            //パレットの行列更新
            updateMatrix(poses[childIndex], matrices, childIndex);
        }

        {
            u16 jointIndex = entry.effectorJoint_;

            //パレットの行列更新
            updateMatrix(poses[jointIndex], matrices, jointIndex);
        }

    }
}

/**
@file AnimationControlerIK.cpp
@author t-sakai
@date 2010/12/30 create
@date 2011/05/02 足IKを専用処理に変更
*/
#include <lmath/lmath.h>
#include <lmath/utility.h>
#include <System.h>
#include "AnimationControlerIK.h"

//#define LIME_ANIM_IK_DEBUG (1)

namespace lanim
{
    f32 AnimationControlerIK::IKEpsilon = 1.0e-4f;
    f32 AnimationControlerIK::IKKneeLimit = -PI*0.9f;
    f32 AnimationControlerIK::IKKneeStepLimit = -PI_2*1.2f;

#if defined(LIME_ANIM_IK_DEBUG)
    inline void drawDebugTriangle(const lmath::Vector3& pos, u32 color)
    {
        static const f32 unit_leng = 0.5f;

        lmath::Vector3 v1(pos._x-unit_leng, pos._y+unit_leng, pos._z);
        lmath::Vector3 v2(pos._x+unit_leng, pos._y+unit_leng, pos._z);
        lframework::System::debugDraw(pos, v1, v2, color);
    }
#endif

namespace
{
    /**
    @brief ３辺の長さから対角の角度を計算
    @ret bcに挟まれた角の角度
    */
    void calcAnglesFromSides(f32& angle, f32 a2, f32 b2, f32 c2)
    {
        LASSERT(a2>0.0f);
        LASSERT(b2>0.0f);
        LASSERT(c2>0.0f);

        f32 a = lmath::sqrt(a2);
        f32 b = lmath::sqrt(b2);
        f32 c = lmath::sqrt(c2);

        f32 s = (a + b + c)*0.5f;
        f32 S = lmath::sqrt(s*(s-a)*(s-b)*(s-c));
        f32 h = 2.0f*S/(b*b);

        if(h>1.0f){
            h = 1.0f;
        }
        angle = lmath::asin(h);

        if(a2<(b2+c2)){ //鋭角
            angle = PI - angle;
        }
    }
}

    AnimationControlerIK::AnimationControlerIK()
    {
    }

    AnimationControlerIK::~AnimationControlerIK()
    {
    }

    namespace
    {
        inline void transformPosition(lmath::Vector3& dst, const lmath::Vector3& src, lmath::Matrix34 *matrices, s32 index)
        {
            dst.mul(matrices[index], src);
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
            dst.set( src.x_*l, src.y_*l, src.z_*l);
            return true;
        }
    }

    /// 膝の初期化
    f32 AnimationControlerIK::preprocess(SkeletonPose& pose, IKEntry& entry, u16 chainIndex)
    {
        if( chainIndex>=(entry.chainLength_-1) ){
            return 0.0f;
        }

        lmath::Matrix34 *matrices = pose.getMatrices();

        lmath::Vector3 targetPos, effectorPos, jointPos, parentPos;

        u8 jointIndex = entry.children_[chainIndex];
        u8 parentIndex = entry.children_[ chainIndex+1 ];

        Joint& target = skeleton_->getJoint( entry.targetJoint_ );
        Joint& effector = skeleton_->getJoint( entry.effectorJoint_ );
        Joint& joint = skeleton_->getJoint(jointIndex);
        Joint& parent = skeleton_->getJoint( parentIndex );

        //ターゲットの現在位置
        transformPosition(targetPos, target.getPosition(), matrices, entry.targetJoint_);

        //エフェクタの現在位置
        transformPosition(effectorPos, effector.getPosition(), matrices, entry.effectorJoint_);

        //ジョイント位置計算
        transformPosition(jointPos, joint.getPosition(), matrices, jointIndex);

        //親の現在位置
        transformPosition(parentPos, parent.getPosition(), matrices, parentIndex);

        f32 lengthParentToEffector = parentPos.distanceSqr( effectorPos );
        if(lengthParentToEffector < IKEpsilon){
            return 0.0f;
        }
        lengthParentToEffector = lmath::sqrt(lengthParentToEffector);

        f32 lengthParentToTarget = parentPos.distanceSqr( targetPos );
        if(lengthParentToTarget < IKEpsilon){
            return 0.0f;
        }
        lengthParentToTarget = lmath::sqrt(lengthParentToTarget);

        targetPos -= parentPos;
        effectorPos -= parentPos;

        f32 cs = targetPos.dot(effectorPos) / (lengthParentToEffector*lengthParentToTarget);

        f32 radian;
        if(cs>1.0f){
            radian = 0.0f;
        }else if(cs<0.0f){
            radian = -PI_2;
        }else{
            radian = -lmath::acos(cs);
        }

        return radian;
    }


    void AnimationControlerIK::blendPose(SkeletonPose& pose)
    {
        //TODO:物理演算対応
        if(ikPack_ == NULL){
            return;
        }

        JointPose* poses = pose.getPoses();
        lmath::Matrix34 *matrices = pose.getMatrices();
        lmath::Matrix34 matInv;
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

            //ターゲットの現在位置
            transformPosition(targetPos, target.getPosition(), matrices, entry.targetJoint_);

            totalXRot = 0.0f; //曲げ角初期化
            for(u16 j=0; j<entry.numIterations_; ++j){ //イテレーション
                for(u16 k=0; k<entry.chainLength_; ++k){
                    LASSERT(entry.children_[k]<pose.getNumJoints());

                    u8 childIndex = entry.children_[k];
                    Joint& child = skeleton_->getJoint(childIndex);

                    //エフェクタの現在位置
                    transformPosition(effectorPos, effector.getPosition(), matrices, entry.effectorJoint_);

                    //エフェクタに近ければなにもしない
                    f32 l = effectorPos.distanceSqr(targetPos);
                    if(l <= IKEpsilon){
                        j = entry.numIterations_; //イテレーションも終了
                        break;
                    }

                    //エフェクタ、ターゲットをジョイントローカルへ
                    matInv = matrices[childIndex];
                    matInv.preTranslate( child.getPosition() );
                    matInv.invert();

                    localTargetPos.mul(matInv, targetPos);
                    localEffectorPos.mul(matInv, effectorPos);

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
                    if(dotProduct>1.0f){
                        radian = 0.0f;

                    }else if(dotProduct< -IKThreshold){
                        radian = lmath::acos(-IKThreshold);
                    }else{
                        radian = lmath::acos(dotProduct);
                    }

                    LASSERT(false == lcore::isNan(radian));

                    if(child.getFlag() & lanim::JointFlag_IKLimitHeading){

                        rotation.setRotateAxis(axis, radian);

                        f32 head, pitch, bank;
                        if(j == 0){
                            pitch = preprocess(pose, entry, k);
                        }else{
                            lmath::getEulerObjectToInertial(head, pitch, bank, rotation);
                        }

                        if(ltot < ltoe){
                            pitch -= 2.0f*lmath::acos(ltot/ltoe);
                        }
                        if(pitch<IKKneeStepLimit){
                            pitch = IKKneeStepLimit;
                        }

                        f32 sum = totalXRot + pitch;

                        if(sum > 0.0f){ //プラス方向に曲がらないように
                            pitch = -totalXRot;
                        }else if(sum<IKKneeLimit){
                            pitch = IKKneeLimit - totalXRot;
                        }
                        rotation.setRotateAxis(1.0f, 0.0f, 0.0f, pitch);

                        totalXRot += pitch;


                    }else{
                        if(radian>entry.limitAngle_){
                            radian = entry.limitAngle_;
                        }
                        rotation.setRotateAxis(axis, radian);
                    }

                    LASSERT(false == rotation.isNan());
                    //poses[childIndex].rotation_ *= rotation;
                    poses[childIndex].rotation_.mul(rotation, poses[childIndex].rotation_);
                    poses[childIndex].rotation_.normalize();


                    updateMatrices(pose, entry, k);

                }// for(u32 k=0; k<entry.chainLength_
            }// for(u32 j=0; j<entry.numIterations_

            updateChildMatrices(pose, entry.effectorJoint_);
        }// for(u32 i=0; i<ikPack_->getNumIKs()

    }

    void AnimationControlerIK::updateMatrix(const JointPose& jointPose, lmath::Matrix34* matrices, u8 jointIndex)
    {
        lmath::Matrix34& mat = matrices[ jointIndex ];
        const Joint& joint = skeleton_->getJoint(jointIndex);

        jointPose.rotation_.getMatrix( mat );

        mat.preTranslate( -joint.getPosition() );
        mat.translate( jointPose.translation_ );
        mat.translate( jointPose.offset_ );



        //親有なら親のマトリックスかける
        if(joint.getParentIndex() != lanim::InvalidJointIndex){
            mat.mul( matrices[ joint.getParentIndex() ], mat );
        }
    }

    void AnimationControlerIK::updateMatrices(SkeletonPose& pose, u8 childIndex)
    {
        JointPose* poses = pose.getPoses();
        lmath::Matrix34 *matrices = pose.getMatrices();

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
        lmath::Matrix34 *matrices = pose.getMatrices();

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
        lmath::Matrix34 *matrices = pose.getMatrices();


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

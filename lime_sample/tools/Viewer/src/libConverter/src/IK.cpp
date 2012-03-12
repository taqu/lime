/**
@file IK.cpp
@author t-sakai
@date 2009/11/22
*/
#include "stdafx.h"
#include "IK.h"
#include <lmath/lmath.h>
#include <lmath/utility.h>

#include <lmath/DualQuaternion.h>
#include <lgraphics/scene/AnimTree.h>
#include <lgraphics/scene/AnimObject.h>
#include <lgraphics/scene/Bone.h>

#include "loader/Pmd.h"

namespace ltools
{
    using namespace lcore;
    using namespace lgraphics;
    using namespace lmath;

    struct IKNode
    {
        lmath::Vector3 position_;
        lmath::Quaternion rotation_;

        /// オブジェクトローカル座標からボーン座標への変換
        lmath::Matrix43 boneToObject_;

        Matrix43* node_;

        bool angleLimit_;

    };

    namespace
    {
        enum NodeName
        {
            Node_LeftHair1 =0,
            Node_RightHair1,
            Node_LeftKnee,
            Node_LeftLeg,
            Node_RightKnee,
            Node_RightLeg,
            Node_LeftAnkle,
            Node_RightAnkle,
            NodeName_Num,
        };

        static const char* NodeNameTable[NodeName_Num] =
        {
            "左髪１",
            "右髪１",
            "左ひざ",
            "左足",
            "右ひざ",
            "右足",
            "左足首",
            "右足首",
        };


        AnimTree* searchAnimTreeByBone(Bone* bone, AnimTree& node)
        {
            if(node.getElement().getBone() == bone){
                return &node;
            }

            for(u32 i=0; i<node.getNumChildren(); ++i){
                AnimTree *ret = searchAnimTreeByBone(bone, node.getChild(i));
                if(ret != NULL){
                    return ret;
                }
            }
            return NULL;
        }
    }

    IKChain::IKChain()
        :length_(-1)
        ,numIterations_(0)
        ,weight_(0.0f)
        ,nodes_(NULL)
        ,targetNode_(NULL)
        ,targetIndex_(-1)
        ,effectorIndex_(-1)
    {
    }


    IKChain::~IKChain()
    {
        LIME_DELETE_ARRAY(nodes_);
    }

    void IKChain::set(u32 length, u32 effector, u32 target, u32 iterations, f32 weight)
    {
        LASSERT(length<=MAX_LENGTH);

        length_ = length;
        effectorIndex_ = effector;
        targetIndex_ = target;
        numIterations_ = iterations;
        weight_ = weight;

        LIME_DELETE_ARRAY(nodes_);
        nodes_ = LIME_NEW IKNode[length_+1];
    }


    void IKChain::create(lgraphics::AnimObject& object)
    {
        LASSERT(nodes_ != NULL);
#if 0
        AnimTree *t = NULL;
        Bone* bone = &(object.getBone(targetIndex_));
        for(u32 i=0; object.getNumTreeTops(); ++i){
            t = searchAnimTreeByBone(bone, object.getTreeTop(i));
            if(t != NULL){
                break;
            }
        }
        if(t == NULL){
            LASSERT(false);
            length_ = 0;
            return;
        }
        targetNode_ = &(t->getElement().getBone()->get());

        for(u32 i=0; object.getNumTreeTops(); ++i){
            t = searchAnimTreeByBone(effectorIndex_, object.getTreeTop(i));
            if(t != NULL){
                break;
            }
        }

        if(t == NULL){
            LASSERT(false);
            length_ = 0;
            return;
        }

        nodes_[0].node_ = &(t->getElement().getBone()->get());
        nodes_[0].rotation_.identity();
        nodes_[0].angleLimit_ = false;

        for(u32 i=1; i<length_; ++i){
            t = t->getParent();
            if(t==NULL){
                length_ = i;
                return;
            }
            nodes_[i].node_ = &(t->getElement().getBone()->get());

            u32 index = object.getBoneIndex( t->getElement().getBone() );
            const lgraphics::AnimObject::Name& boneName = object.getBoneName(index);

            if((lcore::strncmp(boneName.c_str(), NodeNameTable[Node_LeftKnee], boneName.size()) == 0)
                || (lcore::strncmp(boneName.c_str(), NodeNameTable[Node_RightKnee], boneName.size()) == 0))
            {
                nodes_[i].angleLimit_ = true;
            }else{
                nodes_[i].angleLimit_ = false;
            }
            nodes_[i].rotation_.identity();
        }

        t = t->getParent();
        if(t == NULL){
            rootMatrix_.identity();
            nodes_[length_].node_ = &rootMatrix_;
        }else{
            nodes_[length_].node_ = &(t->getElement().getBone()->get());
        }
#endif
    }

    void IKChain::create(lgraphics::AnimObject& object, const pmd::PMDIK& pmdIK)
    {
        targetIndex_ = pmdIK.boneIndex_; //IKターゲット位置のためのボーン
        effectorIndex_ = pmdIK.targetBoneIndex_;

        length_ = pmdIK.chainLength_+1;
        LASSERT(length_<=MAX_LENGTH);

        numIterations_ = pmdIK.numIterations_;

        weight_ = pmdIK.controlWeight_;

        LASSERT(nodes_ == NULL);
        nodes_ = LIME_NEW IKNode[length_+1];
        create(object);
    }

    void IKChain::swap(IKChain& rhs)
    {
        lcore::swap(length_, rhs.length_);
        lcore::swap(numIterations_, rhs.numIterations_);
        lcore::swap(weight_, rhs.weight_);

        lcore::swap(target_, rhs.target_);
        lcore::swap(rootMatrix_, rhs.rootMatrix_);

        lcore::swap(targetIndex_, rhs.targetIndex_);
        lcore::swap(effectorIndex_, rhs.effectorIndex_);
        lcore::swap(targetNode_, rhs.targetNode_);
        lcore::swap(nodes_, rhs.nodes_);
    }

    void IKChain::calc(lgraphics::AnimObject& object)
    {
        initialize(object);
        ccd(object);
        update(object);
    }

    void IKChain::initialize(const lgraphics::AnimObject& object)
    {
        // オブジェクト空間のターゲット位置
        const lmath::Matrix43 &targetMat = object.getBone( targetIndex_ ).get();
        target_[0] = targetMat._elem[0][3];
        target_[1] = targetMat._elem[1][3];
        target_[2] = targetMat._elem[2][3];
#if 0
        AnimTree *tree = nodes_[length_-1].node_;

        if(tree->getParent() != NULL){
            u32 boneIndex = tree->getParent()->getElement().getBoneIndex();
            rootTransform_ = object.getBone(boneIndex).get();
        }else{
            rootTransform_.identity();
        }

        rootMatrix_ = rootTransform_;

        for(u32 i=0; i<length_; ++i){
            nodes_[i].boneToObject_ = rootMatrix_;

            const Vector3& headPos = nodes_[i].node_->getElement().getHeadPos();

            nodes_[i].boneToObject_.preTranslate(headPos);

            lmath::Vector3 pos = headPos;
            pos.mul(pos, rootTransform_);

            nodes_[i].position_ = pos;
            nodes_[i].rotation_.identity();
        }
#endif
    }

    void IKChain::ccd(const lgraphics::AnimObject& object)
    {
        static const f32 threshold = 0.0001f;

        // エフェクタ位置
        lmath::Vector3 effectorInObject;

        f32 euler[3] = {0.0f, 0.0f, 0.0f};

        for(u32 j=0; j<numIterations_; ++j){

            for(u32 i=1; i<length_; ++i){

                effectorInObject[0] = nodes_[0].node_->_elem[0][3];
                effectorInObject[1] = nodes_[0].node_->_elem[1][3];
                effectorInObject[2] = nodes_[0].node_->_elem[2][3];


                // オブジェクト座標からボーン座標への変換
                lmath::Matrix43 objectToBone = *(nodes_[i].node_);
                objectToBone.invert();

                // ボーン座標でのエフェクタ、ターゲット位置
                Vector3 effector;
                effector.mul(effectorInObject, objectToBone);

                Vector3 target;
                target.mul(target_, objectToBone);

                // エフェクタが目標に近ければ終了
                f32 l = effector.distanceSqr(target);
                if(l<threshold){
                    return;
                }

                effector.normalize();
                target.normalize();

                f32 rot = lmath::acos( effector.dot(target) );
                if(lmath::absolute(rot) < threshold){
                    continue;
                }

                // 回転軸
                Vector3 axis;
                axis.cross(effector, target);
                l = axis.lengthSqr();
                if(l<threshold){
                    continue;
                }

                axis.normalize();

                if(rot<-weight_){
                    rot = -weight_;
                }else if(rot>weight_){
                    rot = weight_;
                }


                Quaternion rotation;
                rotation.setRotateAxis(axis, rot);

                if(nodes_[i].angleLimit_){
                    lmath::getEulerObjectToInertial(euler[0], euler[1], euler[2], rotation);

                    if(euler[1] < -PI){
                        euler[1] = -PI;
                    }else if(-0.001f < euler[1]){
                        euler[1] = -0.001f;
                    }
                    euler[0] = 0.0f;
                    euler[2] = 0.0f;

                    lmath::getQuaternionObjectToInertial(rotation, euler[0], euler[1], euler[2]);
                }


                rotation.normalize();

                rotation.mul(rotation, nodes_[i].rotation_);
                rotation.normalize();

                nodes_[i].rotation_ = rotation;

                for(u32 k=i; k>=0; --k){

                }
            }
        }
    }

    void IKChain::update(lgraphics::AnimObject& object)
    {
#if 0
        AnimTree *tree = NULL;
        lmath::Matrix43 mat;

        for(s32 i=length_-1; i>=0; --i){

            tree = nodes_[i].node_;
            u32 boneIndex = tree->getElement().getBoneIndex();

            //lmath::DualQuaternion &original = object.getBone(boneIndex).get();
            lmath::Matrix43 &original = object.getBone(boneIndex).get();

            const lmath::Vector3 &offset = tree->getElement().getHeadPos();

            nodes_[i].rotation_.getMatrix(mat);

            mat.preTranslate(-offset);
            mat.translate(offset);

            rootTransform_.mul(mat, rootTransform_);
            //rootTransform_.preTranslate(offset);
            //rootTransform_.preRotate(nodes_[i].rotation_);
            //rootTransform_.preTranslate(-offset);
            original = rootTransform_;
        }

        //葉の子ボーンも更新
        tree = nodes_[0].node_;
        for(u32 i=0; i<tree->getNumChildren(); ++i){
            updateLeafChildren(tree->getChild(i), tree->getElement().getBone()->get());
        }
#endif
    }

    void IKChain::updatePosition(const lgraphics::AnimObject& object)
    {
#if 0
        //ルートの変換
        //DualQuaternion rootTransform = rootTransform_;
        Matrix43 rootTransform = rootTransform_;
        lmath::Matrix43 mat;

        for(s32 i=length_-1; i>=0; --i){
            const Vector3& headPos = nodes_[i].node_->getElement().getHeadPos();

            nodes_[i].rotation_.getMatrix(mat);

            mat.preTranslate(-headPos);
            mat.translate(headPos);

            rootTransform.mul(mat, rootTransform);


            //rootTransform.preTranslate(headPos);
            //rootTransform.preRotate(nodes_[i].rotation_);
            //rootTransform.preTranslate(-headPos);

            Vector3 pos = nodes_[i].position_;
            nodes_[i].position_ = headPos;

            nodes_[i].position_.mul(nodes_[i].position_, rootTransform);
            //rootTransform.transform(nodes_[i].position_);

            //rootTransform.getMatrix(nodes_[i].boneToObject_);
            nodes_[i].boneToObject_ = rootTransform;
            nodes_[i].boneToObject_.preTranslate(headPos);
        }
#endif
    }

    void IKChain::updateLeafChildren(AnimTree& tree, const lmath::Matrix43& transform)
    {
        //lmath::DualQuaternion &current = tree.getElement().getBone()->get();
        lmath::Matrix43 &current = tree.getElement().getBone()->get();

        current = transform;

        for(u32 i=0; i<tree.getNumChildren(); ++i){
            updateLeafChildren(tree.getChild(i), transform);
        }
    }
}

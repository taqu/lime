/**
@file AnimControler.cpp
@author t-sakai
@date 2009/11/10
*/
#include <lmath/lmath.h>

#include "AnimControler.h"
#include "AnimDatabase.h"
#include "AnimDataNode.h"
#include "../scene/Bone.h"
#include "../scene/AnimObject.h"

namespace lgraphics
{
    using namespace lmath;

    //-----------------------------------------------------------
    //---
    //--- AnimControler::State
    //---
    //-----------------------------------------------------------
    /// 回転と平行移動
    struct AnimControler::State
    {
        lmath::Quaternion rotation_;
        lmath::Vector3 translation_;
        u32 nodeIndex_;
    };

    //-----------------------------------------------------------
    //---
    //--- AnimControler
    //---
    //-----------------------------------------------------------
#if LIME_USE_DUAL_QUATERNION_FOR_SKINNING
    // デュアルクォーターニオン
    void AnimControler::updateNode(AnimTree& tree, const AnimDataNodeTree& dataTree, DualQuaternion parent, f32 frame)
    {
        u32 frame0 = static_cast<u32>(frame);

        //u32 index = tree.getElement().getBoneIndex();

        const AnimDataNode &dataNode = *( dataTree.getElement() );

        if(dataNode.getDataNum() > 0 && frame0 <= dataNode.getMaxFrame()){
            // 現在のボーン更新

            //一つ目のフレームを探す
            //----------------------
            u32 index0 = 0;
            while(frame0 > dataNode.getData(index0).frameNo_){
                ++index0;
            }
            index0 = (index0 <= 0)? 0 : index0 - 1;
            frame0 = dataNode.getData(index0).frameNo_;

            //次のフレームを探す
            //----------------------
            u32 index1 = (index0 < dataNode.getDataNum()-1)? index0 + 1 : index0;
            u32 frame1 = dataNode.getData(index1).frameNo_;

            f32 ratio = (frame1==frame0)?
                0.0f : (frame - static_cast<f32>( frame0 )) / static_cast<f32>(frame1 - frame0);

            //補間
            //------------------------
            const AnimData &d0 = dataNode.getData(index0);
            const AnimData &d1 = dataNode.getData(index1);

            lmath::Vector3 pos;
            pos.lerp(d0.position_, d1.position_, ratio);

            lmath::Quaternion rot;
            rot.slerp(d0.rotation_, d1.rotation_, ratio);

            //変形をセット
            currentRotation_[ tree.getElement().getBoneIndex() ] = rot;
            currentTranslation_[ tree.getElement().getBoneIndex() ] = pos;
        }

        // ローカル原点に戻して回転して、元の位置に戻す
        //------------------------------------------------
        const Vector3& headPos = tree.getElement().getHeadPos();
        const Quaternion& rotation = currentRotation_[ tree.getElement().getBoneIndex() ];

        Vector3 offset = currentTranslation_[ tree.getElement().getBoneIndex() ];
        offset += headPos;

        parent.preTranslate(offset);
        parent.preRotate(rotation);
        parent.preTranslate(-headPos);

        Bone &bone = *( tree.getElement().getBone() );
        bone.get() = parent;

        for(u32 i=0; i<tree.getNumChildren(); ++i){
            updateNode(tree.getChild(i), dataTree.getChild(i), parent, frame);
        }
    }

    AnimControler::AnimControler()
        :currentRotation_(NULL)
        ,currentTranslation_(NULL)
    {
    }

    AnimControler::AnimControler(const AnimObject& object)
        :currentRotation_(NULL)
        ,currentTranslation_(NULL)
    {
        initialize(object);
    }

    AnimControler::~AnimControler()
    {
        LIME_DELETE_ARRAY(currentRotation_);
        LIME_DELETE_ARRAY(currentTranslation_);
    }

    void AnimControler::initialize(const AnimObject& object)
    {
        LIME_DELETE_ARRAY(currentRotation_);
        currentRotation_ = LIME_NEW Quaternion[ object.getNumBones() ];

        LIME_DELETE_ARRAY(currentTranslation_);
        currentTranslation_ = LIME_NEW Vector3[ object.getNumBones() ];

        for(u32 i=0; i<object.getNumBones(); ++i){
            currentRotation_[i].identity();
            currentTranslation_[i].set(0.0f, 0.0f, 0.0f);
        }
    }


    void AnimControler::update(
        AnimObject& object,
        const AnimDatabase& animDB,
        f32 frame)
    {
        LASSERT(currentRotation_ != NULL);
        LASSERT(currentTranslation_ != NULL);

        u32 num = (object.getNumTreeTops() < animDB.getNumTreeTops())?
            object.getNumTreeTops() : animDB.getNumTreeTops();

        DualQuaternion root;
        root.identity();

        for(u32 i=0; i<num; ++i){
            const AnimDataNodeTree& animTree = animDB.getTreeTop(i);
            AnimTree &tree = object.getTreeTop(i);

            updateNode(tree, animTree, root, frame);
        }
    }

#else
    // 4x3マトリックス
    void AnimControler::updateNode(AnimTree& tree, Matrix43 parent, f32 frame)
    {
        u32 frame0 = static_cast<u32>(frame);

        u32 boneIndex = target_->getBoneIndex(tree.getElement().getBone());
        if(currentState_[boneIndex].nodeIndex_ != -1){ //対応するノードがない

        const AnimDataNode &dataNode = animDB_->getAnimDataNode(currentState_[boneIndex].nodeIndex_);

        if(dataNode.getDataNum() > 0 && frame0 <= dataNode.getMaxFrame()){
            // 現在のボーン更新

            //一つ目のフレームを探す
            //----------------------
            u32 index0 = 0;
            while(frame0 > dataNode.getData(index0).frameNo_){
                ++index0;
            }
            index0 = (index0 <= 0)? 0 : index0 - 1;
            frame0 = dataNode.getData(index0).frameNo_;

            //次のフレームを探す
            //----------------------
            u32 index1 = (index0 < dataNode.getDataNum()-1)? index0 + 1 : index0;
            u32 frame1 = dataNode.getData(index1).frameNo_;

            f32 ratio = (frame1==frame0)?
                0.0f : (frame - static_cast<f32>( frame0 )) / static_cast<f32>(frame1 - frame0);

            //補間
            //------------------------
            const AnimData &d0 = dataNode.getData(index0);
            const AnimData &d1 = dataNode.getData(index1);

            lmath::Vector3 pos;
            pos.lerp(d0.position_, d1.position_, ratio);

            lmath::Quaternion rot;
            rot.slerp(d0.rotation_, d1.rotation_, ratio);

            //変形をセット
            currentState_[ boneIndex ].rotation_ = rot;
            currentState_[ boneIndex ].translation_ = pos;
        }
        }

        // ローカル原点に戻して回転して、元の位置に戻す
        //------------------------------------------------
        const Vector3& headPos = tree.getElement().getHeadPos();
        const Quaternion& rotation = currentState_[ boneIndex ].rotation_;

#if 1
        Vector3 offset = currentState_[ boneIndex ].translation_;
        offset += headPos;

        parent.preTranslate(offset);

        //*******************************
        //*** preRotateが作れれば
        Matrix43 rotMat;
        rotMat.identity();
        rotation.getMatrix(rotMat);
        
        parent.mul(rotMat, parent);

        parent.preTranslate(-headPos);
#else
        Matrix43 mat;
        rotation.getMatrix(mat);

        mat.translate(headPos._x+headOffset._x,
            headPos._y+headOffset._y,
            headPos._z+headOffset._z);
        
        parent.mul(mat, parent);

#endif

        Bone &bone = *( tree.getElement().getBone() );
        bone.get() = parent;

        for(u32 i=0; i<tree.getNumChildren(); ++i){
            updateNode(tree.getChild(i), parent, frame);
        }
    }

    AnimControler::AnimControler()
        :target_(NULL)
        ,animDB_(NULL)
        ,currentState_(NULL)
    {
    }

    AnimControler::AnimControler(AnimObject& object, AnimDatabase& animDB)
        :target_(NULL)
        ,animDB_(NULL)
        ,currentState_(NULL)
    {
        initialize(object, animDB);
    }

    AnimControler::~AnimControler()
    {
        LIME_DELETE_ARRAY(currentState_);
    }

    void AnimControler::initialize(AnimObject& object, AnimDatabase& animDB)
    {
        LIME_DELETE_ARRAY(currentState_);
        currentState_ = LIME_NEW State[ object.getNumBones() ];

        for(u32 i=0; i<object.getNumBones(); ++i){
            currentState_[i].rotation_.identity();
            currentState_[i].translation_.set(0.0f, 0.0f, 0.0f);

            // ボーンとノードを名前で対応させる
            const AnimObject::Name &name = object.getBoneName(i);

            u32 index = animDB.findNodeIndex(name.c_str());
            currentState_[i].nodeIndex_ = (index != animDB.getNumAnimDataNode())? index : -1;
            
        }

        target_ = &object;
        animDB_ = &animDB;
    }


    void AnimControler::update(f32 frame)
    {
        LASSERT(target_ != NULL);
        LASSERT(animDB_ != NULL);
        LASSERT(currentState_ != NULL);

        Matrix43 root;
        root.identity();

        for(u32 i=0; i<target_->getNumTreeTops(); ++i){
            AnimTree &tree = target_->getTreeTop(i);

            updateNode(tree, root, frame);
        }
    }


    lmath::Quaternion& AnimControler::getRotation(u32 index)
    {
        LASSERT(target_ != NULL);
        LASSERT(0<=index && index<target_->getNumBones());
        return currentState_[index].rotation_;
    }

    lmath::Vector3& AnimControler::getTranslation(u32 index)
    {
        LASSERT(target_ != NULL);
        LASSERT(0<=index && index<target_->getNumBones());
        return currentState_[index].translation_;
    }
#endif
}

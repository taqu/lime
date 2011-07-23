/**
@file PmmDef.cpp
@author t-sakai
@date 2011/02/06
*/
#include "PmmDef.h"

#include <lcore/utility.h>
#include <lcore/Search.h>

#include <lgraphics/api/GeometryBuffer.h>
#include <lgraphics/api/VertexBufferRef.h>

#include <lframework/scene/Geometry.h>
#include <lframework/scene/AnimObject.h>
#include <lframework/scene/Object.h>
#include <lframework/anim/AnimationControler.h>

#include "Pmd.h"
#include "charcode/conv_charcode.h"

using namespace lcore;
using namespace lconverter;

namespace pmm
{
    //----------------------------------------------------------------------
    //---
    //--- ModelInfo
    //---
    //----------------------------------------------------------------------
    lcore::istream& operator>>(lcore::istream& is, ModelInfo& rhs)
    {
        io::read(is, rhs.elements_[ ModelInfo::Element_Unknown ]);

        io::read(is, rhs.name_, 20);
        rhs.name_[20] = '\0';

        io::read(is, rhs.path_, 256);
        rhs.path_[255] = '\0';

        extractFileNameUTF8(rhs.path_);

        io::read(is, rhs.elements_[ ModelInfo::Element_SelectedIndex ]);
        io::read(is, rhs.elements_[ ModelInfo::Element_Visible ]);
        io::read(is, rhs.boneElements_[ ModelInfo::BoneElem_SelectedBone ]);
        io::read(is, rhs.boneElements_[ ModelInfo::BoneElem_Unknown ]);
        io::read(is, rhs.blowSkinIndex_);
        io::read(is, rhs.lipSkinIndex_);
        io::read(is, rhs.eyeSkinIndex_);
        io::read(is, rhs.otherSkinIndex_);
        io::read(is, rhs.elements_[ ModelInfo::Element_NumDisplayGroup ]);

        for(u32 i=0; i<ModelInfo::NumBitArrayBytes; ++i){
            rhs.bitIsExpand_[i] = 0;
        }

        u8 c = 0;
        for(u32 i=0; i<rhs.elements_[ModelInfo::Element_NumDisplayGroup]; ++i){
            io::read(is, c);
            if(c == 1){
                u32 index = i/8;
                u32 mod = i - 8*index;
                rhs.bitIsExpand_[index] |= (0x01U<<mod);
            }
        }

        io::read(is, rhs.startFrame_);
        io::read(is, rhs.endFrame_);
        return is;
    }


    //----------------------------------------------------------------------
    //---
    //--- JointFrame
    //---
    //----------------------------------------------------------------------
    void JointFrame::readInit(lcore::istream& is, JointFrame& rhs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.nextDataID_);
        io::read(is, rhs.interp_, 16);
        io::read(is, rhs.position_);
#if 1
        f32 tmp[4];
        io::read(is, tmp, sizeof(f32)*4);
        rhs.rotation_[0] = tmp[3];
        rhs.rotation_[1] = tmp[0];
        rhs.rotation_[2] = tmp[1];
        rhs.rotation_[3] = tmp[2];
#else
        io::read(is, rhs.rotation_);
#endif
        io::read(is, rhs.selected_);
    }

    void JointFrame::readInit2(lcore::istream& is)
    {
        s32 offset = sizeof(f32)*7 + sizeof(f32) + sizeof(u8)*2;

#if 1
        is.seekg(offset, lcore::ios::cur);
#else
        f32 buffer[4];

        io::read(is, buffer, sizeof(lmath::Vector3)); //位置
        io::read(is, buffer, sizeof(lmath::Vector4)); //回転

        io::read(is, buffer[0]);

        u8 unknownByte[2];
        io::read(is, unknownByte, 2);
#endif
    }

    lcore::istream& operator>>(lcore::istream& is, JointFrame& rhs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.prevDataID_);
        io::read(is, rhs.nextDataID_);
        io::read(is, rhs.interp_, 16);
        io::read(is, rhs.position_);

#if 1
        f32 tmp[4];
        io::read(is, tmp, sizeof(f32)*4);
        rhs.rotation_[0] = tmp[3];
        rhs.rotation_[1] = tmp[0];
        rhs.rotation_[2] = tmp[1];
        rhs.rotation_[3] = tmp[2];
#else
        io::read(is, rhs.rotation_);
#endif
        io::read(is, rhs.selected_);
        return is;
    }


    //----------------------------------------------------------------------
    //---
    //--- SkinFrame
    //---
    //----------------------------------------------------------------------
    void SkinFrame::readInit(lcore::istream& is, SkinFrame& rhs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.nextID_);
        io::read(is, rhs.weight_);
        io::read(is, rhs.selected_);
    }

    void SkinFrame::readInit2(lcore::istream& is)
    {
        f32 unknown;
        io::read(is, unknown);
    }

    lcore::istream& operator>>(lcore::istream& is, SkinFrame& rhs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.prevID_);
        io::read(is, rhs.nextID_);
        io::read(is, rhs.weight_);
        io::read(is, rhs.selected_);
        return is;
    }

    //----------------------------------------------------------------------
    //---
    //--- IKFrame
    //---
    //----------------------------------------------------------------------
    void IKFrame::read(lcore::istream& is, IKFrame& rhs, u32 numIKs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.prevID_);
        io::read(is, rhs.nextID_);
        io::read(is, rhs.elements_[ Element_IsDisp ]);
#if 0
        if(numIKs>0){
            rhs.isIKOn_ = new u8[numIKs];
            io::read(is, rhs.isIKOn_, numIKs);
        }else{
            rhs.isIKOn_ = NULL;
        }
#else
        //読み飛ばす
        u8 buffer[256];
        io::read(is, buffer, numIKs);
#endif
        io::read(is, rhs.elements_[ Element_Selected ]);
    }

    void IKFrame::readInit(lcore::istream& is, IKFrame& rhs, u32 numIKs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.nextID_);
        io::read(is, rhs.elements_[ Element_IsDisp ]);
#if 0
        if(numIKs>0){
            rhs.isIKOn_ = new u8[numIKs];
            io::read(is, rhs.isIKOn_, numIKs);
        }else{
            rhs.isIKOn_ = NULL;
        }
#else
        //読み飛ばす
        u8 buffer[256];
        io::read(is, buffer, numIKs);
#endif
        io::read(is, rhs.elements_[ Element_Selected ]);
    }

    void IKFrame::readInit2(lcore::istream& is)
    {
        u8 unknown;
        io::read(is, unknown);
    }


    //----------------------------------------------------------------------
    //---
    //--- CameraFrame
    //---
    //----------------------------------------------------------------------
    void CameraFrame::readInit(lcore::istream& is, CameraFrame& rhs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.nextID_);
        io::read(is, rhs.length_);
        io::read(is, rhs.center_, sizeof(f32)*3);
        io::read(is, rhs.angle_, sizeof(f32)*3);
        io::read(is, rhs.interp_, 24);
        io::read(is, rhs.flag_);
        io::read(is, rhs.fov_);
        io::read(is, rhs.selected_);
    }

    void CameraFrame::readInit2(lcore::istream& is)
    {
        s32 offset = sizeof(f32)*7 + sizeof(u32)*2 + sizeof(u8);
        is.seekg(offset, lcore::ios::cur);
    }

    lcore::istream& operator>>(lcore::istream& is, CameraFrame& rhs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.prevID_);
        io::read(is, rhs.nextID_);
        io::read(is, rhs.length_);
        io::read(is, rhs.center_, sizeof(f32)*3);
        io::read(is, rhs.angle_, sizeof(f32)*3);
        io::read(is, rhs.interp_, 24);
        io::read(is, rhs.flag_);
        io::read(is, rhs.fov_);
        io::read(is, rhs.selected_);
        return is;
    }


    //----------------------------------------------------------------------
    //---
    //--- LightFrame
    //---
    //----------------------------------------------------------------------
    void LightFrame::readInit(lcore::istream& is, LightFrame& rhs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.nextID_);
        io::read(is, rhs.rgb_, sizeof(f32)*3);
        io::read(is, rhs.direction_, sizeof(f32)*3);
        io::read(is, rhs.selected_);
    }

    void LightFrame::readInit2(lcore::istream& is)
    {
        s32 offset = sizeof(f32)*6;
        is.seekg(offset, lcore::ios::cur);
    }

    lcore::istream& operator>>(lcore::istream& is, LightFrame& rhs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.prevID_);
        io::read(is, rhs.nextID_);
        io::read(is, rhs.rgb_, sizeof(f32)*3);
        io::read(is, rhs.direction_, sizeof(f32)*3);
        io::read(is, rhs.selected_);
        return is;
    }


    //----------------------------------------------------------------------
    //---
    //--- AccessoryInfo
    //---
    //----------------------------------------------------------------------
    void AccessoryInfo::readName(lcore::istream& is, AccessoryInfo& rhs)
    {
        io::read(is, rhs.name_, 100);
        rhs.name_[99] = '\0';
    }

    lcore::istream& operator>>(lcore::istream& is, AccessoryInfo& rhs)
    {
        io::read(is, rhs.number_);

        AccessoryInfo::readName(is, rhs);

        io::read(is, rhs.path_, 256);
        rhs.path_[255] = '\0';

        extractFileNameUTF8(rhs.path_);

        io::read(is, rhs.unknown_);
        return is;
    }



    //----------------------------------------------------------------------
    //---
    //--- AccessoryFrame
    //---
    //----------------------------------------------------------------------
    void AccessoryFrame::readInit(lcore::istream& is, AccessoryFrame& rhs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.nextID_);
        io::read(is, rhs.isDisp_);
        io::read(is, rhs.bindModel_);
        io::read(is, rhs.bindBone_);
        io::read(is, rhs.translation_, sizeof(f32)*3);
        io::read(is, rhs.rotation_, sizeof(f32)*3);
        io::read(is, rhs.scale_);
        io::read(is, rhs.shadow_);
        io::read(is, rhs.selected_);
    }

    void AccessoryFrame::readInit2(lcore::istream& is)
    {
        s32 offset = sizeof(f32)*7 + sizeof(u32)*2 + sizeof(u8)*2;
        is.seekg(offset, lcore::ios::cur);
    }

    lcore::istream& operator>>(lcore::istream& is, AccessoryFrame& rhs)
    {
        io::read(is, rhs.dataID_);
        io::read(is, rhs.frameNo_);
        io::read(is, rhs.prevID_);
        io::read(is, rhs.nextID_);
        io::read(is, rhs.isDisp_);
        io::read(is, rhs.bindModel_);
        io::read(is, rhs.bindBone_);
        io::read(is, rhs.translation_, sizeof(f32)*3);
        io::read(is, rhs.rotation_, sizeof(f32)*3);
        io::read(is, rhs.scale_);
        io::read(is, rhs.shadow_);
        io::read(is, rhs.selected_);
        return is;
    }

    //----------------------------------------------------------------------
    //---
    //--- SkinAnimPack
    //---
    //----------------------------------------------------------------------
    SkinAnimPack::SkinAnimPack()
        :numSkinPoses_(NULL)
        ,ptrSkinPoses_(NULL)
        ,skinPoses_(NULL)
    {
    }

    SkinAnimPack::~SkinAnimPack()
    {
    }

    void SkinAnimPack::create(u32 totalFrames, u32 numSkins)
    {
        u32 sizeNumSkinPoses = sizeof(u32) * numSkins;
        u32 sizePtrSkinPoses = sizeof(SkinPose*) * numSkins;
        u32 sizeSkinPoses = sizeof(SkinPose) * totalFrames;

        buffer_.resize(sizeNumSkinPoses + sizePtrSkinPoses + sizeSkinPoses);

        u32 offset = 0;
        numSkinPoses_ = buffer_.get<u32>(offset);
        offset += sizeNumSkinPoses;

        ptrSkinPoses_ = buffer_.get<SkinPose*>(offset);
        offset += sizePtrSkinPoses;

        skinPoses_ = buffer_.get<SkinPose>(offset);
    }

    f32 SkinAnimPack::searchSkinWeight(u32 index, u32 frame) const
    {
        u32 ret = 
            lcore::binarySearchLessThanEqualIndex<
            SkinPose,
            u32,
            u32,
            FuncGetFrameNo>(numSkinPoses_[index], ptrSkinPoses_[index], frame, FuncGetFrameNo());

        //最後のフレームだったらそのまま返す
        if(ret == (numSkinPoses_[index] - 1)){
            return ptrSkinPoses_[index][ret].weight_;
        }

        u32 next = ret + 1;

        u32 frame0 = ptrSkinPoses_[index][ret].frameNo_;
        u32 frame1 = ptrSkinPoses_[index][next].frameNo_;

        LASSERT(frame0 < frame1);

        f32 ratio = static_cast<f32>(frame-frame0)/(frame1 - frame0);
        
        f32 w0 = ptrSkinPoses_[index][ret].weight_;
        f32 w1 = ptrSkinPoses_[index][next].weight_;
        return (w0*(1.0f-ratio) + ratio*w1);
    }

    void SkinAnimPack::swap(SkinAnimPack& rhs)
    {
        lcore::swap(numSkinPoses_, rhs.numSkinPoses_);
        lcore::swap(ptrSkinPoses_, rhs.ptrSkinPoses_);
        lcore::swap(skinPoses_, rhs.skinPoses_);

        buffer_.swap(rhs.buffer_);
    }

    //----------------------------------------------------------------------
    //---
    //--- ModelPack
    //---
    //----------------------------------------------------------------------
    ModelPack::ModelPack()
        :object_(NULL)
        ,animControler_(NULL)
        ,boneMap_(NULL)
    {
    }

    ModelPack::~ModelPack()
    {
        LIME_DELETE(animControler_);
        LIME_DELETE(object_);
        LIME_DELETE_ARRAY(boneMap_);
    }

    void ModelPack::set(lscene::AnimObject* object)
    {
        LIME_DELETE(object_);
        object_ = object;
    }

    u32 ModelPack::getNumJoints() const
    {
        LASSERT(object_ != NULL);
        return (object_->getSkeleton())? object_->getSkeleton()->getNumJoints() : 0;
    }

    void ModelPack::setAnimationControler(lanim::AnimationControler* controler)
    {
        LIME_DELETE(animControler_);
        animControler_ = controler;
    }

    //マルチストリーム実験
#if defined(LIME_LIBCONVERT_PMD_USE_MULTISTREAM)
    void ModelPack::resetMorph()
    {
        if(skinPack_.getNumSkins()<=0){
            return;
        }

        lmath::Vector3* vertices = skinPack_.getMorphBaseVertices();

        u32 minIndex = skinPack_.getMinIndex();
        pmd::Skin& skin = skinPack_.getSkin(0);
        for(u32 i=0; i<skin.numVertices_; ++i){
            u32 index = skin.vertices_[i].index_ - minIndex;
            vertices[index].set(
                skin.vertices_[i].position_[0],
                skin.vertices_[i].position_[1],
                skin.vertices_[i].position_[2]);
        }

        u32 offset = skinPack_.getMinIndex() * sizeof(lmath::Vector3);
        u32 size = (skinPack_.getMaxIndex() - skinPack_.getMinIndex()) + 1;
        size *= sizeof(lmath::Vector3);

        lgraphics::VertexBufferRef& vb = object_->getGeometry(0).getGeometryBuffer()->getVertexBuffer(0);
        vb.blit(vertices, offset, size);

        //バックバッファも初期化
        lgraphics::VertexBufferRef& back = skinPack_.getBackVertexBuffer();
        back.blit(vertices, offset, size);
    }

    void ModelPack::updateMorph(u32 /*frame*/, u32 nextFrame)
    {
        if(skinPack_.getNumSkins()<=0){
            return;
        }

        lgraphics::VertexBufferRef& vb = object_->getGeometry(0).getGeometryBuffer()->getVertexBuffer(0);
        lgraphics::VertexBufferRef& back = skinPack_.getBackVertexBuffer();
        //バックバッファと入れ替え
        vb.swap(back);

        lmath::Vector3* vertices = skinPack_.getMorphBaseVertices();

        u32 minIndex = skinPack_.getMinIndex();
        pmd::Skin& skin = skinPack_.getSkin(0);
        for(u32 i=0; i<skin.numVertices_; ++i){
            u32 index = skin.vertices_[i].index_ - minIndex;
            vertices[index].set(
                skin.vertices_[i].position_[0],
                skin.vertices_[i].position_[1],
                skin.vertices_[i].position_[2]);
        }

        for(u32 i=1; i<skinPack_.getNumSkins(); ++i){
            f32 weight = skinAnimPack_.searchSkinWeight(i-1, nextFrame);

            pmd::Skin& offsetSkin = skinPack_.getSkin(i);

            for(u32 j=0; j<offsetSkin.numVertices_; ++j){
                u32 index = skin.vertices_[ offsetSkin.vertices_[j].index_ ].index_ - minIndex;
                vertices[index]._x += weight * offsetSkin.vertices_[j].position_[0];
                vertices[index]._y += weight * offsetSkin.vertices_[j].position_[1];
                vertices[index]._z += weight * offsetSkin.vertices_[j].position_[2];
            }
        }

        u32 offset = skinPack_.getMinIndex() * sizeof(lmath::Vector3);
        u32 size = (skinPack_.getMaxIndex() - skinPack_.getMinIndex()) + 1;
        size *= sizeof(lmath::Vector3);

        //vb.blit(vertices, offset, size);
        back.blit(vertices, offset, size); //バックバッファに転送
    }

#else
    void ModelPack::resetMorph()
    {
        if(skinPack_.getNumSkins()<=0){
            return;
        }

        pmd::Vertex* vertices = skinPack_.getMorphBaseVertices();

        u32 minIndex = skinPack_.getMinIndex();
        pmd::Skin& skin = skinPack_.getSkin(0);
        for(u32 i=0; i<skin.numVertices_; ++i){
            u32 index = skin.vertices_[i].index_ - minIndex;
            vertices[index].position_[0] = skin.vertices_[i].position_[0];
            vertices[index].position_[1] = skin.vertices_[i].position_[1];
            vertices[index].position_[2] = skin.vertices_[i].position_[2];
        }

        u32 offset = skinPack_.getMinIndex() * sizeof(pmd::Vertex);
        u32 size = (skinPack_.getMaxIndex() - skinPack_.getMinIndex()) + 1;
        size *= sizeof(pmd::Vertex);

        lgraphics::VertexBufferRef& vb = object_->getGeometry(0).getGeometryBuffer()->getVertexBuffer(0);
        vb.blit(vertices, offset, size);
    }

    void ModelPack::updateMorph(u32 frame, u32 nextFrame)
    {
        if(skinPack_.getNumSkins()<=0){
            return;
        }

        pmd::Vertex* vertices = skinPack_.getMorphBaseVertices();

        u32 minIndex = skinPack_.getMinIndex();
        pmd::Skin& skin = skinPack_.getSkin(0);
        for(u32 i=0; i<skin.numVertices_; ++i){
            u32 index = skin.vertices_[i].index_ - minIndex;
            vertices[index].position_[0] = skin.vertices_[i].position_[0];
            vertices[index].position_[1] = skin.vertices_[i].position_[1];
            vertices[index].position_[2] = skin.vertices_[i].position_[2];
        }

        for(u32 i=1; i<skinPack_.getNumSkins(); ++i){
            f32 weight = skinAnimPack_.searchSkinWeight(i-1, frame);

            pmd::Skin& offsetSkin = skinPack_.getSkin(i);

            for(u32 j=0; j<offsetSkin.numVertices_; ++j){
                u32 index = skin.vertices_[ offsetSkin.vertices_[j].index_ ].index_ - minIndex;
                vertices[index].position_[0] += weight * offsetSkin.vertices_[j].position_[0];
                vertices[index].position_[1] += weight * offsetSkin.vertices_[j].position_[1];
                vertices[index].position_[2] += weight * offsetSkin.vertices_[j].position_[2];
            }
        }

        u32 offset = skinPack_.getMinIndex() * sizeof(pmd::Vertex);
        u32 size = (skinPack_.getMaxIndex() - skinPack_.getMinIndex()) + 1;
        size *= sizeof(pmd::Vertex);

        lgraphics::VertexBufferRef& vb = object_->getGeometry(0).getGeometryBuffer()->getVertexBuffer(0);
        vb.blit(vertices, offset, size);
    }
#endif


    //----------------------------------------------------------------------
    //---
    //--- CameraAnimPack
    //---
    //----------------------------------------------------------------------
    void CameraAnimPack::set(u32 index, const CameraFrame& frame)
    {
        LASSERT(0<=index && index<numPoses_);
        CameraPose& pose = poses_[index];

        pose.frameNo_ = frame.frameNo_;

        pose.center_.set(frame.center_[0], frame.center_[1], frame.center_[2]);
        pose.angle_.set(frame.angle_[0], frame.angle_[1], frame.angle_[2]);

        static const f32 Limit = (PI_2*0.98f);
        lmath::clamp(pose.angle_.x_, -Limit, Limit);

        pose.length_ = frame.length_;

        pose.fov_ = static_cast<f32>( frame.fov_ ) / 180.0f * PI;
    }


    //----------------------------------------------------------------------
    //---
    //--- LightAnimPack
    //---
    //----------------------------------------------------------------------
    void LightAnimPack::set(u32 index, const LightFrame& frame)
    {
        LASSERT(0<=index && index<numPoses_);
        LightPose& pose = poses_[index];
        pose.frameNo_ = frame.frameNo_;

        //ライト方向の指定の仕方が逆
        pose.direction_.set( -frame.direction_[0], -frame.direction_[1], -frame.direction_[2]);
        //正規化
        f32 length = pose.direction_.length();
        if(lmath::isEqual(length, 0.0f)){
            pose.direction_.set(0.0f, 1.0f, 0.0f);
        }else{
            pose.direction_ *= (1.0f/length);
        }

        pose.rgbx_[0] = static_cast<u8>( 255*frame.rgb_[0] );
        pose.rgbx_[1] = static_cast<u8>( 255*frame.rgb_[1] );
        pose.rgbx_[2] = static_cast<u8>( 255*frame.rgb_[2] );
        pose.rgbx_[3] = 255;
    }

    //----------------------------------------------------------------------
    //---
    //--- AccessoryPack
    //---
    //----------------------------------------------------------------------
    AccessoryPack::AccessoryPack()
        :object_(NULL)
        ,targetObject_(NULL)
        ,targetMat_(NULL)
        ,bindBone_(0)
        ,alphaBlendEnable_(false)
        ,isDisp_(true)
    {
        matrix_.identity();
    }

    AccessoryPack::~AccessoryPack()
    {
        LIME_DELETE(object_);
    }

    void AccessoryPack::setObject(lscene::Object* object)
    {
        LIME_DELETE(object_);
        object_ = object;
    }


    void AccessoryPack::set(u32 index, const AccessoryFrame& frame)
    {
        LASSERT(0<=index && index<numPoses_);
        AccessoryPose& pose = poses_[index];

        pose.frameNo_ = frame.frameNo_;

        pose.translation_.set(frame.translation_[0], frame.translation_[1], frame.translation_[2]);

        pose.rotation_.set(frame.rotation_[0], frame.rotation_[1], frame.rotation_[2]);

        pose.scale_ = frame.scale_;

        pose.elements_[ AccessoryPose::Elem_Disp ] = frame.isDisp_;
        pose.elements_[ AccessoryPose::Elem_BindModel ] = static_cast<u8>(frame.bindModel_);
        pose.elements_[ AccessoryPose::Elem_BindBone ] = static_cast<u8>(frame.bindBone_);
        pose.elements_[ AccessoryPose::Elem_Shadow ] = frame.shadow_;
    }


    void AccessoryPack::reset(u32 /*numModels*/, ModelPack* models)
    {
        LASSERT(models != NULL);
        LASSERT(getNumPoses()>=1);
        LASSERT(object_ != NULL);

        const AccessoryPose& pose = getPose(0);

        matrix_.scale( pose.scale_ );
        //matrix_.rotateX( pose.rotation_._x );
        //matrix_.rotateY( pose.rotation_._y );
        //matrix_.rotateZ( pose.rotation_._z );

        matrix_.translate( pose.translation_ );


        lmath::Matrix34& mat = object_->getWorldMatrix();
        mat = matrix_;

        u8 bindModel = pose.getBindModel();

        //地面は特別
        if(bindModel == AccessoryPose::GroundID){
            return;
        }
        bindBone_ = pose.getBindBone();

        lscene::AnimObject* target = models[bindModel].getObject();

        LASSERT(target != NULL);
        LASSERT(0<=bindBone_ && bindBone_<target->getNumJointPoses());

        const lanim::Joint& joint = target->getSkeleton()->getJoint( bindBone_ );

        const lmath::Matrix34* targetMatrices = target->getSkeletonPose()->getMatrices();

        mat.translate(joint.getPosition());

        mat.mul( targetMatrices[bindBone_], mat );
        mat.mul( target->getWorldMatrix(), mat );


        targetObject_ = target;
        targetMat_ = &targetMatrices[bindBone_];
    }

    void AccessoryPack::update(u32 /*frame*/)
    {
        if(targetObject_ == NULL){
            return;
        }

        const lanim::Joint& joint = targetObject_->getSkeleton()->getJoint( bindBone_ );

        lmath::Matrix34& mat = object_->getWorldMatrix();
        mat = matrix_;
        mat.translate( joint.getPosition() );

        mat.mul( *targetMat_, mat );
        mat.mul( targetObject_->getWorldMatrix(), mat );
        
    }
}

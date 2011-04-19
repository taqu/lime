/**
@file Pmd.cpp
@author t-sakai
@date 2010/05/16 create
*/
#if defined(ANDROID)
#include <stdio.h>
#include <stdarg.h>
#include <android/log.h>
#endif

#include "Pmd.h"
#include <lframework/scene/AnimObject.h>

#include <lcore/HashMap.h>
#include <lcore/String.h>
#include <lgraphics/lgraphicscore.h>
#include <lgraphics/api/VertexDeclarationRef.h>
#include <lgraphics/api/VertexBufferRef.h>
#include <lgraphics/api/GeometryBuffer.h>
#include <lgraphics/api/IndexBufferRef.h>
#include <lgraphics/api/RenderStateRef.h>
#include <lgraphics/api/TextureRef.h>

#include <lgraphics/io/IOBMP.h>
#include <lgraphics/io/IODDS.h>
#include <lgraphics/io/IOTGA.h>
#include <lgraphics/io/IOPNG.h>

#include <lframework/IOUtil.h>
#include <lframework/scene/Geometry.h>
#include <lframework/scene/Material.h>
#include <lframework/scene/AnimObject.h>

#include <lframework/anim/Skeleton.h>
#include <lframework/anim/Joint.h>
#include <lframework/anim/IKPack.h>

#include <lframework/scene/shader/DefaultShader.h>

#include "converter.h"
#include "SplitBone.h"

#if defined(_WIN32) && defined(_DEBUG)
//#define LIME_LIB_CONV_DEBUG (1)
#endif

using namespace lconverter;

namespace pmd
{

    using namespace lcore;
    using namespace lgraphics;

    // ������������W���C���g�L�[���[�h�BShift-JIS?
    const Char* LimitJointName[NumLimitJoint] =
    {
        "\x82\xD0\x82\xB4\x00", //"�Ђ�"
    };


    static const u32 NULL_BONE_INDEX = 0xFFFFU;
    static const u32 TexNameSize = lgraphics::MAX_NAME_BUFFER_SIZE;
    typedef lgraphics::NameString TextureName;

namespace
{
    void extractTextureName(TextureName& dst, const Char* path, u32 size)
    {
        LASSERT(path != NULL);
        u32 i;
        for(i=0; i<size; ++i){
            if(path[i] == '*'
                || path[i]=='\0'){
                break;
            }
        }
        dst.assign(path, i);
    }
}

    //--------------------------------------
    //---
    //--- Header
    //---
    //--------------------------------------
    void Header::swap(Header& rhs)
    {
        Header tmp;
        lcore::memcpy(&tmp, this, sizeof(Header));
        lcore::memcpy(this, &rhs, sizeof(Header));
        lcore::memcpy(&rhs, &tmp, sizeof(Header));
    }

    lcore::istream& operator>>(lcore::istream& is, Header& rhs)
    {
        lcore::io::read(is, rhs.magic_, Header::MagicSize);
        lcore::io::read(is, rhs.version_);
        lcore::io::read(is, rhs.name_, NameSize);
        lcore::io::read(is, rhs.comment_, CommentSize);
        return is;
    }

    //--------------------------------------
    //---
    //--- Vertex
    //---
    //--------------------------------------
    lcore::istream& operator>>(lcore::istream& is, Vertex& rhs)
    {
        //TODO: �@�������K������Ă��Ȃ��AUV���}�C�i�X�̏ꍇ�ǂ����邩
        FLOAT buffer[3];
        lcore::io::read(is, rhs.position_, sizeof(FLOAT)*3);

        lcore::io::read(is, buffer, sizeof(FLOAT)*3);
        rhs.normal_[0] = F32ToS16(buffer[0]);
        rhs.normal_[1] = F32ToS16(buffer[1]);
        rhs.normal_[2] = F32ToS16(buffer[2]);

        lcore::io::read(is, buffer, sizeof(FLOAT)*2);
        rhs.uv_[0] = F32ToS16Clamp(buffer[0]);
        rhs.uv_[1] = F32ToS16Clamp(buffer[1]);

        WORD boneNo[2];
        lcore::io::read(is, boneNo, sizeof(WORD)*2);

        rhs.element_[Vertex::Index_Bone0] = static_cast<BYTE>(boneNo[0]);
        rhs.element_[Vertex::Index_Bone1] = static_cast<BYTE>(boneNo[1]);

        lcore::io::read(is, rhs.element_[Vertex::Index_Weight]);
        lcore::io::read(is, rhs.element_[Vertex::Index_Edge]);
        return is;
    }

    //--------------------------------------
    //---
    //--- FaceIndex
    //---
    //--------------------------------------
    lcore::istream& operator>>(lcore::istream& is, FaceIndex& rhs)
    {
        lcore::io::read(is, rhs.index_);
        return is;
    }

    //--------------------------------------
    //---
    //--- Material
    //---
    //--------------------------------------
    lcore::istream& operator>>(lcore::istream& is, Material& rhs)
    {
        lcore::io::read(is, rhs.diffuse_, sizeof(FLOAT)*4);
        lcore::io::read(is, rhs.specularity_);
        lcore::io::read(is, rhs.specularColor_, sizeof(FLOAT)*3);
        lcore::io::read(is, rhs.ambient_, sizeof(FLOAT)*3);
        lcore::io::read(is, rhs.toonIndex_);
        //0xFFU�Ȃ�Œ�̃e�N�X�`���ɂȂ�̂ŁA�Œ��0�ɂ��đ��̃C���f�b�N�X���P���炷
        rhs.toonIndex_ += 1;
        lcore::io::read(is, rhs.edgeFlag_);
        lcore::io::read(is, rhs.faceVertexCount_);
        lcore::io::read(is, rhs.textureFileName_, sizeof(CHAR)*FileNameSize);
        return is;
    }

    //--------------------------------------
    //---
    //--- Bone
    //---
    //--------------------------------------
    void Bone::swap(Bone& rhs)
    {
        CHAR buffer[NameSize];
        lcore::memcpy(buffer, name_, NameSize);
        lcore::memcpy(name_, rhs.name_, NameSize);
        lcore::memcpy(rhs.name_, buffer, NameSize);

        lcore::swap(parentIndex_, rhs.parentIndex_);
        lcore::swap(tailPosIndex_, rhs.tailPosIndex_);
        lcore::swap(type_, rhs.type_);
        lcore::swap(ikParentIndex_, rhs.ikParentIndex_);
        lcore::swap(headPos_[0], rhs.headPos_[0]);
        lcore::swap(headPos_[1], rhs.headPos_[1]);
        lcore::swap(headPos_[2], rhs.headPos_[2]);
    }


    lcore::istream& operator>>(lcore::istream& is, Bone& rhs)
    {
        lcore::io::read(is, rhs.name_, NameSize);
        lcore::io::read(is, rhs.parentIndex_);
        lcore::io::read(is, rhs.tailPosIndex_);
        lcore::io::read(is, rhs.type_);
        lcore::io::read(is, rhs.ikParentIndex_);
        lcore::io::read(is, rhs.headPos_, sizeof(FLOAT)*3);
        return is;
    }

    //----------------------------------------------------------
    //---
    //--- IK
    //---
    //----------------------------------------------------------
    // IK�f�[�^�p�X�g���[�����[�h
    IK::IK()
        :childBoneIndex_(NULL)
        ,prevNumChildren_(0)
    {
    }

    IK::~IK()
    {
        LIME_DELETE_ARRAY(childBoneIndex_);
    }

    lcore::istream& operator>>(lcore::istream& is, IK& rhs)
    {
        lcore::io::read(is, rhs.boneIndex_);
        lcore::io::read(is, rhs.targetBoneIndex_);
        lcore::io::read(is, rhs.chainLength_);
        lcore::io::read(is, rhs.numIterations_);
        lcore::io::read(is, rhs.controlWeight_);

        rhs.controlWeight_ *= PI;

        if(rhs.prevNumChildren_<rhs.chainLength_){
            rhs.prevNumChildren_ = rhs.chainLength_;
            LIME_DELETE_ARRAY(rhs.childBoneIndex_);
            rhs.childBoneIndex_ = LIME_NEW WORD[rhs.chainLength_];
        }

        lcore::io::read(is, rhs.childBoneIndex_, sizeof(WORD)*rhs.chainLength_);

        return is;
    }

    //----------------------------------------------------------
    //---
    //--- Skin
    //---
    //----------------------------------------------------------
    // Skin�f�[�^�p�X�g���[�����[�h
    Skin::Skin()
        :numVertices_(0)
        ,vertices_(NULL)
    {
    }

    Skin::~Skin()
    {
        LIME_DELETE_ARRAY(vertices_);
    }

    lcore::istream& operator>>(lcore::istream& is, Skin& rhs)
    {
        LASSERT(rhs.vertices_ == NULL);

        CHAR name[NameSize];
        lcore::io::read(is, name, NameSize);
        lcore::io::read(is, rhs.numVertices_);

        BYTE type;
        lcore::io::read(is, type);
        rhs.type_ = type;

        rhs.vertices_ = LIME_NEW pmd::Skin::SkinVertex[rhs.numVertices_];

        for(u32 i=0; i<rhs.numVertices_; ++i){
            lcore::io::read(is, rhs.vertices_[i].index_);
            lcore::io::read(is, rhs.vertices_[i].position_, sizeof(f32)*3);
        }

        return is;
    }

    void Skin::swap(Skin& rhs)
    {
        lcore::swap(numVertices_, rhs.numVertices_);
        lcore::swap(vertices_, rhs.vertices_);
        lcore::swap(type_, rhs.type_);

    }


    //----------------------------------------------------------
    //---
    //--- SkinPack
    //---
    //----------------------------------------------------------
    SkinPack::SkinPack()
        :numSkins_(0)
        ,skins_(NULL)
        ,morphBaseVertices_(NULL)
    {
        indexRange_[0] = 0xFFFFU;
        indexRange_[1] = 0;
    }

    SkinPack::SkinPack(u32 numSkins)
        :numSkins_(numSkins)
        ,morphBaseVertices_(NULL)
    {
        if(numSkins_>0){
            skins_ = LIME_NEW Skin[numSkins_];
        }else{
            skins_ = NULL;
        }
        indexRange_[0] = 0xFFFFU;
        indexRange_[1] = 0;
    }

    SkinPack::~SkinPack()
    {
        LIME_DELETE_ARRAY(morphBaseVertices_);
        LIME_DELETE_ARRAY(skins_);
    }

    void SkinPack::swap(SkinPack& rhs)
    {
        lcore::swap(numSkins_, rhs.numSkins_);
        lcore::swap(skins_, rhs.skins_);
        lcore::swap(indexRange_[0], rhs.indexRange_[0]);
        lcore::swap(indexRange_[1], rhs.indexRange_[1]);
        lcore::swap(morphBaseVertices_, rhs.morphBaseVertices_);
    }

    void SkinPack::createMorphBaseVertices(u32 numVertices, const Vertex* vertices)
    {
        LASSERT(vertices != NULL);
        if(skins_ == NULL || morphBaseVertices_ != NULL){
            return;
        }

        Skin& skin = skins_[0];

        for(u32 i=0; i<skin.numVertices_; ++i){
            u32 index = skin.vertices_[i].index_;

            LASSERT(index<numVertices);
            if(index<indexRange_[0]){
                indexRange_[0] = index;
            }

            if(index>indexRange_[1]){
                indexRange_[1] = index;
            }
        }

        if(indexRange_[0]<=indexRange_[1]){
            u32 size = indexRange_[1] - indexRange_[0] + 1;
            morphBaseVertices_ = LIME_NEW Vertex[size];
        }

        u32 index = 0;
        for(u16 i=indexRange_[0]; i<=indexRange_[1]; ++i){
            morphBaseVertices_[index] = vertices[i];
            ++index;
        }
    }

    void SkinPack::sortVertexIndices()
    {
        Skin& skin = skins_[0];
        u16* toIndices = LIME_NEW u16[skin.numVertices_];
        u16 numVertices = static_cast<u16>(skin.numVertices_);
        for(u16 i=0; i<numVertices; ++i){
        }

        LIME_DELETE_ARRAY(toIndices);
    }


    //----------------------------------------------------------
    //---
    //--- DispLabel
    //---
    //----------------------------------------------------------
    DispLabel::DispLabel()
        :numBoneGroups_(0)
        ,buffer_(NULL)
        ,skinIndices_(NULL)
        ,boneLabels_(NULL)
        ,boneMap_(NULL)
    {
    }

    DispLabel::~DispLabel()
    {
        LIME_DELETE_ARRAY(buffer_);
        LIME_DELETE_ARRAY(boneMap_);
    }

    u16 DispLabel::getSkinLabelCount() const
    {
        return *reinterpret_cast<u16*>(buffer_);
    }

    u16 DispLabel::getBoneLabelCount() const
    {
        return *reinterpret_cast<u16*>(buffer_+sizeof(u16));
    }

    void DispLabel::read(lcore::istream& is)
    {
        u8 skinCount = 0;
        lcore::io::read(is, skinCount);

        s32 offsetToSkin = 0; //�\��C���f�b�N�X���X�g�̐擪�ւ̃I�t�Z�b�g

        //�\��C���f�b�N�X���X�g�X�L�b�v
        s32 offset = skinCount * sizeof(u16);
        offsetToSkin -= offset;
        is.seekg(offset, lcore::ios::cur);

        //�{�[�����x�����X�L�b�v
        u8 numBoneLabelNames = 0;        
        offsetToSkin -= sizeof(u8);
        lcore::io::read(is, numBoneLabelNames);

        numBoneGroups_ = numBoneLabelNames;

        offset = LabelNameSize * numBoneLabelNames;
        offsetToSkin -= offset;
        is.seekg(offset, lcore::ios::cur);

        offsetToSkin -= sizeof(u32);
        u32 numBoneLabels = 0;
        lcore::io::read(is, numBoneLabels);

        is.seekg(offsetToSkin, lcore::ios::cur);

        //�o�b�t�@�쐬
        u32 size = 2*sizeof(u16) + skinCount * sizeof(u16) + numBoneLabels * sizeof(BoneLabel);
        LIME_DELETE_ARRAY(buffer_);
        buffer_ = LIME_NEW u8[size];

        //�e���x�����ۑ�
        *reinterpret_cast<u16*>(buffer_) = skinCount;
        *reinterpret_cast<u16*>(buffer_+sizeof(u16)) = static_cast<u16>(numBoneLabels);

        skinIndices_ = reinterpret_cast<u16*>(buffer_ + 2*sizeof(u16));
        boneLabels_  = reinterpret_cast<BoneLabel*>(buffer_ + 2*sizeof(u16) + skinCount*sizeof(u16));

        //�\��C���f�b�N�X���[�h
        lcore::io::read(is, skinIndices_, skinCount*sizeof(u16));


        offset = sizeof(u8) + 50*numBoneLabelNames + sizeof(u32);
        is.seekg(offset, lcore::ios::cur);

        u8 label = 0;
        u16 boneIndex = 0;
        for(u32 i=0; i<numBoneLabels; ++i){
            lcore::io::read(is, boneIndex);
            lcore::io::read(is, label);

            boneLabels_[i].boneIndex_ = boneIndex;
            boneLabels_[i].labelIndex_ = label;
        }
    }

    void DispLabel::swapBoneMap(u16*& boneMap)
    {
        lcore::swap(boneMap, boneMap_);
    }

    void DispLabel::swap(DispLabel& rhs)
    {
        lcore::swap(numBoneGroups_, rhs.numBoneGroups_);
        lcore::swap(buffer_, rhs.buffer_);
        lcore::swap(skinIndices_, rhs.skinIndices_);
        lcore::swap(boneLabels_, rhs.boneLabels_);
        lcore::swap(boneMap_, rhs.boneMap_);
    }

    //--------------------------------------
    //---
    //--- Geometry
    //---
    //--------------------------------------
    Geometry::Geometry()
        :next_(NULL)
        ,materialIndex_(-1)
        ,palette_(NULL)
    {
    }

    Geometry::~Geometry()
    {
    }

    //--------------------------------------
    //---
    //--- Pack
    //---
    //--------------------------------------
    Pack::Pack()
        :numVertices_(0)
        ,vertices_(NULL)
        ,numFaceIndices_(0)
        ,faceIndices_(NULL)
        ,numMaterials_(0)
        ,materials_(NULL)
        ,numGeometries_(0)
        ,geometries_(NULL)
        ,numBones_(0)
        ,bones_(NULL)
        ,boneMap_(NULL)
        ,skeleton_(NULL)
        ,ikPack_(NULL)
    {
    }

    Pack::Pack(lconverter::NameTextureMap* nameTexMap)
        :numVertices_(0)
        ,vertices_(NULL)
        ,numFaceIndices_(0)
        ,faceIndices_(NULL)
        ,numMaterials_(0)
        ,materials_(NULL)
        ,numGeometries_(0)
        ,geometries_(NULL)
        ,numBones_(0)
        ,bones_(NULL)
        ,boneMap_(NULL)
        ,skeleton_(NULL)
        ,ikPack_(NULL)
        ,nameTexMap_(nameTexMap)
    {
    }

    Pack::~Pack()
    {
        release();
    }

    bool Pack::open(const char* filepath)
    {
        LASSERT(filepath != NULL);
        if(false == input_.open(filepath, lcore::ios::binary|lcore::ios::in)){
            //lcore::Log("pmd fail to open %s", filepath);
            return false;
        }

        return true;
    }


    bool Pack::loadInternal(const char* directory)
    {
        input_ >> header_;

        loadElements(input_, &vertices_, numVertices_);
        if(numVertices_>MaxVertices){ //�ő�l�𒴂���Ύ��s
            return false;
        }

        loadElements(input_, &faceIndices_, numFaceIndices_);
        if(numVertices_>MaxIndices){ //�ő�l�𒴂���Ύ��s
            return false;
        }

        loadElements(input_, &materials_, numMaterials_);

        loadElements(input_, &bones_, numBones_);
        if(MaxBones<numBones_){
            lcore::Log("pmd fail to load too many bones");
            return false;
        }

        


        //IK�쐬
        //-------------------------------------------------------
        u16 numIKs = 0;
        lcore::io::read(input_, numIKs);
        if(numIKs> 0){
            LASSERT(ikPack_ == NULL);
            IK ik;
            lanim::IKPack *pack = LIME_NEW lanim::IKPack(numIKs);
            for(u16 i=0; i<numIKs; ++i){
                input_ >> ik;

                lanim::IKEntry& entry = pack->get(i);
                entry.targetJoint_ = ik.boneIndex_;
                entry.effectorJoint_ = ik.targetBoneIndex_;
                entry.chainLength_ = ik.chainLength_;
                entry.numIterations_ = (MaxIKIterations<ik.numIterations_)? MaxIKIterations : ik.numIterations_;
                //entry.numIterations_ = ik.numIterations_;
                entry.limitAngle_ = ik.controlWeight_;

                entry.children_ = LIME_NEW u8[entry.chainLength_];
                for(u16 j=0; j<entry.chainLength_; ++j){
                    LASSERT(ik.childBoneIndex_[j]<=MaxBones);
                    entry.children_[j] = (0xFFU & ik.childBoneIndex_[j]);
                }
            }
            ikPack_ = pack;
        }


        {
            u16 numSkins = 0;
            lcore::io::read(input_, numSkins);
            SkinPack pack(numSkins);
            skinPack_.swap(pack);
            Skin* skins = skinPack_.getSkins();
            for(u32 i=0; i<skinPack_.getNumSkins(); ++i){
                input_ >> skins[i];
            }
        }


        //�{�[�����\�[�g���ă}�b�s���O
        boneMap_ = LIME_NEW u16[numBones_];
        sortBones();
        Bone* tmp = LIME_NEW Bone[numBones_];
        for(u16 i=0; i<numBones_; ++i){
            tmp[ boneMap_[i] ] = bones_[i];
        }
        LIME_DELETE_ARRAY(bones_);
        bones_ = tmp;
        tmp = NULL;

        for(u16 i=0; i<numBones_; ++i){
            if(bones_[i].parentIndex_ != lanim::InvalidJointIndex){
                bones_[i].parentIndex_ = boneMap_[ bones_[i].parentIndex_ ];
            }
            bones_[i].tailPosIndex_ = boneMap_[ bones_[i].tailPosIndex_ ];
        }

        for(u32 i=0; i<numVertices_; ++i){
            vertices_[i].element_[ Vertex::Index_Bone0 ] = static_cast<BYTE>( boneMap_[ vertices_[i].element_[ Vertex::Index_Bone0 ] ] );
            vertices_[i].element_[ Vertex::Index_Bone1 ] = static_cast<BYTE>( boneMap_[ vertices_[i].element_[ Vertex::Index_Bone1 ] ] );
        }


        if(numMaterials_>0){
            //���ǃ{�[����������܂ŁA���_�E�C���f�b�N�X�E�}�e���A���̏��̓������ɂȂ��Ƃ���
            splitBone(numVertices_, vertices_, numFaceIndices_, faceIndices_, numMaterials_, materials_, skinPack_.getSkins(), numBones_, lgraphics::LIME_MAX_SKINNING_MATRICES, numGeometries_, &geometries_);
        }

        numVertices_ = 0;
        LIME_DELETE_ARRAY(vertices_);

        // �X�P���g���쐬
        //------------------------------------------------------
        {
            lmath::Vector3 headPosition;
            lanim::Skeleton *skeleton = LIME_NEW lanim::Skeleton(numBones_);
            for(s32 i=0; i<numBones_; ++i){
                skeleton->getJointName(i).assignMemory( bones_[i].name_, pmd::NameSize );
                headPosition.set(bones_[i].headPos_[0], bones_[i].headPos_[1], bones_[i].headPos_[2]);

                lanim::Joint& joint = skeleton->getJoint(i);

                //�L�[���[�h���������W���C���g���Ȃ琧����݂���
                for(u32 j=0; j<NumLimitJoint; ++j){
                    if(NULL != lcore::strstr(skeleton->getJointName(i).c_str(), LimitJointName[j])){
                        joint.setFlag(lanim::JointFlag_IKLimitHeading);
                    }
                }

                joint.setPosition(headPosition);
                joint.setParentIndex(bones_[i].parentIndex_);
                joint.setType(bones_[i].type_);
            }
            skeleton->setNameMemory(header_.name_, NameSize);
            skeleton->recalcHash(); //�ĂіY���Ȃ�
            skeleton_ = skeleton;
        }


        //IK�̃{�[�����}�b�s���O
        //-------------------------------------------------------
        if(numIKs> 0){
            for(u16 i=0; i<numIKs; ++i){
                lanim::IKEntry& entry = ikPack_->get(i);
                entry.targetJoint_ = boneMap_[ entry.targetJoint_ ];
                entry.effectorJoint_ = boneMap_[  entry.effectorJoint_ ];

                for(u16 j=0; j<entry.chainLength_; ++j){
                    entry.children_[j] = (0xFFU & boneMap_[ entry.children_[j] ]);
                }
            }
        }

        dispLabel_.read(input_);
        dispLabel_.swapBoneMap(boneMap_);


        //�p�ꖼ���[�h
        loadNamesForENG();

        //
        loadToonTextures(directory);
        return true;
    }

    void Pack::release()
    {
        for(NameTextureMap::size_type pos = texMapInternal_.begin();
            pos != texMapInternal_.end();
            pos = texMapInternal_.next(pos))
        {
            TextureRef *tex = texMapInternal_.getValue(pos);
            LIME_DELETE(tex);
        }


        LIME_DELETE(ikPack_);
        LIME_DELETE(skeleton_);

        numBones_ = 0;
        LIME_DELETE_ARRAY(boneMap_);
        LIME_DELETE_ARRAY(bones_);

        numGeometries_= 0;
        Geometry *next = geometries_;
        while(geometries_ != NULL){
            next = geometries_->next_;
            LIME_DELETE(geometries_);
            geometries_ = next;
        }

        numMaterials_ = 0;
        LIME_DELETE_ARRAY(materials_);

        numFaceIndices_ = 0;
        LIME_DELETE_ARRAY(faceIndices_);

        numVertices_ = 0;
        LIME_DELETE_ARRAY(vertices_);

        input_.close();
    }


    namespace
    {
        enum RenderStateType
        {
            RenderState_Default,
            RenderState_AlphaBlend,
            RenderState_Num,
        };

        RenderStateRef createRenderState(RenderStateType type)
        {
            //�X�e�[�g�ݒ�͂Ă��Ƃ�
            RenderStateRef state;

            // Z�o�b�t�@�ݒ�
            state.setZEnable(true);

            //type = RenderState_Default;
            switch(type)
            {
            case RenderState_Default:
                {
                    state.setCullMode(lgraphics::CullMode_CCW);

                    state.setAlphaBlendEnable(false);
                    state.setAlphaTest(true);
                    state.setAlphaTestFunc(lgraphics::Cmp_Greater);

                    // Z�o�b�t�@�ݒ�
                    state.setZWriteEnable(true);
                }
                break;

            case RenderState_AlphaBlend:
                {
                    state.setCullMode(lgraphics::CullMode_None);

                    state.setAlphaBlendEnable(true);
                    state.setAlphaBlend(lgraphics::Blend_SrcAlpha, lgraphics::Blend_InvSrcAlpha);

                    state.setAlphaTest(false);
                    state.setAlphaTestFunc(lgraphics::Cmp_Greater);

                    // Z�o�b�t�@�ݒ�
                    state.setZWriteEnable(false);
                }
                break;
            };
            return state;
        }

        //-----------------------------------------------------------------------------
        /// �����_�����O�X�e�[�g�ݒ�
        void setRenderStateToMaterial(lscene::Material& dst, lgraphics::RenderStateRef* stock, u32 createFlag, RenderStateType type)
        {
            //�쐬���Ă��Ȃ���������
            u32 flag = 0x01U << type;
            if(0 == (createFlag & flag)){
                stock[type] = createRenderState(type);
                createFlag |= flag;
            }
            dst.setRenderState( stock[type] );
        }


        //-----------------------------------------------------------------------------
        /// PMD�̃}�e���A����lgraphics::Material�ɑ��
        void setMaterial(lscene::Material& dst, const pmd::Material& src)
        {
            dst.diffuse_.set(src.diffuse_[0], src.diffuse_[1], src.diffuse_[2], src.diffuse_[3]);
            dst.specular_.set(src.specularColor_[0], src.specularColor_[0], src.specularColor_[0], src.specularity_);
            dst.ambient_.set(src.ambient_[0], src.ambient_[1], src.ambient_[2]);
            dst.reflectance_ = 0.05f;

            u32 shader = lscene::shader::ShaderName[lscene::shader::Shader_Toon];
            dst.setShaderID( shader );
        }

        bool createVertexBuffer(VertexBufferRef& vb, pmd::VertexBuffer& pmdVB)
        {
            pmd::Vertex* vertices = &(pmdVB.elements_[0]);
            u32 numVertices = pmdVB.elements_.size();

            //���_�o�b�t�@�쐬
            vb = lgraphics::VertexBuffer::create(sizeof(pmd::Vertex), numVertices, Pool_Default, Usage_None);
            if(vb.valid() == false){
                return false;
            }

#if defined(LIME_GL)
            vb.blit(vertices, false);
#else
            pmd::Vertex* v = NULL;
            u32 vsize = sizeof(pmd::Vertex)*numVertices;
            if(false == vb.lock(0, vsize, (void**)&v)){
                return false;
            }
            lcore::memcpy(v, vertices, vsize);
            vb.unlock();
#endif

            return true;
        }
    }

    bool Pack::createObject(lscene::AnimObject& obj, const char* directory, bool swapOrigin)
    {
        LASSERT(directory != NULL);


        loadInternal(directory);

        // ���_�錾�쐬
        VertexDeclarationRef decl;
        IndexBufferRef ib;
        if(numGeometries_>0){
            {
                VertexDeclCreator creator(4);
                u16 voffset = 0;
                voffset += creator.add(0, voffset, DeclType_Float3, DeclMethod_Default, DeclUsage_Position, 0);
                voffset += creator.add(0, voffset, DeclType_Short4N, DeclMethod_Normalize, DeclUsage_Normal, 0);
                voffset += creator.add(0, voffset, DeclType_Short2N, DeclMethod_Normalize, DeclUsage_Texcoord, 0);
                voffset += creator.add(0, voffset, DeclType_UB4, DeclMethod_Default, DeclUsage_BlendIndicies, 0);

                creator.end(decl);
            }

            //�C���f�b�N�X�o�b�t�@�쐬
            ib = IndexBuffer::create(numFaceIndices_, Pool_Default, Usage_None);
            {
                if(ib.valid() == false){
                    return false;
                }

#if defined(LIME_GLES2)
                ib.blit(faceIndices_);
#else
                u16* indices = NULL;
                u32 isize = sizeof(u16)*numFaceIndices_;
                if(false == ib.lock(0, isize, (void**)&indices)){
                    return false;
                }
                lcore::memcpy(indices, reinterpret_cast<u16*>(faceIndices_), isize);
                ib.unlock();
#endif
            }
        }

        lscene::AnimObject tmp(numGeometries_, numMaterials_);

        if(numMaterials_>0){
            // �}�e���A���ݒ�
            lgraphics::RenderStateRef renderState[RenderState_Num];
            u32 stateCreateFlag = 0;
            RenderStateType renderStateType = RenderState_Default;

            TextureName texName;

            NameTextureMap *texMap = nameTexMap_;
            if(NULL == nameTexMap_){
                NameTextureMap tmp(numMaterials_*2);
                texMapInternal_.swap(tmp);
                texMap = &texMapInternal_;
            }

            for(u32 i=0; i<numMaterials_; ++i){
                Material& material = materials_[i];

                lscene::Material& dstMaterial = tmp.getMaterial(i);
                setMaterial(dstMaterial, material);

                extractTextureName(texName, material.textureFileName_, pmd::TexNameSize);
                TextureRef* texture = loadTexture(texName.c_str(), texName.size(), directory, *texMap);

                //�A���t�@�l���P�Ȃ�A���t�@�u�����h�Ȃ�
                if(lmath::isEqual(dstMaterial.diffuse_._w, 1.0f)){
                    renderStateType = RenderState_Default;
                }else{
                    renderStateType = RenderState_AlphaBlend;
                }

                if(texture != NULL){
                    if(swapOrigin){
                        lframework::io::swapOrigin(*texture);
                    }
                    dstMaterial.setTextureNum(1);
                    dstMaterial.setTexture(0, *texture);
                    //�e�N�X�`�����������̂Ńt���O���Ă�
                    dstMaterial.getFlags().setFlag(lscene::Material::MatFlag_Texture0);

                }else{
                    //�e�N�X�`�����Ȃ��Ȃ�g�D�[���e�N�X�`���𒲂ׂ�
                    if(material.toonIndex_<NumToonTextures){
                        texture = toonTextures_.textures_[ material.toonIndex_ ];
                        if(texture != NULL){
                            dstMaterial.setTextureNum(1);
                            dstMaterial.setTexture(0, *texture);
                            //�t���O���Ă�
                            dstMaterial.getFlags().setFlag(lscene::Material::MatFlag_Texture0);
                            dstMaterial.getFlags().setFlag(lscene::Material::MatFlag_TexGrad); //�e�N�X�`���ŃO���f�B�G�[�V��������

                            dstMaterial.getSamplerState(0).setAddressU( lgraphics::TexAddress_Clamp );
                            dstMaterial.getSamplerState(0).setAddressV( lgraphics::TexAddress_Clamp );
                        }
                    }
                }
                setRenderStateToMaterial(dstMaterial, renderState, stateCreateFlag, renderStateType);

            }
        }


        lanim::Skeleton::pointer skeleton( releaseSkeleton() );
        tmp.setSkeleton(skeleton);

        // GeometryBuffer�쐬
        GeometryBuffer::pointer geomBuffer;

        u32 geomIndex = 0;
        Geometry *geometry = geometries_;
        VertexBuffer* bufferPtr = NULL;
        while(geometry != NULL){
            if(bufferPtr != geometry->vertexBuffer_.get()){
                bufferPtr = geometry->vertexBuffer_.get();
                lgraphics::VertexBufferRef vb;
                if(false == createVertexBuffer(vb, *bufferPtr)){
                    return false;
                }

                skinPack_.createMorphBaseVertices(bufferPtr->elements_.size(), &(bufferPtr->elements_[0]));

                geomBuffer = LIME_NEW GeometryBuffer(Primitive_TriangleList, decl, vb, ib);
            }

            u16 count = static_cast<u16>( geometry->faceIndices_.size()/3 );
            lscene::Geometry geom(geomBuffer, count, geometry->indexOffset_, static_cast<u8>(geometry->materialIndex_));
            //geom.getFlags().setFlag(lscene::Geometry::GeomFlag_DiffuseVS);

            //�e�N�X�`���t���O�Z�b�g
            lscene::Material& material = tmp.getMaterial( geom.getMaterialIndex() );
            if(material.getTextureNum() > 0){
                geom.getFlags().setFlag(lscene::Geometry::GeomFlag_Texture0);
            }

            if(skeleton != NULL){
                geom.getFlags().setFlag(lscene::Geometry::GeomFlag_Skinning);
            }


            tmp.getGeometry(geomIndex).swap(geom);

            //�p���b�g�֌W�̓X���b�v����Ȃ��̂Œ���
            lscene::Geometry& tmpGeom = tmp.getGeometry(geomIndex);
            tmpGeom.setNumBonesInPalette( static_cast<u8>(geometry->palette_->size()) );
            for(u8 i=0; i<tmpGeom.getNumBonesInPalette(); ++i){
                tmpGeom.setBoneInPalette(i, geometry->palette_->get(i));
            }

            ++geomIndex;
            geometry = geometry->next_;
        }

        obj.swap(tmp);

        lanim::IKPack::pointer ikPack( releaseIKPack() );
        obj.setIKPack( ikPack );

        return true;
    }


    //-------------------------------------
    // �p�ꖼ���[�h
    void Pack::loadNamesForENG()
    {
        //�K�v�Ȃ��̂œǂݔ�΂�
        u8 enable = 0;
        lcore::io::read(input_, enable);

        if(enable != 0){
            Char buffer[CommentSize];
            lcore::io::read(input_, buffer, NameSize);
            lcore::io::read(input_, buffer, CommentSize);
            for(u16 i=0; i<numBones_; ++i){
                lcore::io::read(input_, buffer, NameSize);
            }
            for(u32 i=1; i<skinPack_.getNumSkins(); ++i){
                lcore::io::read(input_, buffer, NameSize);
            }
            for(u16 i=0; i<dispLabel_.getNumBoneGroups(); ++i){
                lcore::io::read(input_, buffer, LabelNameSize);
            }
        }

    }


    //-------------------------------------
    // �g�D�[���V�F�[�f�B���O�p�e�N�X�`�����[�h
    void Pack::loadToonTextures(const char* directory)
    {
        static const Char ToonTexDirectory[] = "../../Data/"; //�f�B���N�g���Œ�
        static const u32 ToonTexDirectorySize = sizeof(ToonTexDirectory); //�k�������܂�
        static const Char tex0[] = "toon0.bmp"; //0�Ԗڂ͌Œ�Ȃ񂾂Ƃ�

        u32 dlen = lcore::strlen(directory);
        u32 pathSize = dlen + ToonTexDirectorySize;
        if(pathSize>FilePathSize){
            return;
        }

        Char buffer[FilePathSize];
        lcore::memcpy(buffer, directory, dlen);
        lcore::memcpy(buffer + dlen, ToonTexDirectory, ToonTexDirectorySize);


        //0�Ԗڃ��[�h
        toonTextures_.textures_[0] = loadTexture(tex0, sizeof(tex0)-1, buffer, texMapInternal_, true);

        Char name[ToonTexturePathSize+1];

        for(u32 i=1; i<NumToonTextures; ++i){
            lcore::io::read(input_, name, ToonTexturePathSize);
            name[ToonTexturePathSize] = '\0';

            toonTextures_.textures_[i] = loadTexture(name, lcore::strlen(name), buffer, texMapInternal_, true);
        }

    }


    //-------------------------------------
    // �{�[����e���O�ɂȂ�悤�Ƀ\�[�g
    void Pack::sortBones()
    {
        LASSERT(bones_ != NULL);
        LASSERT(boneMap_ != NULL);

        for(u16 i=0; i<numBones_;){
            if(bones_[i].parentIndex_ == lanim::InvalidJointIndex
                || bones_[i].parentIndex_ <= i){
                boneMap_[i] = i;
                ++i;
            }else{
                u16 parent = bones_[i].parentIndex_;
                bones_[i].swap(bones_[parent]);
            }
        }
#if 0
        u16 count = 0;

        //�q�{�[�������Ԃɂ߂�
        u8 stack[MaxBones];
        u32 top = 0;
        for(u16 i=0; i<numBones_; ++i){
            //���[�g��������A�}�b�v�ɓ���ăX�^�b�N�ɐς�
            if(bones_[i].parentIndex_ == lanim::InvalidJointIndex){
                boneMap_[i] = count;
                ++count;

                stack[top++] = static_cast<u8>(i);
            }

            //�X�^�b�N����
            while(0<top){
                //�|�b�v����
                u16 bone = stack[--top];
                //���݂̃{�[�����e�̃{�[�����A�}�b�v�ɓ���ăX�^�b�N�ɐς�
                for(u16 j=0; j<numBones_; ++j){
                    if(bone == bones_[j].parentIndex_){
                        boneMap_[j] = count;
                        ++count;

                        stack[top++] = static_cast<u8>(j);
                    }
                }
            }
        }
#endif

#if defined(LIME_LIB_CONV_DEBUG)
        // debug out
        //-----------------------------------------------------------------------
        lcore::ofstream out("bonesort.txt", lcore::ios::binary);

        for(u16 i=0; i<numBones_; ++i){
            out.print("[%d] -> [%d]\n", i, boneMap_[i]);
        }
#endif

    }
}
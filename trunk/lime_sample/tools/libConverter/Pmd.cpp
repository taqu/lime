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

#include "SplitBone.h"

#if defined(_WIN32) && defined(_DEBUG)
#define LIME_LIB_CONV_DEBUG (1)
#endif

namespace pmd
{

    using namespace lcore;
    using namespace lgraphics;

    // 動き制限するジョイントキーワード。Shift-JIS?
    const Char* LimitJointName[NumLimitJoint] =
    {
        "\x82\xD0\x82\xB4\x00", //"ひざ"
    };


    static const u32 NULL_BONE_INDEX = 0xFFFFU;
    static const u32 TexNameSize = lgraphics::MAX_NAME_BUFFER_SIZE;
    typedef lgraphics::NameString TextureName;

    typedef lcore::HashMapCharArray<lgraphics::TextureRef*> NameTextureMap;

namespace
{
    void extractDirectoryPath(CHAR* dst, const CHAR* path, u32 size)
    {
        if(size<=0){
            return;
        }

        u32 i;
        for(i=size-1; 0<=i; --i){
            if(path[i] == '/'){
                break;
            }
        }
        u32 dstSize = i+1;
        for(i=0; i<dstSize; ++i){
            dst[i] = path[i];
        }
    }

    TextureRef* loadTexture(const CHAR* path, u32 size, const CHAR* directory, NameTextureMap& texMap)
    {
        TextureName name;
        u32 i;
        for(i=0; i<size; ++i){
            if(path[i] == '*'
                || path[i]=='\0'){
                break;
            }
        }
        name.assign(path, i);

        lframework::ImageFormat format = lframework::Img_None;
        const CHAR* ext = lframework::io::rFindChr(name.c_str(), '.', name.size()+1);
        if(ext == NULL){
            lcore::Log("pmd tex name has no ext(%s)", name.c_str());
            return NULL;

        }else{
            format = lframework::io::getFormatFromExt(ext+1);
        }

        u32 mapPos = texMap.find(name.c_str(), name.size());
        if(texMap.isEnd(mapPos) == false){
            //すでに同じ名前のテクスチャが存在した
            lcore::Log("pmd tex has already loaded");
            return texMap.getValue(mapPos);
        }

        char* filepath = NULL;
        if(directory == NULL){
            filepath = LIME_NEW char[name.size()+1];
            lcore::strncpy(filepath, name.size()+1, name.c_str(), name.size());
        }else{
            u32 dlen = lcore::strlen(directory);
            u32 pathLen = dlen + name.size() + 1;
            filepath = LIME_NEW char[pathLen];
            lcore::memcpy(filepath, directory, dlen);
            lcore::memcpy(filepath+dlen, name.c_str(), name.size() + 1);
        }

        lcore::ifstream is(filepath, lcore::ios::in|lcore::ios::binary);
        if(is.is_open() == false){
            lcore::Log("pmd failt to open tex(%s)", filepath);
            LIME_DELETE_ARRAY(filepath);
            return NULL;
        }

        TextureRef *texture = LIME_NEW TextureRef;
        if(texture == NULL){
            LIME_DELETE_ARRAY(filepath);
            return NULL;
        }

        bool ret = false;
        switch(format)
        {
        case lframework::Img_BMP:
            ret = lgraphics::io::IOBMP::read(is, *texture);
            break;

        case lframework::Img_TGA:
            ret = lgraphics::io::IOTGA::read(is, *texture);
            break;

        case lframework::Img_DDS:
            ret = lgraphics::io::IODDS::read(is, *texture);
            break;

        case lframework::Img_PNG:
            ret = lgraphics::io::IOPNG::read(is, *texture);
            break;

        default:
            break;
        };

        if(ret){
            texture->setName(name);

            texMap.insert(texture->getName().c_str(), texture->getName().size(), texture);
            lcore::Log("pmd success to load tex(%s)", filepath);
        }else{
            LIME_DELETE(texture);
            lcore::Log("pmd fail to load tex(%s)", filepath);
        }

        LIME_DELETE_ARRAY(filepath);
        return texture;
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
        lcore::io::read(is, rhs.comment_, Header::CommentSize);
        return is;
    }

    //--------------------------------------
    //---
    //--- Vertex
    //---
    //--------------------------------------
    lcore::istream& operator>>(lcore::istream& is, Vertex& rhs)
    {
        lcore::io::read(is, rhs.position_, sizeof(FLOAT)*3);
        lcore::io::read(is, rhs.normal_, sizeof(FLOAT)*3);
        lcore::io::read(is, rhs.uv_, sizeof(FLOAT)*2);

        WORD boneNo[2];
        lcore::io::read(is, boneNo, sizeof(WORD)*2);

        rhs.boneNo_[0] = static_cast<BYTE>(boneNo[0]);
        rhs.boneNo_[1] = static_cast<BYTE>(boneNo[1]);

        lcore::io::read(is, rhs.boneWeight_);
        lcore::io::read(is, rhs.edgeFlag_);
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
    // IKデータ用ストリームロード
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
    // Skinデータ用ストリームロード
    Skin::Skin()
        :vertices_(NULL)
    {
    }

    Skin::~Skin()
    {
        LIME_DELETE_ARRAY(vertices_);
    }

    lcore::istream& operator>>(lcore::istream& is, Skin& rhs)
    {
        LASSERT(rhs.vertices_ == NULL);

        lcore::io::read(is, rhs.name_, NameSize);
        lcore::io::read(is, rhs.numVertices_);
        lcore::io::read(is, rhs.type_);

        rhs.vertices_ = LIME_NEW pmd::Skin::SkinVertex[rhs.numVertices_];

        for(u32 i=0; rhs.numVertices_; ++i){
            lcore::io::read(is, rhs.vertices_[i].base_.index_);
            lcore::io::read(is, rhs.vertices_[i].base_.position_, sizeof(FLOAT)*3);
        }

        return is;
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
        ,numSkins_(0)
        ,skins_(NULL)
        ,skeleton_(NULL)
        ,ikPack_(NULL)
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
            lcore::Log("pmd fail to open file");
            return false;
        }

        return true;
    }


    bool Pack::loadInternal()
    {
        input_ >> header_;

        loadElements(input_, &vertices_, numVertices_);
        if(numVertices_>MaxVertices){
            return false;
        }

        loadElements(input_, &faceIndices_, numFaceIndices_);
        if(numVertices_>MaxIndices){
            return false;
        }

        loadElements(input_, &materials_, numMaterials_);

        loadElements(input_, &bones_, numBones_);
        if(MaxBones<numBones_){
            lcore::Log("pmd fail to load too many bones");
            return false;
        }

        //ボーンをソートしてマッピング
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
            vertices_[i].boneNo_[0] = static_cast<BYTE>( boneMap_[ vertices_[i].boneNo_[0] ] );
            vertices_[i].boneNo_[1] = static_cast<BYTE>( boneMap_[ vertices_[i].boneNo_[1] ] );
        }


        //結局ボーン分割するまで、頂点・インデックス・マテリアルの情報はメモリにないとだめ
        splitBone(numVertices_, vertices_, numFaceIndices_, faceIndices_, numMaterials_, materials_, numBones_, lgraphics::LIME_MAX_SKINNING_MATRICES, numGeometries_, &geometries_);

        numVertices_ = 0;
        LIME_DELETE_ARRAY(vertices_);

        // スケルトン作成
        //------------------------------------------------------
        {
            lmath::Vector3 headPosition;
            lanim::Skeleton *skeleton = LIME_NEW lanim::Skeleton(numBones_);
            for(s32 i=0; i<numBones_; ++i){
                skeleton->getJointName(i).assignMemory( bones_[i].name_, pmd::NameSize );
                headPosition.set(bones_[i].headPos_[0], bones_[i].headPos_[1], bones_[i].headPos_[2]);

                lanim::Joint& joint = skeleton->getJoint(i);

                //キーワードが入ったジョイント名なら制限を設ける
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
            skeleton->recalcHash(); //呼び忘れるなあ
            skeleton_ = skeleton;
        }


        //IK作成
        //-------------------------------------------------------
        u16 numIKs = 0;
        lcore::io::read(input_, numIKs);
        if(numIKs> 0){
            lcore::Log("start to create IKPack");
            LASSERT(ikPack_ == NULL);
            IK ik;
            lanim::IKPack *pack = LIME_NEW lanim::IKPack(numIKs);
            for(u16 i=0; i<numIKs; ++i){
                input_ >> ik;

                lanim::IKEntry& entry = pack->get(i);
                entry.joint_ = boneMap_[ ik.boneIndex_ ];
                entry.targetJoint_ = boneMap_[  ik.targetBoneIndex_ ];
                entry.chainLength_ = ik.chainLength_;
                entry.numIterations_ = (MaxIKIterations<ik.numIterations_)? MaxIKIterations : ik.numIterations_;
                entry.limitAngle_ = ik.controlWeight_;

                entry.children_ = LIME_NEW u8[entry.chainLength_];
                for(u16 j=0; j<entry.chainLength_; ++j){
                    LASSERT(ik.childBoneIndex_[j]<=MaxBones);

                    ik.childBoneIndex_[j] = boneMap_[ ik.childBoneIndex_[j] ];
                    entry.children_[j] = (0xFFU & ik.childBoneIndex_[j]);
                }
            }
            ikPack_ = pack;
            lcore::Log("end to create IKPack");
        }


        loadElements(input_, &skins_, numSkins_);


        return true;
    }

    void Pack::release()
    {
        LIME_DELETE(ikPack_);
        LIME_DELETE(skeleton_);

        numSkins_ = 0;
        LIME_DELETE_ARRAY(skins_);

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
        RenderStateRef createRenderState(bool alphaBlend)
        {
            //ステート設定はてきとう
            RenderStateRef::begin();
            RenderStateRef::setCullMode(lgraphics::CullMode_CCW);

            // アルファ設定
            if(alphaBlend){
                RenderStateRef::setAlphaBlendEnable(true);
                RenderStateRef::setAlphaBlend(lgraphics::Blend_SrcAlpha, lgraphics::Blend_InvSrcAlpha);

            }else{
                RenderStateRef::setAlphaBlendEnable(false);
            }

            RenderStateRef::setAlphaTest(true);
            RenderStateRef::setAlphaTestFunc(lgraphics::Cmp_Less);

            // Zバッファ設定
            RenderStateRef::setZEnable(true);
            RenderStateRef::setZWriteEnable(true);

            RenderStateRef state;
            RenderStateRef::end(state);
            return state;
        }

        //-----------------------------------------------------------------------------
        /// PMDのマテリアルをlgraphics::Materialに代入
        void setMaterial(lscene::Material& dst, const pmd::Material& src)
        {
            dst.shading_.assign("toon", 4);
            dst.diffuse_.set(src.diffuse_[0], src.diffuse_[1], src.diffuse_[2], src.diffuse_[3]);
            dst.specular_.set(src.specularColor_[0], src.specularColor_[0], src.specularColor_[0], src.specularity_);
            dst.ambient_.set(src.ambient_[0], src.ambient_[0], src.ambient_[0]);

            dst.getFlags().setFlag(lscene::Material::MatFlag_Fresnel);
        }

        bool createVertexBuffer(VertexBufferRef& vb, pmd::VertexBuffer& pmdVB)
        {
            struct VertexInternal
            {
                f32 position_[3];
                f32 normal_[3];
                f32 uv_[2];
                u8 boneIndex_[4];
                f32 weight_;
            };


            VertexBuffer::ElementVector& vertices = pmdVB.elements_;
            u32 numVertices = vertices.size();

            //頂点バッファ作成
            vb = lgraphics::VertexBuffer::create(sizeof(VertexInternal), numVertices, Pool_Default, Usage_None);
            if(vb.valid() == false){
                return false;
            }

            VertexInternal* v = NULL;
#if defined(LIME_GLES2)
            v = LIME_NEW VertexInternal[numVertices];
            if(v == NULL){
                return false;
            }
#else
            u32 vsize = sizeof(Vertex)*numVertices;
            if(false == vb.lock(0, vsize, (void**)&v)){
                return false;
            }
#endif
            for(u32 i=0; i<numVertices; ++i){
                v[i].position_[0] = vertices[i].position_[0];
                v[i].position_[1] = vertices[i].position_[1];
                v[i].position_[2] = vertices[i].position_[2];

                v[i].normal_[0] = vertices[i].normal_[0];
                v[i].normal_[1] = vertices[i].normal_[1];
                v[i].normal_[2] = vertices[i].normal_[2];

                v[i].uv_[0] = vertices[i].uv_[0];
                v[i].uv_[1] = vertices[i].uv_[1];

                v[i].boneIndex_[0] = vertices[i].boneNo_[0];
                v[i].boneIndex_[1] = vertices[i].boneNo_[1];
                v[i].boneIndex_[2] = vertices[i].boneNo_[0];
                v[i].boneIndex_[3] = vertices[i].boneNo_[1];

                v[i].weight_ = 0.01f*vertices[i].boneWeight_;
            }
#if defined(LIME_GLES2)
            vb.blit(v);
            LIME_DELETE_ARRAY(v);
#else
            vb.unlock();
#endif
            return true;
        }

    }

    bool Pack::createObject(lscene::AnimObject& obj, const char* directory, bool swapOrigin)
    {
        LASSERT(directory != NULL);


        loadInternal();

        // 頂点宣言作成
        VertexDeclarationRef decl;
        {
            VertexDeclCreator creator(5);
            u16 voffset = 0;
            voffset += creator.add(0, voffset, DeclType_Float3, DeclUsage_Position, 0);
            voffset += creator.add(0, voffset, DeclType_Float3, DeclUsage_Normal, 0);
            voffset += creator.add(0, voffset, DeclType_Float2, DeclUsage_Texcoord, 0);
            voffset += creator.add(0, voffset, DeclType_UB4, DeclUsage_BlendIndicies, 0);
            voffset += creator.add(0, voffset, DeclType_Float1, DeclUsage_BlendWeight, 0);

            creator.end(decl);
        }

        //インデックスバッファ作成
        IndexBufferRef ib = IndexBuffer::create(numFaceIndices_, Pool_Default, Usage_None);
        {
            if(ib.valid() == false){
                return false;
            }

            u32 count = numFaceIndices_;
            u16* indices = NULL;
#if defined(LIME_GLES2)
            indices = LIME_NEW u16[count];
            if(indices == NULL){
                return false;
            }
#else
            u32 isize = sizeof(u16)*count;
            if(false == ib.lock(0, isize, (void**)&indices)){
                return false;
            }
#endif
            for(u32 i=0; i<count; ++i){
                indices[i] = faceIndices_[i].index_;
            }
#if defined(LIME_GLES2)
            ib.blit(indices);
            LIME_DELETE_ARRAY(indices);
#else
            ib.unlock();
#endif
        }

        lscene::AnimObject tmp(numGeometries_, numMaterials_);

        // マテリアル設定
        lgraphics::RenderStateRef renderState = createRenderState(false);
        lgraphics::RenderStateRef renderStateAlphaBlend = createRenderState(true);

        NameTextureMap texMap(numMaterials_*2);

        for(u32 i=0; i<numMaterials_; ++i){
            Material& material = materials_[i];

            lscene::Material& dstMaterial = tmp.getMaterial(i);
            setMaterial(dstMaterial, material);

            //アルファ値が１ならアルファブレンドなし
            if( lmath::isEqual(dstMaterial.diffuse_._w, 1.0f) ){
                dstMaterial.setRenderState(renderState);
            }else{
                dstMaterial.setRenderState(renderStateAlphaBlend);
            }

            TextureRef* texture = loadTexture(material.textureFileName_, pmd::FileNameSize, directory, texMap);

            if(texture != NULL){
                if(swapOrigin){
                    lframework::io::swapOrigin(*texture);
                }
                dstMaterial.setTextureNum(1);
                dstMaterial.setTexture(0, *texture);
                //テクスチャがあったのでフラグ立てる
                dstMaterial.getFlags().setFlag(lscene::Material::MatFlag_Texture0);
            }
        }

        for(NameTextureMap::size_type pos = texMap.begin();
            pos != texMap.end();
            pos = texMap.next(pos))
        {
            TextureRef *tex = texMap.getValue(pos);
            LIME_DELETE(tex);
        }


        lanim::Skeleton::pointer skeleton( releaseSkeleton() );
        tmp.setSkeleton(skeleton);

        // GeometryBuffer作成
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

                geomBuffer = LIME_NEW GeometryBuffer(Primitive_TriangleList, decl, vb, ib);
            }

            u16 count = static_cast<u16>( geometry->faceIndices_.size()/3 );
            lscene::Geometry geom(geomBuffer, count, geometry->indexOffset_, static_cast<u8>(geometry->materialIndex_));
            geom.getFlags().setFlag(lscene::Geometry::GeomFlag_DiffuseVS);

            //テクスチャフラグセット
            lscene::Material& material = tmp.getMaterial( geom.getMaterialIndex() );
            if(material.getTextureNum() > 0){
                geom.getFlags().setFlag(lscene::Geometry::GeomFlag_Texture0);
            }

            if(skeleton != NULL){
                geom.getFlags().setFlag(lscene::Geometry::GeomFlag_Skinning);
            }


            tmp.getGeometry(geomIndex).swap(geom);

            //パレット関係はスワップされないので注意
            lscene::Geometry& tmpGeom = tmp.getGeometry(geomIndex);
            tmpGeom.setNumBonesInPalette( static_cast<u8>(geometry->palette_->size()) );
            for(u8 i=0; i<tmpGeom.getNumBonesInPalette(); ++i){
                tmpGeom.setBoneInPalette(i, geometry->palette_->get(i));
            }

            ++geomIndex;
            geometry = geometry->next_;
        }

        obj.swap(tmp);

        return true;
    }


    void Pack::sortBones()
    {
        LASSERT(bones_ != NULL);
        LASSERT(boneMap_ != NULL);

        u16 count = 0;

        //子ボーンを順番につめる
        u8 stack[MaxBones];
        u32 top = 0;
        for(u16 i=0; i<numBones_; ++i){
            //ルートだったら、マップに入れてスタックに積む
            if(bones_[i].parentIndex_ == lanim::InvalidJointIndex){
                boneMap_[i] = count;
                ++count;

                stack[top++] = static_cast<u8>(i);
            }

            //スタック処理
            while(0<top){
                //ポップする
                u16 bone = stack[--top];
                //現在のボーンが親のボーンを、マップに入れてスタックに積む
                for(u16 j=0; j<numBones_; ++j){
                    if(bone == bones_[j].parentIndex_){
                        boneMap_[j] = count;
                        ++count;

                        stack[top++] = static_cast<u8>(j);
                    }
                }
            }
        }

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

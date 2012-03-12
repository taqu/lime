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
#include "../scene/AnimObject.h"

#include <lcore/HashMap.h>
#include <lcore/String.h>
#include <lcore/utility.h>
#include <lgraphics/lgraphicscore.h>
#include <lgraphics/api/Enumerations.h>
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
#include "../scene/Geometry.h"
#include "../scene/Material.h"

#include <lframework/anim/Skeleton.h>
#include <lframework/anim/Joint.h>
#include <lframework/anim/IKPack.h>

#include "../scene//shader/DefaultShader.h"

#include "charcode/conv_charcode.h"
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

    // 動き制限するジョイントキーワード。Shift-JIS?
    const Char* LimitJointName[NumLimitJoint] =
    {
        "\x82\xD0\x82\xB4\x00", //"ひざ"
    };


    static const u32 NULL_BONE_INDEX = 0xFFFFU;
    static const u32 TexNameSize = lgraphics::MAX_NAME_BUFFER_SIZE;
    typedef lgraphics::NameString TextureName;

namespace
{
    /**
    @brief アスタリスクで文字列を分割抽出
    @return 次の文字列の先頭ポインタ。なければNULL
    */
    Char* extractTextureName(Char* path)
    {
        LASSERT(path != NULL);

        while(*path != '\0'){
            if(*path == '*'){
                do{
                    *path = '\0';
                    ++path;
                }while(*path == '*');

                return path;
            }
            ++path;
        }
        return NULL;
    }
}

namespace
{
    enum RenderStateType
    {
        RenderState_Default,
        RenderState_AlphaBlend,
        RenderState_Num,
    };

    RenderStateRef createRenderState(RenderStateType type, bool alphaTest)
    {
        //ステート設定はてきとう
        RenderStateRef state;

        // Zバッファ設定
        state.setZEnable(true);

        //type = RenderState_Default;
        switch(type)
        {
        case RenderState_Default:
            {
                state.setCullMode(lgraphics::CullMode_CCW);
                //state.setCullMode(lgraphics::CullMode_None);

                state.setAlphaBlendEnable(false);
                state.setAlphaTest(alphaTest);
                state.setAlphaTestFunc(lgraphics::Cmp_Greater);

                // Zバッファ設定
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

                // Zバッファ設定
                state.setZWriteEnable(true); //普通は書き込み禁止にするが、ソートしないかわりに書き込みしてるらしい
            }
            break;

        default:
            LASSERT(!"unknown state type");
            break;
        };
        return state;
    }

    //-----------------------------------------------------------------------------
    /// レンダリングステート設定
    void setRenderStateToMaterial(lscene::Material& dst, lgraphics::RenderStateRef* stock, u32& createFlag, RenderStateType type, bool alphaTest)
    {
        //作成していなかったら作る
        u32 flag = 0x01U << type;
        if(0 == (createFlag & flag)){
            stock[type] = createRenderState(type, alphaTest);
            createFlag |= flag;
        }
        dst.setRenderState( stock[type] );
    }


    //-----------------------------------------------------------------------------
    /// PMDのマテリアルをlgraphics::Materialに代入
    void setMaterial(lscene::Material& dst, const pmd::Material& src)
    {
        dst.diffuse_.set(src.diffuse_[0], src.diffuse_[1], src.diffuse_[2], src.diffuse_[3]);
        dst.specular_.set(src.specularColor_[0], src.specularColor_[1], src.specularColor_[2], src.specularity_);
        dst.ambient_.set(src.ambient_[0], src.ambient_[1], src.ambient_[2]);
        dst.reflectance_ = 0.05f;

        u32 shader = lscene::shader::ShaderName[lscene::shader::Shader_Toon];
        dst.setShaderID( shader );
    }

    bool createVertexBuffer(VertexBufferRef& vb, pmd::VertexBuffer& pmdVB)
    {
        pmd::Vertex* vertices = &(pmdVB.elements_[0]);
        u32 numVertices = pmdVB.elements_.size();

        //頂点バッファ作成
#if defined(LIME_GLES2)
        vb = lgraphics::VertexBuffer::create(sizeof(pmd::Vertex), numVertices, Pool_Managed, Usage_None);
        if(vb.valid() == false){
            return false;
        }
        vb.blit(vertices, true);
#else
        vb = lgraphics::VertexBuffer::create(sizeof(pmd::Vertex), numVertices, Pool_Managed, Usage_None);
        if(vb.valid() == false){
            return false;
        }
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


    //--------------------------------------------------------------------
    // マルチストリーム頂点バッファ作成
    bool createVertexBuffer(VertexBufferRef& vb0, VertexBufferRef& vb1, VertexBufferRef& vb0Back, pmd::VertexBuffer& pmdVB)
    {
        pmd::Vertex* vertices = &(pmdVB.elements_[0]);
        u32 numVertices = pmdVB.elements_.size();

        static const u32 SizeVertex0 = sizeof(f32) * 3;
        static const u32 SizeVertex1 = 16;

        //頂点バッファ作成
#if defined(LIME_GLES2)

        lcore::ScopedArrayPtr<u8> tmp( LIME_NEW u8[ SizeVertex1 * numVertices ] );

        // ひとつ目
        lmath::Vector3* v0 = reinterpret_cast<lmath::Vector3*>( tmp.get() );
        for(u32 i=0; i<numVertices; ++i){
            v0[i].set(vertices[i].position_[0], vertices[i].position_[1], vertices[i].position_[2]);
        }

        vb0 = lgraphics::VertexBuffer::create(SizeVertex0, numVertices, Pool_Managed, Usage_None);
        if(vb0.valid() == false){
            return false;
        }
        vb0.blit(v0, true);

        //バックバッファも作成
        vb0Back = lgraphics::VertexBuffer::create(SizeVertex1, numVertices, Pool_Managed, Usage_None);
        if(vb0Back.valid() == false){
            return false;
        }
        vb0Back.blit(v0, true);

        // ふたつ目
        VertexStream1* v1 = reinterpret_cast<VertexStream1*>( tmp.get() );
        for(u32 i=0; i<numVertices; ++i){
            v1[i].normal_[0] = vertices[i].normal_[0];
            v1[i].normal_[1] = vertices[i].normal_[1];
            v1[i].normal_[2] = vertices[i].normal_[2];
            v1[i].normal_[3] = vertices[i].normal_[3];

            v1[i].uv_[0] = vertices[i].uv_[0];
            v1[i].uv_[1] = vertices[i].uv_[1];

            v1[i].element_[0] = vertices[i].element_[0];
            v1[i].element_[1] = vertices[i].element_[1];
            v1[i].element_[2] = vertices[i].element_[2];
            v1[i].element_[3] = vertices[i].element_[3];
        }

        vb1 = lgraphics::VertexBuffer::create(SizeVertex1, numVertices, Pool_Managed, Usage_None);
        if(vb1.valid() == false){
            return false;
        }
        vb1.blit(v1, false);

#else
        lcore::ScopedArrayPtr<u8> tmp( LIME_NEW u8[ SizeVertex1 * numVertices ] );

        // ひとつ目
        lmath::Vector3* v0 = reinterpret_cast<lmath::Vector3*>( tmp.get() );
        for(u32 i=0; i<numVertices; ++i){
            v0[i].set(vertices[i].position_[0], vertices[i].position_[1], vertices[i].position_[2]);
        }

        vb0 = lgraphics::VertexBuffer::create(SizeVertex0, numVertices, Pool_Managed, Usage_None);
        if(vb0.valid() == false){
            return false;
        }
        vb0.blit(v0, true);

        //バックバッファも作成
        vb0Back = lgraphics::VertexBuffer::create(SizeVertex1, numVertices, Pool_Managed, Usage_None);
        if(vb0Back.valid() == false){
            return false;
        }
        vb0Back.blit(v0, true);

        // ふたつ目
        VertexStream1* v1 = reinterpret_cast<VertexStream1*>( tmp.get() );
        for(u32 i=0; i<numVertices; ++i){
            v1[i].normal_[0] = vertices[i].normal_[0];
            v1[i].normal_[1] = vertices[i].normal_[1];
            v1[i].normal_[2] = vertices[i].normal_[2];
            v1[i].normal_[3] = vertices[i].normal_[3];

            v1[i].uv_[0] = vertices[i].uv_[0];
            v1[i].uv_[1] = vertices[i].uv_[1];

            v1[i].element_[0] = vertices[i].element_[0];
            v1[i].element_[1] = vertices[i].element_[1];
            v1[i].element_[2] = vertices[i].element_[2];
            v1[i].element_[3] = vertices[i].element_[3];
        }

        vb1 = lgraphics::VertexBuffer::create(SizeVertex1, numVertices, Pool_Managed, Usage_None);
        if(vb1.valid() == false){
            return false;
        }
        vb1.blit(v1, false);
#endif

        return true;
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
        //TODO: 法線が正規化されていない、UVがマイナスの場合どうするか
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
        //0xFFUなら固定のテクスチャになるので、固定を0にして他のインデックスを１ずらす
        rhs.toonIndex_ += 1;
        lcore::io::read(is, rhs.edgeFlag_);
        lcore::io::read(is, rhs.faceVertexCount_);
        lcore::io::read(is, rhs.textureFileName_, sizeof(CHAR)*FileNameSize);
        rhs.textureFileName_[FileNameSize] = '\0'; //必ずヌル文字が入るように

        strSJISToUTF8(rhs.textureFileName_, Material::PathBufferSize); //UTF8へ変換

        rhs.textureFileNames_[Material::TexType_Albedo] = NULL;
        rhs.textureFileNames_[Material::TexType_Sphere] = NULL;
        rhs.sphereAdd_ = 0;

        //先にテクスチャ名を抽出
        CHAR* path = rhs.textureFileName_;
        CHAR* next = path;
        do{
            next = extractTextureName( path );
            if(*path != '\0'){

                //拡張子判別
                const Char* ext = lcore::rFindChr(path, '.', lcore::strlen(path));
                if(ext != NULL){
                    lframework::ImageFormat format = lframework::io::getFormatFromExt(ext+1);
                    switch(format)
                    {
                        case lframework::Img_BMP:
                        case lframework::Img_TGA:
                        case lframework::Img_PNG:
                        case lframework::Img_JPG:
                            rhs.textureFileNames_[Material::TexType_Albedo] = path;
                            break;

                        case lframework::Img_SPH:
                            rhs.sphereAdd_ = 0;
                            rhs.textureFileNames_[Material::TexType_Sphere] = path;
                            break;

                        case lframework::Img_SPA:
                            rhs.sphereAdd_ = 1;
                            rhs.textureFileNames_[Material::TexType_Sphere] = path;
                            break;

                        default:
                            break;
                    }
                }
            }
            path = next;
        }while(next != NULL);

        //シェーダのpow関数の値域におさめるために補正
        if(rhs.specularity_ < 0.01f){
            rhs.specularity_ = 0.01f;
        }

        //色と環境光の補正
        //for(u32 i=0; i<3; ++i){
        //    rhs.diffuse_[i] *= ColorRatio;
        //    rhs.ambient_[i] *= AmbientRatio;
        //}
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

    Bone& Bone::operator=(const Bone& rhs)
    {
        lcore::memcpy(name_, rhs.name_, NameSize);

        parentIndex_ = rhs.parentIndex_;
        tailPosIndex_ = rhs.tailPosIndex_;
        type_ = rhs.type_;
        ikParentIndex_ = rhs.ikParentIndex_;

        headPos_[0] = rhs.headPos_[0];
        headPos_[1] = rhs.headPos_[1];
        headPos_[2] = rhs.headPos_[2];
        return *this;
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

        rhs.controlWeight_ *= PI2;

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
    //マルチストリーム実験
#if defined(LIME_LIBCONVERT_PMD_USE_MULTISTREAM)

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
        vb_.swap( rhs.vb_ );
    }

    void SkinPack::createMorphBaseVertices(u32 numVertices, const Vertex* vertices, lgraphics::VertexBufferRef& backBuffer)
    {
        LASSERT(vertices != NULL);
        if(skins_ == NULL || morphBaseVertices_ != NULL){
            return;
        }

        //ベースを検索
        for(u32 i=1; i<numSkins_; ++i){
            if(skins_[i].type_ == 0){
                skins_[0].swap(skins_[i]);
            }
        }

        //インデックス幅計算
        Skin& skin = skins_[0];

        for(u32 i=0; i<skin.numVertices_; ++i){
            u32 index = skin.vertices_[i].index_;

            LASSERT(index<numVertices);
            if(index<indexRange_[0]){
                indexRange_[0] = static_cast<u16>(index);
            }

            if(index>indexRange_[1]){
                indexRange_[1] = static_cast<u16>(index);
            }
        }

        if(indexRange_[0]<=indexRange_[1]){
            u32 size = indexRange_[1] - indexRange_[0] + 1;
            morphBaseVertices_ = LIME_NEW lmath::Vector3[size];

        }
        //ダブルバッファ用頂点バッファ
        vb_ = backBuffer;

        u32 index = 0;
        for(u16 i=indexRange_[0]; i<=indexRange_[1]; ++i){
            morphBaseVertices_[index].set(vertices[i].position_[0], vertices[i].position_[1], vertices[i].position_[2]);
            ++index;
        }
    }

    void SkinPack::sortVertexIndices()
    {
        LASSERT(false);
        Skin& skin = skins_[0];
        u16* toIndices = LIME_NEW u16[skin.numVertices_];
        u16 numVertices = static_cast<u16>(skin.numVertices_);
        for(u16 i=0; i<numVertices; ++i){
        }

        LIME_DELETE_ARRAY(toIndices);
    }

#else
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
#endif


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

        s32 offsetToSkin = 0; //表情インデックスリストの先頭へのオフセット

        //表情インデックスリストスキップ
        s32 offset = skinCount * sizeof(u16);
        offsetToSkin -= offset;
        is.seekg(offset, lcore::ios::cur);

        //ボーンラベル名スキップ
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

        //バッファ作成
        u32 size = 2*sizeof(u16) + skinCount * sizeof(u16) + numBoneLabels * sizeof(BoneLabel);
        LIME_DELETE_ARRAY(buffer_);
        buffer_ = LIME_NEW u8[size];

        //各ラベル数保存
        *reinterpret_cast<u16*>(buffer_) = skinCount;
        *reinterpret_cast<u16*>(buffer_+sizeof(u16)) = static_cast<u16>(numBoneLabels);

        skinIndices_ = reinterpret_cast<u16*>(buffer_ + 2*sizeof(u16));
        boneLabels_  = reinterpret_cast<BoneLabel*>(buffer_ + 2*sizeof(u16) + skinCount*sizeof(u16));

        //表情インデックスロード
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
        :state_(State_None)
        ,numVertices_(0)
        ,vertices_(NULL)
        ,numFaceIndices_(0)
        ,faceIndices_(NULL)
        ,numMaterials_(0)
        ,countTextureLoadedMaterial_(0)
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

        state_ = State_LoadGeometry;
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
        state_ = State_None;
    }

    bool Pack::loadInternal(const char* /*directory*/)
    {
        input_ >> header_;

        loadElements(input_, &vertices_, numVertices_);
        if(numVertices_>MaxVertices){ //最大値を超えれば失敗
            return false;
        }

        loadElements(input_, &faceIndices_, numFaceIndices_);
        if(numVertices_>MaxIndices){ //最大値を超えれば失敗
            return false;
        }

        loadElements(input_, &materials_, numMaterials_);

        loadElements(input_, &bones_, numBones_);
        if(MaxBones<numBones_){
            lcore::Log("pmd fail to load too many bones");
            return false;
        }

        //IK作成
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


        //ボーンをソートしてマッピング
        boneMap_ = LIME_NEW u16[numBones_];
        sortBones();

        for(u16 i=0; i<numBones_; ++i){
            if(bones_[i].parentIndex_ != InvalidJointIndexU16){
                bones_[i].parentIndex_ = boneMap_[ bones_[i].parentIndex_ ];
            }

            if(bones_[i].tailPosIndex_ != InvalidJointIndexU16){
                bones_[i].tailPosIndex_ = boneMap_[ bones_[i].tailPosIndex_ ];
            }

            if(bones_[i].ikParentIndex_ != 0){
                bones_[i].ikParentIndex_ = boneMap_[ bones_[i].ikParentIndex_ ];
            }
        }

        for(u32 i=0; i<numVertices_; ++i){
            vertices_[i].element_[ Vertex::Index_Bone0 ] = static_cast<BYTE>( boneMap_[ vertices_[i].element_[ Vertex::Index_Bone0 ] ] );
            vertices_[i].element_[ Vertex::Index_Bone1 ] = static_cast<BYTE>( boneMap_[ vertices_[i].element_[ Vertex::Index_Bone1 ] ] );
        }


        if(numMaterials_>0){
            //結局ボーン分割するまで、頂点・インデックス・マテリアルの情報はメモリにないとだめ
            splitBone(numVertices_, vertices_, numFaceIndices_, faceIndices_, numMaterials_, materials_, skinPack_.getSkins(), numBones_, lgraphics::LIME_MAX_SKINNING_MATRICES, numGeometries_, &geometries_);
        }

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
                joint.setSubjectTo(bones_[i].ikParentIndex_);
                joint.setType(bones_[i].type_);
            }
            skeleton->setNameMemory(header_.name_, NameSize);
            skeleton->recalcHash(); //呼び忘れるなあ
            skeleton_ = skeleton;
        }


        //IKのボーンをマッピング
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


        //英語名ロード
        loadNamesForENG();

        return true;
    }


    //--------------------------------------------------------------
    // ロード更新
    void Pack::updateLoad(const char* directory)
    {
        switch(state_)
        {
        case State_LoadGeometry: //ジオメトリ、マテリアルデータロード
            if( loadInternal(directory) ){
                countTextureLoadedMaterial_ = 0;

                //テクスチャマップ初期化
                if(NULL == nameTexMap_){
                    if(texMapInternal_.capacity()<=0){
                        createInternalNameTextureMap();
                    }
                    nameTexMap_ = &texMapInternal_;
                }

                state_ = State_LoadToonTexture;
            }else{
                state_ = State_Error;
            }
            break;

        case State_LoadToonTexture: //トゥーン用テクスチャロード
            loadToonTextures(directory);

            // 剛体、拘束データここでロード
            loadRigidBodies();

            state_ = State_LoadTexture;
            break;

        case State_LoadTexture: //テクスチャロード
            {
                if(countTextureLoadedMaterial_<numMaterials_){
                    SamplerState sampler;

                    Material& material = materials_[countTextureLoadedMaterial_];

                    //環境マップもロードする
                    for(u32 i=0; i<Material::TexType_Num; ++i){
                        if(material.textureFileNames_[i] == NULL){
                            continue;
                        }

                        u32 len = lcore::strlen(material.textureFileNames_[i]);
                        loadTexture(material.textureFileNames_[i], len, directory, *nameTexMap_, sampler);
                    }

                    ++countTextureLoadedMaterial_;
                }

                if(countTextureLoadedMaterial_>=numMaterials_){
                    state_ = State_Finish;
                }
            }
            break;

        default:
            break;
        };
    }

    bool Pack::createObject(lscene::AnimObject& obj, const char* directory, bool swapOrigin)
    {
        LASSERT(directory != NULL);
        if(state_ != State_Finish){
            return false;
        }

        LASSERT(nameTexMap_ != NULL);


        // 頂点宣言作成
        VertexDeclarationRef decl;
        IndexBufferRef ib;
        if(numGeometries_>0){
            //マルチストリーム実験
#if defined(LIME_LIBCONVERT_PMD_USE_MULTISTREAM)
            {
                VertexDeclCreator creator(4);
                u16 voffset = 0;
                creator.add(0, voffset, DeclType_Float3, DeclMethod_Default, DeclUsage_Position, 0);
                voffset += creator.add(1, voffset, DeclType_Short4N, DeclMethod_Normalize, DeclUsage_Normal, 0);
                voffset += creator.add(1, voffset, DeclType_Short2N, DeclMethod_Normalize, DeclUsage_Texcoord, 0);
                voffset += creator.add(1, voffset, DeclType_UB4, DeclMethod_Default, DeclUsage_BlendIndicies, 0);

                creator.end(decl);
            }
#else
            {
                VertexDeclCreator creator(4);
                u16 voffset = 0;
                voffset += creator.add(0, voffset, DeclType_Float3, DeclMethod_Default, DeclUsage_Position, 0);
                voffset += creator.add(0, voffset, DeclType_Short4N, DeclMethod_Normalize, DeclUsage_Normal, 0);
                voffset += creator.add(0, voffset, DeclType_Short2N, DeclMethod_Normalize, DeclUsage_Texcoord, 0);
                voffset += creator.add(0, voffset, DeclType_UB4, DeclMethod_Default, DeclUsage_BlendIndicies, 0);

                creator.end(decl);
            }
#endif

            //インデックスバッファ作成
            ib = IndexBuffer::create(numFaceIndices_, Pool_Default, Usage_WriteOnly);
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
            // マテリアル設定
            lgraphics::RenderStateRef renderState[RenderState_Num];
            u32 stateCreateFlag = 0;
            RenderStateType renderStateType = RenderState_Default;

            TextureName texName;


            for(u32 i=0; i<numMaterials_; ++i){
                Material& material = materials_[i];

                lscene::Material& dstMaterial = tmp.getMaterial(i);
                setMaterial(dstMaterial, material);

                //テクスチャロードは完了しているのでマップを探す
                TextureRef* texture = NULL;

                if(material.textureFileNames_[Material::TexType_Albedo] != NULL){
                    u32 len = lcore::strlen( material.textureFileNames_[Material::TexType_Albedo] );
                    u32 mapPos = nameTexMap_->find(material.textureFileNames_[Material::TexType_Albedo], len);

                    texture = (nameTexMap_->isEnd(mapPos) == false)? nameTexMap_->getValue(mapPos) : NULL;
                }

                //アルファ値が１ならアルファブレンドなし
                bool alphaTest = false;
                if(dstMaterial.diffuse_.w_ > 0.999f){
                    renderStateType = RenderState_Default;

                    //アルファありのテクスチャなら、アルファテストあり
                    if(lconverter::Config::getInstance().isAlphaTest()
                        && NULL != texture)
                    {
                        s32 format = texture->getFormat();

                        if(lgraphics::Buffer_A8R8G8B8 == format
                            || lgraphics::Buffer_A8B8G8R8 == format
                            || lgraphics::Buffer_A4R4G4B4 == format
                            || lgraphics::Buffer_A4B4G4R4 == format)
                        {
                            alphaTest = true;
                            dstMaterial.getFlags().setFlag( lscene::Material::MatFlag_AlphaTest );
                        }
                    }
                }else{
                    renderStateType = RenderState_AlphaBlend;
                }

                //テクスチャセット
                {
                    u32 numTextures = 0;
                    if(texture != NULL){
                        numTextures = 1;
                    }
                    if(material.toonIndex_<NumToonTextures){
                        numTextures = 2;
                    }
                    if(material.textureFileNames_[Material::TexType_Sphere] != NULL){
                        numTextures = 3;
                    }
                    dstMaterial.setTextureNum(numTextures);
                }

                if(texture != NULL){
                    if(swapOrigin){
                        lframework::io::swapOrigin(*texture);
                    }
                    dstMaterial.setTexture(0, *texture);
                    //テクスチャがあったのでフラグ立てる
                    dstMaterial.getFlags().setFlag(lscene::Material::MatFlag_TexAlbedo);

                    //サンプラステートを合わせておく
                    lgraphics::SamplerState& sampler = dstMaterial.getSamplerState(0);
                    sampler.setAddressU( lgraphics::TexAddress_Clamp );
                    sampler.setAddressV( lgraphics::TexAddress_Clamp );
                    sampler.setMagFilter( lgraphics::TexFilter_Linear );

                    if(texture->getLevels() > 1){ //ミップマップありか
                        sampler.setMinFilter( lgraphics::TexFilter_LinearMipMapPoint );
                    }else{
                        sampler.setMinFilter( lgraphics::TexFilter_Linear );
                    }
                }

                if(material.toonIndex_<NumToonTextures){ //トゥーンテクスチャを調べる
                    texture = toonTextures_.textures_[ material.toonIndex_ ];
                    if(texture != NULL){
                        dstMaterial.setTexture(1, *texture);
                        //フラグ立てる
                        dstMaterial.getFlags().setFlag(lscene::Material::MatFlag_TexGrad); //テクスチャでグラディエーションつける

                        //サンプラステートを合わせておく
                        lgraphics::SamplerState& sampler = dstMaterial.getSamplerState(1);
                        sampler = toonTextures_.samplers_[ material.toonIndex_ ];

                    }
                }

                //環境マップを調べる
                if(material.textureFileNames_[Material::TexType_Sphere] != NULL){
                    u32 len = lcore::strlen( material.textureFileNames_[Material::TexType_Sphere] );
                    u32 mapPos = nameTexMap_->find(material.textureFileNames_[Material::TexType_Sphere], len);

                    texture = (nameTexMap_->isEnd(mapPos) == false)? nameTexMap_->getValue(mapPos) : NULL;
                    if(texture != NULL){
                        dstMaterial.setTexture(2, *texture);
                        //フラグ立てる
                        dstMaterial.getFlags().setFlag(lscene::Material::MatFlag_SphereMap); //テクスチャでグラディエーションつける
                        if(material.sphereAdd_ != 0){
                            dstMaterial.getFlags().setFlag(lscene::Material::MatFlag_SphereMapAdd);
                        }

                        //サンプラステートを合わせておく
                        lgraphics::SamplerState& sampler = dstMaterial.getSamplerState(2);
                        sampler.setAddressU( lgraphics::TexAddress_Clamp );
                        sampler.setAddressV( lgraphics::TexAddress_Clamp );
                        sampler.setMagFilter( lgraphics::TexFilter_Linear );

                        if(texture->getLevels() > 1){ //ミップマップありか
                            sampler.setMinFilter( lgraphics::TexFilter_LinearMipMapPoint );
                        }else{
                            sampler.setMinFilter( lgraphics::TexFilter_Linear );
                        }

                    }
                }


#if defined(LIME_GLES2)
                //頂点でライティング計算
                dstMaterial.getFlags().setFlag(lscene::Material::MatFlag_LightingVS);
#endif

                setRenderStateToMaterial(dstMaterial, renderState, stateCreateFlag, renderStateType, alphaTest);

            } //for(u32 i=0; i<numMaterials_
        } //if(numMaterials_>0)


        lanim::Skeleton::pointer skeleton( releaseSkeleton() );
        tmp.setSkeleton(skeleton);

        // GeometryBuffer作成
        GeometryBuffer::pointer geomBuffer;

        u32 geomIndex = 0;
        Geometry *geometry = geometries_;
        VertexBuffer* bufferPtr = NULL;
        numVertices_ = 0;
        while(geometry != NULL){
            if(bufferPtr != geometry->vertexBuffer_.get()){
                bufferPtr = geometry->vertexBuffer_.get();

//マルチストリーム実験
#if defined(LIME_LIBCONVERT_PMD_USE_MULTISTREAM)
                lgraphics::VertexBufferRef vb0, vb1, vb0Back;
                if(false == createVertexBuffer(vb0, vb1, vb0Back, *bufferPtr)){
                    return false;
                }

                skinPack_.createMorphBaseVertices(bufferPtr->elements_.size(), &(bufferPtr->elements_[0]), vb0Back);

                geomBuffer = LIME_NEW GeometryBuffer(Primitive_TriangleList, decl, vb0, ib);

                geomBuffer->addVertexBufferStream(vb1);

#else
                lgraphics::VertexBufferRef vb;
                if(false == createVertexBuffer(vb, *bufferPtr)){
                    return false;
                }

                skinPack_.createMorphBaseVertices(bufferPtr->elements_.size(), &(bufferPtr->elements_[0]));

                geomBuffer = LIME_NEW GeometryBuffer(Primitive_TriangleList, decl, vb, ib);

#endif

#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
                numVertices_ += bufferPtr->elements_.size();
#endif
            }

            u16 count = static_cast<u16>( geometry->faceIndices_.size()/3 );
            lscene::Geometry geom(geomBuffer, count, geometry->indexOffset_, static_cast<u8>(geometry->materialIndex_));
            //geom.getFlags().setFlag(lscene::Geometry::GeomFlag_DiffuseVS);

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
        } //while(geometry != NULL)

        obj.swap(tmp);

        lanim::IKPack::pointer ikPack( releaseIKPack() );
        obj.setIKPack( ikPack );

//デバッグ用ログ
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
        debugLog_.addNumVertices( numVertices_ );
        debugLog_.addNumBatches( numGeometries_ );
#endif
        return true;
    }


    //-------------------------------------
    // 英語名ロード
    void Pack::loadNamesForENG()
    {
        //必要ないので読み飛ばす
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
    // トゥーンシェーディング用テクスチャロード
    void Pack::loadToonTextures(const char* directory)
    {
        static const Char ToonTexDirectory[] = "../../Data/"; //ディレクトリ固定
        static const u32 ToonTexDirectorySize = sizeof(ToonTexDirectory); //ヌル文字含む
        static const Char tex0[] = "toon0.bmp"; //0番目は固定なんだとさ

        LASSERT(nameTexMap_ != NULL);

        u32 dlen = lcore::strlen(directory);
        u32 pathSize = dlen + ToonTexDirectorySize;
        if(pathSize>FilePathSize){
            return;
        }

        Char buffer[FilePathSize];
        lcore::memcpy(buffer, directory, dlen);
        lcore::memcpy(buffer + dlen, ToonTexDirectory, ToonTexDirectorySize);

        //0番目ロード
        toonTextures_.textures_[0] = loadTexture(tex0, sizeof(tex0)-1, buffer, *nameTexMap_, toonTextures_.samplers_[0], true);


        u32 utf8size = 0;
        Char name[ToonTexturePathSize+1];
        Char tmp[ToonTexturePathSize+1];

        for(u32 i=1; i<NumToonTextures; ++i){
            lcore::io::read(input_, name, ToonTexturePathSize);
            name[ToonTexturePathSize] = '\0';

            utf8size = charcode::strSJISToUTF8(NULL, reinterpret_cast<const u8*>(name));
            if(utf8size>ToonTexturePathSize){
                //変換後がToonTexturePathSizeを超える場合サポートしない
            }else{
                lcore::memcpy(tmp, name, ToonTexturePathSize+1);

                charcode::strSJISToUTF8(reinterpret_cast<u8*>(name), reinterpret_cast<const u8*>(tmp));
                name[utf8size] = '\0';        
            }

            toonTextures_.textures_[i] = loadTexture(name, lcore::strlen(name), buffer, *nameTexMap_, toonTextures_.samplers_[i], true);
            if(toonTextures_.textures_[i] == NULL){ //なければモデルのディレクトリも調べる
                toonTextures_.textures_[i] = loadTexture(name, lcore::strlen(name), directory, *nameTexMap_, toonTextures_.samplers_[i], true);
            }
        }

    }


    //-------------------------------------
    // ボーンを親が前になるようにソート
    void Pack::sortBones()
    {
        LASSERT(bones_ != NULL);
        LASSERT(boneMap_ != NULL);

        for(u16 i=0; i<numBones_; ++i){
            boneMap_[i] = InvalidJointIndexU16;
        }

        Bone *stack = LIME_NEW Bone[numBones_];
        u16 stackIndex = 0;

        for(u16 i=0; i<numBones_; ++i){

            //自分の親をすべてスタックに積む
            u16 j = i;
            while(bones_[j].parentIndex_ != InvalidJointIndexU16){
                u16 parent = bones_[j].parentIndex_;
                if(boneMap_[parent] == InvalidJointIndexU16){
                    boneMap_[parent] = stackIndex;
                    stack[stackIndex] = bones_[parent];
                    ++stackIndex;
                }
                j = parent;
            }

            //自分をスタックに積む
            if(boneMap_[i] == InvalidJointIndexU16){
                boneMap_[i] = stackIndex;
                stack[stackIndex] = bones_[i];
                ++stackIndex;
            }
        }

        for(u16 i=0; i<numBones_; ++i){
            bones_[i] = stack[i];
        }

        LIME_DELETE_ARRAY(stack);

#if defined(LIME_LIB_CONV_DEBUG)
        // debug out
        //-----------------------------------------------------------------------
        lcore::ofstream out("bonesort.txt", lcore::ios::binary);

        for(u16 i=0; i<numBones_; ++i){
            out.print("[%d] -> [%d]\n", i, boneMap_[i]);
        }
#endif

    }

    //-------------------------------------
    void Pack::createInternalNameTextureMap()
    {
        NameTextureMap tmp(numMaterials_*2);
        texMapInternal_.swap(tmp);
    }


    //-------------------------------------
    // 剛体、拘束データロード
    void Pack::loadRigidBodies()
    {
        u32 numRigidBodies = 0;
        RigidBody* rigidBodies = NULL;

        u32 numConstraints = 0;
        Constraint* constraints = NULL;

        const u16* boneMap = dispLabel_.getBoneMap();

        lcore::io::read(input_, numRigidBodies);
        if(numRigidBodies==0){
            return;
        }

        rigidBodies = LIME_NEW RigidBody[ numRigidBodies ];
        for(u32 i=0; i<numRigidBodies; ++i){
            input_ >> rigidBodies[i];

            //ボーンインデックスを新しいものにマッピング
            if(rigidBodies[i].boneIndex_ >= numBones_){
                rigidBodies[i].boneIndex_ = 0xFFFFU;
            }else{
                rigidBodies[i].boneIndex_ = boneMap[ rigidBodies[i].boneIndex_ ];
            }
        }

        lcore::io::read(input_, numConstraints);
        constraints = LIME_NEW Constraint[ numConstraints ];
        for(u32 i=0; i<numConstraints; ++i){
            input_ >> constraints[i];
        }

        RigidBodyPack tmp(numRigidBodies, rigidBodies, numConstraints, constraints);
        rigidBodyPack_.swap(tmp);
    }
}

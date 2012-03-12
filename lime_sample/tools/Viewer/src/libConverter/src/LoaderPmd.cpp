/**
@file LoaderPmd.cpp
@author t-sakai
@date 2010/05/17 create
*/
#include <fstream>
#include <string>

#include "LoaderPmd.h"
#include "Pmd.h"

#include <lcore/HashMap.h>
#include <lcore/String.h>

#include <lgraphics/api/VertexDeclarationRef.h>
#include <lgraphics/api/VertexBufferRef.h>
#include <lgraphics/api/GeometryBuffer.h>
#include <lgraphics/api/IndexBufferRef.h>
#include <lgraphics/api/RenderStateRef.h>
#include <lgraphics/api/TextureRef.h>

#include <lgraphics/scene/AnimTree.h>
#include <lgraphics/scene/Geometry.h>
#include <lgraphics/scene/Material.h>
#include <lgraphics/scene/Bone.h>
#include <lgraphics/scene/AnimObject.h>

#include <lgraphics/io/IOUtil.h>
#include <lgraphics/io/IOBMP.h>
#include <lgraphics/io/IODDS.h>
#include <lgraphics/io/IOTGA.h>

#include "PmdUtil.h"

namespace pmd
{
    using namespace lgraphics;

    LoaderPmd::IKPack::IKPack()
        :size_(0)
        ,iks_(NULL)
    {
    }

    LoaderPmd::IKPack::~IKPack()
    {
        release();
    }

    void LoaderPmd::IKPack::release()
    {
        size_ =  0;
        LIME_DELETE_ARRAY(iks_);
    }

namespace
{
    static const u32 NULL_BONE_INDEX = 0xFFFFU;
    static const u32 TexNameSize = lgraphics::Material::MAX_NAME_BUFFER_SIZE;
    typedef lcore::String<TexNameSize> TextureName;

    typedef lcore::HashMap<TextureName, TextureRef> NameTextureMap;

    void extractDirectoryPath(std::string& dst, const char* path, u32 size)
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
        dst.reserve(dstSize);
        dst.assign(path, dstSize);
    }

    bool loadTexture(TextureRef& texture, std::string& name, const char* path, u32 size, const std::string& directory, NameTextureMap& texMap)
    {
        name.clear();
        name.reserve(size);
        for(u32 i=0; i<size; ++i){
            if(path[i] == '*'){
                break;
            }
            name.push_back(path[i]);
        }
        std::string filepath = directory;
        filepath += name;

        lcore::ifstream is(filepath.c_str(), lcore::ios::in|lcore::ios::binary);
        if(is.is_open() == false){
            return false;
        }

        std::string::size_type pos = name.rfind('.');
        Format format = Format_DDS;
        if(pos == std::string::npos){
            name += ".dds";
        }else{
            format = getFormatFromExt(name, pos+1);
            name.replace(pos, 4, ".dds");
        }

        TextureName texname(name.c_str());

        u32 mapPos = texMap.find(texname);
        if(texMap.isEnd(mapPos) == false){
            //すでに同じ名前のテクスチャが存在した
            texture = texMap.getValue(mapPos);
            return true;
        }

        bool ret = false;
        switch(format)
        {
        case Format_BMP:
            ret = io::IOBMP::read(is, texture);
            break;

        case Format_TGA:
            ret = io::IOTGA::read(is, texture);
            break;

        case Format_DDS:
            ret = io::IODDS::read(is, texture);
            break;
        };

        texMap.insert(texname, texture);

        return ret;
    }

    //-----------------------------------------------------------------------------
    /// PMDのマテリアルをlgraphics::Materialに代入
    void setMaterial(lgraphics::Material& dst, const pmd::Material& src, const std::string& directory)
    {
        dst.shading_.assign("toon");
        dst.diffuse_.set(src.diffuse_[0], src.diffuse_[1], src.diffuse_[2], src.diffuse_[3]);
        dst.specular_.set(src.specularColor_[0], src.specularColor_[0], src.specularColor_[0], src.specularity_);
        dst.ambient_.set(src.ambient_[0], src.ambient_[0], src.ambient_[0]);

        //Create RenderStateRef
        //ステート設定はてきとう
        RenderStateRef::begin();
        RenderStateRef::setCullMode(lgraphics::CullMode_CCW);

        // ライティング
        RenderStateRef::setLighting(true);

        // アルファ設定
        RenderStateRef::setAlphaBlendEnable(false);
        RenderStateRef::setAlphaBlendSrc(lgraphics::Blend_InvDestAlpha);
        RenderStateRef::setAlphaBlendDst(lgraphics::Blend_DestAlpha);
        RenderStateRef::setAlphaTest(false);
        RenderStateRef::setAlphaTestFunc(lgraphics::Cmp_Less);

        // Zバッファ設定
        RenderStateRef::setZEnable(true);
        RenderStateRef::setZWriteEnable(true);

        //RenderStateRef::setVertexBlend(lgraphics::VertexBlend_1WEIGHTS);

        RenderStateRef state;
        RenderStateRef::end(state);

        dst.setRenderState(state);
    }


    //----------------------------------------------------------
    struct LVertexForPMD
    {
        lmath::Vector3 position_;
        lmath::Vector3 normal_;
        lmath::Vector2 uv_;
        u8 boneIndex_[4];
        f32 weight_;

        LVertexForPMD& operator=(const pmd::Vertex& rhs)
        {
            position_.set(rhs.position_[0], rhs.position_[1], rhs.position_[2]);
            normal_.set(rhs.normal_[0], rhs.normal_[1], rhs.normal_[2]);
            uv_.set(rhs.uv_[0], rhs.uv_[1]);
            boneIndex_[0] = rhs.boneNo_[0];
            boneIndex_[1] = rhs.boneNo_[1];
            boneIndex_[2] = rhs.boneNo_[0];
            boneIndex_[3] = rhs.boneNo_[1];

            weight_ = 0.01f * rhs.boneWeight_;
            return *this;
        }
    };

    //-----------------------------------------------------------------------------
    /// ジオメトリ作成
    GeometryBuffer::pointer createGeometryBuffer(pmd::Vertex* pmdVertices, u32 vertexCount, u16* pmdIndices, u32 faceVertexCount)
    {
        // 頂点宣言作成
        VertexDeclCreator creator(5);
        u16 offset = 0;
        offset += creator.add(0, offset, DeclType_Float3, DeclUsage_Position, 0);
        offset += creator.add(0, offset, DeclType_Float3, DeclUsage_Normal, 0);
        offset += creator.add(0, offset, DeclType_Float2, DeclUsage_Texcoord, 0);
        offset += creator.add(0, offset, DeclType_UB4, DeclUsage_BlendIndicies, 0);
        offset += creator.add(0, offset, DeclType_Float1, DeclUsage_BlendWeight, 0);

        VertexDeclarationRef decl;
        creator.end(decl);

        //頂点バッファ作成
        VertexBufferRef vb = VertexBuffer::create(sizeof(LVertexForPMD), vertexCount, Pool_Default, Usage_Dynamic);

        //インデックスバッファ作成
        IndexBufferRef ib = IndexBuffer::create(faceVertexCount, Pool_Default, Usage_None);

        return GeometryBuffer::pointer( LIME_NEW GeometryBuffer(Primitive_TriangleList, decl, vb, ib) );
    }

    //-----------------------------------------------------------------------------
    /// ジオメトリ作成
    void copyGeometryBuffer(GeometryBuffer::pointer dst, pmd::Vertex* pmdVertices, u32 vertexCount, u16* pmdIndices, u32 faceVertexCount)
    {
        //頂点バッファ作成
        VertexBufferRef &vb = dst->getVertexBuffer();

        LVertexForPMD *vertices = NULL;
        u32 bufferSize = sizeof(LVertexForPMD) * vertexCount;
        bool lockSuccess = vb.lock(0, bufferSize, (void**)&vertices);
        LASSERT(lockSuccess);

        for(u32 i=0; i<vertexCount; ++i){
            vertices[i] = pmdVertices[i];
        }

        vb.unlock();

        //インデックスバッファ作成

        IndexBufferRef &ib = dst->getIndexBuffer();

        u16 *indicies = NULL;
        u32 iBufferSize = sizeof(u16) * faceVertexCount;
        lockSuccess = ib.lock(0, iBufferSize, (void**)&indicies);
        LASSERT(lockSuccess);

        for(u32 i=0; i<faceVertexCount; ++i){
            indicies[i] = pmdIndices[i];
        }

        ib.unlock();
    }

    //-----------------------------------------------------------------------------
    u32 findBoneByParentIndex(pmd::Bone* bones, u32 num, u16 parentIndex)
    {
        for(u32 i=0; i<num; ++i){
            if(bones[i].parentIndex_ == parentIndex){
                return i;
            }
        }
        return parentIndex;
    }

    //------------------------------------------------------------------------------
    u32 countBoneByParentIndex(pmd::Bone* bones, u32 num, u16 parentIndex)
    {
        u32 count = 0;
        for(u32 i=0; i<num; ++i){
            count += (bones[i].parentIndex_ == parentIndex)? 1 : 0;
        }
        return count;
    }

    //------------------------------------------------------------------------------
    u32 countTopBones(pmd::Bone* bones, u32 num)
    {
        return countBoneByParentIndex(bones, num, NULL_BONE_INDEX);
    }

    //-----------------------------------------------------------------------------
    void setBonesToTree(AnimObject& object, pmd::Bone* bones, AnimTree& tree, u32 numBones, u32 topIndex)
    {
        u32 numChildren = countBoneByParentIndex(bones, numBones, topIndex);
        tree.setNumChildren(numChildren);

        s32 index = -1;
        for(u32 i=0; i<numChildren; ++i){
            AnimTree &childNode = tree.getChild(i);
            childNode.setParent(&tree);

            u32 childIndex = findBoneByParentIndex(bones+index+1, numBones-index-1, topIndex);

            childIndex += index + 1;
            index = childIndex;

            LASSERT(childIndex != topIndex);

            pmd::Bone &child = bones[childIndex];

            //childNode.getElement().setBoneIndex( childIndex );
            //childNode.getElement().setIKBoneIndex( child.tailPosIndex_ );
            childNode.getElement().setBone( &(object.getBone(childIndex)) );
            childNode.getElement().setHeadPos( lmath::Vector3(child.headPos_[0], child.headPos_[1], child.headPos_[2]) );
            setBonesToTree(object, bones, childNode, numBones, childIndex);
        }
    }

    //-----------------------------------------------------------------------------
    void setBonesToObject(AnimObject& object, pmd::Bone* bones, u32 numBones)
    {
        object.setNumBones(numBones);
        for(u32 i=0; i<numBones; ++i){
            lgraphics::Bone &bone = object.getBone(i);
            bone.identity();
            object.setBoneName(i, bones[i].name_);
        }

        u32 topNum = object.getNumTreeTops();

        s32 index = -1;
        for(u32 i=0; i<topNum; ++i){
            u32 found = findBoneByParentIndex(bones+index+1, numBones-index-1, -1);
            if(found == -1){
                LASSERT(false);
                break;
            }
            found += index + 1;
            index = found;

            object.getTreeTop(i).getElement().setBone( &(object.getBone(found)) );
            object.getTreeTop(i).getElement().setHeadPos( lmath::Vector3(bones[found].headPos_[0], bones[found].headPos_[1], bones[found].headPos_[2]) );

            setBonesToTree(object, bones, object.getTreeTop(i), numBones, found);
        }
    }

}


    lgraphics::AnimObject* LoaderPmd::load(const char* filepath, IKPack* ikPack, bool swapOrigin)
    {
        LASSERT(filepath != NULL);

        lcore::ifstream is(filepath, lcore::ios::in|lcore::ios::binary);
        if(is.is_open() == false){
            return NULL;
        }

        // ヘッダロード
        Header header;
        is >> header;

        // ジオメトリロード
        u32 vertexCount;
        io::read(is, vertexCount);
        pmd::Vertex *pmdVertices = LIME_NEW pmd::Vertex[vertexCount];
        for(u32 i=0; i<vertexCount; ++i){
            is >> pmdVertices[i];
        }

        u32 faceVertexCount =0;
        io::read(is, faceVertexCount);
        u16 *pmdIndicies = LIME_NEW u16[faceVertexCount];
        io::read(is, pmdIndicies, sizeof(u16)*faceVertexCount);

        // マテリアルロード
        u32 materialCount;
        u32 count = 0;
        io::read(is, materialCount);
        pmd::Material *pmdMaterials = LIME_NEW pmd::Material[materialCount];
        pmd::Material tmp;

        for(u32 i=0; i<materialCount; ++i){
            is >> tmp;
            if(tmp.faceVertexCount_ > 0){
                pmdMaterials[count] = tmp;
                ++count;
            }
        }
        materialCount = count;


        // ボーンロード
        u16 boneCount = 0;
        io::read(is, boneCount);
        pmd::Bone *pmdBones = LIME_NEW pmd::Bone[boneCount];
        for(u32 i=0; i<boneCount; ++i){
            is >> pmdBones[i];
        }

        // 境界球計算
        lmath::Vector4 bsphere;
        //calcBoundingSphere(bsphere, pmdVertices, vertexCount);

        // ツリー先頭のボーンをカウント
        u32 treeTopCount = countTopBones(pmdBones, boneCount);

        PmdVertexVector vertices;
        GeomPtrVector geometries;
        pmd::seperateGeometry(
            vertexCount,
            pmdVertices,
            vertices,
            faceVertexCount,
            pmdIndicies,
            materialCount,
            pmdMaterials,
            geometries,
            lgraphics::LIME_MAX_SKINNING_MATRICES);

        // GeometryBuffer作成
        GeometryBuffer::pointer geomBufferPtr = createGeometryBuffer(pmdVertices, vertexCount, pmdIndicies, faceVertexCount);
        // GeometryBufferデータコピー
        copyGeometryBuffer(geomBufferPtr, pmdVertices, vertexCount, pmdIndicies, faceVertexCount);

        AnimObject *animObj = LIME_NEW AnimObject(geometries.size(), materialCount, treeTopCount, geomBufferPtr);

        std::string directory;
        extractDirectoryPath(directory, filepath, std::strlen(filepath));

        TextureName nullName;
        NameTextureMap texMap(materialCount*2, nullName);

        std::string name;
        for(u32 i=0; i<geometries.size(); ++i){

            Geometry &geom = *(geometries[i]);
            pmd::Material &src = pmdMaterials[ geom.getMaterialIndex() ];
            lgraphics::Material& dst = animObj->getMaterial( geom.getMaterialIndex() );
            setMaterial(dst, src, directory);

            TextureRef texture;
            bool ret = loadTexture(texture, name, src.textureFileName_, pmd::Material::FileNameSize, directory, texMap);

            if(ret){
                if(swapOrigin){
                    io::swapOrigin(texture);
                }
                dst.setTextureNum(1);
                dst.setTexture(0, texture);
                dst.setTexName(0, name.c_str());
            }

            animObj->getGeometry(i).swap(geom);
            LIME_DELETE(geometries[i]);
        }


        setBonesToObject(*animObj, pmdBones, boneCount);

        LIME_DELETE_ARRAY(pmdVertices);
        LIME_DELETE_ARRAY(pmdIndicies);
        LIME_DELETE_ARRAY(pmdMaterials);
        LIME_DELETE_ARRAY(pmdBones);


        if(ikPack != NULL){
            // IKデータロード
            //---------------------------------------------------
            pmd::PMDIK::WORD numIK = 0;
            io::read(is, numIK);
            ikPack->size_ = numIK;
            ikPack->iks_ = LIME_NEW pmd::PMDIK[ikPack->size_];
            for(u32 i=0; i<ikPack->size_; ++i){
                is >> ikPack->iks_[i];
            }
        }

        return animObj;
    }
}

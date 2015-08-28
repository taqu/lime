/**
@file ObjectContent.cpp
@author t-sakai
@date 2014/12/10 create
*/
#include "ObjectContent.h"

#include <lmath/lmath.h>
#include <lmath/DualQuaternion.h>

#include "Geometry.h"
#include "Mesh.h"
#include "Material.h"
#include "Node.h"
#include "Texture.h"

namespace lscene
{
namespace lrender
{
    //-----------------------------------------------------
    //---
    //--- ObjectContent
    //---
    //-----------------------------------------------------
    ObjectContent::ObjectContent()
        :geometries_(NULL)
        ,meshes_(NULL)
        ,materials_(NULL)
        ,nodes_(NULL)
        ,textures_(NULL)
        ,numGeometries_(0)
        ,numMeshes_(0)
        ,numMaterials_(0)
        ,numNodes_(0)
        ,numTextures_(0)
    {
    }

    //-----------------------------------------------------
    ObjectContent::~ObjectContent()
    {
        destroy();
    }

    //-----------------------------------------------------
    // バッファ作成
    bool ObjectContent::create(u32 numGeometries, u32 numMeshes, u32 numMaterials, u32 numNodes, u32 numTextures)
    {
        destroy();

        numGeometries_ = numGeometries;
        numMeshes_ = numMeshes;
        numMaterials_ = numMaterials;
        numNodes_ = numNodes;
        numTextures_ = numTextures;

        u32 size = calcBufferSize();

        u8* buffer = static_cast<u8*>( LSCENE_MALLOC(size) );
        if(NULL == buffer){
            return false;
        }

        u32 offset = 0;
        geometries_ = placementArrayConstruct<Geometry>(buffer+offset, numGeometries_);
        offset += numGeometries_ * sizeof(Geometry);

        meshes_ = placementArrayConstruct<Mesh>(buffer+offset, numMeshes_);
        offset += numMeshes_ * sizeof(Mesh);

        materials_ = placementArrayConstruct<Material>(buffer+offset, numMaterials_);
        offset += numMaterials_ * sizeof(Material);

        nodes_ = placementArrayConstruct<Node>(buffer+offset, numNodes_);
        offset += numNodes_ * sizeof(Node);

        textures_ = placementArrayConstruct<Texture2D>(buffer+offset, numTextures_);
        offset += numTextures_ * sizeof(Texture2D);

        return true;
    }

    //-----------------------------------------------------
    void ObjectContent::destroy()
    {
        for(u32 i=0; i<numGeometries_; ++i){
            geometries_[i].~Geometry();
        }

        for(u32 i=0; i<numMeshes_; ++i){
            meshes_[i].~Mesh();
        }

        for(u32 i=0; i<numMaterials_; ++i){
            materials_[i].~Material();
        }

        for(u32 i=0; i<numNodes_; ++i){
            nodes_[i].~Node();
        }

        for(u32 i=0; i<numTextures_; ++i){
            textures_[i].~Texture2D();
        }

        LSCENE_FREE(geometries_);

        numGeometries_ = 0;
        geometries_ = NULL;

        numMeshes_ = 0;
        meshes_ = NULL;

        numMaterials_ = 0;
        materials_ = NULL;

        numNodes_ = 0;
        nodes_ = NULL;

        numTextures_ = 0;
        textures_ = NULL;
    }

    //-----------------------------------------------------
    u32 ObjectContent::calcBufferSize() const
    {
        u32 size = numGeometries_ * sizeof(Geometry)
            + numMeshes_ * sizeof(Mesh)
            + numMaterials_ * sizeof(Material)
            + numNodes_ * sizeof(Node)
            + numTextures_ * sizeof(lgraphics::Texture2DRef);
        return size;
    }

    //-----------------------------------------------------
    void ObjectContent::clone(ObjectContent& dst)
    {
        ObjectContent tmp;
        tmp.create(
            numGeometries_,
            numMeshes_,
            numMaterials_,
            numNodes_,
            numTextures_);
        tmp.swap(dst);

        copy(dst);
    }

    //-----------------------------------------------------
    void ObjectContent::copy(ObjectContent& dst)
    {
        //Geometryクローン
        for(u32 i=0; i<numGeometries_; ++i){
            geometries_[i].clone(dst.geometries_[i]);
        }

        //Meshクローン
        for(u32 i=0; i<numMeshes_; ++i){
            u32 geomIndex = (meshes_[i].getGeometry() - geometries_)/sizeof(Geometry);
            u32 materialIndex = (meshes_[i].getMaterial() - materials_)/sizeof(Material);

            dst.meshes_[i].create(
                meshes_[i].getType(),
                meshes_[i].getIndexOffset(),
                meshes_[i].getNumIndices(),
                &dst.geometries_[geomIndex],
                &dst.materials_[materialIndex],
                meshes_[i].getSphere());
        }

        //Textureクローン
        for(u32 i=0; i<numTextures_; ++i){
            dst.textures_[i] = textures_[i];
        }

        //Materialクローン
        for(u32 i=0; i<numMaterials_; ++i){
            dst.materials_[i].flags_ = materials_[i].flags_;
            dst.materials_[i].diffuse_ = materials_[i].diffuse_;
            dst.materials_[i].specular_ = materials_[i].specular_;
            dst.materials_[i].ambient_ = materials_[i].ambient_;
            dst.materials_[i].shadow_ = materials_[i].shadow_;

            for(u32 j=0; j<lrender::Tex_Num; ++j){
                dst.materials_[i].textureIDs_[j] = materials_[i].textureIDs_[j];

                //if(NULL != materials_[i].textures_[j]){
                //    u32 texIndex = (materials_[i].textures_[j] - textures_)/sizeof(lgraphics::Texture2DRef);
                //    dst.materials_[i].textures_[j] = &dst.textures_[texIndex];
                //}
            }
        }

        //Nodeクローン
        for(u32 i=0; i<numNodes_; ++i){
            dst.nodes_[i] = nodes_[i];
            dst.nodes_[i].meshes_ = dst.meshes_ + dst.nodes_[i].meshStartIndex_;
        }
    }

    //-----------------------------------------------------
    void ObjectContent::swap(ObjectContent& rhs)
    {
        lcore::swap(geometries_, rhs.geometries_);
        lcore::swap(meshes_, rhs.meshes_);
        lcore::swap(materials_, rhs.materials_);
        lcore::swap(nodes_, rhs.nodes_);
        lcore::swap(textures_, rhs.textures_);

        lcore::swap(numGeometries_, rhs.numGeometries_);
        lcore::swap(numMeshes_, rhs.numMeshes_);
        lcore::swap(numMaterials_, rhs.numMaterials_);
        lcore::swap(numNodes_, rhs.numNodes_);
        lcore::swap(numTextures_, rhs.numTextures_);
    }

    //--------------------------------------------------
    /**
    @brief ジオメトリ
    */
    Geometry& ObjectContent::getGeometry(u32 index)
    {
        LASSERT(index<numGeometries_);
        return geometries_[index];
    }

    //--------------------------------------------------
    // メッシュ
    Mesh& ObjectContent::getMesh(u32 index)
    {
        LASSERT(index<numMeshes_);
        return meshes_[index];
    }

    //--------------------------------------------------
    // マテリアル
    Material& ObjectContent::getMaterial(u32 index)
    {
        LASSERT(index<numMaterials_);
        return materials_[index];
    }

    //--------------------------------------------------
    // ノード
    Node& ObjectContent::getNode(u32 index)
    {
        LASSERT(index<numNodes_);
        return nodes_[index];
    }

    //--------------------------------------------------
    // テクスチャ
    Texture2D& ObjectContent::getTexture(u32 index)
    {
        LASSERT(index<numTextures_);
        return textures_[index];
    }

    //--------------------------------------------------
    // テクスチャ
    void ObjectContent::setTexture(u32 index, Texture2D& texture)
    {
        LASSERT(index<numTextures_);
        textures_[index] = texture;
    }


    //-----------------------------------------------------
    //---
    //--- AnimObjectContent
    //---
    //-----------------------------------------------------
    AnimObjectContent::AnimObjectContent()
        //:numMatrices_(0)
        //,skinningMatricesAligned16_(NULL)
    {
    }

    //-----------------------------------------------------
    AnimObjectContent::~AnimObjectContent()
    {
        destroy();
    }

    //-----------------------------------------------------
    // バッファ作成
    bool AnimObjectContent::create(
        u32 numGeometries,
        u32 numMeshes,
        u32 numMaterials,
        u32 numNodes,
        u32 numTextures)
    {
        destroy();

        numGeometries_ = numGeometries;
        numMeshes_ = numMeshes;
        numMaterials_ = numMaterials;
        numNodes_ = numNodes;
        numTextures_ = numTextures;
        //numMatrices_ = numMatrices;

        u32 size = calcBufferSize();

        u8* buffer = static_cast<u8*>( LSCENE_MALLOC(size) );
        if(NULL == buffer){
            return false;
        }

        u32 offset = 0;
        geometries_ = placementArrayConstruct<Geometry>(buffer+offset, numGeometries_);
        offset += numGeometries_ * sizeof(Geometry);

        meshes_ = placementArrayConstruct<Mesh>(buffer+offset, numMeshes_);
        offset += numMeshes_ * sizeof(Mesh);

        materials_ = placementArrayConstruct<Material>(buffer+offset, numMaterials_);
        offset += numMaterials_ * sizeof(Material);

        nodes_ = placementArrayConstruct<Node>(buffer+offset, numNodes_);
        offset += numNodes_ * sizeof(Node);

        textures_ = placementArrayConstruct<Texture2D>(buffer+offset, numTextures_);
        offset += numTextures_ * sizeof(Texture2D);

        //16バイトアライン
        //u8* ptr = lcore::align16(buffer+offset); 
        //skinningMatricesAligned16_ = placementArrayConstruct<lmath::Matrix34>(ptr, numMatrices_);
        //offset += numMatrices_ * sizeof(lmath::Matrix34) + 16;

        //dualQuaternions_ = placementArrayConstruct<lmath::DualQuaternion>(buffer+offset, numMatrices_);
        //offset += numMatrices_ * sizeof(lmath::DualQuaternion);

        //マトリックスクリア
        //for(u32 i=0; i<numMatrices_; ++i){
            //skinningMatricesAligned16_[i].identity();
            //dualQuaternions_[i].identity();
        //}

        return true;
    }

    //-----------------------------------------------------
    void AnimObjectContent::destroy()
    {
        //for(u32 i=0; i<numMatrices_; ++i){
        //    dualQuaternions_[i].~DualQuaternion();
        //}

        //for(u32 i=0; i<numMatrices_; ++i){
        //    skinningMatricesAligned16_[i].~Matrix34();
        //}

        ObjectContent::destroy();

        //numMatrices_ = 0;
        //skinningMatricesAligned16_ = NULL;
        //dualQuaternions_ = NULL;
    }

    //-----------------------------------------------------
    u32 AnimObjectContent::calcBufferSize() const
    {
        u32 size = ObjectContent::calcBufferSize();
            //+ numMatrices_ * sizeof(lmath::Matrix34) + 16; //16バイトアライン
            //+ numMatrices_ * sizeof(lmath::DualQuaternion);
        return size;
    }

    //-----------------------------------------------------
    void AnimObjectContent::clone(AnimObjectContent& dst)
    {
        AnimObjectContent tmp;
        tmp.create(
            numGeometries_,
            numMeshes_,
            numMaterials_,
            numNodes_,
            numTextures_);
        tmp.swap(dst);
        
        ObjectContent::copy(dst);

        //SkinningMaticesクローン
        //dst.numMatrices_ = numMatrices_;
        //lscene::copyAlign16Size16Times(dst.skinningMatricesAligned16_, skinningMatricesAligned16_, sizeof(lmath::Matrix34)*numMatrices_);
    }

    //-----------------------------------------------------
    void AnimObjectContent::swap(AnimObjectContent& rhs)
    {
        ObjectContent::swap(rhs);

        //lcore::swap(numMatrices_, rhs.numMatrices_);
        //lcore::swap(skinningMatricesAligned16_, rhs.skinningMatricesAligned16_);
        //lcore::swap(dualQuaternions_, rhs.dualQuaternions_);
    }
}
}

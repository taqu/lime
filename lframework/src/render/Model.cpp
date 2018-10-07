/**
@file Model.cpp
@author t-sakai
@date 2016/11/23 create
*/
#include "render/Model.h"

namespace lfw
{
    Model::Model()
        :referenceCount_(0)
        ,geometries_(NULL)
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
        sphere_.zero();
    }

    Model::~Model()
    {
        destroy();
        referenceCount_ = 0;
    }

    //-----------------------------------------------------
    // バッファ作成
    bool Model::create(u32 numGeometries, u32 numMeshes, u32 numMaterials, u32 numNodes, u32 numTextures)
    {
        destroy();

        numGeometries_ = numGeometries;
        numMeshes_ = numMeshes;
        numMaterials_ = numMaterials;
        numNodes_ = numNodes;
        numTextures_ = numTextures;

        s32 size = calcBufferSize();
        u8* buffer = static_cast<u8*>(LMALLOC(size));
        if(NULL == buffer){
            return false;
        }

        s32 offset = 0;
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
    void Model::destroy()
    {
        for(s32 i=0; i<numGeometries_; ++i){
            geometries_[i].~Geometry();
        }

        for(s32 i=0; i<numMeshes_; ++i){
            meshes_[i].~Mesh();
        }

        for(s32 i=0; i<numMaterials_; ++i){
            materials_[i].~Material();
        }

        for(s32 i=0; i<numNodes_; ++i){
            nodes_[i].~Node();
        }

        for(s32 i=0; i<numTextures_; ++i){
            textures_[i].~Texture2D();
        }

        LFREE(geometries_);

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
    bool Model::copyTo(Model& dst)
    {
        if(!dst.create(
            numGeometries_,
            numMeshes_,
            numMaterials_,
            numNodes_,
            numTextures_)){
            return false;
        }
        dst.sphere_ = sphere_;

        //Geometryクローン
        for(s32 i=0; i<numGeometries_; ++i){
            geometries_[i].copyTo(dst.geometries_[i]);
        }

        //Meshクローン
        for(s32 i=0; i<numMeshes_; ++i){
            u32 geomIndex = (meshes_[i].getGeometry() - geometries_);
            u32 materialIndex = (meshes_[i].getMaterial() - materials_);

            dst.meshes_[i].create(
                meshes_[i].getType(),
                meshes_[i].getIndexOffset(),
                meshes_[i].getNumIndices(),
                &dst.geometries_[geomIndex],
                &dst.materials_[materialIndex],
                meshes_[i].getSphere());
        }

        //Textureクローン
        for(s32 i=0; i<numTextures_; ++i){
            dst.textures_[i] = textures_[i];
        }

        //Materialクローン
        for(s32 i=0; i<numMaterials_; ++i){
            dst.materials_[i].flags_ = materials_[i].flags_;
            dst.materials_[i].diffuse_ = materials_[i].diffuse_;
            dst.materials_[i].specular_ = materials_[i].specular_;
            dst.materials_[i].ambient_ = materials_[i].ambient_;
            dst.materials_[i].shadow_ = materials_[i].shadow_;
            dst.materials_[i].blendState_ = materials_[i].blendState_;

            for(s32 j=0; j<TextureType_Num; ++j){
                dst.materials_[i].textureIDs_[j] = materials_[i].textureIDs_[j];

                //if(NULL != materials_[i].textures_[j]){
                //    u32 texIndex = (materials_[i].textures_[j] - textures_)/sizeof(lgraphics::Texture2DRef);
                //    dst.materials_[i].textures_[j] = &dst.textures_[texIndex];
                //}
            }
        }

        //Nodeクローン
        for(s32 i=0; i<numNodes_; ++i){
            nodes_[i].copyTo(dst.nodes_[i]);
        }
        dst.resetNodeMeshes();
        return true;
    }

    //-----------------------------------------------------
    void Model::resetNodeMeshes()
    {
        for(s32 i=0; i<numNodes_; ++i){
            nodes_[i].meshes_ = meshes_ + nodes_[i].meshStartIndex_;
        }
    }

    //-----------------------------------------------------
    s32 Model::calcBufferSize() const
    {
        s32 size = numGeometries_ * sizeof(Geometry)
            + numMeshes_ * sizeof(Mesh)
            + numMaterials_ * sizeof(Material)
            + numNodes_ * sizeof(Node)
            + numTextures_ * sizeof(Texture2D);
        return size;
    }
}

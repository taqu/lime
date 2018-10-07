#ifndef INC_LFRAMEWORK_MODEL_H_
#define INC_LFRAMEWORK_MODEL_H_
/**
@file Model.h
@author t-sakai
@date 2016/11/23 create
*/
#include "../lframework.h"
#include <lcore/intrusive_ptr.h>
#include <lmath/Vector4.h>
#include <lmath/Quaternion.h>
#include <lmath/geometry/Sphere.h>

#include "Geometry.h"
#include "Mesh.h"
#include "Material.h"
#include "Node.h"
#include "Texture.h"

namespace lfw
{
    class Geometry;
    class Mesh;
    class Material;
    class Node;
    class Texture2D;

    class Model
    {
    public:
        typedef lcore::intrusive_ptr<Model> pointer;

        Model();
        ~Model();

        /**
        @brief バッファ作成
        @return 成否
        @param numMeshes ... メッシュ数
        @param numMaterials ... マテリアル数
        @param numNodes ...
        @param numTextures ...
        */
        bool create(
            u32 numGeometries,
            u32 numMeshes,
            u32 numMaterials,
            u32 numNodes,
            u32 numTextures);

        void destroy();

        bool copyTo(Model& dst);
        void resetNodeMeshes();

        /// スワップ
        void swap(Model& rhs);

        /// 境界球セット
        inline void setSphere(const lmath::Sphere& s);

        /// 境界球ゲット
        inline const lmath::Sphere& getSphere() const;

        /// ジオメトリ数
        inline s32 getNumGeometries() const;

        /**
        @brief ジオメトリ
        */
        inline Geometry& getGeometry(s32 index);

        /// メッシュ数
        inline s32 getNumMeshes() const;

        /**
        @brief メッシュ
        */
        inline Mesh& getMesh(s32 index);

        /// マテリアル数
        inline s32 getNumMaterials() const;

        /**
        @brief マテリアル
        */
        inline Material& getMaterial(s32 index);

        /// ノード数
        inline s32 getNumNodes() const;

        /// ノード
        inline Node& getNode(s32 index);

        /// テクスチャ数
        inline s32 getNumTextures() const;

        /// テクスチャ
        inline Texture2D& getTexture(s32 index);

        /// テクスチャ
        inline void setTexture(s32 index, Texture2D& texture);

    protected:
        Model(const Model&);
        Model& operator=(const Model&);

        friend void intrusive_ptr_addref(Model* model);
        friend void intrusive_ptr_release(Model* model);

        template<class T>
        static T* placementArrayConstruct(void* ptr, s32 num);

        s32 calcBufferSize() const;

        s32 referenceCount_;
        lmath::Sphere sphere_;
        Geometry* geometries_;
        Mesh* meshes_; /// メッシュ配列
        Material* materials_; /// マテリアル配列
        Node* nodes_;
        Texture2D* textures_;

        s32 numGeometries_;
        s32 numMeshes_; /// メッシュ数
        s32 numMaterials_; /// マテリアル数
        s32 numNodes_;
        s32 numTextures_;
    };

    //--------------------------------------------------
    inline void Model::setSphere(const lmath::Sphere& s)
    {
        sphere_ = s;
    }

    //--------------------------------------------------
    inline const lmath::Sphere& Model::getSphere() const
    {
        return sphere_;
    }

    //--------------------------------------------------
    /// ジオメトリ数
    inline s32 Model::getNumGeometries() const
    {
        return numGeometries_;
    }

    //--------------------------------------------------
    /**
    @brief ジオメトリ
    */
    inline Geometry& Model::getGeometry(s32 index)
    {
        LASSERT(0<=index && index<numGeometries_);
        return geometries_[index];
    }

    //--------------------------------------------------
    // メッシュ数
    inline s32 Model::getNumMeshes() const
    {
        return numMeshes_;
    }

    //--------------------------------------------------
    // メッシュ
    inline Mesh& Model::getMesh(s32 index)
    {
        LASSERT(0<=index && index<numMeshes_);
        return meshes_[index];
    }

    //--------------------------------------------------
    // マテリアル数
    inline s32 Model::getNumMaterials() const
    {
        return numMaterials_;
    }

    //--------------------------------------------------
    // マテリアル
    inline Material& Model::getMaterial(s32 index)
    {
        LASSERT(0<=index && index<numMaterials_);
        return materials_[index];
    }

    //--------------------------------------------------
    // ノード数
    inline s32 Model::getNumNodes() const
    {
        return numNodes_;
    }

    //--------------------------------------------------
    // ノード
    inline Node& Model::getNode(s32 index)
    {
        LASSERT(0<=index && index<numNodes_);
        return nodes_[index];
    }

    //--------------------------------------------------
    // テクスチャ数
    inline s32 Model::getNumTextures() const
    {
        return numTextures_;
    }

    //--------------------------------------------------
    // テクスチャ
    inline Texture2D& Model::getTexture(s32 index)
    {
        LASSERT(0<=index && index<numTextures_);
        return textures_[index];
    }

    //--------------------------------------------------
    // テクスチャ
    inline void Model::setTexture(s32 index, Texture2D& texture)
    {
        LASSERT(0<=index && index<numTextures_);
        textures_[index] = texture;
    }

    //--------------------------------------------------
    template<class T>
    T* Model::placementArrayConstruct(void* ptr, s32 num)
    {
        T* t = static_cast<T*>(ptr);
        for(s32 i=0; i<num; ++i){
            LPLACEMENT_NEW(&t[i]) T();
        }
        return t;
    }

    inline void intrusive_ptr_addref(Model* model)
    {
        ++model->referenceCount_;
    }

    inline void intrusive_ptr_release(Model* model)
    {
        --model->referenceCount_;
        if(0 == model->referenceCount_){
            LDELETE_RAW(model);
        }
    }
}
#endif //INC_LFRAMEWORK_MODEL_H_

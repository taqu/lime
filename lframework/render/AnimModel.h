#ifndef INC_LFRAMEWORK_ANIMMODEL_H__
#define INC_LFRAMEWORK_ANIMMODEL_H__
/**
@file AnimModel.h
@author t-sakai
@date 2016/11/26 create
*/
#include "Model.h"
#include "animation/Skeleton.h"

namespace lfw
{
    class AnimModel : public Model
    {
    public:
        typedef lcore::intrusive_ptr<AnimModel> pointer;

        AnimModel();
        ~AnimModel();

        /**
        @brief バッファ作成
        @return 成否
        @param numMeshes ... メッシュ数
        @param numMaterials ... マテリアル数
        @param numNodes ...
        @param numTextures ...
        @param numJoints ...
        */
        bool create(
            u32 numGeometries,
            u32 numMeshes,
            u32 numMaterials,
            u32 numNodes,
            u32 numTextures,
            s32 numJoints);

        void destroy();

        bool copyTo(AnimModel& dst);

        /// スワップ
        void swap(AnimModel& rhs);

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

        inline Skeleton& getSkeleton();
        inline void swapSkeleton(Skeleton& skeleton);
    protected:
        AnimModel(const AnimModel&);
        AnimModel& operator=(const AnimModel&);

        friend void intrusive_ptr_addref(AnimModel* model);
        friend void intrusive_ptr_release(AnimModel* model);

        Skeleton skeleton_;
    };

    //--------------------------------------------------
    inline void AnimModel::setSphere(const lmath::Sphere& s)
    {
        Model::setSphere(s);
    }

    //--------------------------------------------------
    inline const lmath::Sphere& AnimModel::getSphere() const
    {
        return Model::getSphere();
    }

    //--------------------------------------------------
    /// ジオメトリ数
    inline s32 AnimModel::getNumGeometries() const
    {
        return Model::getNumGeometries();
    }

    //--------------------------------------------------
    /**
    @brief ジオメトリ
    */
    inline Geometry& AnimModel::getGeometry(s32 index)
    {
        return Model::getGeometry(index);
    }

    //--------------------------------------------------
    // メッシュ数
    inline s32 AnimModel::getNumMeshes() const
    {
        return Model::getNumMeshes();
    }

    //--------------------------------------------------
    // メッシュ
    inline Mesh& AnimModel::getMesh(s32 index)
    {
        return Model::getMesh(index);
    }

    //--------------------------------------------------
    // マテリアル数
    inline s32 AnimModel::getNumMaterials() const
    {
        return Model::getNumMaterials();
    }

    //--------------------------------------------------
    // マテリアル
    inline Material& AnimModel::getMaterial(s32 index)
    {
        return Model::getMaterial(index);
    }

    //--------------------------------------------------
    // ノード数
    inline s32 AnimModel::getNumNodes() const
    {
        return Model::getNumNodes();
    }

    //--------------------------------------------------
    // ノード
    inline Node& AnimModel::getNode(s32 index)
    {
        return Model::getNode(index);
    }

    //--------------------------------------------------
    // テクスチャ数
    inline s32 AnimModel::getNumTextures() const
    {
        return Model::getNumTextures();
    }

    //--------------------------------------------------
    // テクスチャ
    inline Texture2D& AnimModel::getTexture(s32 index)
    {
        return Model::getTexture(index);
    }

    //--------------------------------------------------
    // テクスチャ
    inline void AnimModel::setTexture(s32 index, Texture2D& texture)
    {
        Model::setTexture(index, texture);
    }

    inline Skeleton& AnimModel::getSkeleton()
    {
        return skeleton_;
    }

    inline void AnimModel::swapSkeleton(Skeleton& skeleton)
    {
        skeleton_.swap(skeleton);
    }

    //--------------------------------------------------
    inline void intrusive_ptr_addref(AnimModel* model)
    {
        ++model->referenceCount_;
    }

    inline void intrusive_ptr_release(AnimModel* model)
    {
        --model->referenceCount_;
        if(0 == model->referenceCount_){
            LDELETE_RAW(model);
        }
    }
}
#endif //INC_LFRAMEWORK_ANIMMODEL_H__

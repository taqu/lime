#ifndef INC_LSCENE_LRENDER_ANIMOBJECT_H__
#define INC_LSCENE_LRENDER_ANIMOBJECT_H__
/**
@file AnimObject.h
@author t-sakai
@date 2013/07/31 create
*/
#include <lmath/Vector4.h>
#include <lmath/Quaternion.h>
#include <lmath/geometry/Sphere.h>

#include "render.h"
#include "../Resource.h"
#include "../anim/Skeleton.h"
#include "ObjectContent.h"

namespace lscene
{
namespace lrender
{
    class AnimObject : public ResourceTyped<ResourceType_AnimObject>
    {
    public:
        AnimObject();
        virtual ~AnimObject();

        /**
        @brief バッファ作成
        @return 成否
        @param numMeshes ... メッシュ数
        @param numMaterials ... マテリアル数
        */
        inline bool create(
            u32 numGeometries,
            u32 numMeshes,
            u32 numMaterials,
            u32 numNodes,
            u32 numTextures);

        inline bool clone(AnimObject& dst);

        /// スワップ
        inline void swap(AnimObject& rhs);

        /// 境界球セット
        inline void setSphere(const lmath::Sphere& s);

        /// 境界球ゲット
        inline const lmath::Sphere& getSphere() const;

        /// ジオメトリ数
        inline u32 getNumGeometries() const;

        /**
        @brief ジオメトリ
        */
        inline Geometry& getGeometry(u32 index);

        /// メッシュ数
        inline u32 getNumMeshes() const;

        /**
        @brief メッシュ
        */
        inline Mesh& getMesh(u32 index);

        /// マテリアル数
        inline u32 getNumMaterials() const;

        /**
        @brief マテリアル
        */
        inline Material& getMaterial(u32 index);

        /// ノード数
        inline u32 getNumNodes() const;

        /// ノード
        inline Node& getNode(u32 index);

        /// テクスチャ数
        inline u32 getNumTextures() const;

        /// テクスチャ
        inline Texture2D& getTexture(u32 index);

        /// テクスチャ
        inline void setTexture(u32 index, Texture2D& texture);


        //inline u32 getNumSkinningMatrices() const;

        //inline const lmath::Matrix34* getSkinningMatricesAligned16() const;
        //inline lmath::Matrix34* getSkinningMatricesAligned16();

        inline AnimObjectContent& getContent();

        inline lanim::Skeleton::pointer& getSkeleton();
        inline void setSkeleton(lanim::Skeleton::pointer& skeleton);

    private:
        AnimObject(const AnimObject&);
        AnimObject& operator=(const AnimObject&);

        lmath::Sphere sphere_;
        lanim::Skeleton::pointer skeleton_;
        AnimObjectContent content_;
    };

    //-----------------------------------------------------
    // バッファ作成
    inline bool AnimObject::create(
        u32 numGeometries,
        u32 numMeshes,
        u32 numMaterials,
        u32 numNodes,
        u32 numTextures)
    {
        return content_.create(
            numGeometries,
            numMeshes,
            numMaterials,
            numNodes,
            numTextures);
    }

    //--------------------------------------------------
    inline bool AnimObject::clone(AnimObject& dst)
    {
        if(!dst.content_.create(
            content_.numGeometries_,
            content_.numMeshes_,
            content_.numMaterials_,
            content_.numNodes_,
            content_.numTextures_))
        {
            return false;
        }

        dst.sphere_ = sphere_;
        content_.clone(dst.content_);
        return true;
    }

    // スワップ
    inline void AnimObject::swap(AnimObject& rhs)
    {
        sphere_.swap(rhs.sphere_);
        content_.swap(rhs.content_);
    }

    //--------------------------------------------------
    inline void AnimObject::setSphere(const lmath::Sphere& s)
    {
        sphere_ = s;
    }

    //--------------------------------------------------
    inline const lmath::Sphere& AnimObject::getSphere() const
    {
        return sphere_;
    }

    //--------------------------------------------------
    /// ジオメトリ数
    inline u32 AnimObject::getNumGeometries() const
    {
        return content_.getNumGeometries();
    }

    //--------------------------------------------------
    /**
    @brief ジオメトリ
    */
    inline Geometry& AnimObject::getGeometry(u32 index)
    {
        return content_.getGeometry(index);
    }

    //--------------------------------------------------
    // メッシュ数
    inline u32 AnimObject::getNumMeshes() const
    {
        return content_.getNumMeshes();
    }

    //--------------------------------------------------
    // メッシュ
    inline Mesh& AnimObject::getMesh(u32 index)
    {
        return content_.getMesh(index);
    }

    //--------------------------------------------------
    // マテリアル数
    inline u32 AnimObject::getNumMaterials() const
    {
        return content_.getNumMaterials();
    }

    //--------------------------------------------------
    // マテリアル
    inline Material& AnimObject::getMaterial(u32 index)
    {
        return content_.getMaterial(index);
    }

    //--------------------------------------------------
    // ノード数
    inline u32 AnimObject::getNumNodes() const
    {
        return content_.getNumNodes();
    }

    //--------------------------------------------------
    // ノード
    inline Node& AnimObject::getNode(u32 index)
    {
        return content_.getNode(index);
    }

    //--------------------------------------------------
    // テクスチャ数
    inline u32 AnimObject::getNumTextures() const
    {
        return content_.getNumTextures();
    }

    //--------------------------------------------------
    // テクスチャ
    inline Texture2D& AnimObject::getTexture(u32 index)
    {
        return content_.getTexture(index);
    }

    //--------------------------------------------------
    // テクスチャ
    inline void AnimObject::setTexture(u32 index, Texture2D& texture)
    {
        return content_.setTexture(index, texture);
    }

    ////--------------------------------------------------
    //inline u32 AnimObject::getNumSkinningMatrices() const
    //{
    //    return content_.getNumSkinningMatrices();
    //}


    ////--------------------------------------------------
    //inline const lmath::Matrix34* AnimObject::getSkinningMatricesAligned16() const
    //{
    //    return content_.getSkinningMatricesAligned16();
    //}

    ////--------------------------------------------------
    //inline lmath::Matrix34* AnimObject::getSkinningMatricesAligned16()
    //{
    //    return content_.getSkinningMatricesAligned16();
    //}

    //--------------------------------------------------
    inline AnimObjectContent& AnimObject::getContent()
    {
        return content_;
    }

    //--------------------------------------------------
    inline lanim::Skeleton::pointer& AnimObject::getSkeleton()
    {
        return skeleton_;
    }

    //--------------------------------------------------
    inline void AnimObject::setSkeleton(lanim::Skeleton::pointer& skeleton)
    {
        skeleton_ = skeleton;
    }
}
}
#endif //INC_LSCENE_LRENDER_ANIMOBJECT_H__

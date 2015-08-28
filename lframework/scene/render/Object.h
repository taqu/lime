#ifndef INC_LSCENE_LRENDER_OBJECT_H__
#define INC_LSCENE_LRENDER_OBJECT_H__
/**
@file Object.h
@author t-sakai
@date 2012/01/03 create
*/
#include <lmath/Vector4.h>
#include <lmath/Quaternion.h>
#include <lmath/geometry/Sphere.h>

#include "render.h"
#include "../Resource.h"

#include "ObjectContent.h"

namespace lscene
{
namespace lrender
{
    class Object : public ResourceTyped<ResourceType_Object>
    {
    public:
        Object();
        virtual ~Object();

        /**
        @brief バッファ作成
        @return 成否
        @param numMeshes ... メッシュ数
        @param numMaterials ... マテリアル数
        @param numNodes ... 
        @param numTextures ... 
        @param numMatrices ... 
        */
        inline bool create(
            u32 numGeometries,
            u32 numMeshes,
            u32 numMaterials,
            u32 numNodes,
            u32 numTextures);

        inline bool clone(Object& dst);

        /// スワップ
        inline void swap(Object& rhs);

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

        inline ObjectContent& getContent();
    private:
        Object(const Object&);
        Object& operator=(const Object&);

        lmath::Sphere sphere_;
        ObjectContent content_;
    };

    //-----------------------------------------------------
    // バッファ作成
    inline bool Object::create(
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
    inline bool Object::clone(Object& dst)
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
    inline void Object::swap(Object& rhs)
    {
        sphere_.swap(rhs.sphere_);
        content_.swap(rhs.content_);
    }

    //--------------------------------------------------
    inline void Object::setSphere(const lmath::Sphere& s)
    {
        sphere_ = s;
    }

    //--------------------------------------------------
    inline const lmath::Sphere& Object::getSphere() const
    {
        return sphere_;
    }

    //--------------------------------------------------
    /// ジオメトリ数
    inline u32 Object::getNumGeometries() const
    {
        return content_.getNumGeometries();
    }

    //--------------------------------------------------
    /**
    @brief ジオメトリ
    */
    inline Geometry& Object::getGeometry(u32 index)
    {
        return content_.getGeometry(index);
    }

    //--------------------------------------------------
    // メッシュ数
    inline u32 Object::getNumMeshes() const
    {
        return content_.getNumMeshes();
    }

    //--------------------------------------------------
    // メッシュ
    inline Mesh& Object::getMesh(u32 index)
    {
        return content_.getMesh(index);
    }

    //--------------------------------------------------
    // マテリアル数
    inline u32 Object::getNumMaterials() const
    {
        return content_.getNumMaterials();
    }

    //--------------------------------------------------
    // マテリアル
    inline Material& Object::getMaterial(u32 index)
    {
        return content_.getMaterial(index);
    }

    //--------------------------------------------------
    // ノード数
    inline u32 Object::getNumNodes() const
    {
        return content_.getNumNodes();
    }

    //--------------------------------------------------
    // ノード
    inline Node& Object::getNode(u32 index)
    {
        return content_.getNode(index);
    }

    //--------------------------------------------------
    // テクスチャ数
    inline u32 Object::getNumTextures() const
    {
        return content_.getNumTextures();
    }

    //--------------------------------------------------
    // テクスチャ
    inline Texture2D& Object::getTexture(u32 index)
    {
        return content_.getTexture(index);
    }

    //--------------------------------------------------
    // テクスチャ
    inline void Object::setTexture(u32 index, Texture2D& texture)
    {
        return content_.setTexture(index, texture);
    }

    //--------------------------------------------------
    inline ObjectContent& Object::getContent()
    {
        return content_;
    }
}
}
#endif //INC_LSCENE_LRENDER_OBJECT_H__

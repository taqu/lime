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
        @brief �o�b�t�@�쐬
        @return ����
        @param numMeshes ... ���b�V����
        @param numMaterials ... �}�e���A����
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

        /// �X���b�v
        inline void swap(Object& rhs);

        /// ���E���Z�b�g
        inline void setSphere(const lmath::Sphere& s);

        /// ���E���Q�b�g
        inline const lmath::Sphere& getSphere() const;

        /// �W�I���g����
        inline u32 getNumGeometries() const;

        /**
        @brief �W�I���g��
        */
        inline Geometry& getGeometry(u32 index);

        /// ���b�V����
        inline u32 getNumMeshes() const;

        /**
        @brief ���b�V��
        */
        inline Mesh& getMesh(u32 index);

        /// �}�e���A����
        inline u32 getNumMaterials() const;

        /**
        @brief �}�e���A��
        */
        inline Material& getMaterial(u32 index);

        /// �m�[�h��
        inline u32 getNumNodes() const;

        /// �m�[�h
        inline Node& getNode(u32 index);

        /// �e�N�X�`����
        inline u32 getNumTextures() const;

        /// �e�N�X�`��
        inline Texture2D& getTexture(u32 index);

        /// �e�N�X�`��
        inline void setTexture(u32 index, Texture2D& texture);

        inline ObjectContent& getContent();
    private:
        Object(const Object&);
        Object& operator=(const Object&);

        lmath::Sphere sphere_;
        ObjectContent content_;
    };

    //-----------------------------------------------------
    // �o�b�t�@�쐬
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

    // �X���b�v
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
    /// �W�I���g����
    inline u32 Object::getNumGeometries() const
    {
        return content_.getNumGeometries();
    }

    //--------------------------------------------------
    /**
    @brief �W�I���g��
    */
    inline Geometry& Object::getGeometry(u32 index)
    {
        return content_.getGeometry(index);
    }

    //--------------------------------------------------
    // ���b�V����
    inline u32 Object::getNumMeshes() const
    {
        return content_.getNumMeshes();
    }

    //--------------------------------------------------
    // ���b�V��
    inline Mesh& Object::getMesh(u32 index)
    {
        return content_.getMesh(index);
    }

    //--------------------------------------------------
    // �}�e���A����
    inline u32 Object::getNumMaterials() const
    {
        return content_.getNumMaterials();
    }

    //--------------------------------------------------
    // �}�e���A��
    inline Material& Object::getMaterial(u32 index)
    {
        return content_.getMaterial(index);
    }

    //--------------------------------------------------
    // �m�[�h��
    inline u32 Object::getNumNodes() const
    {
        return content_.getNumNodes();
    }

    //--------------------------------------------------
    // �m�[�h
    inline Node& Object::getNode(u32 index)
    {
        return content_.getNode(index);
    }

    //--------------------------------------------------
    // �e�N�X�`����
    inline u32 Object::getNumTextures() const
    {
        return content_.getNumTextures();
    }

    //--------------------------------------------------
    // �e�N�X�`��
    inline Texture2D& Object::getTexture(u32 index)
    {
        return content_.getTexture(index);
    }

    //--------------------------------------------------
    // �e�N�X�`��
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

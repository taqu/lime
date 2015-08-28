#ifndef INC_LSCENE_LRENDER_OBJECTCONTENT_H__
#define INC_LSCENE_LRENDER_OBJECTCONTENT_H__
/**
@file ObjectContent.h
@author t-sakai
@date 2014/12/10 create
*/
#include "render.h"

namespace lmath
{
    class Matrix34;
    class DualQuaternion;
}

namespace lscene
{
namespace lrender
{
    class Geometry;
    class Mesh;
    class Material;
    class Node;
    class Texture2D;

    //------------------------------------------
    //---
    //--- ObjectContent
    //---
    //------------------------------------------
    class ObjectContent
    {
    public:
        ObjectContent();
        ~ObjectContent();

        /**
        @brief �o�b�t�@�쐬
        @return ����
        @param numMeshes ... ���b�V����
        @param numMaterials ... �}�e���A����
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

        u32 calcBufferSize() const;

        void clone(ObjectContent& dst);
        void copy(ObjectContent& dst);

        void swap(ObjectContent& rhs);

        /// �W�I���g����
        inline u32 getNumGeometries() const;

        /**
        @brief �W�I���g��
        */
        Geometry& getGeometry(u32 index);

        /// ���b�V����
        inline u32 getNumMeshes() const;

        /**
        @brief ���b�V��
        */
        Mesh& getMesh(u32 index);

        /// �}�e���A����
        inline u32 getNumMaterials() const;

        /**
        @brief �}�e���A��
        */
        Material& getMaterial(u32 index);

        /// �m�[�h��
        inline u32 getNumNodes() const;

        /// �m�[�h
        Node& getNode(u32 index);

        /// �e�N�X�`����
        inline u32 getNumTextures() const;

        /// �e�N�X�`��
        Texture2D& getTexture(u32 index);

        /// �e�N�X�`��
        void setTexture(u32 index, Texture2D& texture);

        template<class T>
        T* placementArrayConstruct(void* ptr, u32 num)
        {
            T* t = reinterpret_cast<T*>(ptr);
            for(u32 i=0; i<num; ++i){
                LIME_PLACEMENT_NEW(&t[i]) T();
            }
            return t;
        }

        Geometry* geometries_;
        Mesh* meshes_; /// ���b�V���z��
        Material* materials_; /// �}�e���A���z��
        Node* nodes_;
        Texture2D* textures_;

        u32 numGeometries_;
        u32 numMeshes_; /// ���b�V����
        u32 numMaterials_; /// �}�e���A����
        u32 numNodes_;
        u32 numTextures_;
    };

    //--------------------------------------------------
    /// �W�I���g����
    inline u32 ObjectContent::getNumGeometries() const
    {
        return numGeometries_;
    }

    //--------------------------------------------------
    // ���b�V����
    inline u32 ObjectContent::getNumMeshes() const
    {
        return numMeshes_;
    }

    //--------------------------------------------------
    // �}�e���A����
    inline u32 ObjectContent::getNumMaterials() const
    {
        return numMaterials_;
    }

    //--------------------------------------------------
    // �m�[�h��
    inline u32 ObjectContent::getNumNodes() const
    {
        return numNodes_;
    }

    //--------------------------------------------------
    // �e�N�X�`����
    inline u32 ObjectContent::getNumTextures() const
    {
        return numTextures_;
    }

    //------------------------------------------
    //---
    //--- AnimObjectContent
    //---
    //------------------------------------------
    class AnimObjectContent : public ObjectContent
    {
    public:
        AnimObjectContent();
        ~AnimObjectContent();

        /**
        @brief �o�b�t�@�쐬
        @return ����
        @param numMeshes ... ���b�V����
        @param numMaterials ... �}�e���A����
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

        u32 calcBufferSize() const;

        void clone(AnimObjectContent& dst);

        void swap(AnimObjectContent& rhs);

        //inline u32 getNumSkinningMatrices() const;

        //inline const lmath::Matrix34* getSkinningMatricesAligned16() const;
        //inline lmath::Matrix34* getSkinningMatricesAligned16();

        //const lmath::DualQuaternion* getDualQuaternions() const{ return dualQuaternions_;}
        //lmath::DualQuaternion* getDualQuaternions(){ return dualQuaternions_;}


        //u32 numMatrices_;
        //lmath::Matrix34* skinningMatricesAligned16_;
        //lmath::DualQuaternion* dualQuaternions_;
    };

    ////--------------------------------------------------
    //// �X�L�j���O�}�g���b�N�X��
    //inline u32 AnimObjectContent::getNumSkinningMatrices() const
    //{
    //    return numMatrices_;
    //}

    ////--------------------------------------------------
    //// �X�L�j���O�}�g���b�N�X
    //inline const lmath::Matrix34* AnimObjectContent::getSkinningMatricesAligned16() const
    //{
    //    return skinningMatricesAligned16_;
    //}

    ////--------------------------------------------------
    //inline lmath::Matrix34* AnimObjectContent::getSkinningMatricesAligned16()
    //{
    //    return skinningMatricesAligned16_;
    //}
}
}
#endif //INC_LSCENE_LRENDER_OBJECTCONTENT_H__

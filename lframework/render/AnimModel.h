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
        @brief �o�b�t�@�쐬
        @return ����
        @param numMeshes ... ���b�V����
        @param numMaterials ... �}�e���A����
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

        /// �X���b�v
        void swap(AnimModel& rhs);

        /// ���E���Z�b�g
        inline void setSphere(const lmath::Sphere& s);

        /// ���E���Q�b�g
        inline const lmath::Sphere& getSphere() const;

        /// �W�I���g����
        inline s32 getNumGeometries() const;

        /**
        @brief �W�I���g��
        */
        inline Geometry& getGeometry(s32 index);

        /// ���b�V����
        inline s32 getNumMeshes() const;

        /**
        @brief ���b�V��
        */
        inline Mesh& getMesh(s32 index);

        /// �}�e���A����
        inline s32 getNumMaterials() const;

        /**
        @brief �}�e���A��
        */
        inline Material& getMaterial(s32 index);

        /// �m�[�h��
        inline s32 getNumNodes() const;

        /// �m�[�h
        inline Node& getNode(s32 index);

        /// �e�N�X�`����
        inline s32 getNumTextures() const;

        /// �e�N�X�`��
        inline Texture2D& getTexture(s32 index);

        /// �e�N�X�`��
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
    /// �W�I���g����
    inline s32 AnimModel::getNumGeometries() const
    {
        return Model::getNumGeometries();
    }

    //--------------------------------------------------
    /**
    @brief �W�I���g��
    */
    inline Geometry& AnimModel::getGeometry(s32 index)
    {
        return Model::getGeometry(index);
    }

    //--------------------------------------------------
    // ���b�V����
    inline s32 AnimModel::getNumMeshes() const
    {
        return Model::getNumMeshes();
    }

    //--------------------------------------------------
    // ���b�V��
    inline Mesh& AnimModel::getMesh(s32 index)
    {
        return Model::getMesh(index);
    }

    //--------------------------------------------------
    // �}�e���A����
    inline s32 AnimModel::getNumMaterials() const
    {
        return Model::getNumMaterials();
    }

    //--------------------------------------------------
    // �}�e���A��
    inline Material& AnimModel::getMaterial(s32 index)
    {
        return Model::getMaterial(index);
    }

    //--------------------------------------------------
    // �m�[�h��
    inline s32 AnimModel::getNumNodes() const
    {
        return Model::getNumNodes();
    }

    //--------------------------------------------------
    // �m�[�h
    inline Node& AnimModel::getNode(s32 index)
    {
        return Model::getNode(index);
    }

    //--------------------------------------------------
    // �e�N�X�`����
    inline s32 AnimModel::getNumTextures() const
    {
        return Model::getNumTextures();
    }

    //--------------------------------------------------
    // �e�N�X�`��
    inline Texture2D& AnimModel::getTexture(s32 index)
    {
        return Model::getTexture(index);
    }

    //--------------------------------------------------
    // �e�N�X�`��
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

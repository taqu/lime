#ifndef INC_LFRAMEWORK_MODELLOADER_H__
#define INC_LFRAMEWORK_MODELLOADER_H__
/**
@file ModelLoader.h
@author t-sakai
@date 2016/11/23 create
*/
#include "load.h"
#include <lcore/Array.h>
#include <lcore/io/VirtualFileSystem.h>
#include <lgraphics/Enumerations.h>
#include "resource/Resource.h"

namespace lgfx
{
    class Texture2DRef;
    class SamplerStateRef;
    class ShaderResourceViewRef;
    class InputLayoutRef;
}

namespace lfw
{
    class Model;
    class AnimModel;
    class Geometry;
    class Mesh;
    class Material;
    class Node;
    class Texture2D;

    class Texture;
    class Skeleton;

    class LoadTexture;

    class ModelLoader
    {
    public:
        ModelLoader();
        explicit ModelLoader(lcore::FileSystem* fileSystem);
        ~ModelLoader();

        void set(lcore::FileSystem* fileSystem);

        /**
        @brief �I�u�W�F�N�g���[�h
        @return �I�u�W�F�N�g
        @param filepath ... �t�@�C���p�X
        */
        Model* loadModel(s32 setID, const Char* filepath);

        /**
        @brief �I�u�W�F�N�g���[�h
        @return �I�u�W�F�N�g
        @param filepath ... �t�@�C���p�X
        */
        AnimModel* loadAnimModel(s32 setID, const Char* filepath);

        void setReserveGeometryData(bool enable)
        {
            reserveGeometryData_ = enable;
        }

        void setConvertRefractiveIndex(bool enable)
        {
            convertRefractiveIndex_ = enable;
        }

        static bool save(const Char* filepath, Model& model, LoadTexture* textures, Skeleton* skeleton);

        inline s32 getNumTexture() const;
        inline Resource::pointer& getTexture(s32 index);
        inline void clearTextures();

    private:
        ModelLoader(const ModelLoader&);
        ModelLoader& operator=(const ModelLoader&);

        void setDirectoryPath(const Char* path);

        /**
        @brief �w�b�_���[�h
        @return ����
        @param �o�́B�w�b�_
        */
        bool load(LoadHeader& header);

        /**
        @brief ���f�����[�h
        @return ����
        @param �o�́B���f��
        */
        bool loadInternal(Model& model);
        bool loadSkeleton(AnimModel& model);
        bool loadTextures(s32 setID, Model& model);
        void calcBoundingSphere(Model& model);

        /**
        @brief �W�I���g�����[�h
        @return ����
        @param �o�́B�W�I���g��
        */
        bool load(Geometry& geometry);

        /**
        @brief ���b�V�����[�h
        @return ����
        @param �o�́B���b�V��
        */
        bool load(Model& model, Mesh& mesh);

        /**
        @brief �}�e���A�����[�h
        @return ����
        @param �o�́B�}�e���A��
        */
        bool load(Material& material);

        /**
        @brief �m�[�h���[�h
        @return ����
        @param �o�́B�m�[�h
        */
        bool load(Model& model, Node& node);

        /**
        @brief �e�N�X�`�����[�h
        @return ����
        @param �o�́B�e�N�X�`��
        */
        bool load(s32 setID, Texture2D& texture, const LoadTexture& loadTexture);

        /**
        @brief ���_���C�A�E�g�쐬
        @param header ... ���_�t���O
        @param inputLayout ... �o�́B���_���C�A�E�g
        */
        void createInputLayout(u32 vflag, lgfx::InputLayoutRef& inputLayout);

        lcore::FileSystem* fileSystem_;
        lcore::FileProxyIStream fileStream_;

        lcore::Array<Resource::pointer> textures_;

        LoadHeader header_; /// �w�b�_
        s32 directoryPathLength_;
        Char* path_;

        bool reserveGeometryData_;
        bool convertRefractiveIndex_;
    };

    inline s32 ModelLoader::getNumTexture() const
    {
        return textures_.size();
    }

    inline Resource::pointer& ModelLoader::getTexture(s32 index)
    {
        return textures_[index];
    }

    inline void ModelLoader::clearTextures()
    {
        textures_.clear();
    }
}
#endif //INC_LFRAMEWORK_MODELLOADER_H__

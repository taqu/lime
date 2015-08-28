#ifndef INC_LSCENE_LLOAD_MODELLOADER_H__
#define INC_LSCENE_LLOAD_MODELLOADER_H__
/**
@file ModelLoader.h
@author t-sakai
@date 2014/12/09 create
*/
#include "load.h"
#include "../anim/Skeleton.h"
#include <lgraphics/api/Enumerations.h>

namespace lgraphics
{
    class Texture2DRef;
    class SamplerStateRef;
    class ShaderResourceViewRef;
    class InputLayoutRef;
}

namespace lscene
{
namespace lfile
{
    class SharedFile;
}

namespace lrender
{
    class Object;
    class AnimObject;
    class ObjectContent;
    class AnimObjectContent;
    class Geometry;
    class Mesh;
    class Material;
    class Node;
    class Texture2D;
}

namespace lload
{
    class Texture;

    class ModelLoader
    {
    public:
        ModelLoader();
        ~ModelLoader();

        /**
        @brief �f�B���N�g���p�X�Z�b�g
        @param filepath
        */
        void setDirectoryPath(const Char* filepath);

        /**
        @brief �I�u�W�F�N�g���[�h
        @return ����
        @param �o�́B�I�u�W�F�N�g
        */
        bool load(lrender::Object& obj, lscene::lfile::SharedFile* file);

        /**
        @brief �I�u�W�F�N�g���[�h
        @return ����
        @param �o�́B�I�u�W�F�N�g
        */
        bool load(lrender::AnimObject& obj, lscene::lfile::SharedFile* file);

        lanim::Skeleton::pointer getSkeleton(){ return skeleton_;}

        void getTextureNameTable(u32 numTextures, lload::Texture* textures);

        void setTextureAddress(lgraphics::TextureAddress texAddress)
        {
            textureAddress_ = texAddress;
        }

        void setReserveGeometryData(bool enable)
        {
            reserveGeometryData_ = enable;
        }

        void setConvertRefractiveIndex(bool enable)
        {
            convertRefractiveIndex_ = enable;
        }

        void setOpenForceOSFileSystem(bool enable)
        {
            openForceOSFileSystem_ = enable;
        }

        static bool save(lrender::Object& obj, lload::Texture* textures, lanim::Skeleton::pointer& skeleton, const Char* filepath);
    private:
        ModelLoader(const ModelLoader&);
        ModelLoader& operator=(const ModelLoader&);

        /**
        @brief �w�b�_���[�h
        @return ����
        @param �o�́B�w�b�_
        */
        bool load(Header& header);

        /**
        @brief �I�u�W�F�N�g���[�h
        @return ����
        @param �o�́B�I�u�W�F�N�g
        */
        bool loadInternal(lrender::ObjectContent& obj);

        /**
        @brief �W�I���g�����[�h
        @return ����
        @param �o�́B�W�I���g��
        */
        bool load(lrender::Geometry& geometry);

        /**
        @brief ���b�V�����[�h
        @return ����
        @param �o�́B���b�V��
        */
        bool load(lrender::ObjectContent& obj, lrender::Mesh& mesh);

        /**
        @brief �}�e���A�����[�h
        @return ����
        @param �o�́B�}�e���A��
        */
        bool load(lrender::Material& material);

        /**
        @brief �m�[�h���[�h
        @return ����
        @param �o�́B�m�[�h
        */
        bool load(lrender::ObjectContent& obj, lrender::Node& node);

        /**
        @brief �X�P���g�����[�h
        */
        lanim::Skeleton* loadSkeleton(u32 numJoints);

        /**
        @brief �e�N�X�`�����[�h
        @return ����
        @param �o�́B�e�N�X�`��
        */
        bool load(lrender::Texture2D& texture, const lload::Texture& loadTexture);

        /**
        @brief ���_���C�A�E�g�쐬
        @param header ... ���_�t���O
        @param inputLayout ... �o�́B���_���C�A�E�g
        */
        void createInputLayout(u32 vflag, lgraphics::InputLayoutRef& inputLayout);

        lscene::lfile::SharedFile* file_;

        Header header_; /// �w�b�_
        u32 directoryPathLength_;
        Char directoryPath_[MaxPathSize+MaxFileNameSize];

        lanim::Skeleton::pointer skeleton_;

        lgraphics::TextureAddress textureAddress_;

        bool reserveGeometryData_;
        bool convertRefractiveIndex_;
        bool openForceOSFileSystem_;
    };
}
}
#endif //INC_LSCENE_LLOAD_MODELLOADER_H__

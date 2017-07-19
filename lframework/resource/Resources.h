#ifndef INC_LFRAMEWORK_RESOURCES_H__
#define INC_LFRAMEWORK_RESOURCES_H__
/**
@file Resources.h
@author t-sakai
@date 2016/11/16 create
*/
#include "../lframework.h"
#include <lcore/Array.h>
#include <lcore/HashMap.h>
#include "ResourceSet.h"
#include "InputLayoutFactory.h"
#include "ShaderManager.h"
#include "ModelLoader.h"
#include "FontManager.h"

namespace lcore
{
    class FileSystem;
}

namespace lfw
{
    /**
    @brief ���\�[�X�L���b�V���B
    */
    class Resources
    {
    public:
        static const s32 DefaultSet = 0;

        inline static u64 calcID(const Char* path);
        static Resources* create(s32 numSets, lcore::FileSystem* fileSystem);
        static void destroy(Resources* resources);

        /**
        @brief �擾. ID�͕K���L���ł��邱��.
        @return ���\�[�X�Z�b�g
        @param setID ... �Z�b�gID
        */
        const ResourceSet& getSet(s32 setID);

        /**
        @brief �ǉ�. ���łɂ���΂��̂܂ܕԂ�
        @return ���\�[�X�Z�b�g
        @param setID ... �Z�b�gID
        */
        const ResourceSet& addSet(s32 setID);

        /**
        @brief ����
        @return ����
        @param setID ... �Z�b�gID
        @param resID ... ���\�[�XID
        */
        bool existsInSet(s32 setID, u64 resID) const;

        /**
        @brief �ǉ�
        @return ����
        @param setID ... �Z�b�gID
        @param resource ... ���\�[�X
        */
        bool addInSet(s32 setID, Resource::pointer& resource);

        /**
        @brief �폜
        @param setID ... �Z�b�gID
        @param resID ... ���\�[�XID
        */
        bool removeInSet(s32 setID, u64 resID);

        /**
        @brief �폜
        @param setID ... �Z�b�gID
        */
        void removeAllInSet(s32 setID);


        /**
        @brief �ǉ�
        @return ����
        @param setID ... �Z�b�gID
        @param resID ... ���\�[�XID
        @param resource ... ���\�[�X
        */
        bool addUnique(s32 setID, Resource::pointer& resource);

        /**
        @brief �폜
        */
        void removeAll();

        /**
        @brief ����
        @return ����
        @param resID ... ���\�[�XID
        */
        bool exists(u64 resID) const;

        /**
        @brief �擾
        @return ���\�[�X
        @param resID ... ���\�[�XID
        */
        Resource::pointer get(u64 resID);

        inline InputLayoutFactory& getInputLayoutFactory();
        inline ShaderManager& getShaderManager();
        inline FontManager& getFontManager();

        Resource::pointer load(const Char* path, ResourceType type);
        Resource::pointer load(s32& setID, const Char* path, ResourceType type);
        Resource::pointer load(const Char* path, ResourceType type, const TextureParameter& param);
        Resource::pointer load(s32& setID, const Char* path, ResourceType type, const TextureParameter& param);

        inline Resource::pointer& getEmptyTextureWhite();
        inline Resource::pointer& getEmptyTextureBlack();
        inline Resource::pointer& getEmptyTextureClear();
        inline Resource::pointer& getTextureChecker();
    private:
        Resources(const Resources&);
        Resources& operator=(const Resources&);

        friend class ResourceSet;
        friend class LoadRequest;

        explicit Resources(s32 numSets, lcore::FileSystem* fileSystem);
        ~Resources();

        void expand(s64 size);
        Resource* loadInternal(s32 setID, const Char* path, ResourceType type);
        Resource* loadInternal(const Char* path, ResourceType type, const TextureParameter& param);

        typedef lcore::HopscotchHashMap<u64, Resource::pointer> KeyToResourceMap;
        KeyToResourceMap resources_;

        typedef lcore::Array<ResourceSet> ResourceSetArray;
        ResourceSetArray resourceSets_;

        Resource::pointer emptyTextureWhite_;
        Resource::pointer emptyTextureBlack_;
        Resource::pointer emptyTextureClear_;
        Resource::pointer textureChecker_;

        InputLayoutFactory inputLayoutFactory_;
        ShaderManager shaderManager_;
        FontManager fontManager_;
        ModelLoader modelLoader_;
        lcore::FileSystem* fileSystem_;
        s64 bufferSize_;
        u8* buffer_;
    };

    inline u64 Resources::calcID(const Char* path)
    {
        return calcHash(path);
    }

    inline InputLayoutFactory& Resources::getInputLayoutFactory()
    {
        return inputLayoutFactory_;
    }

    inline ShaderManager& Resources::getShaderManager()
    {
        return shaderManager_;
    }

    inline FontManager& Resources::getFontManager()
    {
        return fontManager_;
    }

    inline Resource::pointer& Resources::getEmptyTextureWhite()
    {
        return emptyTextureWhite_;
    }

    inline Resource::pointer& Resources::getEmptyTextureBlack()
    {
        return emptyTextureBlack_;
    }

    inline Resource::pointer& Resources::getEmptyTextureClear()
    {
        return emptyTextureClear_;
    }

    inline Resource::pointer& Resources::getTextureChecker()
    {
        return textureChecker_;
    }
}
#endif //INC_LFRAMEWORK_RESOURCES_H__

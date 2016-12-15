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

namespace lcore
{
    class FileSystem;
}

namespace lfw
{
    /**
    @brief リソースキャッシュ。
    */
    class Resources
    {
    public:
        static const s32 DefaultSet = 0;

        inline static u64 calcID(const Char* path);
        inline static Resources& getInstance();
        static bool initialize(s32 numSets, lcore::FileSystem* fileSystem);
        static void terminate();

        /**
        @brief 取得. IDは必ず有効であること.
        @return リソースセット
        @param setID ... セットID
        */
        const ResourceSet& getSet(s32 setID);

        /**
        @brief 追加. すでにあればそのまま返す
        @return リソースセット
        @param setID ... セットID
        */
        const ResourceSet& addSet(s32 setID);

        /**
        @brief 検索
        @return 成否
        @param setID ... セットID
        @param resID ... リソースID
        */
        bool existsInSet(s32 setID, u64 resID) const;

        /**
        @brief 追加
        @return 成否
        @param setID ... セットID
        @param resID ... リソースID
        @param resource ... リソース
        */
        bool addInSet(s32 setID, Resource::pointer& resource);

        /**
        @brief 削除
        @param setID ... セットID
        @param resID ... リソースID
        */
        bool removeInSet(s32 setID, u64 resID);

        /**
        @brief 削除
        @param setID ... セットID
        */
        void removeAllInSet(s32 setID);


        /**
        @brief 追加
        @return 成否
        @param setID ... セットID
        @param resID ... リソースID
        @param resource ... リソース
        */
        bool addUnique(s32 setID, Resource::pointer& resource);

        /**
        @brief 削除
        */
        void removeAll();

        /**
        @brief 検索
        @return 成否
        @param resID ... リソースID
        */
        bool exists(u64 resID) const;

        /**
        @brief 取得
        @return リソース
        @param resID ... リソースID
        */
        Resource::pointer get(u64 resID);

        inline InputLayoutFactory& getInputLayoutFactory();
        inline ShaderManager& getShaderManager();

        Resource::pointer load(const Char* path, ResourceType type);
        Resource::pointer load(s32& setID, const Char* path, ResourceType type);
        Resource::pointer load(const Char* path, ResourceType type, const TextureParameter& param);
        Resource::pointer load(s32& setID, const Char* path, ResourceType type, const TextureParameter& param);
    private:
        Resources(const Resources&);
        Resources& operator=(const Resources&);

        friend class ResourceSet;
        friend class LoadRequest;

        static Resources* instance_;

        explicit Resources(s32 numSets, lcore::FileSystem* fileSystem);
        ~Resources();

        void expand(s64 size);
        Resource* loadInternal(s32 setID, const Char* path, ResourceType type);
        Resource* loadInternal(const Char* path, ResourceType type, const TextureParameter& param);

        typedef lcore::HopscotchHashMap<u64, Resource::pointer> KeyToResourceMap;
        KeyToResourceMap resources_;

        typedef lcore::Array<ResourceSet> ResourceSetArray;
        ResourceSetArray resourceSets_;

        InputLayoutFactory inputLayoutFactory_;
        ShaderManager shaderManager_;
        ModelLoader modelLoader_;
        lcore::FileSystem* fileSystem_;
        s64 bufferSize_;
        u8* buffer_;
    };

    inline u64 Resources::calcID(const Char* path)
    {
        return calcHash(path);
    }

    inline Resources& Resources::getInstance()
    {
        return *instance_;
    }

    inline InputLayoutFactory& Resources::getInputLayoutFactory()
    {
        return inputLayoutFactory_;
    }

    inline ShaderManager& Resources::getShaderManager()
    {
        return shaderManager_;
    }
}
#endif //INC_LFRAMEWORK_RESOURCES_H__

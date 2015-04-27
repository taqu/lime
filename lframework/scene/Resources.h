#ifndef INC_LSCENE_RESOURCES_H__
#define INC_LSCENE_RESOURCES_H__
/**
@file Resources.h
@author t-sakai
@date 2014/12/08 create
*/
#include "lscene.h"
#include <lcore/HashMap.h>
#include <lcore/allocator/StackAllocator.h>

namespace lscene
{
namespace lfile
{
    class SharedFile;
}

    class Resource;

    /**
    @brief リソースキャッシュ。
    
    NumResourceCategory個に分類して保存
    */
    class Resources : public ResourcesAllocator
    {
    public:
        Resources();
        ~Resources();

        /**
        @brief 検索
        @return リソース。なければNULL
        @param category ... 分類
        @param id ... 検索キー
        */
        Resource* find(s32 category, u64 id);

        /**
        @brief 追加
        @return 成否
        @param category ... 分類
        @param id ... 検索キー
        @param resource ... リソース
        @param traits ... リソースタイプ
        */
        bool add(s32 category, u64 id, Resource* resource);

        /**
        @brief 削除
        @param category ... 分類
        @param resource ... リソース
        */
        void remove(s32 category, Resource* resource);

        /**
        @brief リソースクリア
        @param category ... 分類
        */
        void clear(s32 category);

        /**
        @brief リソースクリア
        */
        void clearAll();
    private:
        Resources(const Resources&);
        Resources& operator=(const Resources&);

        static const u32 PageSize = 2*1024;

        struct Entry
        {
            u64 id_;
            Resource* resource_;
        };

        typedef lcore::HopscotchHashMap<u64, Entry*> IDToEntryMap;
        typedef lcore::StackAllocator<lscene::SceneAllocator> EntryAllocator;

        /// エントリ削除
        void destroy(Entry* entry);

        IDToEntryMap idToEntry_[NumResourceCategory];
        EntryAllocator entryAllocator_;
    };
}
#endif //INC_LSCENE_RESOURCES_H__

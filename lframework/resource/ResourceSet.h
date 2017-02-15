#ifndef INC_LFRAMEWORK_RESOURCESET_H__
#define INC_LFRAMEWORK_RESOURCESET_H__
/**
@file ResourceSet.h
@author t-sakai
@date 2016/11/20 create
*/
#include "../lframework.h"
#include <lcore/Array.h>
#include "Resource.h"

namespace lfw
{
    class ResourceSet
    {
    public:
        ResourceSet();
        ResourceSet(const ResourceSet& resourceSet);
        explicit ResourceSet(s32 numResources);
        ~ResourceSet();

        /**
        @brief 検索
        @return 存在すれば0以上のインデックス, なければ-1
        @param id ... リソースID
        */
        s32 findIndex(u64 id) const;

        /**
        @brief 追加
        @return 成否
        @param id ... リソースID
        @param resource ... リソース
        */
        bool add(Resource::pointer& resource);

        /**
        @brief 削除
        @param id ... リソースID
        */
        bool remove(u64 id);

        /**
        @brief クリア
        */
        void clear();

        inline const Resource::pointer& get(s32 index) const;
        inline const Resource::pointer* begin() const;
        inline const Resource::pointer* end() const;

        /**
        @brief swap
        */
        void swap(ResourceSet& rhs);

        ResourceSet& operator=(const ResourceSet& rhs);
    private:
        typedef lcore::Array<Resource::pointer, lcore::array_static_inc_size<64>> ResourceArray;

        ResourceArray resources_;
    };

    inline const Resource::pointer& ResourceSet::get(s32 index) const
    {
        LASSERT(0<=index && index<resources_.size());
        return resources_[index];
    }

    inline const Resource::pointer* ResourceSet::begin() const
    {
        return resources_.begin();
    }

    inline const Resource::pointer* ResourceSet::end() const
    {
        return resources_.end();
    }
}
#endif //INC_LFRAMEWORK_RESOURCESET_H__

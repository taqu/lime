#ifndef INC_LCORE_POOL_H__
#define INC_LCORE_POOL_H__
/**
@file Pool.h
@author t-sakai
@date 2014/03/24 create
*/
#include "lcore.h"

namespace lcore
{
    template<class T, typename Allocator=DefaultAllocator>
    class PagePool
    {
    public:
        typedef PagePool<T,Allocator> this_type;
        typedef T value_type;
        static const u32 value_size = sizeof(T);
        typedef Allocator allocator_type;

        PagePool();
        explicit PagePool(s32 pageSize);
        ~PagePool();

        void* allocate();
        void deallocate(void*& ptr);

        inline T* construct();
        inline void destruct(T*& ptr);

        void swap(this_type& rhs);
    private:
        PagePool(const this_type&);
        PagePool& operator=(const this_type&);

        union Entry
        {
            Entry* next_;
            u8 value_[value_size];
        };

        struct Page
        {
            Page* next_;
        };

        void createEntries();

        s32 pageSize_;
        Page* page_;
        Entry* entries_;
    };

    template<class T, typename Allocator>
    PagePool<T, Allocator>::PagePool()
        :pageSize_(0)
        ,page_(NULL)
        ,entries_(NULL)
    {
    }

    template<class T, typename Allocator>
    PagePool<T, Allocator>::PagePool(s32 pageSize)
        :pageSize_(pageSize)
        ,page_(NULL)
        ,entries_(NULL)
    {
        LASSERT((sizeof(Page)+sizeof(Entry))<=pageSize_);
    }

    template<class T, typename Allocator>
    PagePool<T, Allocator>::~PagePool()
    {
        entries_ = NULL;

        while(NULL != page_){
            Page* next = page_->next_;
            LALLOCATOR_FREE(allocator_type, page_);
            page_ = next;
        }
    }

    template<class T, typename Allocator>
    void* PagePool<T, Allocator>::allocate()
    {
        if(NULL == entries_){
            createEntries();
        }
        Entry* entry = entries_;
        entries_ = entries_->next_;

        return entry;
    }

    template<class T, typename Allocator>
    void PagePool<T, Allocator>::deallocate(void*& ptr)
    {
        if(NULL == ptr){
            return;
        }
        Entry* entry = static_cast<Entry*>(ptr);
        entry->next_ = entries_;
        entries_ = entry;
        ptr = NULL;
    }

    template<class T, typename Allocator>
    inline typename PagePool<T, Allocator>::value_type* PagePool<T, Allocator>::construct()
    {
        return LPLACEMENT_NEW(allocate()) T();
    }

    template<class T, typename Allocator>
    inline void PagePool<T, Allocator>::destruct(value_type*& ptr)
    {
        if(NULL == ptr){
            return;
        }
        ptr->~T();
        deallocate((void*&)ptr);
    }

    template<class T, typename Allocator>
    void PagePool<T, Allocator>::swap(this_type& rhs)
    {
        lcore::swap(pageSize_, rhs.pageSize_);
        lcore::swap(page_, rhs.page_);
        lcore::swap(entries_, rhs.entries_);
    }

    template<class T, typename Allocator>
    void PagePool<T, Allocator>::createEntries()
    {
        LASSERT((sizeof(Page)+sizeof(Entry))<=pageSize_);

        Page* page = (Page*)LALLOCATOR_MALLOC(allocator_type, pageSize_);
        s32 numEntries = (pageSize_-sizeof(Page))/sizeof(Entry);

        Entry* entries = reinterpret_cast<Entry*>(page+1);
        for(s32 i=1; i<numEntries; ++i){
            entries[i-1].next_ = &entries[i];
        }

        entries[numEntries-1].next_ = entries_;

        entries_ = entries;

        page->next_ = page_;
        page_ = page;
    }
}
#endif //INC_LCORE_POOL_H__

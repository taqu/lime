#ifndef INC_LCORE_OBJECTALLOCATOR_H_
#define INC_LCORE_OBJECTALLOCATOR_H_
/**
@file ObjectAllocator.h
@author t-sakai
@date 2017/07/27 create
*/
#include "lcore.h"

namespace lcore
{
    template<class T, typename Allocator=DefaultAllocator>
    class ObjectAllocator
    {
    public:
        static const s32 DefaultPageSize = 4096;
        typedef T value_type;
        static const u32 value_size = sizeof(T);
        typedef Allocator allocator_type;

        ObjectAllocator();
        ~ObjectAllocator();

        void initialize(s32 pageSize=DefaultPageSize);
        void terminate();

        void* allocate();
        void deallocate(void* ptr);

        void swap(ObjectAllocator& rhs);
    private:
        ObjectAllocator(const ObjectAllocator&) = delete;
        ObjectAllocator& operator=(const ObjectAllocator&) = delete;

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
        Entry* next_;
    };

    template<class T, typename Allocator>
    ObjectAllocator<T, Allocator>::ObjectAllocator()
        :pageSize_(0)
        ,page_(NULL)
        ,next_(NULL)
    {
    }

    template<class T, typename Allocator>
    ObjectAllocator<T, Allocator>::~ObjectAllocator()
    {
        terminate();
    }

    template<class T, typename Allocator>
    void ObjectAllocator<T, Allocator>::initialize(s32 pageSize)
    {
        LASSERT(sizeof(Page) <= pageSize);
        LASSERT(NULL == page_);
        pageSize_ = pageSize;
    }

    template<class T, typename Allocator>
    void ObjectAllocator<T, Allocator>::terminate()
    {
        while(NULL != page_){
            Page* next = page_->next_;
            LALLOCATOR_FREE(allocator_type, page_);
            page_ = next;
        }
        next_ = NULL;
    }

    template<class T, typename Allocator>
    void* ObjectAllocator<T, Allocator>::allocate()
    {
        if(NULL == next_){
            createEntries();
        }
        Entry* entry = next_;
        next_ = next_->next_;

        return entry;
    }

    template<class T, typename Allocator>
    void ObjectAllocator<T, Allocator>::deallocate(void* ptr)
    {
        LASSERT(NULL != ptr);
        Entry* entry = reinterpret_cast<Entry*>(ptr);
        entry->next_ = next_;
        next_ = entry;
    }

    template<class T, typename Allocator>
    void ObjectAllocator<T, Allocator>::swap(ObjectAllocator& rhs)
    {
        lcore::swap(pageSize_, rhs.pageSize_);
        lcore::swap(page_, rhs.page_);
        lcore::swap(next_, rhs.next_);
    }

    template<class T, typename Allocator>
    void ObjectAllocator<T, Allocator>::createEntries()
    {
        s32 entrySize = sizeof(Entry);
        Page* page = (Page*)LALLOCATOR_MALLOC(allocator_type, pageSize_);
        s32 numEntries = (pageSize_-sizeof(Page))/entrySize;

        Entry* entry = reinterpret_cast<Entry*>(reinterpret_cast<u8*>(page) + sizeof(Page));
        for(s32 i=1; i<numEntries; ++i){
            entry[i-1].next_ = &entry[i];
        }
        entry[numEntries-1].next_ = next_;

        next_ = entry;

        page->next_ = page_;
        page_ = page;
    }
}
#endif //INC_LCORE_OBJECTALLOCATOR_H_

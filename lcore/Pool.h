#ifndef INC_LCORE_POOL_H_
#define INC_LCORE_POOL_H_
/**
@file Pool.h
@author t-sakai
@date 2014/03/24 create
*/
#include "lcore.h"

namespace lcore
{
    //--------------------------------------------------------
    //---
    //--- ObjectPool
    //---
    //--------------------------------------------------------
    template<class T>
    struct DefaultObjectPoolInitializer
    {
        void operator()(T&)
        {}
    };

    template<class T, typename Allocator=DefaultAllocator, class Initializer=DefaultObjectPoolInitializer<T> >
    class ObjectPool
    {
    public:
        typedef T value_type;
        static const u32 value_size = sizeof(T);
        typedef Allocator allocator_type;
        typedef Initializer initializer_type;

        static const uintptr_t Flag_Use = 0x01U;

        ObjectPool();
        ~ObjectPool();

        void initialize(u32 pageSize, initializer_type initializer = initializer_type());
        void terminate();

        value_type* pop();
        void push(value_type* ptr);

        void swap(ObjectPool& rhs);
    private:
        ObjectPool(const ObjectPool&) = delete;
        ObjectPool(ObjectPool&&) = delete;
        ObjectPool& operator=(const ObjectPool&) = delete;
        ObjectPool& operator=(ObjectPool&&) = delete;

        struct Entry
        {
            u8 value_[value_size];
            Entry* next_;
        };

        union PageEntry
        {
            PageEntry* next_;
            Entry entry_;
        };

        void createEntries();

        u32 pageSize_;
        PageEntry* page_;
        Entry* entries_;
        initializer_type initializer_;
    };

    template<class T, typename Allocator, typename Initializer>
    ObjectPool<T, Allocator, Initializer>::ObjectPool()
        :pageSize_(0)
        ,page_(NULL)
        ,entries_(NULL)
    {
    }

    template<class T, typename Allocator, typename Initializer>
    ObjectPool<T, Allocator, Initializer>::~ObjectPool()
    {
        LASSERT(NULL == page_);
    }

    template<class T, typename Allocator, typename Initializer>
    void ObjectPool<T, Allocator, Initializer>::initialize(u32 pageSize, initializer_type initializer)
    {
        LASSERT(sizeof(PageEntry)*2 <= pageSize);
        LASSERT(NULL == page_);
        pageSize_ = pageSize;
        initializer_ = initializer;
    }

    template<class T, typename Allocator, typename Initializer>
    void ObjectPool<T, Allocator, Initializer>::terminate()
    {
        while(NULL != entries_){
            reinterpret_cast<value_type*>(entries_->value_)->~T();
            entries_ = entries_->next_;
        }

        while(NULL != page_){
            PageEntry* next = page_->next_;
            LALLOCATOR_FREE(allocator_type, page_);
            page_ = next;
        }
    }

    template<class T, typename Allocator, typename Initializer>
    typename ObjectPool<T, Allocator, Initializer>::value_type* ObjectPool<T, Allocator, Initializer>::pop()
    {
        if(NULL == entries_){
            createEntries();
        }
        Entry* entry = entries_;
        entries_ = entries_->next_;

        return reinterpret_cast<value_type*>(entry->value_);
    }

    template<class T, typename Allocator, typename Initializer>
    void ObjectPool<T, Allocator, Initializer>::push(value_type* ptr)
    {
        LASSERT(NULL != ptr);
        Entry* entry = reinterpret_cast<Entry*>(ptr);
        entry->next_ = entries_;
        entries_ = entry;
    }

    template<class T, typename Allocator, typename Initializer>
    void ObjectPool<T, Allocator, Initializer>::swap(ObjectPool& rhs)
    {
        lcore::swap(pageSize_, rhs.pageSize_);
        lcore::swap(page_, rhs.page_);
        lcore::swap(entries_, rhs.entries_);
        lcore::swap(initializer_, rhs.initializer_);
    }

    template<class T, typename Allocator, typename Initializer>
    void ObjectPool<T, Allocator, Initializer>::createEntries()
    {
        PageEntry* page = (PageEntry*)LALLOCATOR_MALLOC(allocator_type, pageSize_);
        s32 numEntries = (pageSize_-sizeof(PageEntry))/sizeof(PageEntry);

        value_type* ptr = NULL;
        PageEntry* pageEntries = page + 1;
        for(s32 i=1; i<numEntries; ++i){
            ptr = LPLACEMENT_NEW(pageEntries[i].entry_.value_) value_type();
            initializer_(*ptr);
            pageEntries[i-1].entry_.next_ = &pageEntries[i].entry_;
        }
        ptr = LPLACEMENT_NEW(pageEntries[0].entry_.value_) value_type();
        initializer_(*ptr);
        pageEntries[numEntries-1].entry_.next_ = entries_;

        entries_ = &pageEntries->entry_;

        page->next_ = page_;
        page_ = page;
    }

    //--------------------------------------------------------
    //---
    //--- PagePool
    //---
    //--------------------------------------------------------
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
        inline void destruct(value_type*& ptr);

        void swap(this_type& rhs);
    private:
        PagePool(const this_type&) = delete;
        PagePool(this_type&&) = delete;
        PagePool& operator=(const this_type&) = delete;
        PagePool& operator=(this_type&&) = delete;

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
#endif //INC_LCORE_POOL_H_

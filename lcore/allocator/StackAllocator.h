#ifndef INC_LCORE_STACKALLOCATOR_H__
#define INC_LCORE_STACKALLOCATOR_H__
/**
@file StackAllocator.h
@author t-sakai
@date 2014/10/12 create
*/
#include "../lcore.h"

namespace lcore
{
    //--------------------------------------------------
    //---
    //--- StackAllocator
    //---
    //--------------------------------------------------
    template<class Allocator = DefaultAllocator>
    class StackAllocator
    {
    public:
        static const u32 DefaultPageSize = 4*1024;
        static const u32 DefaultAlignSize = sizeof(void*);

        typedef StackAllocator<Allocator> this_type;
        typedef Allocator allocator_type;

        StackAllocator(u32 chunkSize);
        StackAllocator(u32 pageSize, u32 chunkSize, u32 alignSize);
        ~StackAllocator();

        void* allocate();
        void deallocate(void* ptr);
        void clear();

        void swap(this_type& rhs);
    private:
        StackAllocator(const StackAllocator&);
        StackAllocator& operator=(const StackAllocator&);

        struct Page
        {
            Page* next_;
        };

        struct Chunk
        {
            Chunk* next_;
        };

        void createChunk();

        Page* page_;
        Chunk* chunk_;
        u32 pageSize_;
        u32 chunkSize_;
    };

    template<class Allocator>
    StackAllocator<Allocator>::StackAllocator(u32 chunkSize)
        :page_(NULL)
        ,chunk_(NULL)
        ,pageSize_(DefaultPageSize)
        ,chunkSize_(chunkSize)
    {
        u32 alignMask = DefaultAlignSize-1;
        chunkSize_ = lcore::maximum(chunkSize, sizeof(Chunk));
        chunkSize_ = (chunkSize_+alignMask) & ~alignMask;
        LASSERT(chunkSize_<pageSize_);
    }

    template<class Allocator>
    StackAllocator<Allocator>::StackAllocator(u32 pageSize, u32 chunkSize, u32 alignSize)
        :page_(NULL)
        ,chunk_(NULL)
        ,pageSize_(pageSize)
        ,chunkSize_(chunkSize)
    {
        LASSERT(0<chunkSize);
        LASSERT(0<alignSize);

        u32 alignMask = alignSize-1;
        chunkSize_ = lcore::maximum(chunkSize, sizeof(Chunk));
        chunkSize_ = (chunkSize_+alignMask) & ~alignMask;
        LASSERT(chunkSize_<pageSize_);
    }

    template<class Allocator>
    StackAllocator<Allocator>::~StackAllocator()
    {
        clear();
    }

    template<class Allocator>
    void* StackAllocator<Allocator>::allocate()
    {
        if(NULL == chunk_){
            createChunk();
        }
        Chunk* chunk = chunk_;
        chunk_ = chunk_->next_;
        return chunk;
    }

    template<class Allocator>
    void StackAllocator<Allocator>::deallocate(void* ptr)
    {
        LASSERT(NULL != ptr);
        Chunk* chunk = reinterpret_cast<Chunk*>(ptr);
        chunk->next_ = chunk_;
        chunk_ = chunk;
    }

    template<class Allocator>
    void StackAllocator<Allocator>::clear()
    {
        while(NULL != page_){
            Page* next = page_->next_;
            allocator_type::free(page_);
            page_ = next;
        }
        chunk_ = NULL;
    }


    template<class Allocator>
    void StackAllocator<Allocator>::swap(this_type& rhs)
    {
        lcore::swap(page_, rhs.page_);
        lcore::swap(chunk_, rhs.chunk_);
        lcore::swap(pageSize_, rhs.pageSize_);
        lcore::swap(chunkSize_, rhs.chunkSize_);
    }

    template<class Allocator>
    void StackAllocator<Allocator>::createChunk()
    {
        LASSERT(NULL == chunk_);

        //ページ作成・リンク
        Page* newPage = reinterpret_cast<Page*>( LIME_ALLOCATOR_MALLOC(allocator_type, pageSize_) );
        if(NULL == page_){
            newPage->next_ = NULL;
            page_ = newPage;
        }else{
            newPage->next_ = page_;
            page_ = newPage;
        }

        //先頭一つ分はページ情報
        Char* chunk = reinterpret_cast<Char*>(newPage) + chunkSize_;
        chunk_ = reinterpret_cast<Chunk*>(chunk);
        u32 numChunks = (pageSize_-chunkSize_)/chunkSize_;

        //チャンクをリンク
        Char* prev = chunk;
        for(u32 i=1; i<numChunks; ++i){
            Char* cur = prev + chunkSize_;
            reinterpret_cast<Chunk*>(prev)->next_ = reinterpret_cast<Chunk*>(cur);
            prev = cur;
        }
        reinterpret_cast<Chunk*>(prev)->next_ = NULL;
    }
}
#endif //INC_LCORE_STACKALLOCATOR_H__

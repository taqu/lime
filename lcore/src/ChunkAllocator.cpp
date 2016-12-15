/**
@file ChunkAllocator.cpp
@author t-sakai
@date 2013/09/19 create
*/
#include "ChunkAllocator.h"

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace lcore
{
namespace
{
    static u32 AllocSizeTable[ChunkAllocator::MaxNumBanks] =
    {
        128,
        192,
        256,
        320,
        384,
        448,
        512,
        576,
        640,
        704,
        768,
        832,
        896,
        960,
        1024,
    };

    lcore::uintptr_t calc_hash(void* ptr)
    {
        return lcore::hash_FNV1_64(static_cast<const u8*>(ptr), sizeof(void*));
    }
}

    //------------------------------------------------------
    ChunkAllocator::ChunkAllocator()
        :pageBank_(NULL)
        ,freePage_(NULL)
    {
        for(s32 i=0; i<MaxNumBanks; ++i){
            banks_[i].top_ = NULL;
            banks_[i].freeChunk_ = NULL;
        }
    }

    //------------------------------------------------------
    ChunkAllocator::~ChunkAllocator()
    {
        for(s32 i=0; i<MaxNumBanks; ++i){

            Page* page = banks_[i].top_;

            while(NULL != page){
                Page* next = page->next_;
                freemap(page->chunk_);
                page = next;
            }
        }

        while(NULL != freePage_){
            Page* next = freePage_->next_;
            freemap(freePage_->chunk_);
            freePage_ = next;
        }

        while(NULL != pageBank_){
            PageBank* next = pageBank_->next_;;
            freemap(pageBank_);
            pageBank_ = next;
        }
    }

    //------------------------------------------------------
    void* ChunkAllocator::allocate(u32 size)
    {
#ifdef LCORE_CHUNKALLOCATOR_CHECK
        size += sizeof(u32);
#endif
        LASSERT(0<size && size<=MaxSize);

        u32 bankIndex = calcBankIndex(size);
        u32 chunkSize = calcBankAllocSize(bankIndex);
        LASSERT(size<=chunkSize);

        //空きチャンクがあればそこから確保
        Bank& bank = banks_[bankIndex];
        if(NULL != bank.freeChunk_){
            Chunk* ret = bank.freeChunk_;
            bank.freeChunk_ = ret->next_;

#ifdef LCORE_CHUNKALLOCATOR_CHECK
            lcore::memset(ret, 0, chunkSize);
            ret = static_cast<Chunk*>(addCheck(ret));
#endif
            return ret;
        }

        Page* page = bank.top_;
        if(NULL == page || (page->maxChunks_<=page->numChunks_)){
            //ページ割り当て
            page = createPage(chunkSize);
            page->next_ = bank.top_;
            bank.top_ = page;
        }

        //チャンク割り当て
        Chunk* chunk = (Chunk*)((u8*)page->chunk_ + page->numChunks_ * chunkSize);
        ++page->numChunks_;

#ifdef LCORE_CHUNKALLOCATOR_CHECK
        lcore::memset(chunk, 0, chunkSize);
        chunk = static_cast<Chunk*>(addCheck(chunk));
#endif
        return chunk;
    }

    //------------------------------------------------------
    void ChunkAllocator::deallocate(void* ptr, u32 size)
    {
        LASSERT(0<size && size<=MaxSize);

#ifdef LCORE_CHUNKALLOCATOR_CHECK
        size += sizeof(uintptr_t);
        uintptr_t hashVal = calc_hash(ptr);

        ptr = ((u8*)ptr - sizeof(uintptr_t));
        uintptr_t* hash = (uintptr_t*)ptr;
        if(hashVal != *hash){
            lcore::Log("ChunkAllocator::deallocate invalid hash value: %d", size);
        }
#endif
        u32 bankIndex = calcBankIndex(size);
        LASSERT(size<=calcBankAllocSize(bankIndex));

        Bank& bank = banks_[bankIndex];

        Chunk* chunk = (Chunk*)ptr;
        chunk->next_ = bank.freeChunk_;
        bank.freeChunk_ = chunk;
    }

    //------------------------------------------------------
    // 空きページ回収
    void ChunkAllocator::collectEmptyPage()
    {
        for(s32 i=0; i<MaxNumBanks; ++i){

            Page* emptyPage = NULL;

            Chunk* chunk = banks_[i].freeChunk_;

            while(NULL != chunk){
                Chunk* nextChunk = chunk->next_;

                lcore::ptrdiff_t chunkPtr = (lcore::ptrdiff_t)chunk;

                Page* page = banks_[i].top_;
                Page* prev = page;
                while(NULL != page){
                    Page* nextPage = page->next_;

                    lcore::ptrdiff_t pagePtr = (lcore::ptrdiff_t)(page->chunk_);

                    if(pagePtr<=chunkPtr && chunkPtr < (pagePtr+PageSize)){
                        //使用チャンク数を減らす
                        LASSERT(0<page->numChunks_);
                        if(0<page->numChunks_){
                            --page->numChunks_;
                        }

                        //空ならば空ページリストへ
                        if(page->numChunks_<=0){
                            Page* empty = page;

                            if(page == banks_[i].top_){
                                banks_[i].top_ = page->next_;
                                //page = banks_[i].top_;
                            }else{
                                prev->next_ = page->next_;
                                //page = prev;
                            }

                            empty->next_ = emptyPage;
                            emptyPage = empty;
                        }
                        break;
                    }
                    prev = page;
                    page = nextPage;
                } //while(NULL != page)

                chunk = nextChunk;
            } //while(NULL != chunk)


            if(NULL != emptyPage){
                collectFreeChunks(banks_[i], emptyPage);
                while(NULL != emptyPage){
                    Page* nextPage = emptyPage->next_;

                    deallocPage(emptyPage);

                    emptyPage = nextPage;
                }
            }
        }
    }

    //------------------------------------------------------
    void ChunkAllocator::collectFreeChunks(Bank& bank, Page* emptyPage)
    {
        Chunk* chunk = bank.freeChunk_;
        Chunk* prev = chunk;

        while(NULL != chunk){
            Chunk* nextChunk = chunk->next_;

            lcore::ptrdiff_t chunkPtr = (lcore::ptrdiff_t)chunk;

            Page* page = emptyPage;
            while(NULL != page){
                Page* nextPage = page->next_;

                lcore::ptrdiff_t pagePtr = (lcore::ptrdiff_t)(page->chunk_);

                if(pagePtr<=chunkPtr && chunkPtr < (pagePtr+PageSize)){
                    //使用チャンク数は0
                    LASSERT(page->numChunks_ <= 0);
                    if(chunk == bank.freeChunk_){
                        bank.freeChunk_ = chunk->next_;
                        chunk = bank.freeChunk_;
                    }else{
                        prev->next_ = chunk->next_;
                        chunk = prev;
                    }
                    break;
                }
                page = nextPage;
            } //while(NULL != page)

            prev = chunk;
            chunk = nextChunk;
        } //while(NULL != chunk)
    }


    //------------------------------------------------------
    u32 ChunkAllocator::calcAllocSize(u32 size)
    {
        LASSERT(0<size && size<=MaxSize);
        u32 index = calcBankIndex(size);
        return AllocSizeTable[index];
    }

    //------------------------------------------------------
    u32 ChunkAllocator::calcBankIndex(u32 size)
    {
        LASSERT(0<size && size<=MaxSize);

        s32 mid = 7;

        s32 start = (size<AllocSizeTable[mid])? 0 : mid;
        s32 end = (size<AllocSizeTable[mid])? mid-1 : MaxNumBanks - 1;

        for(s32 i=start; i<=end; ++i){
            if(size<=AllocSizeTable[i]){
                return i;
            }
        }
        return end;
    }

    //------------------------------------------------------
    u32 ChunkAllocator::calcBankAllocSize(u32 index)
    {
        return AllocSizeTable[index];
    }

    //------------------------------------------------------
    void* ChunkAllocator::allocmmap(s32 size)
    {
        return VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    }

    //------------------------------------------------------
    void ChunkAllocator::freemap(void* ptr)
    {
        VirtualFree(ptr, 0, MEM_RELEASE);
    }

    //------------------------------------------------------
    ChunkAllocator::Page* ChunkAllocator::createPage(u32 chunkSize)
    {
        Page* page;

        if(NULL != freePage_){
            page = freePage_;
            freePage_ = freePage_->next_;
        }else{
            page = getPage();
            page->chunk_ = (Chunk*)allocmmap(PageSize);
        }

        page->next_ = NULL;
        page->maxChunks_ = PageSize/chunkSize;
        page->numChunks_ = 0;
        return page;
    }

    //------------------------------------------------------
    void* ChunkAllocator::addCheck(void* ptr)
    {
        uintptr_t* hash = (uintptr_t*)ptr;
        ptr = ((u8*)ptr + sizeof(uintptr_t));
        *hash = calc_hash(ptr);
        return ptr;
    }

    //------------------------------------------------------
    ChunkAllocator::Page* ChunkAllocator::getPage()
    {
        if(NULL == pageBank_ || pageBank_->maxPages_<=pageBank_->numPages_){
            PageBank* pageBank = createPageBank();
            pageBank->next_ = pageBank_;
            pageBank_ = pageBank;
        }

        Page* page = (Page*)((u8*)pageBank_ + sizeof(Page)*(pageBank_->numPages_+1));
        ++pageBank_->numPages_;
        return page;
    }

    //------------------------------------------------------
    ChunkAllocator::PageBank* ChunkAllocator::createPageBank()
    {
        PageBank* pageBank = (PageBank*)allocmmap(PageBankSize);
        pageBank->next_ = NULL;
        pageBank->maxPages_ = PageBankSize/sizeof(Page) - 1;
        pageBank->numPages_ = 0;
        return pageBank;
    }

    //------------------------------------------------------
    void ChunkAllocator::deallocPage(Page* page)
    {
#ifdef LCORE_CHUNKALLOCATOR_CHECK
        lcore::memset(page->chunk_, 0, PageSize);
#endif
        page->next_ = freePage_;
        freePage_ = page;
    }
}

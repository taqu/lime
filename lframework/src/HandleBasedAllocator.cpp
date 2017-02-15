/**
@file HandleBasedAllocator.cpp
@author t-sakai
@date 2016/08/12 create
*/
#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <xmmintrin.h>
#include <emmintrin.h>

#include "HandleBasedAllocator.h"

namespace lfw
{
    HandleBasedAllocator::HandleBasedAllocator(u32 pageSize)
        :reservedSize_(0)
        ,pageSize_(pageSize)
        ,capacityFreeAlloc_(0)
        ,freeAllocs_(NULL)
        ,size_(0)
        ,block_(NULL)
    {
        for(s32 i=0; i<TableSize; ++i){
            table_[i].next_ = Invalid;
        }

        SYSTEM_INFO systemInfo = {0};
        GetSystemInfo(&systemInfo);

        pageSize_ = lcore::clamp(pageSize_, static_cast<u32>(systemInfo.dwPageSize), static_cast<u32>(systemInfo.dwAllocationGranularity));
        allocationGranularity_ = systemInfo.dwAllocationGranularity;
    }

    HandleBasedAllocator::~HandleBasedAllocator()
    {
        for(s32 i=0; i<capacityFreeAlloc_; ++i){
            LFREE(freeAllocs_[i]);
        }
        LFREE(freeAllocs_);
        freemmap();
    }

    HandleBasedAllocator::Handle HandleBasedAllocator::allocate(s32 size)
    {
        Handle handle;
        if(size<=0){
            return handle;
        }

#ifdef LFRAMEWORK_HANDLEBAEDALLOCATOR_DEBUG
        size += 4;
#endif
        if(MaxSize<size){
            handle.size_ = size;
            for(s32 i=0; i<capacityFreeAlloc_; ++i){
                if(NULL == freeAllocs_[i]){
                    freeAllocs_[i] = LMALLOC(size);
                    handle.offset_ = i;
#ifdef LFRAMEWORK_HANDLEBAEDALLOCATOR_DEBUG
                    handle.size_ -= 4;
                    setDebug(handle.size(), handle.offset());
#endif
                    return handle;
                }
            }
            handle.offset_ = capacityFreeAlloc_;
            void** freeAllocs = (void**)LMALLOC(sizeof(void*)*(capacityFreeAlloc_+NumFreeAlloc));
            lcore::memcpy(freeAllocs, freeAllocs_, sizeof(void*)*capacityFreeAlloc_);
            lcore::memset(freeAllocs+capacityFreeAlloc_, NULL, sizeof(void*)*NumFreeAlloc);
            LFREE(freeAllocs_);
            freeAllocs_ = freeAllocs;
            freeAllocs_[capacityFreeAlloc_] = LMALLOC(size);
            capacityFreeAlloc_ += NumFreeAlloc;
#ifdef LFRAMEWORK_HANDLEBAEDALLOCATOR_DEBUG
            handle.size_ -= 4;
            setDebug(handle.size(), handle.offset());
#endif
            return handle;
        }
        size = (size+MinMask) & ~MinMask;

        s32 index = static_cast<s32>(size>>MinShift) - 1;

        for(;;){
            s32 tmp = size;
            for(s32 i=index; i<TableSize; ++i){
                if(0<=table_[i].next_){
                    s32 offset = table_[i].next_;
                    handle.size_ = size;
                    handle.offset_ = offset;
                    table_[i].next_ = getLink(offset)->next_;
                    if(size == tmp){
#ifdef LFRAMEWORK_HANDLEBAEDALLOCATOR_DEBUG
                        handle.size_ -= 4;
                        setDebug(handle.size(), handle.offset());
#endif
                        return handle;
                    }
                    s32 diff = tmp-size;
                    LASSERT(MinSize<=diff);
                    offset += size;
                    size = diff;
                    index = (size>>MinShift) - 1;
                    link(index, getLink(offset));
#ifdef LFRAMEWORK_HANDLEBAEDALLOCATOR_DEBUG
                    handle.size_ -= 4;
                    setDebug(handle.size(), handle.offset());
#endif
                    return handle;
                }
                tmp += MinSize;
            }
            expand();
        }
    }

    void HandleBasedAllocator::deallocate(s32 size, s32 offset)
    {
        if(offset<0){
            return;
        }
#ifdef LFRAMEWORK_HANDLEBAEDALLOCATOR_DEBUG
        LASSERT(checkDebug(size, offset));
        size += 4;
#endif
        if(MaxSize<size){
            LASSERT(0<=offset && offset<capacityFreeAlloc_);
            LFREE(freeAllocs_[offset]);
            return;
        }

        s32 index = (size >> MinShift) - 1;
        link(index, getLink(offset));
    }

    void HandleBasedAllocator::gatherFragments()
    {
        u32 size0 = MinSize;
        for(s32 i=0; i<(TableSize>>1); ++i){
            Link* prev0 = &table_[i];
            Link* current0 = getLinkMaybeNull(table_[i].next_);
            while(NULL != current0){
                Link* prev1 = current0;
                Link* current1 = getLinkMaybeNull(current0->next_);
                u32 diff = 0;
                while(NULL != current1){
                    diff = getDiff(current1, current0);
                    if(diff == size0){
                        break;
                    }
                    prev1 = current1;
                    current1 = getLinkMaybeNull(current1->next_);
                }
                if(NULL != current1 && diff == size0){
                    LASSERT((size0+size0)<=MaxSize);
                    prev1->next_ = current1->next_;
                    prev0->next_ = current0->next_;

                    s32 index = ((size0<<1)>>MinShift) - 1;
                    link(index, current0);
                    current1->next_ = 0;
                    current0 = getLinkMaybeNull(prev0->next_);
                    continue;
                }

                diff = 0;
                u32 size1 = size0 + MinSize;
                for(s32 j=i+1; j<(TableSize-i-1); ++j){
                    prev1 = &table_[j];
                    current1 = getLinkMaybeNull(table_[j].next_);

                    while(NULL != current1){
                        diff = getDiff(current1, current0);
                        if(diff == size0){
                            break;
                        }
                        prev1 = current1;
                        current1 = getLinkMaybeNull(current1->next_);
                    }

                    if(diff == size0){
                        break;
                    }
                    size1 += MinSize;
                }

                if(NULL != current1 && diff == size0){
                    LASSERT((size0+size1)<=MaxSize);
                    prev1->next_ = current1->next_;
                    prev0->next_ = current0->next_;

                    s32 index = ((size0+size1)>>MinShift) - 1;
                    link(index, current0);
                    current1->next_ = 0;
                    current0 = getLinkMaybeNull(prev0->next_);
                    continue;
                }

                prev0 = current0;
                current0 = getLinkMaybeNull(current0->next_);
            }
            size0 += MinSize;
        }
    }

    //------------------------------------------------------
    s32 HandleBasedAllocator::countTable(s32 size) const
    {
        if(size<=0){
            return 0;
        }

        s32 count = 0;
        if(MaxSize<size){
            for(s32 i=0; i<capacityFreeAlloc_; ++i){
                if(NULL != freeAllocs_[i]){
                    ++count;
                }
            }
            return count;
        }

        s32 index = (size>>MinShift) - 1;
        const Link* link = getLinkMaybeNull(table_[index].next_);
        while(NULL != link){
            ++count;
            link = getLinkMaybeNull(link->next_);
        }
        return count;
    }

    //------------------------------------------------------
    inline void HandleBasedAllocator::reservemmap()
    {
        u32 reservedSize = reservedSize_ + allocationGranularity_;
        u8* newBlock = (u8*)VirtualAlloc(NULL, reservedSize, MEM_RESERVE, PAGE_READWRITE);
        if(0<reservedSize_){
            VirtualAlloc(newBlock, reservedSize_, MEM_COMMIT, PAGE_READWRITE);
            for(u32 i=0; i<reservedSize_; i+=16){
                __m128 m = _mm_load_ps(reinterpret_cast<const f32*>(block_+i));
                _mm_store_ps(reinterpret_cast<f32*>(newBlock+i), m);
            }
            VirtualFree(block_, 0, MEM_RELEASE);
        }
        reservedSize_ = reservedSize;
        block_ = newBlock;
    }

    //------------------------------------------------------
    inline void HandleBasedAllocator::commitmmap()
    {
        if(reservedSize_<=size_){
            reservemmap();
        }
        VirtualAlloc(block_+size_, pageSize_, MEM_COMMIT, PAGE_READWRITE);
        size_ += pageSize_;
    }

    //------------------------------------------------------
    inline void HandleBasedAllocator::freemmap()
    {
        if(NULL == block_){
            return;
        }
        VirtualFree(block_, 0, MEM_RELEASE);

        reservedSize_ = 0;
        size_ = 0;
        block_ = NULL;
    }

    //------------------------------------------------------
    void HandleBasedAllocator::expand()
    {
        LASSERT(size_<=0xFFFFFFFFU);
        LASSERT(-1 == table_[TableSize-1].next_);
        u32 offset = size_;
        commitmmap();
        u32 num = pageSize_>>MaxShift;
        table_[TableSize-1].next_ = static_cast<s32>(offset);
        for(u32 i=1; i<num; ++i){
            s32 next = offset + MaxSize;
            reinterpret_cast<Link*>(block_+offset)->next_ = next;
            offset = next;
        }
        reinterpret_cast<Link*>(block_+offset)->next_ = -1;
    }

    //------------------------------------------------------
    void HandleBasedAllocator::link(s32 index, Link* link)
    {
        Link* prev = &table_[index];
        Link* current = getLinkMaybeNull(table_[index].next_);
        while(NULL != current){
            if(link<current){
                break;
            }
            prev = current;
            current = getLinkMaybeNull(current->next_);
        }
        link->next_ = prev->next_;
        prev->next_ = static_cast<s32>(reinterpret_cast<u8*>(link)-block_);
    }

    //------------------------------------------------------
    void HandleBasedAllocator::setDebug(s32 size, s32 offset)
    {
        LASSERT(0<=size);
        LASSERT(0<=offset);
        s32* ptr = (size<=MaxSize)
            ? reinterpret_cast<s32*>(block_ + offset)
            : static_cast<s32*>(freeAllocs_[offset]);
        *ptr = size;
    }

    //------------------------------------------------------
    bool HandleBasedAllocator::checkDebug(s32 size, s32 offset) const
    {
        LASSERT(0<=size);
        LASSERT(0<=offset);
        const s32* ptr = (size<=MaxSize)
            ? reinterpret_cast<const s32*>(block_ + offset)
            : static_cast<const s32*>(freeAllocs_[offset]);
        if(NULL == ptr){
            return false;
        }
        return (*ptr == size);
    }
}

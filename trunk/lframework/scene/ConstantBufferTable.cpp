/**
@file ConstantBufferTable.cpp
@author t-sakai
@date 2014/10/09 create
*/
#include <Windows.h>
#include "ConstantBufferTable.h"
#include <lcore/clibrary.h>

namespace lscene
{
namespace
{
    u16 mostSignificantBit(u16 v)
    {
        static const u8 shifttable[] =
        {
            0, 1, 2, 2, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4,
        };
        u16 ret = 0;

        if(v & 0xFF00U){
            ret += 8;
            v >>= 8;
        }

        if(v & 0xF0U){
            ret += 4;
            v >>= 4;
        }
        return ret + shifttable[v];
    }
}

    ConstantBufferTable::ConstantBufferTable()
        :entryAllocator_(PageSize, sizeof(Entry), 4)
    {
    }

    ConstantBufferTable::~ConstantBufferTable()
    {
    }

    void ConstantBufferTable::initialize()
    {
        lcore::memset(entries_, 0, sizeof(TableEntry)*TableSize);
        lcore::memset(largeEntries_, 0, sizeof(TableEntry)*LargeTableSize);
    }

    void ConstantBufferTable::terminate()
    {
        clearCache();
    }

    s32 ConstantBufferTable::calcIndex(u32 size)
    {
        static const u32 table[5]{64, 128, 256, 512, 1024};
        for(s32 i=0; i<5; ++i){
            if(size<=table[i]){
                return i;
            }
        }
        return 5;
    }

    lgraphics::ConstantBufferRef* ConstantBufferTable::allocate(u32 size)
    {
        LASSERT(0<size && size<=MaxLargeBufferSize);
        if(MinLargeBufferSize<=size){
            return allocateLarge(size);
        }

        u16 index = calcIndex(size);
        LASSERT(0<=index && index<TableSize);

        TableEntry& tableEntry = entries_[index];
        Entry* entry;
        if(NULL != tableEntry.top_){
            entry = tableEntry.top_;
            tableEntry.top_ = entry->next_;
        }else{

            entry = LIME_PLACEMENT_NEW( entryAllocator_.allocate() ) Entry();

            u32 allocSize = 0x01U<<(index+AllocBitShift);
            LASSERT(size<=allocSize);

            entry->buffer_ = lgraphics::ConstantBuffer::create(
                allocSize,
                lgraphics::Usage_Dynamic,
                lgraphics::CPUAccessFlag_Write,
                lgraphics::ResourceMisc_None);
        }

        entry->next_ = tableEntry.used_;
        tableEntry.used_ = entry;
        return &entry->buffer_;
    }

    lgraphics::ConstantBufferRef* ConstantBufferTable::allocateLarge(u32 size)
    {
        u32 s = (size + LargeAllocBitMask)>>LargeAllocBitShift;
        u32 index = s - 2;
        LASSERT(0<=index && index<LargeTableSize);

        TableEntry& tableEntry = largeEntries_[index];
        Entry* entry;
        if(NULL != tableEntry.top_){
            entry = tableEntry.top_;
            tableEntry.top_ = entry->next_;
        }else{

            entry = LIME_PLACEMENT_NEW( entryAllocator_.allocate() ) Entry();

            u32 allocSize = s<<LargeAllocBitShift;
            LASSERT(size<=allocSize);

            entry->buffer_ = lgraphics::ConstantBuffer::create(
                allocSize,
                lgraphics::Usage_Dynamic,
                lgraphics::CPUAccessFlag_Write,
                lgraphics::ResourceMisc_None);
        }

        entry->next_ = tableEntry.used_;
        tableEntry.used_ = entry;
        return &entry->buffer_;
    }

    void ConstantBufferTable::clearUsed(TableEntry& tableEntry)
    {
        if(NULL == tableEntry.top_){
            tableEntry.top_ = tableEntry.used_;
            tableEntry.used_ = NULL;
        }else{
            Entry* current = tableEntry.top_;
            while(NULL != current->next_){
                current = current->next_;
            }
            current->next_ = tableEntry.used_;
            tableEntry.used_ = NULL;
        }
    }

    void ConstantBufferTable::clearUsed()
    {
        for(s32 i=0; i<TableSize; ++i){
            clearUsed(entries_[i]);
        }

        for(s32 i=0; i<LargeTableSize; ++i){
            clearUsed(largeEntries_[i]);
        }
    }

    void ConstantBufferTable::clearCache(Entry*& top)
    {
        Entry* entry = top;
        while(NULL != entry){
            Entry* next = entry->next_;
            entry->~Entry();
            entry = next;
        }
        top = NULL;
    }

    void ConstantBufferTable::clearCache()
    {
        for(s32 i=0; i<TableSize; ++i){
            TableEntry& tableEntry = entries_[i];
            clearCache(tableEntry.used_);
            clearCache(tableEntry.top_);
        }

        for(s32 i=0; i<LargeTableSize; ++i){
            TableEntry& tableEntry = largeEntries_[i];
            clearCache(tableEntry.used_);
            clearCache(tableEntry.top_);
        }

        entryAllocator_.clear();
    }

    void ConstantBufferTable::swap(ConstantBufferTable& rhs)
    {
        for(s32 i=0; i<TableSize; ++i){
            lcore::swap(entries_[i], rhs.entries_[i]);
        }

        for(s32 i=0; i<LargeTableSize; ++i){
            lcore::swap(largeEntries_[i], rhs.largeEntries_[i]);
        }
        entryAllocator_.swap(rhs.entryAllocator_);
    }
}

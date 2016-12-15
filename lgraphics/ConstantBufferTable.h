#ifndef INC_LGRAPHICS_CONSTANTBUFFERTABLE_H__
#define INC_LGRAPHICS_CONSTANTBUFFERTABLE_H__
/**
@file ConstantBufferTable.h
@author t-sakai
@date 2016/11/07 create
*/
#include "lgraphics.h"
#include <lcore/StackAllocator.h>
#include "ConstantBufferRef.h"

namespace lgfx
{
    class ConstantBufferTable
    {
    public:
        typedef lcore::StackAllocator<lcore::DefaultAllocator> EntryAllocator;

        static const u32 PageSize = 4*1024;

        static const u32 MaxNumConstants = 4096/4;
        static const u32 ConstantSize = 4*4;
        static const u32 MinLargeBufferSize = 1024+1;
        static const u32 MaxLargeBufferSize = 24*1024;

        static const u32 AllocSize = 4*4*4;
        static const s32 AllocBitShift = 6;
        static const u32 AllocBitMask = (0x01U<<AllocBitShift) - 1;

        static const s32 LargeAllocBitShift = 10;
        static const u32 LargeAllocBitMask = (0x01U<<LargeAllocBitShift) - 1;

        static const s32 TableSize = 5;
        static const s32 LargeTableSize = 23;

        ConstantBufferTable();
        ~ConstantBufferTable();

        void initialize();
        void terminate();

        lgfx::ConstantBufferRef* allocate(u32 size);

        void clearUsed();
        void clearCache();

        void swap(ConstantBufferTable& rhs);
    private:
        ConstantBufferTable(const ConstantBufferTable&);
        ConstantBufferTable& operator=(const ConstantBufferTable&);

        s32 calcIndex(u32 size);

        lgfx::ConstantBufferRef* allocateLarge(u32 size);

        struct Entry
        {
            Entry* next_;
            lgfx::ConstantBufferRef buffer_;
        };

        struct TableEntry
        {
            Entry* top_;
            Entry* used_;
        };

        void clearUsed(TableEntry& tableEntry);
        void clearCache(Entry*& top);

        TableEntry entries_[TableSize];
        TableEntry largeEntries_[LargeTableSize];
        EntryAllocator entryAllocator_;
    };
}
#endif //INC_LGRAPHICS_CONSTANTBUFFERTABLE_H__

#ifndef INC_LGRAPHICS_CONSTANTBUFFERTABLESET_H__
#define INC_LGRAPHICS_CONSTANTBUFFERTABLESET_H__
/**
@file ConstantBufferTableSet.h
@author t-sakai
@date 2016/11/07 create
*/
#include "lgraphics.h"
#include <lcore/SyncObject.h>
#include "ConstantBufferTable.h"

namespace lgfx
{
    class FrameSyncQuery;
    struct MappedSubresource;

    //--------------------------------------------------
    //---
    //--- ConstantBuffer
    //---
    //--------------------------------------------------
    class ConstantBufferTableSet
    {
    public:
        ConstantBufferTableSet();
        ~ConstantBufferTableSet();

        void initialize(FrameSyncQuery* frameSyncQuery, s32 numEntries);
        void terminate();
        void begin();

        void clear();
        lgfx::ConstantBufferRef* allocate(u32 size);

    private:
        ConstantBufferTableSet(const ConstantBufferTableSet&);
        ConstantBufferTableSet& operator=(const ConstantBufferTableSet&);

        struct Entry
        {
            s32 event_;
            ConstantBufferTable table_;
        };

        lcore::CriticalSection cs_;

        FrameSyncQuery* frameSyncQuery_;
        s32 numEntries_;
        s32 current_;
        Entry* entries_;
    };
}
#endif //INC_LGRAPHICS_CONSTANTBUFFERTABLESET_H__

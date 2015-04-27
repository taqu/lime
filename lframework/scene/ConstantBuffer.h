#ifndef INC_LSCENE_CONSTANTBUFFER_H__
#define INC_LSCENE_CONSTANTBUFFER_H__
/**
@file ConstantBuffer.h
@author t-sakai
@date 2014/10/22 create
*/
#include "lscene.h"
#include <lcore/async/SyncObject.h>
#include "ConstantBufferTable.h"

namespace lgraphics
{
    struct MappedSubresource;
}

namespace lscene
{
    //--------------------------------------------------
    //---
    //--- ConstantBuffer
    //---
    //--------------------------------------------------
    class ConstantBuffer
    {
    public:
        ConstantBuffer();
        ~ConstantBuffer();

        void initialize(s32 numEntries);
        void terminate();
        void begin();

        void clear();
        lgraphics::ConstantBufferRef* allocate(u32 size);

    private:
        struct Entry
        {
            s32 event_;
            ConstantBufferTable table_;
        };

        lcore::CriticalSection cs_;

        s32 current_;
        s32 numEntries_;
        Entry* entries_;
    };
}
#endif //INC_LSCENE_CONSTANTBUFFER_H__

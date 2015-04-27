#ifndef INC_LSCENE_RINGBUFFER_H__
#define INC_LSCENE_RINGBUFFER_H__
/**
@file RingBuffer.h
@author t-sakai
@date 2014/10/12 create
*/
#include "lscene.h"
#include <lgraphics/api/QueryRef.h>
namespace lgraphics
{
    class ContextRef;
}

namespace lscene
{
    //--------------------------------------------------
    //---
    //--- RingBuffer
    //---
    //--------------------------------------------------
    class RingBuffer
    {
    public:
        struct Resource
        {
            Resource(s32 offset, s32 size)
                :offsetInBytes_(offset)
                ,sizeInBytes_(size)
            {}

            s32 offsetInBytes_;
            s32 sizeInBytes_;
        };

        RingBuffer();
        ~RingBuffer();

        void initialize(s32 totalSize);
        void terminate();

        void begin();
        Resource allocate(s32 size);
        void end();

    private:
        RingBuffer(const RingBuffer&);
        RingBuffer& operator=(const RingBuffer&);

        static const s16 Flag_None = 0;
        static const s16 Flag_Alloc = 1;

        struct Entry
        {
            s16 flag_;
            s16 event_;
            s32 end_;
        };

        s32 calcReserve() const;

        s32 totalSize_;

        s32 start_;
        s32 end_;
        s32 countFrame_;
        Entry entries_[NumSyncFrames];
    };
}
#endif //INC_LSCENE_RINGBUFFER_H__

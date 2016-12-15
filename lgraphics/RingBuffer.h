#ifndef INC_LGRAPHICS_RINGBUFFER_H__
#define INC_LGRAPHICS_RINGBUFFER_H__
/**
@file RingBuffer.h
@author t-sakai
@date 2016/11/07 create
*/
#include "lgraphics.h"
#include "QueryRef.h"

namespace lgfx
{
    class ContextRef;
    class FrameSyncQuery;

    //--------------------------------------------------
    //---
    //--- RingBuffer
    //---
    //--------------------------------------------------
    class RingBuffer
    {
    public:
        static const s32 MaxSyncFrames = 8;

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

        void initialize(FrameSyncQuery* frameSyncQuery, s32 totalSize, s32 syncFrames);
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

        inline s32 prevFrame(s32 frame) const
        {
            --frame;
            return (frame<0)? syncFrames_ : frame;
        }

        inline s32 nextFrame(s32 frame) const
        {
            ++frame;
            return (syncFrames_<=frame)? 0 : frame;
        }

        s32 calcReserve() const;

        FrameSyncQuery* frameSyncQuery_;
        s32 totalSize_;
        s32 syncFrames_;
        s32 start_;
        s32 end_;
        s32 countFrame_;
        Entry entries_[MaxSyncFrames];
    };
}
#endif //INC_LGRAPHICS_RINGBUFFER_H__

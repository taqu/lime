#ifndef INC_LGRAPHICS_NBUFFER_H__
#define INC_LGRAPHICS_NBUFFER_H__
/**
@file NBuffer.h
@author t-sakai
@date 2017/02/21 create
*/
#include "lgraphics.h"
#include "QueryRef.h"

namespace lgfx
{
    class ContextRef;
    class FrameSyncQuery;

    //--------------------------------------------------
    //---
    //--- NBuffer
    //---
    //--------------------------------------------------
    class NBuffer
    {
    public:
        static const s32 MaxSyncFrames = 8;

        struct Resource
        {
            s32 offsetInBytes_;
            s32 sizeInBytes_;
        };

        NBuffer();
        ~NBuffer();

        void initialize(FrameSyncQuery* frameSyncQuery, s32 totalSize, s32 syncFrames);
        void terminate();

        void begin();
        Resource allocate(s32 size);
        void end();

    private:
        NBuffer(const NBuffer&);
        NBuffer& operator=(const NBuffer&);

        static const s16 Flag_None = 0;
        static const s16 Flag_Alloc = 1;

        struct Entry
        {
            s16 flag_;
            s16 event_;
        };

        inline s32 nextFrame(s32 frame) const
        {
            ++frame;
            return (syncFrames_<=frame)? 0 : frame;
        }

        FrameSyncQuery* frameSyncQuery_;
        s32 size_;
        s32 syncFrames_;
        s32 start_;
        s32 end_;
        s32 countFrame_;
        Entry entries_[MaxSyncFrames];
    };
}
#endif //INC_LGRAPHICS_NBUFFER_H__

#ifndef INC_LGRAPHICS_FRAMESYNCQUERY_H__
#define INC_LGRAPHICS_FRAMESYNCQUERY_H__
/**
@file FrameSyncQuery.h
@author t-sakai
@date 2016/11/07 create
*/
#include "lgraphics.h"
#include "QueryRef.h"
#include "GraphicsDeviceRef.h"

namespace lgfx
{
    class FrameSyncQuery
    {
    public:
        static const s32 FrameSyncFlag_None = 0;
        static const s32 FrameSyncFlag_InGPUUse = 1;

        static const s32 DefaultSyncFrames = 3;
        static const s32 MaxFrames = 8;

        static const s32 BusyLoop = 64;

        FrameSyncQuery();
        ~FrameSyncQuery();

        void initialize(s32 frames);
        void terminate();

        void begin(lgfx::ContextRef& context);
        void end(lgfx::ContextRef& context);

        s32 getCurrentFrame() const{ return current_;}
        bool checkInGPUUse(s32 frame) const{ return events_[frame].flag_ == FrameSyncFlag_InGPUUse;}

    private:
        FrameSyncQuery(const FrameSyncQuery&);
        FrameSyncQuery& operator=(const FrameSyncQuery&);

        struct EventQuery
        {
            s32 flag_;
            lgfx::QueryRef event_;
        };

        s32 frames_;
        s32 current_;
        EventQuery events_[MaxFrames];
    };
}
#endif //INC_LGRAPHICS_FRAMESYNCQUERY_H__

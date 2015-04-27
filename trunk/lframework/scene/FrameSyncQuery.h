#ifndef INC_LSCENE_FRAMESYNCQUERY_H__
#define INC_LSCENE_FRAMESYNCQUERY_H__
/**
@file FrameSyncQuery.h
@author t-sakai
@date 2014/10/21 create
*/
#include "lscene.h"
#include <lgraphics/api/QueryRef.h>
#include <lgraphics/api/GraphicsDeviceRef.h>

namespace lscene
{
    template<s32 N>
    class FrameSyncQuery
    {
    public:
        static const s32 MaxFrames = N;
        static const s32 FrameMask = MaxFrames-1;

        FrameSyncQuery();
        ~FrameSyncQuery();

        void initialize();
        void terminate();

        void begin(lgraphics::ContextRef& context);
        void end(lgraphics::ContextRef& context);

        s32 getCurrentFrame() const{ return current_;}
        bool checkInGPUUse(s32 frame) const{ return events_[frame].flag_ == FrameSyncFlag_InGPUUse;}

    private:
        struct EventQuery
        {
            s32 flag_;
            lgraphics::QueryRef event_;
        };

        s32 current_;
        EventQuery events_[MaxFrames];
    };

    template<s32 N>
    FrameSyncQuery<N>::FrameSyncQuery()
        :current_(0)
    {
    }

    template<s32 N>
    FrameSyncQuery<N>::~FrameSyncQuery()
    {
    }

    template<s32 N>
    void FrameSyncQuery<N>::initialize()
    {
        current_ = 0;
        for(s32 i=0; i<MaxFrames; ++i){
            events_[i].flag_ = FrameSyncFlag_None;
            events_[i].event_ = lgraphics::Query::create(lgraphics::QueryType_Event, lgraphics::QueryMiscFlag_None);
        }
    }

    template<s32 N>
    void FrameSyncQuery<N>::terminate()
    {
        for(s32 i=0; i<MaxFrames; ++i){
            events_[i].flag_ = FrameSyncFlag_None;
            events_[i].event_.destroy();
        }
    }

    template<s32 N>
    void FrameSyncQuery<N>::begin(lgraphics::ContextRef& context)
    {
        for(s32 i=0; i<MaxFrames; ++i){
            if(events_[i].flag_ == FrameSyncFlag_None){
                continue;
            }

            if(S_OK == context.getData(events_[i].event_, NULL, 0, lgraphics::AsyncGetDataFlag_DoNotFlush)){
                events_[i].flag_ = FrameSyncFlag_None;
            }
        }

        if(FrameSyncFlag_InGPUUse == events_[current_].flag_){
            s32 count = 0;
            while(S_OK != context.getData(events_[current_].event_, NULL, 0, 0)){
                if(100<++count){
                    break;
                }
            }
            events_[current_].flag_ = FrameSyncFlag_None;
        }
    }

    template<s32 N>
    void FrameSyncQuery<N>::end(lgraphics::ContextRef& context)
    {
        context.end(events_[current_].event_);
        events_[current_].flag_ = FrameSyncFlag_InGPUUse;
        current_ = (current_+1)&FrameMask;
    }
}
#endif //INC_LSCENE_FRAMESYNCQUERY_H__

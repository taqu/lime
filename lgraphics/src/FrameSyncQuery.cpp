/**
@file FrameSyncQuery.cpp
@author t-sakai
@date 2016/11/07 create
*/
#include "FrameSyncQuery.h"

namespace lgfx
{
    FrameSyncQuery::FrameSyncQuery()
        :frames_(0)
        ,current_(0)
    {
    }

    FrameSyncQuery::~FrameSyncQuery()
    {
    }

    void FrameSyncQuery::initialize(s32 frames)
    {
        LASSERT(0<=frames && frames<MaxFrames);
        frames_ = frames;
        current_ = 0;
        for(s32 i=0; i<frames_; ++i){
            events_[i].flag_ = FrameSyncFlag_None;
            events_[i].event_ = lgfx::Query::create(lgfx::QueryType_Event, lgfx::QueryMiscFlag_None);
        }
    }

    void FrameSyncQuery::terminate()
    {
        for(s32 i=0; i<frames_; ++i){
            events_[i].flag_ = FrameSyncFlag_None;
            events_[i].event_.destroy();
        }
    }

    void FrameSyncQuery::begin(lgfx::ContextRef& context)
    {
        for(s32 i=0; i<frames_; ++i){
            if(events_[i].flag_ == FrameSyncFlag_None){
                continue;
            }

            if(S_OK == context.getData(events_[i].event_, NULL, 0, lgfx::AsyncGetDataFlag_DoNotFlush)){
                events_[i].flag_ = FrameSyncFlag_None;
            }
        }

        if(FrameSyncFlag_InGPUUse == events_[current_].flag_){
            s32 count = 0;
            while(S_OK != context.getData(events_[current_].event_, NULL, 0, 0)){
                if(BusyLoop<++count){
                    break;
                }
            }
            events_[current_].flag_ = FrameSyncFlag_None;
        }
    }

    void FrameSyncQuery::end(lgfx::ContextRef& context)
    {
        context.end(events_[current_].event_);
        events_[current_].flag_ = FrameSyncFlag_InGPUUse;
        ++current_;
        current_ = (frames_<=current_)? 0 : current_;
    }
}

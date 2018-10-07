/**
@file NBuffer.cpp
@author t-sakai
@date 2017/02/21 create
*/
#include "NBuffer.h"
#include "FrameSyncQuery.h"

namespace lgfx
{
    //--------------------------------------------------
    //---
    //--- NBuffer
    //---
    //--------------------------------------------------
    NBuffer::NBuffer()
        :frameSyncQuery_(NULL)
        ,size_(0)
        ,syncFrames_(0)
        ,start_(0)
        ,end_(0)
        ,countFrame_(0)
    {
    }

    NBuffer::~NBuffer()
    {
    }

    void NBuffer::initialize(FrameSyncQuery* frameSyncQuery, s32 totalSize, s32 syncFrames)
    {
        LASSERT(NULL != frameSyncQuery);
        LASSERT(0<totalSize);
        LASSERT(0<syncFrames);

        frameSyncQuery_ = frameSyncQuery;
        size_ = totalSize/syncFrames;
        syncFrames_ = lcore::minimum(MaxSyncFrames, syncFrames);
        start_ = 0;
        end_ = totalSize;
        countFrame_ = 0;
        for(s32 i=0; i<syncFrames_; ++i){
            entries_[i].flag_ = Flag_None;
            entries_[i].event_ = 0;
        }
    }

    void NBuffer::terminate()
    {
        for(s32 i=0; i<syncFrames_; ++i){
            entries_[i].flag_ = Flag_None;
            entries_[i].event_ = 0;
        }
        frameSyncQuery_ = NULL;
    }

    void NBuffer::begin()
    {
        s32 frame = countFrame_;
        do{
            if(entries_[frame].flag_ || false == frameSyncQuery_->checkInGPUUse(entries_[frame].event_)){
                entries_[frame].flag_ = Flag_None;
                countFrame_ = frame;
                start_ = size_*countFrame_;
                end_ = start_ + size_;
                return;
            }
            frame = nextFrame(frame);
        }while(frame != countFrame_);
        start_ = end_ = 0;
    }

    NBuffer::Resource NBuffer::allocate(s32 size)
    {
        Resource resource = {start_, 0};
        s32 e = start_ + size;
        if(start_<=end_){
            if(end_<=e){
                resource.sizeInBytes_ = end_ - start_;
                start_ = end_;
            }else{
                resource.sizeInBytes_ = size;
                start_ = e;
            }
            return resource;
        }
        return resource;
    }

    void NBuffer::end()
    {
        entries_[countFrame_].flag_ = Flag_Alloc;
        entries_[countFrame_].event_ = static_cast<s16>(frameSyncQuery_->getCurrentFrame());
        countFrame_ = nextFrame(countFrame_);
    }
}

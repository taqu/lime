/**
@file RingBuffer.cpp
@author t-sakai
@date 2016/11/07 create
*/
#include "RingBuffer.h"
#include "FrameSyncQuery.h"

namespace lgfx
{
    //--------------------------------------------------
    //---
    //--- RingBuffer
    //---
    //--------------------------------------------------
    RingBuffer::RingBuffer()
        :frameSyncQuery_(NULL)
        ,totalSize_(0)
        ,syncFrames_(0)
        ,start_(0)
        ,end_(0)
        ,countFrame_(0)
    {
    }

    RingBuffer::~RingBuffer()
    {
    }

    void RingBuffer::initialize(FrameSyncQuery* frameSyncQuery, s32 totalSize, s32 syncFrames)
    {
        LASSERT(NULL != frameSyncQuery);
        LASSERT(0<totalSize);
        LASSERT(0<syncFrames);

        frameSyncQuery_ = frameSyncQuery;
        totalSize_ = totalSize;
        syncFrames_ = lcore::minimum(MaxSyncFrames, syncFrames);
        start_ = 0;
        end_ = totalSize;
        countFrame_ = 0;
        for(s32 i=0; i<syncFrames_; ++i){
            entries_[i].flag_ = Flag_None;
            entries_[i].event_ = 0;
            entries_[i].end_ = totalSize_;
        }
    }

    void RingBuffer::terminate()
    {
        for(s32 i=0; i<syncFrames_; ++i){
            entries_[i].flag_ = Flag_None;
            entries_[i].event_ = 0;
            entries_[i].end_ = totalSize_;
        }
        frameSyncQuery_ = NULL;
    }

    void RingBuffer::begin()
    {
        s32 frame = countFrame_;
        do{
            if(entries_[frame].flag_){
                if(false == frameSyncQuery_->checkInGPUUse(entries_[frame].event_)){
                    end_ = entries_[frame].end_;
                }
            }
            frame = nextFrame(frame);
        }while(frame != countFrame_);
    }

    RingBuffer::Resource RingBuffer::allocate(s32 size)
    {
        Resource resource(start_, 0);

        s32 e = start_ + size;
        if(start_<end_){
            if(end_<e){
                return resource;
            }
        }else{
            if(totalSize_<e){
                if(end_<size){
                    return resource;
                }
                resource.offsetInBytes_ = 0;
                e = size;
            }
        }
        start_ = e;
        resource.sizeInBytes_ = size;
        return resource;
    }

    void RingBuffer::end()
    {
        entries_[countFrame_].flag_ = Flag_Alloc;
        entries_[countFrame_].event_ = static_cast<s16>(frameSyncQuery_->getCurrentFrame());
        entries_[countFrame_].end_ = start_;
        countFrame_ = nextFrame(countFrame_);
    }

    s32 RingBuffer::calcReserve() const
    {
        return (start_<end_)? end_-start_ : end_ + totalSize_-start_;
    }
}

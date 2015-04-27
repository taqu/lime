/**
@file RingBuffer.cpp
@author t-sakai
@date 2014/10/12 create
*/
#include "RingBuffer.h"
#include "SystemBase.h"

namespace lscene
{
    //--------------------------------------------------
    //---
    //--- RingBuffer
    //---
    //--------------------------------------------------
    RingBuffer::RingBuffer()
        :totalSize_(0)

        ,start_(0)
        ,end_(0)
        ,countFrame_(0)
    {
    }

    RingBuffer::~RingBuffer()
    {
    }

    void RingBuffer::initialize(s32 totalSize)
    {
        LASSERT(0<totalSize);

        totalSize_ = totalSize;
        start_ = 0;
        end_ = totalSize;
        countFrame_ = 0;
        for(s32 i=0; i<NumSyncFrames; ++i){
            entries_[i].flag_ = Flag_None;
            entries_[i].event_ = 0;
            entries_[i].end_ = totalSize_;
        }
    }

    void RingBuffer::terminate()
    {
        for(s32 i=0; i<NumSyncFrames; ++i){
            entries_[i].flag_ = Flag_None;
            entries_[i].event_ = 0;
            entries_[i].end_ = totalSize_;
        }
    }

    void RingBuffer::begin()
    {
        lscene::FrameSyncQueryType& frameSync = SystemInstance::getInstance().getFrameSync();

        s32 frame = countFrame_;
        do{
            if(entries_[frame].flag_){
                if(false == frameSync.checkInGPUUse(entries_[frame].event_)){
                    end_ = entries_[frame].end_;
                }
            }
            frame = calcNextFrame(frame);
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
        lscene::FrameSyncQueryType& frameSync = SystemInstance::getInstance().getFrameSync();

        entries_[countFrame_].flag_ = Flag_Alloc;
        entries_[countFrame_].event_ = static_cast<s16>(frameSync.getCurrentFrame());
        entries_[countFrame_].end_ = start_;
        countFrame_ = calcNextFrame(countFrame_);
    }

    s32 RingBuffer::calcReserve() const
    {
        return (start_<end_)? end_-start_ : end_ + totalSize_-start_;
    }
}

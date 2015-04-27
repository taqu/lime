/**
@file ConstantBuffer.cpp
@author t-sakai
@date 2014/10/22 create
*/
#include "ConstantBuffer.h"
#include "SystemBase.h"

namespace lscene
{
    //--------------------------------------------------
    //---
    //--- ConstantBuffer
    //---
    //--------------------------------------------------
    ConstantBuffer::ConstantBuffer()
        :current_(0)
        ,numEntries_(0)
        ,entries_(NULL)
    {
    }

    ConstantBuffer::~ConstantBuffer()
    {
    }

    void ConstantBuffer::initialize(s32 numEntries)
    {
        LASSERT(NULL == entries_);
        numEntries_ = numEntries;
        entries_ = (Entry*)LSCENE_MALLOC(sizeof(Entry)*numEntries_);
        for(s32 i=0; i<numEntries_; ++i){
            LIME_PLACEMENT_NEW(&entries_[i]) Entry();
            entries_[i].event_ = -1;
            entries_[i].table_.initialize();
        }
    }

    void ConstantBuffer::terminate()
    {
        for(s32 i=0; i<numEntries_; ++i){
            entries_[i].table_.terminate();
            entries_[i].~Entry();
        }
        LSCENE_FREE(entries_);
    }

    void ConstantBuffer::begin()
    {
        FrameSyncQueryType& frameSync = SystemInstance::getInstance().getFrameSync();

        for(s32 i=0; i<numEntries_; ++i){
            if(entries_[i].event_<0){
                current_ = i;
                continue;
            }
            if(false == frameSync.checkInGPUUse(entries_[i].event_)){
                //entries_[i].table_.clearUsed();
                entries_[i].event_ = -1;
                current_ = i;
            }
        }
        entries_[current_].event_ = frameSync.getCurrentFrame();
        entries_[current_].table_.clearUsed();
    }

    void ConstantBuffer::clear()
    {
        for(s32 i=0; i<numEntries_; ++i){
            entries_[i].event_ = -1;
            entries_[i].table_.clearCache();
        }
    }

    lgraphics::ConstantBufferRef* ConstantBuffer::allocate(u32 size)
    {
        lcore::CSLock lock(cs_);

        return entries_[current_].table_.allocate(size);
    }
}

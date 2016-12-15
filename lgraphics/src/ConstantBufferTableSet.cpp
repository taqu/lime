/**
@file ConstantBufferTableSet.cpp
@author t-sakai
@date 2016/11/07 create
*/
#include "ConstantBufferTableSet.h"
#include "FrameSyncQuery.h"

namespace lgfx
{
    //--------------------------------------------------
    //---
    //--- ConstantBufferTableSet
    //---
    //--------------------------------------------------
    ConstantBufferTableSet::ConstantBufferTableSet()
        :frameSyncQuery_(NULL)
        ,numEntries_(0)
        ,current_(0)
        ,entries_(NULL)
    {
    }

    ConstantBufferTableSet::~ConstantBufferTableSet()
    {
    }

    void ConstantBufferTableSet::initialize(FrameSyncQuery* frameSyncQuery, s32 numEntries)
    {
        LASSERT(NULL != frameSyncQuery);
        LASSERT(NULL == entries_);
        frameSyncQuery_ = frameSyncQuery;
        numEntries_ = numEntries;
        entries_ = (Entry*)LMALLOC(sizeof(Entry)*numEntries_);
        for(s32 i=0; i<numEntries_; ++i){
            LPLACEMENT_NEW(&entries_[i]) Entry();
            entries_[i].event_ = -1;
            entries_[i].table_.initialize();
        }
    }

    void ConstantBufferTableSet::terminate()
    {
        for(s32 i=0; i<numEntries_; ++i){
            entries_[i].table_.terminate();
            entries_[i].~Entry();
        }
        LFREE(entries_);
        numEntries_ = 0;
        current_ = 0;
        frameSyncQuery_ = NULL;
    }

    void ConstantBufferTableSet::begin()
    {
        LASSERT(NULL != frameSyncQuery_);

        for(s32 i=0; i<numEntries_; ++i){
            if(entries_[i].event_<0){
                current_ = i;
                continue;
            }
            if(false == frameSyncQuery_->checkInGPUUse(entries_[i].event_)){
                //entries_[i].table_.clearUsed();
                entries_[i].event_ = -1;
                current_ = i;
            }
        }
        entries_[current_].event_ = frameSyncQuery_->getCurrentFrame();
        entries_[current_].table_.clearUsed();
    }

    void ConstantBufferTableSet::clear()
    {
        for(s32 i=0; i<numEntries_; ++i){
            entries_[i].event_ = -1;
            entries_[i].table_.clearCache();
        }
    }

    lgfx::ConstantBufferRef* ConstantBufferTableSet::allocate(u32 size)
    {
        lcore::CSLock lock(cs_);

        return entries_[current_].table_.allocate(size);
    }
}

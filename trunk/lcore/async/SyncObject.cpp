/**
@file SyncObject.cpp
@author t-sakai
@date 2011/08/06
*/
#include "SyncObject.h"

namespace lcore
{
    //-------------------------------------------------------
    //---
    //--- ConditionVariable
    //---
    //-------------------------------------------------------
    ConditionVariable::ConditionVariable(CriticalSection& cs)
        :external_(cs)
        ,numWaitings_(0)
        ,numToWakes_(0)
    {
        signalNotify_ = CreateEvent(0, TRUE, FALSE, NULL);
        signalWaked_ = CreateEvent(0, FALSE, FALSE, NULL);
    }

    ConditionVariable::~ConditionVariable()
    {
    }

    void ConditionVariable::notifyAll()
    {
        if(numWaitings_>0){
            numToWakes_ = numWaitings_;
            SetEvent(signalNotify_);
            WaitForSingleObject(signalWaked_, INFINITE);
        }
    }

    void ConditionVariable::wait()
    {
        ++numWaitings_;
        external_.leave();

        for(;;){
            WaitForSingleObject(signalNotify_, INFINITE);
            {
                CSLock lock(internal_);
                if(0<numToWakes_){
                    --numWaitings_;
                    if(0 == --numToWakes_){
                        ResetEvent(signalNotify_);
                        SetEvent(signalWaked_);
                    }
                    break;
                }
            }
        }

        external_.enter();
    }


    //-------------------------------------------------------
    //---
    //--- ReadersWriterLock
    //---
    //-------------------------------------------------------
    ReadersWriterLock::ReadersWriterLock()
        :readers_(0)
    {
        InitializeCriticalSection(&csReaders_);
        InitializeCriticalSection(&csWrite_);
        readersCleared_ = CreateEvent(NULL, TRUE, TRUE, NULL);

    }

    ReadersWriterLock::~ReadersWriterLock()
    {
        WaitForSingleObject(readersCleared_, INFINITE);
        CloseHandle(readersCleared_);
        DeleteCriticalSection(&csWrite_);
        DeleteCriticalSection(&csReaders_);
    }

    void ReadersWriterLock::enterReader()
    {
        EnterCriticalSection(&csWrite_);
        EnterCriticalSection(&csReaders_);

        if(++readers_ == 1){
            ResetEvent(readersCleared_);
        }
        
        LeaveCriticalSection(&csReaders_);
        LeaveCriticalSection(&csWrite_);
    }

    void ReadersWriterLock::leaveReader()
    {
        EnterCriticalSection(&csReaders_);
        if(--readers_ == 0){
            SetEvent(readersCleared_);
        }
        LeaveCriticalSection(&csReaders_);
    }

    void ReadersWriterLock::enterWriter()
    {
        EnterCriticalSection(&csWrite_);
        WaitForSingleObject(readersCleared_, INFINITE);
    }

    void ReadersWriterLock::leaveWriter()
    {
        LeaveCriticalSection(&csWrite_);
    }
}

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
    //--- Event
    //---
    //-------------------------------------------------------
#if _WIN32
    Event::Event(bool manualReset, bool initState)
    {
        event_ = CreateEvent(0, (manualReset)?TRUE:FALSE, (initState)?TRUE:FALSE, NULL);
    }

    Event::~Event()
    {
        CloseHandle(event_);
    }

#else
    Event::Event(bool manualReset, bool initState)
    {
        pthread_cond_init(&condVariable_, 0);
        pthread_mutex_init(&lock_, 0);

        state_ = 0;
        manualReset_ = (manualReset)? 1 : 0;
        if(initState){
            set();
        }
    }

    Event::~Event()
    {
        pthread_mutex_destroy(&lock_);
        pthread_cond_destroy(&condVariable_);
    }

    thread::WaitStatus Event::wait(u32 timeout)
    {
        if(0 == timeout){
            if(EBUSY == pthread_mutex_trylock(&lock_)){
                return thread::Wait_Timeout;
            }
        }else{
            pthread_mutex_lock(&lock_);
        }

        thread::WaitStatus result;
        if(!state_){
            if(0 == timeout){
                result = thread::Wait_Timeout;

            }else{

                if(timeout != thread::Infinite){
                    timespec ts;
                    clock_gettime(CLOCK_REALTIME, &ts);

                    u64 nanosec = ts.tv_nsec + timeout * 1000 * 1000;
                    u64 sec = nanosec/1000/1000/1000;
                    nanosec -= sec * 1000 * 1000 * 1000;
                    ts.tv_sec += sec;
                    ts.tv_nsec = nanosec;
                    do{
                        result = pthread_cond_timedwait(&condVariable_, &lock_, &ts);
                    }while(0==result && !state_);

                }else{
                    do{
                        result = pthread_cond_wait(&condVariable_, &lock_);
                    }while(0==result && !state_);
                }

                if(0==result && !manualReset_){
                    state_ = 0;
                }
            }

        }else if(!manualReset_){
            state_ = 0;
            result = 0;
        }

        pthread_mutex_unlock(&lock_);
        return result;
    }

    void Event::set()
    {
        pthread_mutex_lock(&lock_);
        state_ = 1;

        if(manualReset_){
            pthread_mutex_unlock(&lock_);
            pthread_cond_broadcast(&condVariable_);
        }else{
            pthread_mutex_unlock(&lock_);
            pthread_cond_signal(&condVariable_);
        }
    }

    void Event::reset()
    {
        pthread_mutex_lock(&lock_);
        state_ = 0;
        pthread_mutex_unlock(&lock_);
    }
#endif

    //-------------------------------------------------------
    //---
    //--- ConditionVariable
    //---
    //-------------------------------------------------------
    ConditionVariable::ConditionVariable()
        :waitCounter_(0)
    {
        events_[Event_Signal] = CreateEvent(0, FALSE, FALSE, NULL);
        events_[Event_Broadcast] = CreateEvent(0, TRUE, FALSE, NULL);
    }

    ConditionVariable::~ConditionVariable()
    {
        CloseHandle(events_[Event_Broadcast]);
        CloseHandle(events_[Event_Signal]);
    }

    void ConditionVariable::wait(CriticalSection& externalLock, u32 timeout)
    {
        waitCounterLock_.enter();
        ++waitCounter_;
        waitCounterLock_.leave();


        externalLock.leave();

        WaitForMultipleObjects(2, events_, FALSE, timeout);

        waitCounterLock_.enter();
        --waitCounter_;
        s32 lastWait = (WAIT_OBJECT_0 + Event_Broadcast) && (waitCounter_ == 0);
        waitCounterLock_.leave();

        if(lastWait){
            ResetEvent(events_[Event_Broadcast]);
        }

        externalLock.enter();
    }

    void ConditionVariable::broadcast()
    {
        waitCounterLock_.enter();
        s32 haveWait = (0<waitCounter_);
        waitCounterLock_.leave();

        if(haveWait){
            SetEvent(events_[Event_Broadcast]);
        }
    }

    void ConditionVariable::signal()
    {
        waitCounterLock_.enter();
        s32 haveWait = (0<waitCounter_);
        waitCounterLock_.leave();

        if(haveWait){
            SetEvent(events_[Event_Signal]);
        }
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

    //-------------------------------------------------------
    //---
    //--- Semaphore
    //---
    //-------------------------------------------------------
    Semaphore::Semaphore(s32 initCount, s32 maxCount)
    {
        LASSERT(0<=initCount);
        LASSERT(0<maxCount);
        semaphore_ = CreateSemaphore(NULL, initCount, maxCount, NULL);
    }

    Semaphore::~Semaphore()
    {
        CloseHandle(semaphore_);
    }

    thread::WaitStatus Semaphore::wait(u32 timeout)
    {
        return (thread::WaitStatus)WaitForSingleObject(semaphore_, timeout);
    }

    s32 Semaphore::release(s32 count)
    {
        LONG prev;
        ReleaseSemaphore(semaphore_, count, &prev);
        return prev;
    }
}

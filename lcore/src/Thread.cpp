/**
@file Thread.cpp
@author t-sakai
@date 2011/08/06
*/
#include "Thread.h"

#if defined(_WIN32)
#include <process.h>
#else
#include <errno.h>
#endif

namespace lcore
{
#if defined(_WIN32)
    //----------------------------------------------------
    //---
    //--- Thread Affinity
    //---
    //----------------------------------------------------
    LHANDLE getCurrentProcess()
    {
        return GetCurrentProcess();
    }

    u64 getProcessAffinityMask(LHANDLE handle)
    {
        u64 processAffinity = 0;
        u64 systemAffinity = 0;
        return (TRUE == GetProcessAffinityMask(handle, &processAffinity, &systemAffinity))? processAffinity : 0;
    }

    bool setProcessAffinityMask(LHANDLE handle, u64 affinityMask)
    {
        return TRUE == SetProcessAffinityMask(handle, affinityMask);
    }

    LHANDLE getCurrentThread()
    {
        return GetCurrentThread();
    }

    u64 setThreadAffinityMask(LHANDLE handle, u64 affinityMask)
    {
        return SetThreadAffinityMask(handle, affinityMask);
    }

    //----------------------------------------------------
    //---
    //--- Thread
    //---
    //----------------------------------------------------
    ThreadRaw::ThreadRaw()
        :handle_(NULL)
        ,id_(0)
        ,proc_(NULL)
        ,data_(NULL)
    {
    }

    ThreadRaw::~ThreadRaw()
    {
        release();
    }

    bool ThreadRaw::create(Proc proc, void* data, bool suspend)
    {
        LASSERT(NULL == handle_);
        proc_ = proc;
        data_ = data;
        handle_ = reinterpret_cast<HANDLE>( _beginthreadex(
            NULL,
            0,
            ThreadRaw::proc,
            this,
            (suspend)? CREATE_SUSPENDED : 0,
            &id_));

        return (handle_ != NULL);
    }

    void ThreadRaw::start()
    {
        resume();
    }

    void ThreadRaw::resume()
    {
        ResumeThread(handle_);
    }

    void ThreadRaw::suspend()
    {
        SuspendThread(handle_);
    }

    void ThreadRaw::terminate()
    {
        TerminateThread(handle_, 0);
    }

    thread::WaitStatus ThreadRaw::join(u32 timeout)
    {
        return static_cast<thread::WaitStatus>(WaitForSingleObject(handle_, timeout));
    }


    void ThreadRaw::release()
    {
        if(NULL != handle_){
            CloseHandle(handle_);
            handle_ = NULL;
            id_ = 0;
        }
    }


    u32 __stdcall ThreadRaw::proc(void* args)
    {
        ThreadRaw* thread = static_cast<ThreadRaw*>(args);
        LASSERT(NULL != thread);
        if(NULL != thread->proc_){
            thread->proc_(thread->id_, thread->data_);
        }
        _endthreadex(0);
        return 0;
    }

    //----------------------------------------------------
    //---
    //--- Thread
    //---
    //----------------------------------------------------
    Thread::Thread()
        :handle_(NULL)
        ,id_(0)
        ,canRun_(true)
        ,suspend_(true)
    {
    }

    Thread::~Thread()
    {
        release();
    }

    bool Thread::create()
    {
        LASSERT(NULL == handle_);
        canRun_ = true;
        suspend_ = true;
        handle_ = reinterpret_cast<HANDLE>( _beginthreadex(
            NULL,
            0,
            proc,
            this,
            CREATE_SUSPENDED,
            &id_));

        return (handle_ != NULL);
    }

    void Thread::start()
    {
        resume();
    }

    void Thread::resume()
    {
        CSLock lock(cs_);
        if(suspend_){
            ResumeThread(handle_);
            suspend_ = false;
        }
    }

    void Thread::suspend()
    {
        CSLock lock(cs_);
        if(!suspend_){
            SuspendThread(handle_);
            suspend_ = true;
        }
    }

    void Thread::stop()
    {
        {
            CSLock lock(cs_);
            canRun_ = false;
        }
        resume();
    }

    void Thread::terminate()
    {
        TerminateThread(handle_, 0);
    }

    thread::WaitStatus Thread::join(u32 timeout)
    {
        return static_cast<thread::WaitStatus>(WaitForSingleObject(handle_, timeout));
    }

    // スレッド終了すべきか
    bool Thread::canRun()
    {
        CSLock lock(cs_);
        return canRun_;
    }

    void Thread::release()
    {
        if(NULL != handle_){
            CloseHandle(handle_);
            handle_ = NULL;
            id_ = 0;
        }
    }


    u32 __stdcall Thread::proc(void* args)
    {
        Thread* thread = static_cast<Thread*>(args);
        if(thread){
            thread->run();
        }

        _endthreadex(0);
        return 0;
    }

    //----------------------------------------------------
    //---
    //--- MultipleWait
    //---
    //----------------------------------------------------
    MultipleWait::MultipleWait()
    {
        for(u32 i=0; i<NumMaxThreads; ++i){
            handles_[i] = NULL;
        }
    }

    MultipleWait::~MultipleWait()
    {
    }

    void MultipleWait::set(u32 index, ThreadRaw* thread)
    {
        LASSERT(index<NumMaxThreads);
        handles_[index] = thread->handle_;
    }

    void MultipleWait::set(u32 index, Thread* thread)
    {
        LASSERT(index<NumMaxThreads);
        handles_[index] = thread->handle_;
    }

    u32 MultipleWait::join(u32 numThreads, u32 timeout)
    {
        return WaitForMultipleObjects(numThreads, handles_, TRUE, timeout);
    }

#else
    //----------------------------------------------------
    //---
    //--- Thread
    //---
    //----------------------------------------------------
#ifndef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
    s32 ThreadRaw::getState()
    {
        CSLock lock(cs_);
        return state_;
    }
    void ThreadRaw::setState(s32 state)
    {
        CSLock lock(cs_);
        state_ = state;
    }
#endif

    ThreadRaw::ThreadRaw()
        :handle_(0)
        ,proc_(NULL)
        ,data_(NULL)
#ifndef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
        ,state_(State_None)
#endif
    {
    }

    ThreadRaw::~ThreadRaw()
    {
        release();
    }

    bool ThreadRaw::create(Proc proc, void* data, bool suspend)
    {
        LASSERT(0 == handle_);

        proc_ = proc;
        data_ = data;
        s32 ret = pthread_create(&handle_, NULL, ThreadRaw::proc, this);
        return (0==ret);
    }

    void ThreadRaw::start()
    {
    }

    void ThreadRaw::resume()
    {
    }

    void ThreadRaw::suspend()
    {
    }

    void ThreadRaw::terminate()
    {
#ifdef LCORE_THREAD_HAS_PTHREAD_CANCEL
        pthread_cancel(handle_);
#endif
    }

    thread::WaitStatus ThreadRaw::join(u32 timeout)
    {
        s32 ret;
        if(Infinite == timeout){
            ret = pthread_join(handle_, NULL);
        } else{
#ifdef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
            timespec ts;
            if(clock_gettime(CLOCK_REALTIME, &ts) == -1) {
                return thread::Wait_Failed;
            }
            while(1000<timeout){
                ts.tv_sec += 1;
                timeout -= 1000;
            }
            ts.tv_nsec += 1000000L * timeout;

            ret = pthread_timedjoin_np(handle_, NULL, &ts);
#else
            s32 state = getState();
            if(State_Term != state){
                lcore::sleep(timeout);
                state = getState();
            }
            if(State_Term == state){
                ret = pthread_join(handle_, NULL);
            }else{
                ret = ETIMEDOUT;
            }
#endif
        }

        switch(ret)
        {
        case EINVAL:
        case ESRCH:
        case EDEADLK:
            return thread::Wait_Failed;

        case EBUSY:
        case ETIMEDOUT:
            return thread::Wait_Timeout;

        default:
            return thread::Wait_Success;
        }
    }


    void ThreadRaw::release()
    {
#ifdef LCORE_THREAD_HAS_PTHREAD_CANCEL
        if(0 != handle_){
            pthread_cancel(handle_);
            pthread_join(handle_, NULL);
            handle_ = 0;
        }
#else
        if(0 != handle_){
            //pthread_detach(handle_);
            pthread_join(handle_, NULL);
            handle_ = 0;
        }
#endif
    }


    void* ThreadRaw::proc(void* args)
    {
        ThreadRaw* thread = static_cast<ThreadRaw*>(args);
        LASSERT(NULL != thread);
        if(NULL != thread->proc_){
#ifndef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
            thread->setState(State_Proc);
#endif
            thread->proc_(thread->data_);
        }

#ifndef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
        thread->setState(State_Term);
#endif
        return NULL;
    }

    //----------------------------------------------------
    //---
    //--- Thread
    //---
    //----------------------------------------------------
#ifndef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
    s32 Thread::getState()
    {
        CSLock lock(cs_);
        return state_;
    }
    void Thread::setState(s32 state)
    {
        CSLock lock(cs_);
        state_ = state;
    }
#endif

    Thread::Thread()
        :handle_(0)
        ,canRun_(true)
        ,suspend_(true)
#ifndef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
        ,state_(State_None)
#endif
    {
    }

    Thread::~Thread()
    {
        release();
    }

    bool Thread::create()
    {
        LASSERT(0 == handle_);
        canRun_ = true;
        suspend_ = true;
        s32 ret = pthread_create(&handle_, NULL, Thread::proc, this);

        return (0==ret);
    }

    void Thread::start()
    {
        resume();
    }

    void Thread::resume()
    {
        CSLock lock(cs_);
        suspend_ = false;
    }

    void Thread::suspend()
    {
        CSLock lock(cs_);
        suspend_ = true;
    }

    void Thread::stop()
    {
        CSLock lock(cs_);
        canRun_ = false;
        suspend_ = false;
    }

    void Thread::terminate()
    {
#ifdef LCORE_THREAD_HAS_PTHREAD_CANCEL
        pthread_cancel(handle_);
#endif
    }

    thread::WaitStatus Thread::join(u32 timeout)
    {
        s32 ret;
        if(Infinite == timeout){
            ret = pthread_join(handle_, NULL);
        } else{
#ifdef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
            timespec ts;
            if(clock_gettime(CLOCK_REALTIME, &ts) == -1) {
                return thread::Wait_Failed;
            }
            while(1000<timeout){
                ts.tv_sec += 1;
                timeout -= 1000;
            }
            ts.tv_nsec += 1000000L * timeout;
            ret = pthread_timedjoin_np(handle_, NULL, &ts);
#else
            s32 state = getState();
            if(State_Term != state){
                lcore::sleep(timeout);
                state = getState();
            }
            if(State_Term == state){
                ret = pthread_join(handle_, NULL);
            }else{
                ret = ETIMEDOUT;
            }
#endif
        }

        switch(ret)
        {
        case EINVAL:
        case ESRCH:
        case EDEADLK:
            return thread::Wait_Failed;

        case EBUSY:
        case ETIMEDOUT:
            return thread::Wait_Timeout;

        default:
            return thread::Wait_Success;
        }
    }

    // スレッド終了すべきか
    bool Thread::canRun()
    {
        CSLock lock(cs_);
        return canRun_;
    }

    void Thread::release()
    {
#ifdef LCORE_THREAD_HAS_PTHREAD_CANCEL
        if(0 != handle_){
            pthread_cancel(handle_);
            pthread_join(handle_, NULL);
            handle_ = 0;
        }
#else
        if(0 != handle_){
            //pthread_detach(handle_);
            pthread_join(handle_, NULL);
            handle_ = 0;
        }
#endif
    }


    void* Thread::proc(void* args)
    {
        Thread* thread = static_cast<Thread*>(args);
        if(thread){
#ifndef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
            thread->setState(State_Proc);
#endif
            thread->run();
        }

#ifndef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
        thread->setState(State_Term);
#endif
        return NULL;
    }
#endif

    //----------------------------------------------------
    //---
    //--- ThreadPool
    //---
    //----------------------------------------------------
    ThreadPool::WorkerThread::WorkerThread(ThreadPool* threadPool)
        :threadPool_(threadPool)
    {
    }

    ThreadPool::WorkerThread::~WorkerThread()
    {
    }

    void ThreadPool::WorkerThread::proc(u32 /*threadId*/, void* data)
    {
        WorkerThread* thread = static_cast<WorkerThread*>(data);
        ThreadPool* threadPool = thread->threadPool_;

        for(;;){
            threadPool->waitJobAvailable();

            if(!threadPool->canRun()){
                return;
            }

            Job* job = threadPool->popPendingJob();
            while(NULL != job){
                LASSERT(NULL != job->proc_);
                job->proc_(thread->getID(), job->jobId_, job->data_);

                threadPool->pushJob(job);
                job = threadPool->popPendingJob();
            }
        }
    }


    ThreadPool::ThreadPool(s32 maxThreads, s32 maxJobs)
        :canRun_(true)
        , jobSemaphore_(0, maxJobs*2)
        , maxJobs_(maxJobs)
        , numPendigJobs_(0)
        , numActiveJobs_(0)
        , pendingJobs_(NULL)
        , maxThreads_(maxThreads)
    {
        LASSERT(0<maxThreads_);
        LASSERT(0<maxJobs_);

        jobs_ = LNEW Job[maxJobs_];

        freeJobs_ = &jobs_[0];
        for(s32 i = 0; i<(maxJobs_-1); ++i){
            jobs_[i].jobId_ = i;
            jobs_[i].next_ = &jobs_[i+1];
        }
        jobs_[maxJobs_-1].jobId_ = maxJobs_-1;
        jobs_[maxJobs_-1].next_ = NULL;

        threads_ = LNEW WorkerThread*[maxThreads_];
        for(s32 i = 0; i<maxThreads_; ++i){
            threads_[i] = LNEW WorkerThread(this);
            threads_[i]->create(WorkerThread::proc, threads_[i], true);
        }
    }

    ThreadPool::~ThreadPool()
    {
        setCanRun(false);
        clearPendingJobs();
        jobSemaphore_.release(maxJobs_);
        {
            lcore::MultipleWait multiWait;
            for(s32 i = 0; i<maxThreads_; ++i){
                multiWait.set(i, threads_[i]);
            }
            for(s32 i = 0; i<10; ++i){
                lcore::u32 status = multiWait.join(maxThreads_, 500);
                if(lcore::MultipleWait::Wait_Success<= status && status<lcore::MultipleWait::Wait_Abandoned){
                    break;
                }
            }
        }

        for(s32 i = 0; i<maxThreads_; ++i){
            threads_[i]->terminate();
            LDELETE(threads_[i]);
        }

        LDELETE_ARRAY(threads_);
        LDELETE_ARRAY(jobs_);
    }

    void ThreadPool::start()
    {
        for(s32 i = 0; i<maxThreads_; ++i){
            threads_[i]->resume();
        }
    }

    s32 ThreadPool::add(JobProc proc, void* data)
    {
        LASSERT(NULL != proc);
        Job* job = popJob();
        if(NULL == job){
            return InvalidJobId;
        }
        job->proc_ = proc;
        job->data_ = data;
        pushPendingJob(job);
        return job->jobId_;
    }

    ThreadPool::WaitStatus ThreadPool::waitAllFinish(u32 timeout)
    {
        u32 prev = lcore::getTimeMilliSec();
        for(;;){
            localCS_.enter();
            if(numPendigJobs_<=0 && numActiveJobs_<=0){
                localCS_.leave();
                return WaitStatus_Success;
            }
            localCS_.leave();

            u32 time = lcore::getTimeMilliSec();
            u32 d = (time>=prev) ? time - prev : lcore::numeric_limits<u32>::maximum() - prev + time;
            if(timeout<d){
                return WaitStatus_Timeout;
            }

            lcore::sleep(1);
        }
    }

    //ThreadPool::WaitStatus ThreadPool::busyWaitAllFinish(u32 timeout)
    //{
    //    u32 prev = lcore::getTime();
    //    for(;;){
    //        for(s32 i=0; i<512; ++i){
    //            localCS_.enter();
    //            if(numPendigJobs_<=0 && numActiveJobs_<=0){
    //                localCS_.leave();
    //                return WaitStatus_Success;
    //            }
    //            localCS_.leave();
    //        }

    //        u32 time = lcore::getTime();
    //        u32 d = (time>=prev)? time - prev : lcore::numeric_limits<u32>::maximum() - prev + time;
    //        if(timeout<d){
    //            return WaitStatus_Timeout;
    //        }
    //    }
    //}

    ThreadPool::WaitStatus ThreadPool::waitFinish(s32 numJobs, u32 timeout)
    {
        s32 numFreeJobs = getNumFreeJobs();

        u32 prev = lcore::getTimeMilliSec();
        while(numFreeJobs<numJobs){

            u32 time = lcore::getTimeMilliSec();
            u32 d = (time>=prev) ? time - prev : lcore::numeric_limits<u32>::maximum() - prev + time;
            if(timeout<d){
                return WaitStatus_Timeout;
            }

            numFreeJobs = getNumFreeJobs();
        }
        return WaitStatus_Success;
    }

    ThreadPool::WaitStatus ThreadPool::waitJobFinish(s32 jobId, u32 timeout)
    {
        u32 prev = lcore::getTimeMilliSec();
        for(;;){
            for(s32 i = 0; i<512; ++i){
                localCS_.enter();
                Job* job = freeJobs_;
                while(NULL != job){
                    if(jobId == job->jobId_){
                        localCS_.leave();
                        return WaitStatus_Success;
                    }
                    job = job->next_;
                }
                localCS_.leave();
            }

            u32 time = lcore::getTimeMilliSec();
            u32 d = (time>=prev) ? time - prev : lcore::numeric_limits<u32>::maximum() - prev + time;
            if(timeout<d){
                return WaitStatus_Timeout;
            }
        }
    }

    s32 ThreadPool::getNumFreeJobs()
    {
        CSLock lock(localCS_);
        LASSERT((numPendigJobs_ + numActiveJobs_)<=maxJobs_);
        return (maxJobs_ - numPendigJobs_ - numActiveJobs_);
    }

    s32 ThreadPool::getNumPendingJobs()
    {
        CSLock lock(localCS_);
        return numPendigJobs_;
    }

    s32 ThreadPool::getNumActiveJobs()
    {
        CSLock lock(localCS_);
        return numActiveJobs_;
    }

    void ThreadPool::getNumJobs(s32& numFree, s32& numPending, s32& numActive)
    {
        CSLock lock(localCS_);
        numFree = (maxJobs_ - numPendigJobs_ - numActiveJobs_);
        numPending = numPendigJobs_;
        numActive = numActiveJobs_;
    }

    bool ThreadPool::removePendingJob(s32 jobId)
    {
        CSLock lock(localCS_);

        Job* job = pendingJobs_;
        Job* prev = NULL;
        while(NULL != job){
            if(job->jobId_ == jobId){
                --numPendigJobs_;
                LASSERT(0<=numPendigJobs_);
                if(NULL == prev){
                    pendingJobs_ = job->next_;
                } else{
                    prev->next_ = job->next_;
                    job->next_ = freeJobs_;
                    freeJobs_ = job;
                }
                return true;
            }

            prev = job;
            job = job->next_;
        }
        return false;
    }

    void ThreadPool::clearPendingJobs()
    {
        CSLock lock(localCS_);
        while(NULL != pendingJobs_){
            Job* job = pendingJobs_;
            pendingJobs_ = pendingJobs_->next_;
            job->next_ = freeJobs_;
            freeJobs_ = job;
        }
        numPendigJobs_ = 0;
    }

    u32 ThreadPool::getThreadId(s32 index) const
    {
        return threads_[index]->getID();
    }

    bool ThreadPool::canRun()
    {
        CSLock lock(localCS_);
        return canRun_;
    }

    void ThreadPool::setCanRun(bool value)
    {
        CSLock lock(localCS_);
        canRun_ = value;
    }

    void ThreadPool::waitJobAvailable()
    {
        jobSemaphore_.wait(thread::Infinite);
    }

    ThreadPool::Job* ThreadPool::popJob()
    {
        CSLock lock(localCS_);
        if(NULL == freeJobs_){
            return NULL;
        }

        Job* job = freeJobs_;
        freeJobs_ = freeJobs_->next_;
        return job;
    }

    void ThreadPool::pushJob(Job* job)
    {
        LASSERT(NULL != job);
        CSLock lock(localCS_);
        --numActiveJobs_;
        LASSERT(0<=numActiveJobs_);
        job->next_ = freeJobs_;
        freeJobs_ = job;
    }

    ThreadPool::Job* ThreadPool::popPendingJob()
    {
        CSLock lock(localCS_);
        if(NULL == pendingJobs_){
            return NULL;
        }
        --numPendigJobs_;
        LASSERT(0<=numPendigJobs_);
        ++numActiveJobs_;
        LASSERT(numActiveJobs_<=maxJobs_);
        Job* job = pendingJobs_;
        pendingJobs_ = pendingJobs_->next_;
        return job;
    }

    void ThreadPool::pushPendingJob(Job* job)
    {
        LASSERT(NULL != job);
        CSLock lock(localCS_);
        ++numPendigJobs_;
        LASSERT(numPendigJobs_<=maxJobs_);
        job->next_ = pendingJobs_;
        pendingJobs_ = job;
        jobSemaphore_.release(1);
    }

    //----------------------------------------------------
    //---
    //--- ThreadAffinity
    //---
    //----------------------------------------------------
    ThreadAffinity::ThreadAffinity()
    {
    }

    ThreadAffinity::~ThreadAffinity()
    {
    }

    void ThreadAffinity::initialize()
    {
        cpuInformation_.initialize();
        random_.srand(getDefaultSeed());
        for(s32 i = 0; i<cpuInformation_.getNumCores(); ++i){
            flags_[i] = Flag_None;
        }
    }

    s32 ThreadAffinity::setAffinity(LHANDLE thread, bool occupy)
    {
        u8 numAllowed = 0;
        u8 minAllocated = MaxAllocated;
        u8 num;
        for(s32 i = 0; i<cpuInformation_.getNumCores(); ++i){
            if(0 != (Flag_Occupied&flags_[i])){
                continue;
            }
            num = flags_[i]>>1;
            minAllocated = minimum(minAllocated, num);
        }
        for(s32 i = 0; i<cpuInformation_.getNumCores(); ++i){
            if(0 != (Flag_Occupied&flags_[i])){
                continue;
            }
            num = flags_[i]>>1;
            if(num == minAllocated){
                ++numAllowed;
            }
        }
        LASSERT(0<numAllowed);
        s32 index = static_cast<s32>(random_.rand()&0x7FFFFFFFU)%numAllowed;
        numAllowed = 0;
        s32 allocated = -1;
        for(s32 i = 0; i<cpuInformation_.getNumCores(); ++i){
            if(0 != (Flag_Occupied&flags_[i])){
                continue;
            }
            num = flags_[i]>>1;
            if(num == minAllocated){
                if(numAllowed == index){
                    allocated = i;
                    break;
                }
                ++numAllowed;
            }
        }
        if(allocated<0){
            return -1;
        }
        num = flags_[allocated]>>1;
        if(num<MaxAllocated){
            num += 1;
        }
        flags_[allocated] = (num<<1) | (flags_[allocated] & Flag_Occupied);
        setThreadAffinityMask(thread, cpuInformation_.getCore(allocated).groupMask_);
        if(1<cpuInformation_.getNumCores() && occupy){
            flags_[allocated] |= Flag_Occupied;
        }
        return cpuInformation_.getCore(allocated).id_;
    }

    s32 ThreadAffinity::setAffinity(ThreadRaw& thread, bool occupy)
    {
        LASSERT(thread.valid());
        return setAffinity(thread.handle_, occupy);
    }

    s32 ThreadAffinity::setAffinity(Thread& thread, bool occupy)
    {
        LASSERT(thread.valid());
        return setAffinity(thread.handle_, occupy);
    }
}

/**
@file ThreadPool.cpp
@author t-sakai
@date 2014/04/26 create
*/
#include "ThreadPool.h"
#include "../utility.h"

namespace lcore
{
    ThreadPool::WorkerThread::WorkerThread(ThreadPool* threadPool)
        :threadPool_(threadPool)
    {
    }

    ThreadPool::WorkerThread::~WorkerThread()
    {
    }

    void ThreadPool::WorkerThread::proc(void* data)
    {
        WorkerThread* thread = reinterpret_cast<WorkerThread*>(data);
        ThreadPool* threadPool = thread->threadPool_;

        for(;;){
            threadPool->waitJobAvailable();

            if(!threadPool->canRun()){
                return;
            }

            Job* job = threadPool->popPendingJob();
            while(NULL != job){
                LASSERT(NULL != job->proc_);
                job->proc_(job->jobId_, job->data_);

                threadPool->pushJob(job);
                job = threadPool->popPendingJob();
            }
        }
    }


    ThreadPool::ThreadPool(s32 maxThreads, s32 maxJobs)
        :canRun_(true)
        ,jobSemaphore_(0, maxJobs*2)
        ,maxJobs_(maxJobs)
        ,numPendigJobs_(0)
        ,numActiveJobs_(0)
        ,pendingJobs_(NULL)
        ,maxThreads_(maxThreads)
    {
        LASSERT(0<maxThreads_);
        LASSERT(0<maxJobs_);

        jobs_ = LIME_NEW Job[maxJobs_];

        freeJobs_ = &jobs_[0];
        for(s32 i=0; i<(maxJobs_-1); ++i){
            jobs_[i].jobId_ = i;
            jobs_[i].next_ = &jobs_[i+1];
        }
        jobs_[maxJobs_-1].jobId_ = maxJobs_-1;
        jobs_[maxJobs_-1].next_ = NULL;

        threads_ = LIME_NEW WorkerThread*[maxThreads_];
        for(s32 i=0; i<maxThreads_; ++i){
            threads_[i] = LIME_NEW WorkerThread(this);
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
            for(s32 i=0; i<maxThreads_; ++i){
                multiWait.set(i, threads_[i]);
            }
            for(s32 i=0; i<10; ++i){
                lcore::u32 status = multiWait.join(maxThreads_, 500);
                if(lcore::MultipleWait::Wait_Success<= status && status<lcore::MultipleWait::Wait_Abandoned){
                    break;
                }
            }
        }

        for(s32 i=0; i<maxThreads_; ++i){
            LIME_DELETE(threads_[i]);
        }

        LIME_DELETE_ARRAY(threads_);
        LIME_DELETE_ARRAY(jobs_);
    }

    void ThreadPool::start()
    {
        for(s32 i=0; i<maxThreads_; ++i){
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
        u32 prev = lcore::getTime();
        for(;;){
            {
                CSLock lock(localCS_);
                if(numPendigJobs_<=0 && numActiveJobs_<=0){
                    return WaitStatus_Success;
                }
            }
            u32 time = lcore::getTime();
            u32 d = (time>=prev)? time - prev : lcore::numeric_limits<u32>::maximum() - prev + time;
            if(timeout<d){
                return WaitStatus_Timeout;
            }

            lcore::thread::sleep(1);
        }
    }

    ThreadPool::WaitStatus ThreadPool::waitFinish(s32 numJobs, u32 timeout)
    {
        s32 numFreeJobs = getNumFreeJobs();

        u32 prev = lcore::getTime();
        while(numFreeJobs<numJobs){

            u32 time = lcore::getTime();
            u32 d = (time>=prev)? time - prev : lcore::numeric_limits<u32>::maximum() - prev + time;
            if(timeout<d){
                return WaitStatus_Timeout;
            }

            numFreeJobs = getNumFreeJobs();
        }
        return WaitStatus_Success;
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
                }else{
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
}

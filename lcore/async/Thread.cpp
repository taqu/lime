/**
@file Thread.cpp
@author t-sakai
@date 2011/08/06
*/
#include "Thread.h"
#include <process.h>

namespace lcore
{
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
        ThreadRaw* thread = reinterpret_cast<ThreadRaw*>(args);
        LASSERT(NULL != thread);
        if(NULL != thread->proc_){
            thread->proc_(thread->data_);
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
        CSLock lock(cs_);
        canRun_ = false;
        resume();
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
        Thread* thread = reinterpret_cast<Thread*>(args);
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
        LASSERT(index<=NumMaxThreads);
        handles_[index] = thread->handle_;
    }

    void MultipleWait::set(u32 index, Thread* thread)
    {
        LASSERT(index<=NumMaxThreads);
        handles_[index] = thread->handle_;
    }

    u32 MultipleWait::join(u32 numThreads, u32 timeout)
    {
        return WaitForMultipleObjects(numThreads, handles_, TRUE, timeout);
    }
}

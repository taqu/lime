/**
@file Thread.cpp
@author t-sakai
@date 2011/08/06
*/
#include "Thread.h"
#include <process.h>

namespace lcore
{
    u32 getNumberOfProcessors()
    {
        SYSTEM_INFO info = {0};
        GetSystemInfo(&info);
        return info.dwNumberOfProcessors;
    }

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
        if(suspend_){
            CSLock lock(cs_);
            if(suspend_){
                ResumeThread(handle_);
                suspend_ = false;
            }
        }
    }

    void Thread::suspend()
    {
        if(!suspend_){
            CSLock lock(cs_);
            if(!suspend_){
                SuspendThread(handle_);
                suspend_ = true;
            }
        }
    }

    void Thread::stop()
    {
        if(canRun_){
            CSLock lock(cs_);
            if(canRun_){
                canRun_ = false;
            }
            resume();
        }
    }

    Thread::WaitStatus Thread::join(u32 timeout)
    {
        return static_cast<WaitStatus>(WaitForSingleObject(handle_, timeout));
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
        thread->release();
        return 0;
    }

    MultipleWait::MultipleWait()
    {
        for(u32 i=0; i<NumMaxThreads; ++i){
            handles_[i] = NULL;
        }
    }

    MultipleWait::~MultipleWait()
    {
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

#ifndef INC_LCORE_THREAD_H_
#define INC_LCORE_THREAD_H_
/**
@file Thread.h
@author t-sakai
@date 2011/08/06 create
*/
#include "lcore.h"
#include "SyncObject.h"
#include "CPU.h"
#include "Random.h"

namespace lcore
{
#if defined(_WIN32)
    //----------------------------------------------------
    //---
    //--- Thread Affinity
    //---
    //----------------------------------------------------
    LHANDLE getCurrentProcess();
    u64 getProcessAffinityMask(LHANDLE handle);
    bool setProcessAffinityMask(LHANDLE handle, u64 affinityMask);

    LHANDLE getCurrentThread();
    u64 setThreadAffinityMask(LHANDLE handle, u64 affinityMask);

    //----------------------------------------------------
    //---
    //--- ThreadRaw
    //---
    //----------------------------------------------------
    class ThreadRaw
    {
    public:
        static const u32 Infinite = lcore::thread::Infinite;
        typedef void (*Proc)(u32 threadId, void* data);

        ThreadRaw();
        virtual ~ThreadRaw();

        /**
        @return 成否
        */
        bool create(Proc proc, void* data, bool suspend);

        bool valid() const{return  NULL != handle_;}

        /// スレッドID取得
        u32 getID() const
        {
            return id_;
        }

        /// 開始
        void start();

        /// 再開
        void resume();

        /// 一時停止
        void suspend();

        /// 強制終了
        void terminate();

        /**
        @brief 終了を待つ
        @return 結果
        @param timeout ... タイムアウト時間（ミリ秒）
        */
        thread::WaitStatus join(u32 timeout=Infinite);

    private:
        ThreadRaw(const ThreadRaw&);
        ThreadRaw& operator=(const ThreadRaw&);

        friend class MultipleWait;
        friend class ThreadAffinity;

        void release();

        static u32 __stdcall proc(void* args);

        HANDLE handle_;
        u32 id_;
        Proc proc_;
        void* data_;
    };


    //----------------------------------------------------
    //---
    //--- Thread
    //---
    //----------------------------------------------------
    class Thread
    {
    public:
        static const u32 Infinite = lcore::thread::Infinite;

        Thread();
        virtual ~Thread();

        /**
        @return 成否
        @brief スレッドを待機状態で作成
        */
        bool create();

        /// スレッドID取得
        u32 getID() const
        {
            return id_;
        }

        bool valid() const{ return NULL != handle_;}

        /// 開始
        void start();

        /// 再開
        void resume();

        /// 一時停止
        void suspend();

        /// 終了
        void stop();

        /// 強制終了
        void terminate();

        /**
        @brief 終了を待つ
        @return 結果
        @param timeout ... タイムアウト時間（ミリ秒）
        */
        thread::WaitStatus join(u32 timeout=Infinite);

    protected:
        /// スレッド終了すべきか
        bool canRun();

        /// 処理本体
        virtual void run() =0;

    private:
        friend class MultipleWait;
        friend class ThreadAffinity;

        void release();

        static u32 __stdcall proc(void* args);

        Thread(const Thread&);
        Thread& operator=(const Thread&);

        HANDLE handle_;
        u32 id_;
        CriticalSection cs_;
        bool canRun_;
        bool suspend_;
    };

    //----------------------------------------------------
    //---
    //--- MultipleWait
    //---
    //----------------------------------------------------
    class MultipleWait
    {
    public:
        static const u32 NumMaxThreads = 32;
        static const u32 Infinite = lcore::thread::Infinite;

        enum WaitStatus
        {
            Wait_Abandoned = WAIT_ABANDONED_0, /// 所有していたオブジェクトが解放しないで終了した
            Wait_Success = WAIT_OBJECT_0, /// シグナル状態になった
            Wait_Timeout = WAIT_TIMEOUT, /// タイムアウトした
            Wait_Failed = WAIT_FAILED,
        };

        MultipleWait();
        ~MultipleWait();

        /**
        @brief スレッドセット
        @param index ...
        @param thread ...
        @warn 0からjoinで指定する個数分埋めること
        */
        void set(u32 index, ThreadRaw* thread);

        /**
        @brief スレッドセット
        @param index ...
        @param thread ...
        @warn 0からjoinで指定する個数分埋めること
        */
        void set(u32 index, Thread* thread);

        /**
        @brief 終了を待つ
        @return Wait_Success以上（Wait_Success + numThreads - 1）以下：全てシグナル状態、
        @return Wait_Abandoned以上（Wait_Abandoned + numThreads - 1）以下：どれかひとつが解放しないで終了した
        @return Wait_Timeout
        @param timeout ... タイムアウト時間（ミリ秒）
        */
        u32 join(u32 numThreads, u32 timeout=Infinite);
    private:
        MultipleWait(const MultipleWait&);
        MultipleWait& operator=(const MultipleWait&);

        HANDLE handles_[NumMaxThreads];
    };



#else

#if defined(ANDROID)
#else
#define LCORE_THREAD_HAS_PTHREAD_CANCEL 1
#define LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN 1
#endif
    //----------------------------------------------------
    //---
    //--- ThreadRaw
    //---
    //----------------------------------------------------
    class ThreadRaw
    {
    public:
        static const u32 Infinite = lcore::thread::Infinite;
        typedef void (*Proc)(void* data);

        ThreadRaw();
        virtual ~ThreadRaw();

        /**
        @return 成否
        */
        bool create(Proc proc, void* data, bool suspend);

        bool valid() const{return  0 != handle_;}

        /// スレッドID取得
        pthread_t getID() const
        {
            return pthread_self();
        }

        /// 開始
        void start();

        /// 再開
        void resume();

        /// 一時停止
        void suspend();

        /// 強制終了
        void terminate();

        /**
        @brief 終了を待つ
        @return 結果
        @param timeout ... タイムアウト時間（ミリ秒）
        */
        thread::WaitStatus join(u32 timeout=Infinite);

    private:
        ThreadRaw(const ThreadRaw&);
        ThreadRaw& operator=(const ThreadRaw&);

        friend class MultipleWait;

        void release();

        static void* proc(void* args);

        pthread_t handle_;
#ifndef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
        static const s32 State_None = 0;
        static const s32 State_Proc = 1;
        static const s32 State_Term = 2;
        s32 getState();
        void setState(s32 state);
        CriticalSection cs_;
        s32 state_;
#endif
        Proc proc_;
        void* data_;
    };


    //----------------------------------------------------
    //---
    //--- Thread
    //---
    //----------------------------------------------------
    class Thread
    {
    public:
        static const u32 Infinite = lcore::thread::Infinite;

        Thread();
        virtual ~Thread();

        /**
        @return 成否
        @brief スレッドを待機状態で作成
        */
        bool create();

        /// スレッドID取得
        pthread_t getID() const
        {
            return pthread_self();
        }

        bool valid() const{ return 0 != handle_;}

        /// 開始
        void start();

        /// 再開
        void resume();

        /// 一時停止
        void suspend();

        /// 終了
        void stop();

        /// 強制終了
        void terminate();

        /**
        @brief 終了を待つ
        @return 結果
        @param timeout ... タイムアウト時間（ミリ秒）
        */
        thread::WaitStatus join(u32 timeout=Infinite);

    protected:
        /// スレッド終了すべきか
        bool canRun();

        /// 処理本体
        virtual void run() =0;

    private:
        friend class MultipleWait;

        void release();

        static void* proc(void* args);

        Thread(const Thread&);
        Thread& operator=(const Thread&);

        pthread_t handle_;
        CriticalSection cs_;
#ifndef LCORE_THREAD_HAS_PTHREAD_TIMEDJOIN
        static const s32 State_None = 0;
        static const s32 State_Proc = 1;
        static const s32 State_Term = 2;
        s32 getState();
        void setState(s32 state);
        s32 state_;
#endif
        bool canRun_;
        bool suspend_;
    };
#endif

    //----------------------------------------------------
    //---
    //--- ThreadPool
    //---
    //----------------------------------------------------
    class ThreadPool
    {
    public:
        static const s32 InvalidJobId = -1;
        typedef void(*JobProc)(u32 threadId, s32 jobId, void* data);

        enum WaitStatus
        {
            WaitStatus_Success = 0,
            WaitStatus_Timeout,
        };

        ThreadPool(s32 maxThreads, s32 maxJobs);
        ~ThreadPool();

        void start();

        s32 add(JobProc proc, void* data);

        WaitStatus waitAllFinish(u32 timeout);
        //WaitStatus busyWaitAllFinish(u32 timeout);
        WaitStatus waitFinish(s32 numJobs, u32 timeout);
        WaitStatus waitJobFinish(s32 jobId, u32 timeout);

        s32 getNumMaxJobs() const{ return maxJobs_; }
        s32 getNumMaxThreads() const{ return maxThreads_; }

        s32 getNumFreeJobs();
        s32 getNumPendingJobs();
        s32 getNumActiveJobs();
        void getNumJobs(s32& numFree, s32& numPending, s32& numActive);

        bool removePendingJob(s32 jobId);
        void clearPendingJobs();

        u32 getThreadId(s32 index) const;
    private:
        ThreadPool(const ThreadPool&);
        ThreadPool& operator=(const ThreadPool&);

        struct Job
        {
            s32 jobId_;
            Job* next_;
            JobProc proc_;
            void* data_;
        };

        class WorkerThread : public ThreadRaw
        {
        public:
            explicit WorkerThread(ThreadPool* threadPool);
            virtual ~WorkerThread();

            ThreadPool* threadPool_;

            static void proc(u32 threadId, void* data);
        };

        friend class WorkerThread;

        bool canRun();
        void setCanRun(bool value);

        void waitJobAvailable();

        Job* popJob();
        void pushJob(Job* job);

        Job* popPendingJob();
        void pushPendingJob(Job* job);

        bool canRun_;
        CriticalSection localCS_;
        Semaphore jobSemaphore_;

        s32 maxJobs_;
        s32 numPendigJobs_;
        s32 numActiveJobs_;
        Job* jobs_;
        Job* freeJobs_;
        Job* pendingJobs_;

        s32 maxThreads_;
        WorkerThread** threads_;
    };

    //----------------------------------------------------
    //---
    //--- ThreadAffinity
    //---
    //----------------------------------------------------
    class ThreadAffinity
    {
    public:
        static const s32 MaxCores = 32;
        static const u8 Flag_None = 0;
        static const u8 Flag_Occupied = 0x01U;
        static const u8 MaxAllocated = 0x7F;

        ThreadAffinity();
        ~ThreadAffinity();

        /**
        */
        void initialize();
        /**
        */
        inline const CPUInformation& getCPUInformation() const;

        /**
        */
        s32 setAffinity(LHANDLE thread, bool occupy);

        /**
        */
        s32 setAffinity(ThreadRaw& thread, bool occupy);

        /**
        */
        s32 setAffinity(Thread& thread, bool occupy);
    private:
        ThreadAffinity(const ThreadAffinity&) = delete;
        ThreadAffinity(ThreadAffinity&&) = delete;
        ThreadAffinity& operator=(const ThreadAffinity&) = delete;
        ThreadAffinity& operator=(ThreadAffinity&&) = delete;

        CPUInformation cpuInformation_;
        RandXorshift128Plus32 random_;
        u8 flags_[MaxCores];
    };

    inline const CPUInformation& ThreadAffinity::getCPUInformation() const
    {
        return cpuInformation_;
    }
}

#endif //INC_LCORE_THREAD_H_


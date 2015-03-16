/**
@file Coroutine.cpp
@author t-sakai
@date 2015/03/11 create
*/
#include "Coroutine.h"
#include <stdint.h>
#include <Windows.h>
#if defined(_MSC_VER)
#include <intrin.h>
#else
#endif

namespace lcore
{
namespace
{
#if defined(_MSC_VER)

#define LCORE_THREAD __declspec(thread)

#if defined(_M_AMD64) || defined(_M_X64)
    inline void* getFiber()
    {
        return reinterpret_cast<void*>(__readgsqword(0x20));
    }
#elif defined(_M_IX86)
    inline void* getFiber()
    {
        return reinterpret_cast<void*>(__readfsdword(0x10));
    }
#elif defined( 	_M_ARM)

#endif

#elif defined(__GNUC__)

#define LCORE_THREAD __thread

#if defined(__amd64__) || defined(__x86_64__)
    inline void* getFiber()
    {
        void* fiberData;
        __asm__
        (
            "movq %%gs:0x20,%0"
            : "=r" (fiberData)
        );
        return fiberData;
    }
#elif defined(__i386__)
    inline void* getFiber()
    {
        void* fiberData;
        __asm__
        (
            "movl %%fs:0x10,%0"
            : "=r" (fiberData)
        );
        return fiberData;
    }
#elif defined(__arm__)
#endif

#endif

    inline bool currentIsFiber()
    {
        void* fiberData = getFiber();
        return (NULL != fiberData) && (fiberData != reinterpret_cast<void*>(0x00001E00));
    }


    class CoroutineAllocator
    {
    public:
        static const u32 BufferSize = 4096;

        CoroutineAllocator();
        ~CoroutineAllocator();

        Coroutine* allocate();
        void deallocate(Coroutine* coroutine);
    private:
        union Entry
        {
            Coroutine coroutine_;
            Entry* next_;
        };

        struct Buffer
        {
            Buffer* next_;
        };

        void create();

        Entry* top_;
        Buffer* buffer_;
    };

    CoroutineAllocator::CoroutineAllocator()
        :top_(NULL)
        ,buffer_(NULL)
    {
    }

    CoroutineAllocator::~CoroutineAllocator()
    {
        Buffer* buffer = buffer_;
        while(NULL != buffer){
            Buffer* next = buffer->next_;
            free(buffer);
            buffer = next;
        }
        buffer_ = NULL;
        top_ = NULL;
    }

    Coroutine* CoroutineAllocator::allocate()
    {
        if(NULL == top_){
            create();
        }
        Coroutine* coroutine = &top_->coroutine_;
        top_ = top_->next_;
        return coroutine;
    }

    void CoroutineAllocator::deallocate(Coroutine* coroutine)
    {
        Entry* entry = reinterpret_cast<Entry*>(coroutine);
        entry->next_ = top_;
        top_ = entry;
    }

    void CoroutineAllocator::create()
    {
        uintptr_t ptr = reinterpret_cast<uintptr_t>(LIME_MALLOC(BufferSize));
        Buffer* buffer = reinterpret_cast<Buffer*>(ptr);
        buffer->next_ = buffer_;
        buffer_ = buffer;

        u32 count = (BufferSize-sizeof(Buffer))/sizeof(Entry);
        Entry* entries = reinterpret_cast<Entry*>(ptr + sizeof(Buffer));
        for(u32 i=1; i<count; ++i){
            entries[i-1].next_ = &entries[i];
        }
        entries[count-1].next_ = NULL;
        top_ = entries;
    }

    LCORE_THREAD CoroutineAllocator* coroutineAllocator_ = NULL;
    LCORE_THREAD Coroutine* threadCoroutine_ = NULL;
    LCORE_THREAD volatile Coroutine* currentCoroutine_ = NULL;
}

    void __stdcall FiberRoutine(void* param)
    {
        Coroutine* context = reinterpret_cast<Coroutine*>(param);
        context->status_ = Coroutine_Exit;
        context->func_(*context);
    }

    void Coroutine::initialize()
    {
        if(NULL != threadCoroutine_){
            return;
        }
        coroutineAllocator_ = LIME_NEW CoroutineAllocator();
        threadCoroutine_ = coroutineAllocator_->allocate();
        threadCoroutine_->context_ = ConvertThreadToFiber(NULL);
        threadCoroutine_->func_ = NULL;
        threadCoroutine_->userData_ = NULL;
        currentCoroutine_ = threadCoroutine_;
    }

    void Coroutine::terminate()
    {
        if(NULL == threadCoroutine_){
            return;
        }

        threadCoroutine_->context_ = NULL;
        coroutineAllocator_->deallocate(threadCoroutine_);
        threadCoroutine_ = NULL;
        LIME_DELETE(coroutineAllocator_);
    }

    Coroutine* Coroutine::create(u32 stackSize, CoroutineFunc func, void* userData)
    {
        LASSERT(0<stackSize);
        LASSERT(NULL != func);

        Coroutine* context = coroutineAllocator_->allocate();
        context->func_ = func;
        context->userData_ = userData;

        context->context_ = CreateFiber(stackSize, (LPFIBER_START_ROUTINE)FiberRoutine, context);
        if(NULL == context->context_){
            coroutineAllocator_->deallocate(context);
            return NULL;
        }
        return context;
    }

    void Coroutine::destroy(Coroutine*& coroutine)
    {
        if(NULL == coroutine){
            return;
        }
        coroutine->~Coroutine();
        coroutineAllocator_->deallocate(coroutine);
        coroutine = NULL;
    }

    void Coroutine::destroy()
    {
        if(NULL != context_){
            DeleteFiber(context_);
            context_ = NULL;
        }
    }

    bool Coroutine::valid() const
    {
        return NULL != context_;
    }

    s32 Coroutine::call()
    {
        currentCoroutine_ = this;
        SwitchToFiber(context_);
        return status_;
    }

    void Coroutine::yeild()
    {
        status_ = Coroutine_Continue;
        threadCoroutine_->call();
    }

    void Coroutine::exit()
    {
        status_ = Coroutine_Exit;
        threadCoroutine_->call();
    }
}

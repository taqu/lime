#ifndef INC_LCORE_COROUTINE_H__
#define INC_LCORE_COROUTINE_H__
/**
@file Coroutine.h
@author t-sakai
@date 2015/03/11 create
*/
#include "lcore.h"

namespace lcore
{
    class Coroutine;

    typedef void* PCONTEXT;
    typedef void(*CoroutineFunc)(Coroutine& coroutine);

    static const s32 Coroutine_Exit = 0;
    static const s32 Coroutine_Continue = 1;

    extern void __stdcall FiberRoutine(void* param);

    class Coroutine
    {
    public:
        static void initialize();
        static void terminate();

        static Coroutine* create(u32 stackSize, CoroutineFunc func, void* userData);
        static void destroy(Coroutine*& context);

        bool valid() const;

        s32 call();
        void yeild();
        void exit();

        template<class T>
        T* getUserData(){ reinterpret_cast<T*>(userData_); }
    private:
        void destroy();

        //friend void getContext(Context& context);
        //friend void setContext(const Context& context);
        //friend void swapContext(Context& fromContext, Context& toContext);
        //friend void makeContext(Context& context, unsigned int stackSize, ContextFunc func, void* userData);
        friend void __stdcall FiberRoutine(void* param);

        PCONTEXT context_;
        CoroutineFunc func_;
        void* userData_;
        volatile s32 status_;
    };
}
#endif //INC_LCORE_COROUTINE_H__

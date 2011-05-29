#ifndef INC_LCORE_H__
#define INC_LCORE_H__
/**
@file lcore.h
@author t-sakai
@date 2009/01/17 create
*/
#define NOMINMAX
#include <assert.h>
#include <limits.h>
#include <float.h>
#include <utility>
#include <new>

//-------------------
#if defined(_WIN32)

#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#endif //_DEBUG

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif //_WIN32

//-------------------
#if defined(ANDROID) || defined(__linux__)
#include <stdint.h>
#endif //ANDROID __linux__

//-------------------
#if defined(ANDROID)
#include <android/log.h>
#endif //ANDROID


#include "LangSpec.h"

// メモリ確保・開放
//-------------------
#define LIME_NEW new
#define LIME_PLACEMENT_NEW(ptr) new(ptr)
#define LIME_DELETE(p) { delete p; (p)=NULL;}
#define LIME_DELETE_NONULL delete
#define LIME_OPERATOR_NEW ::operator new
#define LIME_OPERATOR_DELETE ::operator delete

#define LIME_DELETE_ARRAY(p) {delete[] (p); (p) = NULL;}

#define LIME_MALLOC(size) (malloc(size))
#define LIME_FREE(mem) {free(mem); mem = NULL;}

/// 16バイトアライメント変数指定
#define LIME_ALIGN16 _declspec(align(16))

/// アライメント指定malloc
#define LIME_ALIGNED_MALLOC(size, align) (_aligned_malloc(size, align))

/// アライメント指定free
#define LIME_ALIGNED_FREE(mem) (_aligned_free(mem))

// 例外
//-------------------
#define LIME_THROW0 throw()

// Assertion
//-------------------
#if defined(_DEBUG)

#if defined(ANDROID)
#define LASSERT(expression) {if((expression)==false){__android_log_assert("assert", "lime", "%s (%d)", __FILE__, __LINE__);}}while(0)

#elif defined(__linux__)
#define LASSERT(expression) ( assert(expression) )

#else
#define LASSERT(expression) ( assert(expression) )
#endif

#else
#define LASSERT(expression)
#endif


//ユーティリティ
#define LCORE_BIT(n) (0x00000001U<<n)

namespace lcore
{


#ifdef _WIN32
    typedef char Char;
    typedef __int8 s8;
    typedef __int16 s16;
    typedef __int32 s32;
    typedef __int64 s64;

    typedef unsigned __int8 u8;
    typedef unsigned __int16 u16;
    typedef unsigned __int32 u32;
    typedef unsigned __int64 u64;

    typedef float f32;
    typedef double f64;

    typedef ptrdiff_t  ptrdiff_t;

    typedef HINSTANCE HMODULE;

#elif defined(ANDROID) || defined(__linux__)
    typedef char Char;
    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef float f32;
    typedef double f64;

    typedef ptrdiff_t  ptrdiff_t;

    typedef void* HMODULE;

#else
    typedef char Char;
    typedef char s8;
    typedef short s16;
    typedef long s32;
    typedef long long s64;

    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned long u32;
    typedef unsigned long long u64;

    typedef float f32;
    typedef double f64;

    typedef ptrdiff_t  ptrdiff_t;

    typedef void* HMODULE;
#endif

#define LIME_MAKE_FOURCC(c0, c1, c2, c3)\
    ( (lcore::u32)(c0) | ((lcore::u32)(c1) << 8) | ((lcore::u32)(c2) << 16) | ((lcore::u32)(c3) << 24) )

    class LeakCheck
    {
    public:
        LeakCheck();
        ~LeakCheck();

    private:
        void* operator new(size_t);
        void* operator new[](size_t);
    };

    class System
    {
    public:
        System();
        ~System();

    private:
        LeakCheck leakCheck_;
    };

    enum LogLevel
    {
        LogLevel_Info,
        LogLevel_Warn,
        LogLevel_Error,
    };

    void LogImpl(LogLevel level, const char* file, s32 line, const char* message);


#ifdef _DEBUG
#define Debug_LogInfo(message) {lcore::LogImpl(lcore::LogLevel_Info, __FILE__, __LINE__, message);}
#define Debug_LogWarn(message) {lcore::LogImpl(lcore::LogLevel_Warn, __FILE__, __LINE__, message);}
#define Debug_LogError(message) {lcore::LogImpl(lcore::LogLevel_Error, __FILE__, __LINE__, message);}
#define LASSERT_MSG(exp, message) {if(!(exp)){lcore::LogImpl(lcore::LogLevel_Error, __FILE__, __LINE__, message); LASSERT(false);}}

#else
#define Debug_LogInfo(message)
#define Debug_LogWarn(message)
#define Debug_LogError(message)
#define LASSERT_MSG(exp, message)
#endif

//#define LIME_ENABLE_LOG (1)
    void Log(const Char* format, ...);

    template<class T>
    class ScopedPtr
    {
    public:
        ScopedPtr(T* pointer)
            :pointer_(pointer)
        {
        }

        ~ScopedPtr()
        {
            LIME_DELETE(pointer_);
        }

        T* get()
        {
            return pointer_;
        }

        T* release()
        {
            T* tmp = pointer_;
            pointer_ = NULL;
            return tmp;
        }

        T* operator->()
        {
            LASSERT(pointer_ != NULL);
            return pointer_;
        }

        T& operator*() const
        {
            LASSERT(pointer_ != NULL);
            return *pointer_;
        }

        operator bool() const
        {
            return pointer_ != NULL;
        }

        bool operator!() const
        {
            return pointer_ == NULL;
        }
    private:
        // コピー禁止
        explicit ScopedPtr(const ScopedPtr&);
        ScopedPtr& operator=(const ScopedPtr&);

        T *pointer_;
    };

    template<class T>
    class ScopedArrayPtr
    {
    public:
        ScopedArrayPtr(T* pointer)
            :pointer_(pointer)
        {
        }

        ~ScopedArrayPtr()
        {
            LIME_DELETE_ARRAY(pointer_);
        }

        T* get()
        {
            return pointer_;
        }

        T* release()
        {
            T* tmp = pointer_;
            pointer_ = NULL;
            return tmp;
        }

        T& operator[](int index)
        {
            LASSERT(pointer_ != NULL);
            return pointer_[index];
        }

        const T& operator[](int index) const
        {
            LASSERT(pointer_ != NULL);
            return pointer_[index];
        }

        operator bool() const
        {
            return pointer_ != NULL;
        }

        bool operator!() const
        {
            return pointer_ == NULL;
        }
    private:
        // コピー禁止
        explicit ScopedArrayPtr(const ScopedArrayPtr&);
        ScopedArrayPtr& operator=(const ScopedArrayPtr&);

        T *pointer_;
    };

    //---------------------------------------------------------
    //---
    //--- タイム関係
    //---
    //---------------------------------------------------------
    /// CPUクロック取得
    u32 getPerformanceCounter();

    /// マイクロ秒単位で時間取得
    u32 getTimeFromPerformanCounter();

    /// ミリ秒単位の時間を取得
    u32 getTime();


    //---------------------------------------------------------
    //---
    //--- Utility
    //---
    //---------------------------------------------------------
    template<class T>
    inline void swap(T& l, T& r)
    {
        T tmp = l;
        l = r;
        r = tmp;
    }


    template<class Itr>
    struct iterator_traits
    {
        typedef typename Itr::iterator_category iterator_category;
        typedef typename Itr::value_type value_type;
        typedef typename Itr::difference_type difference_type;
        typedef typename Itr::difference_type distance_type;
        typedef typename Itr::pointer pointer;
        typedef typename Itr::reference reference;
    };

    template<class T>
    struct iterator_traits<T*>
    {
        //typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef ptrdiff_t distance_type;	// retained
        typedef T *pointer;
        typedef T& reference;
    };

    template<class FwdIt, class T>
    inline FwdIt lower_bound(FwdIt first, FwdIt last, const T& val)
    {
        typename iterator_traits<FwdIt>::difference_type count = last - first;
        while(0<count){
            typename iterator_traits<FwdIt>::difference_type d = count/2;
            FwdIt m = first + d;
            if(*m<val){
                first = ++m;
                count -= d+1;
            }else{
                count = d;
            }
        }
        return first;
    }


    template<class FwdIt, class T>
    inline FwdIt upper_bound(FwdIt first, FwdIt last, const T& val)
    {
        typename iterator_traits<FwdIt>::difference_type count = last - first;
        while(0<count){
            typename iterator_traits<FwdIt>::difference_type d = count/2;
            FwdIt m = first + d;
            if(*m<=val){
                first = ++m;
                count -= d+1;
            }else{
                count = d;
            }
        }
        return first;
    }

    //---------------------------------------------------------
    //---
    //--- numeric_limits
    //---
    //---------------------------------------------------------
    union LimitsDefine32
    {
        u8 byte_[4];
        f32 float_;
    };

    union LimitsDefine64
    {
        u8 byte_[8];
        f64 double_;
    };


    template<typename T>
    class numeric_limits
    {
    public:
        static T epsilon() LIME_THROW0
        {
            return (T(0));
        }
    };

    // 特殊化
    template<>
    class numeric_limits<f32>
    {
    public:

        static f32 epsilon() LIME_THROW0
        {
            return (FLT_EPSILON);
        }
    };
}

#include "CLibrary.h"
#endif //INC_LCORE_H__

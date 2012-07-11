#ifndef INC_LCORE_H__
#define INC_LCORE_H__
/**
@file lcore.h
@author t-sakai
@date 2009/01/17 create
*/
//#define NOMINMAX
#include <assert.h>
#include <float.h>

//-------------------
#if defined(_WIN32)

#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <malloc.h>
#include <new>

#define LIME_NEW new(_NORMAL_BLOCK,__FILE__,__LINE__)
#define LIME_RAW_NEW new

#else //_DEBUG
#include <malloc.h>
#include <new>

#define LIME_NEW new
#define LIME_RAW_NEW new

#endif

//#if !defined(WIN32_LEAN_AND_MEAN)
//#define WIN32_LEAN_AND_MEAN
//#endif //WIN32_LEAN_AND_MEAN
//#include <windows.h>

#else //_WIN32
#include <malloc.h>
#include <new>
#define LIME_NEW new
#define LIME_RAW_NEW new
#endif

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


#if defined(_WIN32) || defined(_WIN64)
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
    typedef size_t lsize_t;

    typedef void* LHMODULE;

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
    typedef size_t lsize_t;

    typedef void* LHMODULE;

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
    typedef size_t lsize_t;

    typedef void* LHMODULE;
#endif

    //---------------------------------------------------------
    //--- Utility
    //---------------------------------------------------------
    template<class T>
    inline void swap(T& l, T& r)
    {
        T tmp = l;
        l = r;
        r = tmp;
    }

    struct U32F32Union
    {
        union
        {
            u32 u_;
            f32 f_;
        };
    };

    u16 toBinary16Float(f32 f);

    f32 fromBinary16Float(u16 s);

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


//#define LIME_ENABLE_LOG (1)
    void Log(const Char* format, ...);
}

#endif //INC_LCORE_H__

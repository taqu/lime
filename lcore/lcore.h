#ifndef INC_LCORE_H_
#define INC_LCORE_H_
/**
@file lcore.h
@author t-sakai
@date 2009/01/17 create
*/

//#define NOMINMAX
#include <cassert>
#include <cstdint>
#include <cfloat>
#include <malloc.h>
#include <cstring>
#include <cctype>
#include <limits>
#include <new>

//#define LCORE_DISABLE_F16C

#if defined(ANDROID) || defined(__GNUC__)
#include <time.h>
#include <cmath>
#include <utility>
#endif //ANDROID __GNUC__

#if defined(ANDROID)
#include <android/log.h>
#endif //ANDROID

//#include <mmintrin.h>  //MMX intrinsics
//#include <xmmintrin.h> //SSE intrinsics
//#include <emmintrin.h> //SSE2 intrinsics
#include <immintrin.h> //AVX intrinsics

//-------------------
void* operator new(std::size_t size);
void operator delete(void* ptr);
void* operator new[](std::size_t size);
void operator delete[](void* ptr);

//-------------------
void* operator new(std::size_t size, const char* file, int line);
void operator delete(void* ptr, const char* file, int line);
void* operator new[](std::size_t size, const char* file, int line);
void operator delete[](void* ptr, const char* file, int line);

//-------------------
#ifdef _DEBUG
#    define LNEW new(__FILE__,__LINE__)
#    define LNEW_RAW new
#else //_DEBUG
#    define LNEW new
#    define LNEW_RAW new
#endif

//C++98 199711L
//C++11 201103L
#ifdef __cplusplus
#   if 201103L<=__cplusplus || 1900<=_MSC_VER
#       define LCPP11 1
#   endif
#endif

#ifdef NULL
#   ifdef __cplusplus
#       ifdef LCPP11
#           undef NULL
#           define NULL nullptr
#       endif
#   endif
#else
#   ifdef __cplusplus
#       ifdef LCPP11
#           define NULL nullptr
#       else
#           define NULL 0
#       endif
#   else
#       define NULL ((void*)0)
#   endif
#endif


// Exception
//-------------------
#ifdef __cplusplus
#   ifdef LCPP11
#       define LNOTHROW noexcept
#   else
#       define LNOTHROW throw()
#   endif
#else
#    define LNOTHROW
#endif

// メモリ確保・開放
//-------------------
void* lcore_malloc(std::size_t size);
void* lcore_malloc(std::size_t size, std::size_t alignment);
void* lcore_realloc(void* ptr, size_t size);

void lcore_free(void* ptr);
void lcore_free(void* ptr, std::size_t alignment);

void* lcore_malloc(std::size_t size, const char* file, int line);
void* lcore_malloc(std::size_t size, std::size_t alignment, const char* file, int line);

/// 16バイトアライメント変数指定
#ifdef _MSC_VER
#define LALIGN16 __declspec(align(16))
#define LALIGN(a) __declspec(align(a))
#define LALIGN_VAR16(type,x) __declspec(align(16)) type x
#define LALIGN_VAR(a,type,x) __declspec(align(a)) type x
#else
#define LALIGN16 __attribute__((aligned(16)))
#define LALIGN(a) __attribute__((aligned(a)))
#define LALIGN_VAR16(type,x) type x __attribute__((aligned(16)))
#define LALIGN_VAR(a,type,x) type x __attribute__((aligned(a)))
#endif

static const uintptr_t LALIGN16_MASK = (0xFU);

#if defined(_DEBUG)

#define LPLACEMENT_NEW(ptr) new(ptr)
#define LDELETE(ptr) delete (ptr); (ptr)=NULL
#define LDELETE_RAW(ptr) delete (ptr)

#define LDELETE_ARRAY(ptr) delete[] (ptr); (ptr)=NULL

#define LMALLOC(size) lcore_malloc(size, __FILE__, __LINE__)
#define LMALLOC_DEBUG(size, file, line) lcore_malloc(size, file, line)
#define LFREE(ptr) lcore_free(ptr); (ptr)=NULL
#define LFREE_RAW(ptr) lcore_free(ptr)
#define LREALLOC(ptr, size) lcore_realloc(ptr, size)

/// アライメント指定malloc
#define LALIGNED_MALLOC(size, align) lcore_malloc(size, align, __FILE__, __LINE__)
#define LALIGNED_MALLOC_DEBUG(size, align, file, line) lcore_malloc(size, align, file, line)
/// アライメント指定free
#define LALIGNED_FREE(ptr, align) lcore_free(ptr, align); (ptr)=NULL
#define LALIGNED_FREE_RAW(ptr, align) lcore_free(ptr, align)

#else //defined(_DEBUG)

#define LPLACEMENT_NEW(ptr) new(ptr)
#define LDELETE(ptr) delete ptr; (ptr)=NULL
#define LDELETE_RAW(ptr) delete (ptr)

#define LDELETE_ARRAY(ptr) delete[] (ptr); (ptr)=NULL

#define LMALLOC(size) lcore_malloc(size)
#define LMALLOC_DEBUG(size, file, line) lcore_malloc(size)
#define LFREE(ptr) lcore_free(ptr); (ptr)=NULL
#define LFREE_RAW(ptr) lcore_free(ptr)
#define LREALLOC(ptr, size) lcore_realloc(ptr, size)

/// アライメント指定malloc
#define LALIGNED_MALLOC(size, align) lcore_malloc(size, align)
#define LALIGNED_MALLOC_DEBUG(size, align, file, line) lcore_malloc(size, align)
/// アライメント指定free
#define LALIGNED_FREE(ptr, align) lcore_free(ptr, align); (ptr)=NULL
#define LALIGNED_FREE_RAW(ptr, align) lcore_free(ptr, align)
#endif


// Assertion
//-------------------
#if defined(_DEBUG)

#   if defined(ANDROID)
#       define LASSERT(expression) {if(false == (expression)){__android_log_assert("assert", "lime", "%s (%d)", __FILE__, __LINE__);}}while(0)

#   elif defined(__GNUC__)
#       define LASSERT(expression) assert(expression)
#   else
#      define LASSERT(expression) assert(expression)
#   endif
#else
#   define LASSERT(expression)
#endif


// Memory Debug Break
//-------------------
#if defined(_DEBUG) && defined(_MSC_VER)
#define LCORE_DEBUG_MEMORY_BREAK
#endif

#ifdef LCORE_DEBUG_MEMORY_BREAK
void lcore_setMallocBreak(unsigned int id);
#else
#define lcore_setMallocBreak(id)
#endif

#ifdef _MSC_VER
#define LRESTRICT __restrict
#else
#define LRESTRICT restrict
#endif

// String Literal
//-------------------
#define TEXT_C(quote) quote
#define TEXT_U8(quote) u8##quote
#define TEXT_W(quote) L##quote
#define TEXT_U16(quote) u##quote
#define TEXT_U32(quote) U##quote

namespace lcore
{
    typedef char Char;
    typedef unsigned char UChar;
    typedef char16_t Char16;
    typedef wchar_t WChar;
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

    typedef intptr_t  intptr_t;
    typedef uintptr_t  uintptr_t;
    typedef ptrdiff_t  ptrdiff_t;
    typedef size_t size_t;
    typedef s64 off_t;

    typedef void* LHANDLE;

#if defined(ANDROID) || defined(__GNUC__)
    typedef clock_t ClockType;
#else
    typedef u64 ClockType;
#endif

#if defined(ANDROID)
    static constexpr f32 F32_EPSILON = 1.192092896e-07F;
    static constexpr f32 F64_EPSILON = 2.2204460492503131e-016;
#else
    static constexpr f32 F32_EPSILON = FLT_EPSILON;
    static constexpr f32 F64_EPSILON = DBL_EPSILON;
#endif

    static constexpr f32 F32_ANGLE_COSINE_ALMOST_ONE = 0.9999f;
    static constexpr f32 F32_DOT_EPSILON = 1.0e-6f;

    static constexpr f32 PI = static_cast<f32>(3.14159265358979323846);
    static constexpr f32 PI2 = static_cast<f32>(6.28318530717958647692);
    static constexpr f32 INV_PI = static_cast<f32>(0.31830988618379067153);
    static constexpr f32 INV_PI2 = static_cast<f32>(0.15915494309189533576);
    static constexpr f32 PI_2 = static_cast<f32>(1.57079632679489661923);
    static constexpr f32 INV_PI_2 = static_cast<f32>(0.63661977236758134308);
    static constexpr f32 LOG2 = static_cast<f32>(0.693147180559945309417);
    static constexpr f32 INV_LOG2 = static_cast<f32>(1.0/0.693147180559945309417);

    static constexpr f64 PI_64 = static_cast<f64>(3.14159265358979323846);
    static constexpr f64 PI2_64 = static_cast<f64>(6.28318530717958647692);
    static constexpr f64 INV_PI_64 = static_cast<f64>(0.31830988618379067153);
    static constexpr f64 INV_PI2_64 = static_cast<f64>(0.15915494309189533576);
    static constexpr f64 PI_2_64 = static_cast<f64>(1.57079632679489661923);
    static constexpr f64 INV_PI_2_64 = static_cast<f64>(0.63661977236758134308);
    static constexpr f64 LOG2_64 = static_cast<f64>(0.693147180559945309417);
    static constexpr f64 INV_LOG2_64 = static_cast<f64>(1.0/0.693147180559945309417);

    static constexpr f32 DEG_TO_RAD = static_cast<f32>(1.57079632679489661923/90.0);
    static constexpr f32 RAD_TO_DEG = static_cast<f32>(90.0/1.57079632679489661923);

    static const s32 SSE_ALIGN = 16;
    static const s32 SSE_ALIGN_MASK = 15;
    static const Char CharNull = '\0';
    static const Char CharLF = '\n'; //Line Feed
    static const Char CharCR = '\r'; //Carriage Return
    static const Char PathDelimiter = '/';

    //---------------------------------------------------------
    //---
    //---
    //---
    //---------------------------------------------------------
    using std::move;
    using std::forward;
    using std::is_pod;
    using std::true_type;
    using std::false_type;
    using std::declval;

#define LSTATIC_ASSERT(exp, message) static_assert(exp, message)

    //---------------------------------------------------------
    //---
    //--- Allocator Function
    //---
    //---------------------------------------------------------
    typedef void*(*AllocFunc)(u32 size);
    typedef void*(*AllocFuncDebug)(u32 size, const Char* file, int line);
    typedef void(*FreeFunc)(void* ptr);

    typedef void*(*AlignedAllocFunc)(u32 size, u32 alignment);
    typedef void*(*AlignedAllocFuncDebug)(u32 size, u32 alignment, const Char* file, int line);
    typedef void(*AlignedFreeFunc)(void* ptr, u32 alignment);

    struct DefaultAllocator
    {
#if defined(_DEBUG)
        static inline void* malloc(u32 size, const char* file, int line)
        {
            return LMALLOC_DEBUG(size, file, line);
        }

        static inline void* malloc(u32 size, u32 alignment, const char* file, int line)
        {
            return LALIGNED_MALLOC_DEBUG(size, alignment, file, line);
        }
#else
        static inline void* malloc(u32 size, const char* /*file*/, int /*line*/)
        {
            return LMALLOC(size);
        }

        static inline void* malloc(u32 size, u32 alignment, const char* /*file*/, int /*line*/)
        {
            return LALIGNED_MALLOC(size, alignment);
        }

        static inline void* malloc(u32 size)
        {
            return LMALLOC(size);
        }

        static inline void* malloc(u32 size, u32 alignment)
        {
            return LALIGNED_MALLOC(size, alignment);
        }
#endif
        static inline void free(void* mem)
        {
            LFREE_RAW(mem);
        }
        static inline void free(void* mem, u32 alignment)
        {
            LALIGNED_FREE_RAW(mem, alignment);
        }
    };

#if defined(_DEBUG)
#define LALLOCATOR_MALLOC(allocator, size) allocator::malloc(size, __FILE__, __LINE__)
#define LALLOCATOR_ALIGNED_MALLOC(allocator, size, alignment) allocator::malloc(size, alignment, __FILE__, __LINE__)
#else
#define LALLOCATOR_MALLOC(allocator, size) allocator::malloc(size)
#define LALLOCATOR_ALIGNED_MALLOC(allocator, size, alignment) allocator::malloc(size, alignment)
#endif

#define LALLOCATOR_FREE(allocator, ptr) allocator::free((ptr));(ptr)=NULL
#define LALLOCATOR_ALIGNED_FREE(allocator, ptr, alignment) allocator::free((ptr), alignment);(ptr)=NULL

    template<class T>
    inline T* construct(void* ptr)
    {
        return LPLACEMENT_NEW(ptr) T();
    }

    template<class T>
    struct ConstructImpl
    {
        inline static T* construct(void* ptr, const T& x)
        {
            return LPLACEMENT_NEW(ptr) T(x);
        }

        inline static T* construct(void* ptr, T&& x)
        {
            return LPLACEMENT_NEW(ptr) T(x);
        }
    };

    template<class T>
    struct ConstructImpl<T*>
    {
        inline static T** construct(void* ptr, const T* x)
        {
            return LPLACEMENT_NEW(ptr) T*(const_cast<T*>(x));
        }
    };
    template<class T>
    inline T* construct(void* ptr, const T& x)
    {
        return ConstructImpl<T>::construct(ptr, x);
    }


    //---------------------------------------------------------
    //---
    //--- numeric_limits
    //---
    //---------------------------------------------------------
    template<typename T>
    class numeric_limits
    {
    public:
        static constexpr T epsilon() LNOTHROW
        {
            return std::numeric_limits<T>::epsilon();
        }

        static constexpr T minimum() LNOTHROW
        {
            return (std::numeric_limits<T>::min)();
        }

        static constexpr T maximum() LNOTHROW
        {
            return (std::numeric_limits<T>::max)();
        }

        static constexpr T lowest() LNOTHROW
        {
            return std::numeric_limits<T>::lowest();
        }

        static constexpr T inifinity() LNOTHROW
        {
            return std::numeric_limits<T>::infinity();
        }
    };

    static constexpr f32 LINFINITY = numeric_limits<f32>::maximum();

    //---------------------------------------------------------
    //---
    //--- Utility
    //---
    //---------------------------------------------------------
    #define LBIT32(n) (0x00000001U<<n)

    union UnionS32F32
    {
        s32 s32_;
        f32 f32_;
    };

    union UnionU32F32
    {
        u32 u32_;
        f32 f32_;
    };

    union UnionS64F64
    {
        s64 s64_;
        f64 f64_;
    };

    union UnionU64F64
    {
        u64 u64_;
        f64 f64_;
    };

    template<class T>
    inline T* align16(T* ptr)
    {
        return (T*)(((lcore::uintptr_t)(ptr)+LALIGN16_MASK) & ~LALIGN16_MASK);
    }

    template<class T>
    inline void swap(T& l, T& r)
    {
        T tmp(lcore::move(l));
        l = lcore::move(r);
        r = lcore::move(tmp);
    }

    template<class T, class U>
    inline U lerp(const T& v0, const T& v1, f32 ratio)
    {
        return static_cast<U>(v0 + ratio*(v1-v0));
        //return (1.0f-ratio)*v0 + ratio*v1;
    }

    template<class T>
    inline f32 lerpf(const T& v0, const T& v1, f32 ratio)
    {
        return v0 + ratio*(v1-v0);
    }

#define LCORE_DEFINE_MINMAX_FUNC(TYPE) \
    inline TYPE maximum(TYPE left, TYPE right){ return (left<right)? right : left;}\
    inline TYPE minimum(TYPE left, TYPE right){ return (right<left)? right : left;}\

    LCORE_DEFINE_MINMAX_FUNC(s8)
    LCORE_DEFINE_MINMAX_FUNC(s16)
    LCORE_DEFINE_MINMAX_FUNC(s32)
    LCORE_DEFINE_MINMAX_FUNC(u8)
    LCORE_DEFINE_MINMAX_FUNC(u16)
    LCORE_DEFINE_MINMAX_FUNC(u32)
    LCORE_DEFINE_MINMAX_FUNC(f32)
    LCORE_DEFINE_MINMAX_FUNC(f64)
#undef LCORE_DEFINE_MINMAX_FUNC

    template<class T>
    inline const T& maximum(const T& left, const T& right)
    {
        return (left<right)? right : left;
    }

    template<class T>
    inline const T& minimum(const T& left, const T& right)
    {
        return (left<right)? left : right;
    }

    template<class T>
    inline T clamp(T val, T low, T high)
    {
        if (val <= low) return low;
        else if (val >= high) return high;
        else return val;
    }

    f32 clamp01(f32 v);

    f32 clampRotate0(f32 val, f32 total);
    s32 clampRotate0(s32 val, s32 total);

    inline bool isPow2(u32 x)
    {
        return 0 == (x & (x-1));
    }

    inline bool isPow2(u64 x)
    {
        return 0 == (x & (x-1));
    }

    template<class T>
    inline bool isSameSign(T x0, T x1);

    template<>
    inline bool isSameSign<s8>(s8 x0, s8 x1)
    {
        return 0 == ((x0^x1)&(0x1U<<7));
    }
    template<>
    inline bool isSameSign<s16>(s16 x0, s16 x1)
    {
        return 0 == ((x0^x1)&(0x1U<<15));
    }
    template<>
    inline bool isSameSign<s32>(s32 x0, s32 x1)
    {
        return 0 == ((x0^x1)&(0x1U<<31));
    }
    template<>
    inline bool isSameSign<s64>(s64 x0, s64 x1)
    {
        return 0 == ((x0^x1)&(0x1ULL<<63));
    }

    template<> inline bool isSameSign<u8>(u8, u8){ return true;}
    template<> inline bool isSameSign<u16>(u16, u16){ return true;}
    template<> inline bool isSameSign<u32>(u32, u32){ return true;}
    template<> inline bool isSameSign<u64>(u64, u64){ return true;}

    template<>
    inline bool isSameSign<f32>(f32 x0, f32 x1)
    {
        static const u32 mask = 0x1U<<31;
        UnionU32F32 u0,u1;
        u0.f32_ = x0;
        u1.f32_ = x1;
        u32 t = u0.u32_ ^ u1.u32_;
        return 0 == (t&mask);
    }

    template<>
    inline bool isSameSign<f64>(f64 x0, f64 x1)
    {
        static const u64 mask = 0x1ULL<<63;
        UnionU64F64 u0, u1;
        u0.f64_ = x0;
        u1.f64_ = x1;
        u64 t = u0.u64_ ^ u1.u64_;
        return 0 == (t&mask);
    }

    s32 roundUpPow2(s32 x);
    s64 roundUpPow2(s64 x);

    u32 roundUpPow2(u32 x);
    u64 roundUpPow2(u64 x);

    template<class T>
    T roundUp(const T& dividend, const T& divisor)
    {
        T quotient = dividend / divisor;
        T remainder = dividend - divisor * quotient;
        quotient += (0<remainder)? 1 : 0;
        return quotient;
    }

    //--- SIMD Math
    //-----------------------------------------------------
    __m128 log(const __m128& x);
    __m128 exp(const __m128& x);
    __m128 pow(const __m128& x, const __m128& y);

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

    /**
    @brief x以上の要素が最初に現れる位置を検索
    */
    template<class FwdIt, class T>
    FwdIt lower_bound(FwdIt first, FwdIt last, const T& val)
    {
        typename iterator_traits<FwdIt>::difference_type count = last - first;
        while(0<count){
            typename iterator_traits<FwdIt>::difference_type d = count/2;
            FwdIt m = first + d;
            if(*m<val){
                first = m+1;
                count -= d+1;
            } else{
                count = d;
            }
        }
        return first;
    }

    /**
    @brief x以上の要素が最初に現れる位置を検索
    */
    template<class FwdIt, class T, class LessThan>
    FwdIt lower_bound(FwdIt first, FwdIt last, const T& val, LessThan LT)
    {
        typename iterator_traits<FwdIt>::difference_type count = last - first;
        while(0<count){
            typename iterator_traits<FwdIt>::difference_type d = count/2;
            FwdIt m = first + d;
            if(LT(*m,val)){
                first = m+1;
                count -= d+1;
            } else{
                count = d;
            }
        }
        return first;
    }

    /**
    @brief xより大きい要素が最初に現れる位置を検索
    */
    template<class FwdIt, class T>
    FwdIt upper_bound(FwdIt first, FwdIt last, const T& x)
    {
        typename iterator_traits<FwdIt>::difference_type count = last - first;
        while(0<count){
            typename iterator_traits<FwdIt>::difference_type d = count/2;
            FwdIt m = first + d;
            if(*m<=x){
                first = m+1;
                count -= d+1;
            } else{
                count = d;
            }
        }
        return first;
    }

    /**
    @brief xより大きい要素が最初に現れる位置を検索
    */
    template<class FwdIt, class T, class LessEqual>
    FwdIt upper_bound(FwdIt first, FwdIt last, const T& x, LessEqual LE)
    {
        typename iterator_traits<FwdIt>::difference_type count = last - first;
        while(0<count){
            typename iterator_traits<FwdIt>::difference_type d = count/2;
            FwdIt m = first + d;
            if(LE(*m,x)){
                first = m+1;
                count -= d+1;
            } else{
                count = d;
            }
        }
        return first;
    }

    template<class T>
    s32 indexof(const T* start, const T* item)
    {
        uintptr_t ustart = (uintptr_t)(start);
        uintptr_t uitem = (uintptr_t)(item);
        return static_cast<s32>((uitem-ustart)/sizeof(T));
    }

    bool isLittleEndian();

    template<class T>
    class Nothing
    {
    };

    template<class T>
    struct DefaultComparator
    {
        /**
        v0<v1 : <0
        v0==v1 : 0
        v0>v1 : >0
        */
        s32 operator()(const T& v0, const T& v1) const
        {
            return (v0==v1)? 0 : ((v0<v1)? -1 : 1);
        }
    };

    template<class T>
    struct DefaultTraversal
    {
        void operator()(T& v0)
        {
        }
    };

    /**
    @brief Convert IEEE 754 single precision float to half precision float
    */
    u16 toFloat16(f32 f);

    /**
    @brief Convert IEEE 754 half precision float to single precision float
    */
    f32 toFloat32(u16 s);

    s8 floatTo8SNORM(f32 f);
    u8 floatTo8UNORM(f32 f);
    s16 floatTo16SNORM(f32 f);
    u16 floatTo16UNORM(f32 f);

    inline u8 populationCount(u8 v)
    {
        v = (v & 0x55U) + ((v>>1) & 0x55U);
        v = (v & 0x33U) + ((v>>2) & 0x33U);
        return v = (v & 0x0FU) + ((v>>4) & 0x0FU);
    }

    u32 populationCount(u32 v);

    /**
    */
    u32 mostSignificantBit(u32 val);

    /**
    */
    u32 leastSignificantBit(u32 val);

    u32 bitreverse(u32 x);
    u64 bitreverse(u64 x);

    u32 leadingzero(u32 x);

    #define LMAKE_FOURCC(c0, c1, c2, c3)\
    ( (lcore::u32)(c0) | ((lcore::u32)(c1) << 8) | ((lcore::u32)(c2) << 16) | ((lcore::u32)(c3) << 24) )

    void initializeSystem();
    void terminateSystem();

    void Log(const Char* format, ...);

    class System
    {
    public:
        inline System()
        {
            initializeSystem();
        }
        inline ~System()
        {
            terminateSystem();
        }
    };
    //---------------------------------------------------------
    //---
    //---
    //---
    //---------------------------------------------------------

    template<class T>
    inline T absolute(T val)
    {
        return abs(val);
    }


    template<>
    inline u8 absolute<u8>(u8 val)
    {
        return val;
    }

    template<>
    inline u16 absolute<u16>(u16 val)
    {
        return val;
    }

    template<>
    inline u32 absolute<u32>(u32 val)
    {
        return val;
    }

    template<>
    inline f32 absolute<f32>(f32 val)
    {
        UnionU32F32 u;
        u.f32_ = val;
        u.u32_ &= 0x7FFFFFFFU;
        return u.f32_;
        //return fabs(val);
    }

    template<>
    inline f64 absolute<f64>(f64 val)
    {
        return fabs(val);
    }

    inline bool isPositive(f32 v)
    {
        static const u32 mask = ~(((u32)-1)>>1);
        return (*((u32*)&v) & mask) == 0;
    }

    inline bool isEqual(f32 x1, f32 x2)
    {
        return (lcore::absolute<f32>(x1 - x2) <= F32_EPSILON);
    }

    inline bool isEqual(f32 x1, f32 x2, f32 epsilon)
    {
        return (lcore::absolute<f32>(x1 - x2) <= epsilon);
    }

    inline bool isEqual(f64 x1, f64 x2)
    {
        return (lcore::absolute<f64>(x1 - x2) <= F64_EPSILON);
    }

    inline bool isEqual(f64 x1, f64 x2, f64 epsilon)
    {
        return (lcore::absolute<f64>(x1 - x2) <= epsilon);
    }

    inline bool isZero(f32 x1)
    {
        return (lcore::absolute<f32>(x1) <= F32_EPSILON);
    }

    inline bool isZero(f32 x1, f32 epsilon)
    {
        return (lcore::absolute<f32>(x1) <= epsilon);
    }

    inline bool isZero(f64 x1)
    {
        return (lcore::absolute<f64>(x1) <= F64_EPSILON);
    }

    inline bool isZero(f64 x1, f64 epsilon)
    {
        return (lcore::absolute<f64>(x1) <= epsilon);
    }


    inline bool isZeroPositive(f32 x1)
    {
        LASSERT(0.0f<=x1);
        return (x1 <= F32_EPSILON);
    }

    inline bool isZeroPositive(f32 x1, f32 epsilon)
    {
        LASSERT(0.0f<=x1);
        return (x1 <= epsilon);
    }

    inline bool isZeroPositive(f64 x1)
    {
        LASSERT(0.0f<=x1);
        return (x1 <= F64_EPSILON);
    }

    inline bool isZeroPositive(f64 x1, f64 epsilon)
    {
        LASSERT(0.0f<=x1);
        return (x1 <= epsilon);
    }

    inline bool isZeroNegative(f32 x1)
    {
        LASSERT(x1<=0.0f);
        return (-F32_EPSILON<=x1);
    }

    inline bool isZeroNegative(f32 x1, f32 epsilon)
    {
        LASSERT(x1<0.0f);
        return (epsilon<=x1);
    }

    inline bool isZeroNegative(f64 x1)
    {
        LASSERT(x1<0.0f);
        return (-F64_EPSILON<=x1);
    }

    inline bool isZeroNegative(f64 x1, f64 epsilon)
    {
        LASSERT(x1<0.0f);
        return (epsilon<=x1);
    }

    inline bool isNan(f32 f)
    {
        return std::isnan(f);
    }

    inline bool isNan(f64 f)
    {
        return std::isnan(f);
    }

    inline bool isSpace(Char c)
    {
        return (isspace(c) != 0);
    }

    inline bool isNullChar(Char c)
    {
        return (c == CharNull);
    }

    inline bool isAscii(Char c)
    {
        return isascii(c);
    }

    inline bool isAlnum(Char c)
    {
        return 0 != isalnum(c);
    }

    inline bool isAlpha(Char c)
    {
        return 0 != isalpha(c);
    }

    inline bool isAsciiNum(Char c)
    {
        return '0'<=c && c<='9';
    }

    inline int isEOL(const Char* str)
    {
        if(0x0D == *str){
            return (0x0A == *(str+1))? 2 : 1;
        }else if(0x0A == *str){
            return 1;
        }
        return 0;
    }

    inline bool isNullChar(Char16 c)
    {
        return (c == CharNull);
    }

    inline int strncmp(const Char* str1, const Char* str2, size_t maxCount)
    {
        return ::strncmp(str1, str2, maxCount);
    }

    inline char* strncpy(Char* dst, size_t dstSize, const Char* src, size_t count)
    {
#if defined(_WIN32)
        strncpy_s(dst, dstSize, src, count);
        return dst;
#else
        return ::strncpy(dst, src, count);
#endif
    }

    inline size_t strlen(const Char* str)
    {
        return ::strlen(str);
    }

    inline size_t strnlen(const Char* str, size_t size)
    {
        return ::strnlen(str, size);
    }

    inline s32 strlen_s32(const Char* str)
    {
        return static_cast<s32>(::strlen(str));
    }

    inline s32 strnlen_s32(const Char* str, size_t size)
    {
        return static_cast<s32>(::strnlen(str, size));
    }


    inline const Char* strstr(const Char* str, const Char* key)
    {
        return ::strstr(str, key);
    }

    inline void strcat(Char* dst, u32 dstSize, const Char* str)
    {
#if defined(_WIN32)
        ::strcat_s(dst, dstSize, str);
#else
        ::strcat(dst, str);
#endif
    }


    inline s32 memcmp(const void* buff0, const void* buff1, size_t size)
    {
        return ::memcmp(buff0, buff1, size);
    }

    inline void* memset(void* dst, int val, size_t size)
    {
        return ::memset(dst, val, size);
    }

    inline void* memcpy(void* dst, const void* src, size_t size)
    {
        return ::memcpy(dst, src, size);
    }

    //---------------------------------------------------------
    //---
    //--- Color
    //---
    //---------------------------------------------------------
    u8 toColorU8(f32 x);
    f32 toColorF32(u8 x);

    u32 getARGB(u8 a, u8 r, u8 g, u8 b);
    u32 getABGR(u8 a, u8 r, u8 g, u8 b);
    u32 getRGBA(u8 a, u8 r, u8 g, u8 b);


    u8 getAFromARGB(u32 argb);
    u8 getRFromARGB(u32 argb);
    u8 getGFromARGB(u32 argb);
    u8 getBFromARGB(u32 argb);


    u8 getAFromABGR(u32 abgr);
    u8 getRFromABGR(u32 abgr);
    u8 getGFromABGR(u32 abgr);
    u8 getBFromABGR(u32 abgr);

    u8 getRFromRGBA(u32 rgba);
    u8 getGFromRGBA(u32 rgba);
    u8 getBFromRGBA(u32 rgba);
    u8 getAFromRGBA(u32 rgba);

    u8 getLuminance(u8 r, u8 g, u8 b);
    f32 getLuminance(f32 r, f32 g, f32 b);

    inline u8 getGray(u8 r, u8 g, u8 b)
    {
        return getLuminance(r, g, b);
    }

    inline f32 getGray(f32 r, f32 g, f32 b)
    {
        return getLuminance(r, g, b);
    }

    u16 getARGB4444(u32 argb);
    u16 getABGR4444(u32 abgr);
    u16 getRGBA4444(u32 rgba);

    u32 getARGB4444(u8 a, u8 r, u8 g, u8 b);
    u32 getABGR4444(u8 a, u8 r, u8 g, u8 b);
    u32 getRGBA4444(u8 a, u8 r, u8 g, u8 b);

    u32 toSRGB_NOSIMD(u32 rgba);
    void toSRGB_NOSIMD(f32* drgba, const f32* srgba);

    u32 toLinear_NOSIMD(u32 rgba);
    void toLinear_NOSIMD(f32* drgba, const f32* srgba);

    u32 toSRGB_SIMD(u32 rgba);
    void toSRGB_SIMD(f32* drgba, const f32* srgba);

    u32 toLinear_SIMD(u32 rgba);
    void toLinear_SIMD(f32* drgba, const f32* srgba);

    inline u32 toSRGB(u32 rgba)
    {
        return toSRGB_SIMD(rgba);
    }
    inline void toSRGB(f32* drgba, const f32* srgba)
    {
        toSRGB_SIMD(drgba, srgba);
    }

    inline u32 toLinear(u32 rgba)
    {
        return toLinear_SIMD(rgba);
    }
    inline void toLinear(f32* drgba, const f32* srgba)
    {
        toLinear_SIMD(drgba, srgba);
    }

    u8 toSRGB(u8 x);
    f32 toSRGB(f32 x);
    u8 toLinear(u8 x);
    f32 toLinear(f32 x);

    enum RefractiveIndex
    {
        RefractiveIndex_Vacuum =0,

        //気体
        RefractiveIndex_Air,

        //液体
        RefractiveIndex_Water,
        RefractiveIndex_Sea,

        //個体
        RefractiveIndex_SodiumChloride, //塩化ナトリウム
        RefractiveIndex_Carbon, //炭素
        RefractiveIndex_Silicon, //ケイ素

        //樹脂
        RefractiveIndex_FluorocarbonPolymers, //フッ素樹脂
        RefractiveIndex_SiliconPolymers, //シリコン樹脂
        RefractiveIndex_AcrylicResin, //アクリル樹脂
        RefractiveIndex_Polyethylene, //ポリエチレン
        RefractiveIndex_Polycarbonate, //ポリカーボネート
        RefractiveIndex_Asphalt, //アスファルト

        //ガラス
        RefractiveIndex_Fluorite, //フローライト
        RefractiveIndex_SodaLimeGlass, //ソーダ石灰ガラス
        RefractiveIndex_LeadGlass, //鉛ガラス、クリスタルガラス

        //結晶
        RefractiveIndex_Ice, //氷
        RefractiveIndex_RockCrystal, //水晶
        RefractiveIndex_Peridot, //ペリドット
        RefractiveIndex_Diamond, //ダイヤモンド

        //その他
        RefractiveIndex_Perl, //パール

        RefractiveIndex_Num,
    };

    f32 getRefractiveIndex(RefractiveIndex index);

    /**
    @brief 真空に対するフレネル反射係数の実部
    @param refract ... 出射側媒質の屈折率
    */
    f32 calcFresnelTerm(f32 refract);

    /**
    @brief フレネル反射係数の実部
    @param refract0 ... 入射側媒質の屈折率
    @param refract1 ... 出射側媒質の屈折率
    */
    f32 calcFresnelTerm(f32 refract0, f32 refract1);

    //---------------------------------------------------------
    //---
    //--- ScopedPtr
    //---
    //---------------------------------------------------------
    template<class T>
    class ScopedPtr
    {
    public:
        explicit ScopedPtr(T* pointer)
            :pointer_(pointer)
        {
        }

        ~ScopedPtr()
        {
            LDELETE(pointer_);
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

        operator const T*() const
        {
            return pointer_;
        }

        operator T*()
        {
            return pointer_;
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
        ScopedPtr(const ScopedPtr&);
        ScopedPtr& operator=(const ScopedPtr&);

        T *pointer_;
    };

    template<class T>
    class ScopedArrayPtr
    {
    public:
        explicit ScopedArrayPtr(T* pointer)
            :pointer_(pointer)
        {
        }

        ~ScopedArrayPtr()
        {
            LDELETE_ARRAY(pointer_);
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

        operator void*()
        {
            return pointer_;
        }

        operator const T*() const
        {
            return pointer_;
        }

        operator T*()
        {
            return pointer_;
        }
    private:
        ScopedArrayPtr(const ScopedArrayPtr&);
        ScopedArrayPtr& operator=(const ScopedArrayPtr&);

        T *pointer_;
    };

    //---------------------------------------------------------
    //---
    //--- 文字列操作
    //---
    //---------------------------------------------------------
    s32 printf(const Char* format, ...);
    s32 snprintf(Char* dst, size_t n, const Char* format, ...);

    s32 scprintf(const Char* format, ...);
    s32 vscprintf(const Char* format, va_list args);
    void replace(Char* str, Char dst, Char src);

    /**
    @brief 後方から文字探索
    @return 見つからなければNULL
    @param length ... 文字列の長さ
    @param src ... 入力
    @param c ... 探索文字
    */
    const Char* rFindChr(s32 length, const Char* src, Char c);

    //---------------------------------------------------------
    //---
    //--- Time
    //---
    //---------------------------------------------------------
    void sleep(u32 milliSeconds);

    /// Get performance counter
    ClockType getPerformanceCounter();

    /// Get performance count per second
    ClockType getPerformanceFrequency();

    /// Calculate duration time from performance count
    f64 calcTime64(ClockType prevTime, ClockType currentTime);

    /// Calculate duration time from performance count
    inline f32 calcTime(ClockType prevTime, ClockType currentTime)
    {
        return static_cast<f32>(calcTime64(prevTime, currentTime));
    }

    /// Get time in milli second
    u32 getTimeMilliSec();


    template<bool enable>
    struct Timer
    {
        Timer()
            :time_(0)
            ,count_(0)
            ,totalTime_(0.0f)
        {}

        void begin()
        {
            time_ = getPerformanceCounter();
        }

        void end()
        {
            totalTime_ += calcTime64(time_, getPerformanceCounter());
            ++count_;
        }

        f64 getAverage() const
        {
            return (0 == count_)? 0.0 : totalTime_/count_;
        }

        void reset();

        ClockType time_;
        s32 count_;
        f64 totalTime_;
    };

    template<bool enable>
    void Timer<enable>::reset()
    {
        time_ = 0;
        count_ = 0;
        totalTime_ = 0.0f;
    }

    template<>
    struct Timer<false>
    {
        void begin(){}
        void end(){}
        f64 getAverage() const{return 0.0;}
        void reset(){}
    };

    //---------------------------------------------------------
    //---
    //--- Character Code
    //---
    //---------------------------------------------------------
    /**
    @brief UTF8のバイト数計算
    @return UTF8のバイト数
    @param utf8 ... UTF8文字の先頭文字
    */
    s32 getUTF8Size(Char utf8);

    /**
    @brief UTF8のバイト数計算
    @return UTF8のバイト数
    @param utf16 ... UTF16もじ
    */
    s32 getUTF8Size(Char16 utf16);

    /**
    @brief UTF8 to UTF16
    @return 変換したUTF8のバイト数
    @param utf16 ... 出力。UTF16コード
    @param utf8 ... UTF8文字
    */
    s32 UTF8toUTF16(Char16& utf16, const Char* utf8);

    /**
    @brief UTF16 to UTF8
    @return 変換されたUTF8のバイト数
    @param utf8 ... 出力。UTF8コード
    @param utf16 ... UTF16文字
    */
    s32 UTF16toUTF8(Char* utf8, Char16 utf16);

    void setLocale(const Char* locale);

    /**
    @brief SJIS -> UTF16変換
    @return 成功なら変換した文字数、失敗なら-1
    */
    s32 MBSToWCS(WChar* dst, u32 sizeInWords, const Char* src, u32 srcSize);

    /**
    @brief SJIS -> UTF16変換
    @return 成功なら変換した文字数、失敗なら-1
    */
    s32 MBSToWCS(WChar* dst, u32 sizeInWords, const Char* src);

    //---------------------------------------------------------
    //---
    //--- DLL
    //---
    //---------------------------------------------------------
    class DLL
    {
    public:
        static LHANDLE openDLL(const Char* path);
        static void closeDLL(LHANDLE& handle);
        static void* getProcAddress(LHANDLE handle, const Char* name);

        DLL();
        explicit DLL(const Char* path);
        ~DLL();

        bool open(const Char* path);
        bool is_open() const;

        void close();

        void* getProcAddress(const Char* name);

        void swap(DLL& rhs)
        {
            lcore::swap(handle_, rhs.handle_);
        }

    private:
        DLL(const DLL&);
        DLL& operator=(const DLL&);

        LHANDLE handle_;
    };

    //---------------------------------------------------------
    //---
    //--- Low-Discrepancy
    //---
    //---------------------------------------------------------
    f32 halton(s32 index, s32 prime);
    f32 halton_next(f32 prev, s32 prime);

    f32 vanDerCorput(u32 n, u32 base);

    f32 radicalInverseVanDerCorput(u32 bits, u32 scramble);
    f32 radicalInverseSobol(u32 i, u32 scramble);
    f32 radicalInverseLarcherPillichshammer(u32 i, u32 scramble);

    inline void sobol02(f32& v0, f32& v1, u32 i, u32 scramble0, u32 scramble1)
    {
        v0 = radicalInverseVanDerCorput(i, scramble0);
        v1 = radicalInverseSobol(i, scramble1);
    }

    //---------------------------------------------------------
    //---
    //--- Hash Functions
    //---
    //---------------------------------------------------------
#if 1
    /**
    */
    //u32 hash_Bernstein(const u8* v, u32 count);

    /**
    */
    u32 hash_FNV1(const u8* v, u32 count);

    /**
    */
    u32 hash_FNV1a(const u8* v, u32 count);

    /**
    */
    u64 hash_FNV1_64(const u8* v, u32 count);

    /**
    */
    u64 hash_FNV1a_64(const u8* v, u32 count);

    /**
    */
    //u32 hash_Bernstein(const Char* str);

    /**
    */
    u32 hash_FNV1a(const Char* str);

    /**
    */
    u64 hash_FNV1a_64(const Char* str);
#endif

    //----------------------------------------------
    //---
    //--- BitSet32
    //---
    //----------------------------------------------
    class BitSet32
    {
    public:
        BitSet32()
            :flags_(0)
        {}

        explicit BitSet32(u32 flags)
            :flags_(flags)
        {}

        ~BitSet32()
        {}

        inline void clear()
        {
            flags_ = 0;
        }

        inline bool check(u32 flag) const
        {
            return 0 != (flags_&flag);
        }

        inline void set(u32 flag)
        {
            flags_ |= flag;
        }

        inline void reset(u32 flag)
        {
            flags_ &= ~flag;
        }
    private:
        u32 flags_;
    };
}

#endif //INC_LCORE_H_

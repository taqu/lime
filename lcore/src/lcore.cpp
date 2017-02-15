/**
@file lcore.cpp
@author t-sakai
@date 2009/01/17 create
*/
#include "lcore.h"

#include <stdio.h>
#include <stdarg.h>

#if defined(_WIN32) || defined(_WIN64)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <locale.h>
#include <Windows.h>
#include <mmsystem.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#else
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <locale.h>
#include <stdlib.h>
#include <dlfcn.h>
#endif

//#include "dlmalloc.h"
#define JEMALLOC_EXPORT
#include "jemalloc/jemalloc.h"
#include "SyncObject.h"

#if defined(_DEBUG) && !defined(ANDROID)
#define LCORE_DEBUG_MEMORY_INFO
#endif

namespace
{

#ifdef LCORE_DEBUG_MEMORY_INFO
    static const lcore::s32 DebugInfoMemorySize = 1024*1024;
    //lcore::MemorySpace debugInfoMemorySpace_;

    class DebugMemory
    {
    public:
        struct MemoryInfo
        {
            inline void reset()
            {
                prev_ = next_ = this;
            }

            inline void unlink()
            {
                next_->prev_ = prev_;
                prev_->next_ = next_;
                prev_ = next_ = this;
            }

            inline void link(MemoryInfo* info)
            {
                prev_ = info->prev_;
                next_ = info;
                prev_->next_ = info->prev_ = this;
            }

            MemoryInfo* prev_;
            MemoryInfo* next_;
            void* memory_;
            lcore::u32 id_;
            lcore::s32 line_;
            const lcore::Char* file_;
        };

        DebugMemory()
            :id_(0)
            ,breakAtMalloc_(false)
            ,breakMallocId_(0)
        {
            memoryInfoTop_.reset();
        }

        ~DebugMemory()
        {
        }

        void pushMemoryInfo(void* ptr, const lcore::Char* file, lcore::s32 line);
        void popMemoryInfo(void* ptr);

        void print();
        void setMallocBreak(lcore::u32 id);

        lcore::CriticalSection debugCS_;

        lcore::u32 id_;
        MemoryInfo memoryInfoTop_;

        bool breakAtMalloc_;
        lcore::u32 breakMallocId_;
    };

    void DebugMemory::pushMemoryInfo(void* ptr, const lcore::Char* file, lcore::s32 line)
    {
        lcore::CSLock lock(debugCS_);

        if(breakAtMalloc_ && breakMallocId_ == id_){
            breakAtMalloc_ = false;
#ifdef LCORE_DEBUG_MEMORY_BREAK
            _CrtDbgBreak();
#endif
        }
        MemoryInfo* info = (MemoryInfo*)je_malloc(sizeof(MemoryInfo));
        info->reset();
        info->memory_ = ptr;
        info->id_ = id_++;
        info->line_ = line;
        info->file_ = file;

        info->link(memoryInfoTop_.next_);
    }

    void DebugMemory::popMemoryInfo(void* ptr)
    {
        lcore::CSLock lock(debugCS_);

        MemoryInfo* info = memoryInfoTop_.next_;

        while(info != &memoryInfoTop_){
            if(info->memory_ == ptr){
                info->unlink();
                je_free(info);
                break;
            }

            info = info->next_;
        }
    }

    void DebugMemory::print()
    {
        lcore::CSLock lock(debugCS_);

        lcore::Log("-------------------");
        lcore::Log("--- malloc info ---");
        lcore::Log("-------------------");

        MemoryInfo* info = memoryInfoTop_.next_;
        while(info != &memoryInfoTop_){
            lcore::Log("[%d] %s (%d)", info->id_, info->file_, info->line_);
            info = info->next_;
        }

        lcore::Log("-------------------");
    }

    void DebugMemory::setMallocBreak(lcore::u32 id)
    {
        breakMallocId_ = id;
        breakAtMalloc_ = true;
    }

    DebugMemory* debugMemory_ = NULL;
#endif //LCORE_DEBUG_MEMORY_INFO
}

void* lcore_malloc(std::size_t size)
{
    return je_malloc(size);
}

void* lcore_malloc(std::size_t size, std::size_t alignment)
{
    return je_aligned_alloc(alignment, size);
}

void lcore_free(void* ptr)
{
#ifdef LCORE_DEBUG_MEMORY_INFO
    if(NULL != debugMemory_){
        debugMemory_->popMemoryInfo(ptr);
    }
#endif
    je_free(ptr);
}

void lcore_free(void* ptr, std::size_t /*alignment*/)
{
#ifdef LCORE_DEBUG_MEMORY_INFO
    if(NULL != debugMemory_){
        debugMemory_->popMemoryInfo(ptr);
    }
#endif
    je_free(ptr);
}

#ifdef _DEBUG
void* lcore_malloc(std::size_t size, const char* file, int line)
#else
void* lcore_malloc(std::size_t size, const char*, int)
#endif
{
    void* ptr = je_malloc(size);

#ifdef LCORE_DEBUG_MEMORY_INFO
    if(NULL != debugMemory_){
        debugMemory_->pushMemoryInfo(ptr, file, line);
    }
#endif
    return ptr;
}

#ifdef _DEBUG
void* lcore_malloc(std::size_t size, std::size_t alignment, const char* file, int line)
#else
void* lcore_malloc(std::size_t size, std::size_t alignment, const char*, int)
#endif
{
    void* ptr = je_aligned_alloc(alignment, size);

#ifdef LCORE_DEBUG_MEMORY_INFO
    if(NULL != debugMemory_){
        debugMemory_->pushMemoryInfo(ptr, file, line);
    }
#endif
    return ptr;
}

void* lcore_realloc(void* ptr, std::size_t size)
{
    ptr = je_realloc(ptr, size);
    return ptr;
}


//-------------------
void* operator new(std::size_t size)
{
    return lcore_malloc(size);
}

void operator delete(void* ptr)
{
    lcore_free(ptr);
}

void* operator new[](std::size_t size)
{
    return lcore_malloc(size);
}

void operator delete[](void* ptr)
{
    lcore_free(ptr);
}

//-------------------
void* operator new(std::size_t size, const char* file, int line)
{
    return lcore_malloc(size, file, line);
}

void operator delete(void* ptr, const char* /*file*/, int /*line*/)
{
    lcore_free(ptr);
}

void* operator new[](std::size_t size, const char* file, int line)
{
    return lcore_malloc(size, file, line);
}

void operator delete[](void* ptr, const char* /*file*/, int /*line*/)
{
    lcore_free(ptr);
}


#ifdef LCORE_DEBUG_MEMORY_BREAK
void lcore_setMallocBreak(unsigned int  id)
{
    if(NULL != debugMemory_){
        debugMemory_->setMallocBreak(id);
    }
}
#endif

namespace lcore
{
namespace
{
    static const f32 RefractiveIndexTable[] =
    {
        1.0f,

        1.000293f,//空気

        1.333f, //水
        1.343f, //海水

        1.49065f,
        2.0f,
        3.4179f,

        1.35f,
        1.43f,
        1.51f, //1.49 - 1.53
        1.53f,
        1.59f,
        1.63f,

        1.43f,
        1.51f,
        1.56f,

        1.309f,
        1.545f, //1.54-1.55
        1.67f, //1.65-1.69
        2.4195f,

        1.608f, //1.53～1.686
    };

#if defined(_DEBUG) && !defined(ANDROID)
    void beginMalloc()
    {
        if(NULL == debugMemory_){
            void* ptr = je_malloc(sizeof(DebugMemory));
            debugMemory_ = LPLACEMENT_NEW(ptr) DebugMemory();
        }
    }

    void endMalloc()
    {
        if(NULL != debugMemory_){
            debugMemory_->print();
            debugMemory_->~DebugMemory();
            je_free(debugMemory_);
            debugMemory_ = NULL;
        }
    }
#endif
}

    f32 clampRotate0(f32 val, f32 total)
    {
        while(val<0.0f){
            val += total;
        }
        while(total<val){
            val -= total;
        }
        return val;
    }

    s32 clampRotate0(s32 val, s32 total)
    {
        while(val<0){
            val += total;
        }
        while(total<val){
            val -= total;
        }
        return val;
    }

    s32 roundUpPow2(s32 x)
    {
        --x;
        x |= (x>>1);
        x |= (x>>2);
        x |= (x>>4);
        x |= (x>>8);
        x |= (x>>16);
        return ++x;
    }

    s64 roundUpPow2(s64 x)
    {
         --x;
        x |= (x>>1);
        x |= (x>>2);
        x |= (x>>4);
        x |= (x>>8);
        x |= (x>>16);
        x |= (x>>32);
        return ++x;
    }

    u32 roundUpPow2(u32 x)
    {
        --x;
        x |= (x>>1);
        x |= (x>>2);
        x |= (x>>4);
        x |= (x>>8);
        x |= (x>>16);
        return ++x;
    }

    u64 roundUpPow2(u64 x)
    {
         --x;
        x |= (x>>1);
        x |= (x>>2);
        x |= (x>>4);
        x |= (x>>8);
        x |= (x>>16);
        x |= (x>>32);
        return ++x;
    }

    bool isLittleEndian()
    {
        u32 v = 1;
        return *reinterpret_cast<u8*>(&v) != 0;
    }

    u16 toBinary16Float(f32 f)
    {
        U32F32Union t;
        t.f_ = f;

        u16 sign = (t.u_>>16) & 0x8000U;
        s32 exponent = (t.u_>>23) & 0x00FFU;
        u32 fraction = t.u_ & 0x007FFFFFU;

        if(exponent == 0){
            return sign; //符号付き0

        }else if(exponent == 0xFFU){
            if(fraction == 0){
                return sign | 0x7C00U; //符号付きInf
            }else{
                return static_cast<u16>((fraction>>13) | 0x7C00U); //NaN
            }
        }else {
            exponent += (-127 + 15);
            if(exponent>=0x1F){ //オーバーフロー
                return sign | 0x7C00U;
            }else if(exponent<=0){ //アンダーフロー
                s32 shift = 14 - exponent;
                if(shift>24){ //表現できないほど小さい
                    return sign;
                }else{
                    fraction |= 0x800000U; //隠れた整数ビット足す
                    u16 frac = static_cast<u16>(fraction >> shift);
                    if((fraction>>(shift-1)) & 0x01U){ //１ビット下位を丸める
                        frac += 1;
                    }
                    return sign | frac;
                }
            }
        }

        u16 ret = static_cast<u16>(sign | ((exponent<<10) & 0x7C00U) | (fraction>>13));
        if((fraction>>12) & 0x01U){ //１ビット下位を丸める
            ret += 1;
        }
        return ret;
    }

    f32 fromBinary16Float(u16 s)
    {
        u32 sign = (s & 0x8000U) << 16;
        u32 exponent = ((s & 0x7C00U) >> 10);
        u32 fraction = (s & 0x03FFU);

        U32F32Union t;
        if(exponent == 0){
            if(fraction != 0){
                fraction <<= 1;
                while(0==(fraction & 0x0400U)){
                    ++exponent;
                    fraction <<= 1;
                }
                exponent = (127 - 15) - exponent;
                fraction &= 0x03FFU;
            }

        }else if(exponent == 0x1FU){
            exponent = 0xFFU; //Infinity か NaN

        }else{
            exponent += (127 - 15);
        }

         t.u_ = sign | (exponent<<23) | (fraction<<13);

        return t.f_;
    }

    s8 floatTo8SNORM(f32 f)
    {
        s32 s = static_cast<s32>(f*128);
        return static_cast<s8>(lcore::clamp(s, -128, 127));
    }

    u8 floatTo8UNORM(f32 f)
    {
        u32 s = static_cast<u32>(f*255 + 0.5f);
        return static_cast<u8>((255<s)? 255 : s);
    }

    s16 floatTo16SNORM(f32 f)
    {
        s32 s = static_cast<s32>(f*32768);
        return static_cast<s16>(lcore::clamp(s, -32768, 32767));
    }

    u16 floatTo16UNORM(f32 f)
    {
        u32 s = static_cast<u32>(f*65535 + 0.5f);
        return static_cast<u16>((65535<s)? 65535 : s);
    }

#define LCORE_POPULATIONCOUNT(val)\
    val = (val & 0x55555555U) + ((val>>1) & 0x55555555U);\
    val = (val & 0x33333333U) + ((val>>2) & 0x33333333U);\
    val = (val & 0x0F0F0F0FU) + ((val>>4) & 0x0F0F0F0FU);\
    val = (val & 0x00FF00FFU) + ((val>>8) & 0x00FF00FFU);\
    return (val & 0x0000FFFFU) + ((val>>16) & 0x0000FFFFU)

    u32 populationCount(u32 v)
    {
        LCORE_POPULATIONCOUNT(v);
    }

    u32 mostSignificantBit(u32 v)
    {
#if defined(_MSC_VER)
        unsigned long index;
        return (_BitScanReverse(&index, v))? (u32)index : 0;

#elif defined(__GNUC__)
        return (0!=v)? (__builtin_clzl(v) ^ 0x3FU) : 0;
#else
        static const u32 shifttable[] =
        {
            0, 1, 2, 2, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4,
        };
        u32 ret = 0;

        if(v & 0xFFFF0000U){
            ret += 16;
            v >>= 16;
        }

        if(v & 0xFF00U){
            ret += 8;
            v >>= 8;
        }

        if(v & 0xF0U){
            ret += 4;
            v >>= 4;
        }
        return ret + shifttable[v];
#endif
    }

    u32 leastSignificantBit(u32 v)
    {
#if defined(_MSC_VER)
        unsigned long index;
        return _BitScanForward(&index, v)? (u32)index : 0;

#elif defined(__GNUC__)
        return (0!=v)? (__builtin_ctzl(v)) : 0;
#else
        if(0 == v){
            return 32U;
        }
        u32 count = (v&0xAAAAAAAAU)? 0x01U:0;

        if(v&0xCCCCCCCCU){
            count |= 0x02U;
        }

        if(v&0xF0F0F0F0U){
            count |= 0x04U;
        }

        if(v&0xFF00FF00U){
            count |= 0x08U;
        }

        return 31U-((v&0xFFFF0000U)? count|0x10U:count);
#endif
    }

    //u16 mostSignificantBit(u16 v)
    //{
    //    static const u16 shifttable[] =
    //    {
    //        0, 1, 2, 2, 3, 3, 3, 3,
    //        4, 4, 4, 4, 4, 4, 4, 4,
    //    };
    //    u16 ret = 0;

    //    if(v & 0xFF00U){
    //        ret += 8;
    //        v >>= 8;
    //    }

    //    if(v & 0xF0U){
    //        ret += 4;
    //        v >>= 4;
    //    }
    //    return ret + shifttable[v];
    //}

    //u8 mostSignificantBit(u8 v)
    //{
    //    static const u8 shifttable[] =
    //    {
    //        0, 1, 2, 2, 3, 3, 3, 3,
    //    };
    //    u8 ret = 0;
    //    if(v & 0xF0U){
    //        ret += 4;
    //        v >>= 4;
    //    }

    //    if(v & 0xCU){
    //        ret += 2;
    //        v >>= 2;
    //    }
    //    return ret + shifttable[v];
    //}

#undef LCORE_POPULATIONCOUNT

    u32 bitreverse(u32 x)
    {
#if defined(__GNUC__)
        x = __builtin_bswap32(x);
#elif defined(__clang__)
        x = __builtin_bswap32(x);
#else
        x = (x << 16) | (x >> 16);
        x = ((x & 0x00FF00FFU) << 8) | ((x & 0xFF00FF00U) >> 8);
#endif
        x = ((x & 0x0F0F0F0FU) << 4) | ((x & 0xF0F0F0F0U) >> 4);
        x = ((x & 0x33333333U) << 2) | ((x & 0xCCCCCCCCU) >> 2);
        x = ((x & 0x55555555U) << 1) | ((x & 0xAAAAAAAAU) >> 1);
        return x;
    }

    u64 bitreverse(u64 x)
    {
#if defined(__GNUC__)
        x = __builtin_bswap64(x);
#elif defined(__clang__)
        x = __builtin_bswap64(x);
#else
        x = (x << 32) | (x >> 32);
        x = ((x & 0x0000FFFF0000FFFFULL) << 16) | ((x & 0xFFFF0000FFFF0000ULL) >> 16);
        x = ((x & 0x00FF00FF00FF00FFULL) << 8) | ((x & 0xFF00FF00FF00FF00ULL) >> 8);
#endif
        x = ((x & 0x0F0F0F0F0F0F0F0FULL) << 4) | ((x & 0xF0F0F0F0F0F0F0F0ULL) >> 4);
        x = ((x & 0x3333333333333333ULL) << 2) | ((x & 0xCCCCCCCCCCCCCCCCULL) >> 2);
        x = ((x & 0x5555555555555555ULL) << 1) | ((x & 0xAAAAAAAAAAAAAAAAULL) >> 1);
        return x;
    }

    void initializeSystem()
    {
#ifdef _DEBUG
#if defined(_WIN32) || defined(_WIN64)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

#if !defined(ANDROID)
        beginMalloc();
#endif
#endif

#if defined(_WIN32) || defined(_WIN64)
        timeBeginPeriod(1);
#endif
    }

    void terminateSystem()
    {
#ifdef _DEBUG
#if !defined(ANDROID)
        endMalloc();
#endif
#endif

#if defined(_WIN32) || defined(_WIN64)
        timeEndPeriod(1);
#endif
    }

#if defined(LENABLE_LOG)
    void Log(const Char* format, ...)
#else
    void Log(const Char*, ...)
#endif
    {
#if defined(LENABLE_LOG)

        if(format == NULL){
            return;
        }

        va_list ap;
        va_start(ap, format);

#if defined(ANDROID)
        __android_log_vprint(ANDROID_LOG_DEBUG, "LIME", format, ap);
        //__android_log_vprint(ANDROID_LOG_ERROR, "LIME", format, ap);
#else

#ifdef _DEBUG
        static const u32 MaxBuffer = 1024;
#else
        static const u32 MaxBuffer = 64;
#endif //_DEBUG

        Char buffer[MaxBuffer+2];

#if defined(_WIN32) || defined(_WIN64)
        s32 count=vsnprintf_s(buffer, MaxBuffer, format, ap);
#else
        s32 count = ::vsnprintf(buffer, MaxBuffer, format, ap);
#endif //defined(_WIN32) || defined(_WIN64)
        if(count<0){
            count = MaxBuffer;
        }
#if defined(_WIN32) || defined(_WIN64)
        buffer[count] = '\n';
        buffer[count+1] = CharNull;
        OutputDebugString(buffer);
#else
        buffer[count] = '\n';
        buffer[count+1] = CharNull;
        std::cerr << buffer;
#endif //defined(_WIN32) || defined(_WIN64)

#endif //defined(ANDROID)

        va_end(ap);
#endif
    }

    void Print(const Char* format, ...)
    {
        if(format == NULL){
            return;
        }

        va_list ap;
        va_start(ap, format);

#if defined(ANDROID)
        __android_log_vprint(ANDROID_LOG_DEBUG, "LIME", format, ap);
#elif defined(_WIN32) || defined(_WIN64)
        vprintf_s(format, ap);
#else
        vprintf(format, ap);
#endif
        va_end(ap);
    }

//    MemorySpace::MemorySpace()
//        :mspace_(NULL)
//    {
//    }
//
//    MemorySpace::~MemorySpace()
//    {
//        destroy();
//    }
//
//    bool MemorySpace::create(u32 capacity, Locked locked)
//    {
//#ifdef ANDROID
//        return false;
//#else
//        destroy();
//        mspace_ = create_mspace(capacity, locked);
//        return (NULL != mspace_);
//#endif
//    }
//
//    void MemorySpace::destroy()
//    {
//#ifdef ANDROID
//#else
//        if(NULL != mspace_){
//            size_t size = destroy_mspace(mspace_);
//            lcore::Log("mspace size freed %d", size);
//            mspace_ = NULL;
//        }
//#endif
//    }
//
//    bool MemorySpace::valid() const
//    {
//        return (NULL != mspace_);
//    }
//
//    void* MemorySpace::allocate(u32 size)
//    {
//#ifdef ANDROID
//        return NULL;
//#else
//        LASSERT(NULL != mspace_);
//        return mspace_malloc(mspace_, size);
//#endif
//    }
//
//    void MemorySpace::deallocate(void* mem)
//    {
//#ifdef ANDROID
//#else
//        LASSERT(NULL !=  mspace_);
//        mspace_free(mspace_, mem);
//#endif
//    }

    //---------------------------------------------------------
    //---
    //--- Color
    //---
    //---------------------------------------------------------
    u8 getLuminance(u8 r, u8 g, u8 b)
    {
        return static_cast<u8>(r * 0.212671f + g * 0.715160f + b * 0.072169f);
    }

    f32 getLuminance(f32 r, f32 g, f32 b)
    {
        return r * 0.212671f + g * 0.715160f + b * 0.072169f;
    }

    f32 getRefractiveIndex(RefractiveIndex index)
    {
        return RefractiveIndexTable[index];
    }

    // 真空に対するフレネル反射係数の実部
    f32 calcFresnelTerm(f32 refract)
    {
        f32 v0 = refract - 1.0f;
        f32 v1 = 1.0f + refract;

        f32 ret = v0/v1;
        return (ret*ret);
    }

    // フレネル反射係数の実部
    f32 calcFresnelTerm(f32 refract0, f32 refract1)
    {
        f32 v0 = refract1 - refract0;
        f32 v1 = refract0 + refract1;

        f32 ret = v0/v1;
        return (ret*ret);
    }

    //---------------------------------------------------------
    //---
    //--- 文字列操作
    //---
    //---------------------------------------------------------
    //-------------------------------------------------------------
    // 後方から文字探索
    const Char* rFindChr(s32 length, const Char* src, Char c)
    {
        LASSERT(0<=length);
        LASSERT(NULL != src);
        src += (length-1);
        for(s32 i=0; i<length; ++i){
            if(*src == c){
                return src;
            }
            --src;
        }
        return NULL;
    }


    //-------------------------------------------------------------
    // パスからディレクトリパス抽出
    s32 extractDirectoryPath(Char* dst, s32 length, const Char* path)
    {
        LASSERT(NULL != path);
        LASSERT(0<=length);
        if(length<=0){
            if(NULL != dst){
                dst[0] = CharNull;
            }
            return 0;
        }

        s32 i = length-1;
        for(; 0<=i; --i){
            if(PathDelimiter == path[i]){
                break;
            }
        }
        s32 dstLen = i+1;
        if(NULL != dst){
            for(s32 j=0; j<dstLen; ++j){
                dst[j] = path[j];
            }
            dst[dstLen] = CharNull;
        }
        return dstLen;
    }

    // パスからディレクトリ名抽出
    s32 extractDirectoryName(Char* dst, s32 length, const Char* path)
    {
        LASSERT(NULL != path);
        LASSERT(0<=length);
        if(NULL != dst){
            dst[0] = CharNull;
        }
        if(length<=0){
            return 0;
        }

        if(PathDelimiter == path[length-1]){
            --length;
            if(length<=0){
                return 0;
            }
        }
        s32 i = length-1;
        for(; 0<=i; --i){
            if(PathDelimiter == path[i]){
                break;
            }
        }

        s32 dstLen = length-i-1;
        if(NULL != dst){
            for(s32 j=i+1; j<length; ++j){
                dst[j-i-1] = path[j];
            }
            dst[dstLen] = CharNull;
        }
        return dstLen;
    }

    //-------------------------------------------------------------
    // パスからファイル名抽出
    s32 extractFileName(Char* dst, s32 length, const Char* path)
    {
        LASSERT(NULL != path);
        LASSERT(0<=length);
        if(length<=0){
            if(NULL != dst){
                dst[0] = CharNull;
            }
            return 0;
        }

        s32 i = length-1;
        for(; 0<=i; --i){
            if(PathDelimiter == path[i]){
                break;
            }
        }

        s32 dstLen = length-i-1;
        if(NULL != dst){
            for(s32 j=i+1; j<length; ++j){
                dst[j-i-1] = path[j];
            }
            dst[dstLen] = CharNull;
        }
        return dstLen;
    }

    // パスからファイル名抽出
    s32 extractFileNameWithoutExt(Char* dst, s32 length, const Char* path)
    {
        LASSERT(NULL != path);
        LASSERT(0<=length);
        if(length<=0){
            if(NULL != dst){
                dst[0] = CharNull;
            }
            return 0;
        }

        s32 i = length-1;
        for(; 0<=i; --i){
            if(PathDelimiter == path[i]){
                break;
            }
        }

        s32 dstLen = length-i-1;
        if(NULL != dst){
            for(s32 j=length-1; i<j; --j){
                if('.' == path[j]){
                    dstLen = j-i-1;
                }
                dst[j-i-1] = path[j];
            }
            dst[dstLen] = CharNull;
        }
        return dstLen;
    }

    // パスから最初のファイル名抽出
    const Char* parseFirstNameFromPath(s32& length, Char* name, s32 pathLength, const Char* path)
    {
        LASSERT(NULL != name);
        LASSERT(NULL != path);

        length = 0;
        while(CharNull != *path && length<(pathLength-1)){
            if(PathDelimiter == *path){
                ++path;
                break;
            }
            name[length++] = *path;
            ++path;
        }
        name[length] = CharNull;
        return path;
    }

    //パスから拡張子抽出
    const Char* getExtension(s32 length, const Char* path)
    {
        LASSERT(0<=length);
        LASSERT(NULL != path);
        for(s32 i=length-1; 0<=i; --i){
            if(path[i] == '.'){
                return &path[i+1];
            }
        }
        return &path[length];
    }

    s32 scprintf(const Char* format, ...)
    {
        va_list args;
        va_start(args, format);
#ifdef _MSC_VER
        s32 count = ::_vscprintf(format, args);
#else
        s32 count = vsnprintf(NULL, 0, format, args);
#endif
        va_end(args);
        return count;
    }

    s32 vscprintf(const Char* format, va_list args)
    {
#ifdef _MSC_VER
        return ::_vscprintf(format, args);
#else
        va_list cargs;
        va_copy(cargs, args);
        s32 count = vsnprintf(NULL, 0, format, cargs);
        va_end(cargs);
        return count;
#endif
    }

    void replace(Char* str, Char dst, Char src)
    {
        LASSERT(NULL != str);
        while(CharNull != *str){
            if(src == *str){
                *str = dst;
            }
            ++str;
        }
    }

    //---------------------------------------------------------
    //---
    //--- タイム関係
    //---
    //---------------------------------------------------------
    void sleep(u32 milliSeconds)
    {
#if defined(_WIN32) || defined(_WIN64)
        ::Sleep(milliSeconds);
#else
        timespec ts;
        ts.tv_sec = 0;
        while(1000<milliSeconds){
            ts.tv_sec += 1;
            milliSeconds -= 1000;
        }
        ts.tv_nsec = 1000000L * milliSeconds;
        nanosleep(&ts, NULL);
#endif
    }

    // カウント取得
    ClockType getPerformanceCounter()
    {
#if defined(_WIN32) || defined(_WIN64)
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        return count.QuadPart;
#else
        clock_t t = 0;
        t = clock();
        return t;
#endif
    }

    // 秒間カウント数
    ClockType getPerformanceFrequency()
    {
#if defined(_WIN32) || defined(_WIN64)
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return freq.QuadPart;
#else
        return CLOCKS_PER_SEC;
#endif
    }

    // 秒単位の時間差分計算
    lcore::f64 calcTime64(lcore::ClockType prevTime, lcore::ClockType currentTime)
    {
        lcore::ClockType d = (currentTime>=prevTime)? currentTime - prevTime : lcore::numeric_limits<lcore::ClockType>::maximum() - prevTime + currentTime;
        lcore::f64 delta = static_cast<lcore::f64>(d)/lcore::getPerformanceFrequency();
        return delta;
    }

    // ミリ秒単位の時間を取得
    u32 getTime()
    {
#if defined(_WIN32) || defined(_WIN64)
        DWORD time = timeGetTime();
        return static_cast<u32>(time);
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);

        return static_cast<u32>(tv.tv_sec*1000 + tv.tv_usec/1000);
#endif
    }

    namespace
    {
        inline u32 getUsageMSec()
        {
#if defined(_WIN32) || defined(_WIN64)
            return 1;
#else
            rusage t;
            getrusage(RUSAGE_SELF, &t);
            return static_cast<u32>(t.ru_utime.tv_sec * 1000 + t.ru_utime.tv_usec/1000);
#endif
        }
    }

    //---------------------------------------------------------
    //---
    //--- Character Code
    //---
    //---------------------------------------------------------
    s32 getUTF8Size(Char utf8)
    {
        u8 firstByte = ~(*((u8*)&utf8));
        if(firstByte & 0x80U){
            return 1;

        }else if(firstByte & 0x40U){

        }else if(firstByte & 0x20U){
            return 2;
        }else if(firstByte & 0x10U){
            return 3;
        }
        return 0;
    }

    s32 getUTF8Size(Char16 utf16)
    {
        if(utf16 < 0x80U){
            return 1;
        }else if(utf16 < 0x800U){
            return 2;
        }else{
            return 3;
        }
    }

    // UTF8 to UTF16
    s32 UTF8toUTF16(Char16& utf16, const Char* utf8)
    {
        LASSERT(NULL != utf8);
        u8 firstByte = ~(*((u8*)utf8));
        if(firstByte & 0x80U){
            utf16 = utf8[0];
            return 1;

        }else if(firstByte & 0x40U){

        }else if(firstByte & 0x20U){
            u8 c0 = (utf8[0] & 0x1FU);
            u8 c1 = (utf8[1] & 0x3FU);
            utf16 = (c0<<6) | c1;
            return 2;
        }else if(firstByte & 0x10U){
            u8 c0 = (utf8[0] & 0x0FU);
            u8 c1 = (utf8[1] & 0x3FU);
            u8 c2 = (utf8[2] & 0x3FU);
            utf16 = (c0<<12) | (c1<<6) | c2;
            return 3;
        }
        utf16 = 0;
        return 0;
    }

    // UTF16 to UTF8
    s32 UTF16toUTF8(Char* utf8, Char16 utf16)
    {
        LASSERT(NULL != utf8);
        if(utf16 < 0x80U){
            utf8[0] = static_cast<u8>(utf16);
            return 1;
        }else if(utf16 < 0x800U){
            u8 c0 = static_cast<u8>((utf16>>6) | 0xC0U);
            u8 c1 = static_cast<u8>((utf16&0x3FU) | 0x80U);
            *((u8*)(utf8+0)) = c0;
            *((u8*)(utf8+1)) = c1;
            return 2;
        }else{
            u8 c0 = (utf16>>12) | 0xE0U;
            u8 c1 = ((utf16>>6)&0x3FU) | 0x80U;
            u8 c2 = ((utf16>>0)&0x3FU) | 0x80U;
            *((u8*)(utf8+0)) = c0;
            *((u8*)(utf8+1)) = c1;
            *((u8*)(utf8+2)) = c2;
            return 3;
        }
    }

    void setLocale(const Char* locale)
    {
        setlocale(LC_ALL, locale);
    }

    // SJIS -> UTF16変換
    s32 MBSToWCS(WChar* dst, u32 sizeInWords, const Char* src, u32 srcSize)
    {
#if defined(_WIN32) || defined(_WIN64)
        size_t converted = 0;
        s32 ret = mbstowcs_s(&converted, dst, sizeInWords, src, srcSize);
        return (0==ret)? static_cast<s32>(converted) : -1;
#else
        size_t ret = ::mbstowcs(dst, src, srcSize);
        return (ret==(size_t)(-1))? -1 : static_cast<s32>(ret);
#endif
    }

    // SJIS -> UTF16変換
    s32 MBSToWCS(WChar* dst, u32 sizeInWords, const Char* src)
    {
#if defined(_WIN32) || defined(_WIN64)
        size_t converted = 0;
        s32 ret = mbstowcs_s(&converted, dst, sizeInWords, src, _TRUNCATE);
        return (0==ret)? static_cast<s32>(converted) : -1;
#else
        size_t ret = ::mbstowcs(dst, src, sizeInWords);
        return (ret==(size_t)(-1))? -1 : static_cast<s32>(ret);
#endif
    }

    //---------------------------------------------------------
    //---
    //--- DLL
    //---
    //---------------------------------------------------------
    DLL::DLL()
        :handle_(NULL)
    {
    }

    DLL::DLL(const Char* path)
        :handle_(NULL)
    {
        open(path);
    }

    DLL::~DLL()
    {
        close();
    }

    bool DLL::is_open() const
    {
        return NULL != handle_;
    }

    bool DLL::open(const Char* path)
    {
        LASSERT(NULL != path);
        if(handle_){
            closeDLL(handle_);
        }

        handle_ = openDLL(path);
        return NULL != handle_;
    }

    void DLL::close()
    {
        closeDLL(handle_);
    }

    void* DLL::getProcAddress(const Char* name)
    {
        return getProcAddress(handle_, name);
    }

#if defined(_WIN32) || defined(_WIN64)
    LHANDLE DLL::openDLL(const Char* path)
    {
        LASSERT(NULL != path);
        return (LHANDLE)LoadLibrary(path);
    }

    void DLL::closeDLL(LHANDLE& handle)
    {
        if(handle){
            FreeLibrary((HMODULE)handle);
            handle = NULL;
        }
    }

    void* DLL::getProcAddress(LHANDLE handle, const Char* name)
    {
        LASSERT(NULL != handle);
        LASSERT(NULL != name);
        return (void*)GetProcAddress((HMODULE)handle, name);
    }

#else

    LHANDLE DLL::openDLL(const Char* path)
    {
        LASSERT(NULL != path);
        return dlopen(path, RTLD_NOW);
    }

    void DLL::closeDLL(LHANDLE& handle)
    {
        if(handle){
            dlclose(handle);
            handle = NULL;
        }
    }

    void* DLL::getProcAddress(LHANDLE module, const Char* name)
    {
        LASSERT(NULL != handle);
        LASSERT(NULL != name);
        return (void*)dlsym(module, name);
    }
#endif


    //---------------------------------------------------------
    //---
    //--- Low-Discrepancy
    //---
    //---------------------------------------------------------
    f32 halton(s32 index, s32 prime)
    {
        f32 result = 0.0f;
        f32 f = 1.0f / prime;
        int i = index;
        while(0 < i) {
            result = result + f * (i % prime);
            i = (s32)floor((f32)i / prime);
            f = f / prime;
        }
        return result;
    }

    f32 halton_next(f32 prev, s32 prime)
    {
        float r = 1.0f - prev - 0.000001f;
        float f = 1.0f/prime;
        if(f < r) {
            return prev + f;
        } else {
            float h = f;
            float hh;
            do {
                hh = h;
                h *= f;
            } while(h >= r);
            return prev + hh + h - 1.0f;
        }
    }

    f32 vanDerCorput(u32 n, u32 base)
    {
        f32 vdc = 0.0f;
        f32 inv = 1.0f/base;
        f32 factor = inv;

        while(n){
            vdc += static_cast<f32>(n%base) * factor;
            n /= base;
            factor *= inv;
        }
        return vdc;
    }

    f32 radicalInverseVanDerCorput(u32 bits, u32 scramble)
    {
        bits = bitreverse(bits);
        bits ^= scramble;
        return static_cast<f32>(bits) / static_cast<f32>(0x100000000L);
    }

    f32 radicalInverseSobol(u32 i, u32 scramble)
    {
        for(u32 v=1U<<31; i; i>>=1, v ^= v>>1){
            if(i&1){
                scramble ^= v;
            }
        }
        return static_cast<f32>(scramble) / static_cast<f32>(0x100000000L);
    }

    f32 radicalInverseLarcherPillichshammer(u32 i, u32 scramble)
    {
        for(u32 v=1U<<31; i; i>>=1, v |= v>>1){
            if(i&1){
                scramble ^= v;
            }
        }
        return static_cast<f32>(scramble) / static_cast<f32>(0x100000000L);
    }

    //---------------------------------------------------------
    //---
    //--- Hash Functions
    //---
    //---------------------------------------------------------
    /**
    */
    u32 hash_Bernstein(const u8* v, u32 count)
    {
        u32 hash = 5381U;

        for(u32 i=0; i<count; ++i){
            //hash = 33*hash + v[i];
            hash = ((hash<<5)+hash) + v[i];
        }
        return hash;
    }

    /**
    */
    u32 hash_FNV1(const u8* v, u32 count)
    {
        u32 hash = 2166136261U;

        for(u32 i=0; i<count; ++i){
            hash *= 16777619U;
            hash ^= v[i];
        }
        return hash;
    }

    /**
    */
    u32 hash_FNV1a(const u8* v, u32 count)
    {
        u32 hash = 2166136261U;

        for(u32 i=0; i<count; ++i){
            hash ^= v[i];
            hash *= 16777619U;
        }
        return hash;
    }

    /**
    */
    u64 hash_FNV1_64(const u8* v, u32 count)
    {
        u64 hash = 14695981039346656037ULL;

        for(u32 i=0; i<count; ++i){
            hash *= 1099511628211ULL;
            hash ^= v[i];
        }
        return hash;
    }

    /**
    */
    u64 hash_FNV1a_64(const u8* v, u32 count)
    {
        u64 hash = 14695981039346656037ULL;

        for(u32 i=0; i<count; ++i){
            hash ^= v[i];
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    /**
    */
    u32 hash_Bernstein(const Char* str)
    {
        u32 hash = 5381U;

        while(CharNull != *str){
            //hash = 33*hash + static_cast<u8>(*str);
            hash = ((hash<<5)+hash) + static_cast<u8>(*str);
            ++str;
        }
        return hash;
    }

    /**
    */
    u32 hash_FNV1a(const Char* str)
    {
        u32 hash = 2166136261U;

        while(CharNull != *str){
            hash ^= static_cast<u8>(*str);
            hash *= 16777619U;
            ++str;
        }
        return hash;
    }

    /**
    */
    u64 hash_FNV1a_64(const Char* str)
    {
        u64 hash = 14695981039346656037ULL;

        while(CharNull != *str){
            hash ^= static_cast<u8>(*str);
            hash *= 1099511628211ULL;
            ++str;
        }
        return hash;
    }
}

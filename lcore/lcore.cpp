/**
@file lcore.cpp
@author t-sakai
@date 2009/01/17 create
*/

#include "lcore.h"

#if defined(ANDROID)
#include <stdio.h>
#include <stdarg.h>

#else
#include <iostream>
#include <sstream>
#endif //defined(ANDROID)


#ifdef _WIN32

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#else
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "allocator/dlmalloc.h"
#include "utility.h"
#include "clibrary.h"
#include "async/SyncObject.h"

namespace
{
#ifdef _DEBUG
    static const lcore::s32 DebugInfoMemorySize = 1024*1024;
    lcore::MemorySpace debugInfoMemorySpace_;

    class DebugMemory
    {
    public:
        static const lcore::s32 DebugInfoFileNameLength = 63;

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
            lcore::s32 line_;
            lcore::Char file_[DebugInfoFileNameLength+1];
        };

        DebugMemory()
        {
            memoryInfoTop_.reset();
        }

        ~DebugMemory()
        {
        }

        void pushMemoryInfo(void* ptr, const lcore::Char* file, lcore::s32 line);
        void popMemoryInfo(void* ptr);

        void print();

        lcore::CriticalSection debugCS_;

        MemoryInfo memoryInfoTop_;
    };

    void DebugMemory::pushMemoryInfo(void* ptr, const lcore::Char* file, lcore::s32 line)
    {
        lcore::CSLock lock(debugCS_);
        if(!debugInfoMemorySpace_.valid()){
            return;
        }

        MemoryInfo* info = (MemoryInfo*)debugInfoMemorySpace_.allocate(sizeof(MemoryInfo));
        info->reset();
        info->memory_ = ptr;
        info->line_ = line;
        info->file_[0] = '\0';


        lcore::s32 len = 0;
        const lcore::Char* name = "";

        if(NULL != file){
            len = lcore::strlen(file);
            const lcore::Char* slash = lcore::rFindChr(file, '\\', len);
            if(NULL != slash){
                name = slash + 1;
                len = lcore::strlen(name);
                len = lcore::minimum(DebugInfoFileNameLength, len);
            }
        }

        for(lcore::s32 i=0; i<=len; ++i){
            info->file_[i] = name[i];
        }

        info->link(memoryInfoTop_.next_);
    }

    void DebugMemory::popMemoryInfo(void* ptr)
    {
        lcore::CSLock lock(debugCS_);

        MemoryInfo* info = memoryInfoTop_.next_;

        while(info != &memoryInfoTop_){
            if(info->memory_ == ptr){
                info->unlink();
                if(debugInfoMemorySpace_.valid()){
                    debugInfoMemorySpace_.deallocate(info);
                }
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
            lcore::Log("%s (%d)", info->file_, info->line_);
            info = info->next_;
        }

        lcore::Log("-------------------");
    }

    DebugMemory* debugMemory_ = NULL;
#endif
}

void* lcore_malloc(std::size_t size)
{
    return dlmalloc(size);
}

void* lcore_malloc(std::size_t size, std::size_t alignment)
{
    return dlmemalign(alignment, size);
}

void lcore_free(void* ptr)
{
#ifdef _DEBUG
    if(NULL != debugMemory_){
        debugMemory_->popMemoryInfo(ptr);
    }
#endif
    dlfree(ptr);
}

void lcore_free(void* ptr, std::size_t /*alignment*/)
{
#ifdef _DEBUG
    if(NULL != debugMemory_){
        debugMemory_->popMemoryInfo(ptr);
    }
#endif
    dlfree(ptr);
}


void* lcore_malloc(std::size_t size, const char* file, int line)
{
    void* ptr = dlmalloc(size);
#ifdef _DEBUG
    if(NULL != debugMemory_){
        debugMemory_->pushMemoryInfo(ptr, file, line);
    }
#endif
    return ptr;
}

void* lcore_malloc(std::size_t size, std::size_t alignment, const char* file, int line)
{
    void* ptr = dlmemalign(alignment, size);
#ifdef _DEBUG
    if(NULL != debugMemory_){
        debugMemory_->pushMemoryInfo(ptr, file, line);
    }
#endif
    return ptr;
}


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

#ifdef _DEBUG
    void beginMalloc()
    {
        if(NULL == debugMemory_){
            debugInfoMemorySpace_.create(DebugInfoMemorySize);
            void* ptr = debugInfoMemorySpace_.allocate(sizeof(DebugMemory));
            debugMemory_ = LIME_PLACEMENT_NEW(ptr) DebugMemory();
        }
    }

    void endMalloc()
    {
        if(NULL != debugMemory_){
            debugMemory_->print();
            debugMemory_->~DebugMemory();
            debugInfoMemorySpace_.deallocate(debugMemory_);
            debugMemory_ = NULL;
            debugInfoMemorySpace_.destroy();
        }
    }

#else
    void beginMalloc()
    {
    }

    void endMalloc()
    {
    }
#endif
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
                return (fraction>>13) | 0x7C00U; //NaN
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
                    u16 frac = fraction >> shift;
                    if((fraction>>(shift-1)) & 0x01U){ //１ビット下位を丸める
                        frac += 1;
                    }
                    return sign | frac;
                }
            }
        }

        u16 ret = sign | ((exponent<<10) & 0x7C00U) | (fraction>>13);
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

#if defined(_WIN32)
    LeakCheck::LeakCheck()
    {
//#ifdef _DEBUG
//        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
        beginMalloc();
    }

    LeakCheck::~LeakCheck()
    {
        endMalloc();
    }

    System::System()
    {
        timeBeginPeriod(1);
    }

    System::~System()
    {
        timeEndPeriod(1);
    }

#else
    LeakCheck::LeakCheck()
    {
        beginMalloc();
    }

    LeakCheck::~LeakCheck()
    {
        endMalloc();
    }

    System::System()
    {
    }

    System::~System()
    {
    }
#endif


    void Log(const Char* format, ...)
    {
#if defined(LIME_ENABLE_LOG)

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
#endif
        Char buffer[MaxBuffer+2];
        int count=vsnprintf(buffer, MaxBuffer, format, ap);
        if(count<0){
            count = MaxBuffer;
        }
#if defined(_WIN32)
        buffer[count]='\n';
        buffer[count+1]='\0';
        OutputDebugString(buffer);
#else
        buffer[count]='\n';
        buffer[count+1]='\0';
        std::cerr << buffer;
#endif

#endif

        va_end(ap);
#endif
    }

    MemorySpace::MemorySpace()
        :mspace_(NULL)
    {
    }

    MemorySpace::~MemorySpace()
    {
        destroy();
    }

    bool MemorySpace::create(u32 capacity, Locked locked)
    {
        destroy();
        mspace_ = create_mspace(capacity, locked);
        return (NULL != mspace_);
    }

    void MemorySpace::destroy()
    {
        if(NULL != mspace_){
            u32 size = destroy_mspace(mspace_);
            lcore::Log("mspace size freed %d", size);
            mspace_ = NULL;
        }
    }

    bool MemorySpace::valid() const
    {
        return (NULL != mspace_);
    }

    void* MemorySpace::allocate(u32 size)
    {
        LASSERT(NULL != mspace_);
        return mspace_malloc(mspace_, size);
    }

    void MemorySpace::deallocate(void* mem)
    {
        LASSERT(NULL !=  mspace_);
        mspace_free(mspace_, mem);
    }

#define LCORE_POPULATIONCOUNT(val)\
    val = (val & 0x55555555U) + ((val>>1) & 0x55555555U);\
    val = (val & 0x33333333U) + ((val>>2) & 0x33333333U);\
    val = (val & 0x0F0F0F0FU) + ((val>>4) & 0x0F0F0F0FU);\
    val = (val & 0x00FF00FFU) + ((val>>8) & 0x00FF00FFU);\
    return (val & 0x0000FFFFU) + ((val>>16) & 0x0000FFFFU)

    u32 populationCount(u32 val)
    {
        LCORE_POPULATIONCOUNT(val);
    }

    //u32 mostSignificantBit(u32 v)
    //{
    //    static const u32 shifttable[] =
    //    {
    //        0, 1, 2, 2, 3, 3, 3, 3,
    //        4, 4, 4, 4, 4, 4, 4, 4,
    //    };
    //    u32 ret = 0;

    //    if(v & 0xFFFF0000U){
    //        ret += 16;
    //        v >>= 16;
    //    }

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

    u32 mostSiginificantBit(u32 val)
    {
#if defined(_MSC_VER)
        unsigned long index;
#if defined(_WIN64)
        return (_BitScanReverse64(&index, val))? (u32)index : 0;
#else
        return (_BitScanReverse(&index, val))? (u32)index : 0;
#endif

#elif defined(__GNUC__)
        return (0!=val)? (__builtin_clzl(value) ^ 0x3FU) : 0;
#else
        val = (~val) & (val-1);
        LCORE_POPULATIONCOUNT(val);
#endif
    }

    u32 leastSignificantBit(u32 val)
    {
#if defined(_MSC_VER)
        unsigned long index;
#if defined(_WIN64)
        return (_BitScanForward64(&index, val))? (u32)index : 0;
#else
        return (_BitScanForward(&index, val))? (u32)index : 0;
#endif

#elif defined(__GNUC__)
        return (0!=val)? (__builtin_ctzl(value)) : 0;
#else
        if(0 == val){
            return 32U;
        }
        u32 count = (val&0xAAAAAAAAU)? 0x01U:0;

        if(val&0xCCCCCCCCU){
            count |= 0x02U;
        }

        if(val&0xF0F0F0F0U){
            count |= 0x04U;
        }

        if(val&0xFF00FF00U){
            count |= 0x08U;
        }

        return 31U-((val&0xFFFF0000U)? count|0x10U:count);
#endif
    }

#undef LCORE_POPULATIONCOUNT

}

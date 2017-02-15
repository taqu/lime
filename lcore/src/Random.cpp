/**
@file Random.cpp
@author t-sakai
@date 2011/09/04
*/
#include "Random.h"

#ifdef _WIN32

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

namespace lcore
{
    namespace
    {
        u32 getRandomBasedOnDisckUsage()
        {
            u32 t = getTime();
            lcore::ClockType counter = getPerformanceCounter();

            LARGE_INTEGER freeBytesAvalable;
            LARGE_INTEGER totalBytes;
            LARGE_INTEGER totalFreeBytes;

            u32 discBytes = 0;
            BOOL ret = GetDiskFreeSpaceEx(
                NULL,
                (PULARGE_INTEGER)&freeBytesAvalable,
                (PULARGE_INTEGER)&totalBytes,
                (PULARGE_INTEGER)&totalFreeBytes);

            if(ret){
                discBytes = totalFreeBytes.LowPart;
            }

            MEMORYSTATUSEX memstat;

            GlobalMemoryStatusEx(&memstat);
            u32 memoryBytes = static_cast<u32>(memstat.ullAvailPhys & 0xFFFFFFFFU);
            u32 other = ( (discBytes<<16) | (memoryBytes & 0xFFFFU));
            return ((t<<16) | static_cast<u32>(counter&0xFFFFU)) ^ other;
        }

        u64 getRandomBasedOnDisckUsage64()
        {
            u32 t = getTime();
            lcore::ClockType counter = getPerformanceCounter();

            LARGE_INTEGER freeBytesAvalable;
            LARGE_INTEGER totalBytes;
            LARGE_INTEGER totalFreeBytes;

            s64 discBytes = 0;
            BOOL ret = GetDiskFreeSpaceEx(
                NULL,
                (PULARGE_INTEGER)&freeBytesAvalable,
                (PULARGE_INTEGER)&totalBytes,
                (PULARGE_INTEGER)&totalFreeBytes);

            if(ret){
                discBytes = totalFreeBytes.QuadPart;
            }

            MEMORYSTATUSEX memstat;

            GlobalMemoryStatusEx(&memstat);
            u64 memoryBytes = memstat.ullAvailPhys;
            u64 other = ( (discBytes<<32) | (memoryBytes & 0xFFFFFFFFU));
            return ((counter<<32) | t) ^ other;
        }

        u32 scramble(u32 v, u32 i)
        {
            return (1812433253 * (v^(v >> 30)) + i);
        }

        u64 scramble(u64 v, u64 i)
        {
            return (1812433253 * (v^(v >> 41)) + i);
        }
    }

    u32 getStaticSeed()
    {
        return 13249876;
    }

    u32 getDefaultSeed()
    {
        u32 seed;

#ifdef _WIN32
        seed = getRandomBasedOnDisckUsage();

#else //_WIN32

        u32 t = getTime();
        lcore::ClockType counter = getPerformanceCounter();
        seed = ((t<<16) | static_cast<u32>(counter&0xFFFFU));
#endif
        return seed;
    }

    u64 getStaticSeed64()
    {
        return 1181783497276652981ULL;
    }

    u64 getDefaultSeed64()
    {
        u64 seed;

#ifdef _WIN32
        seed = getRandomBasedOnDisckUsage64();

#else //_WIN32

        u32 t = getTime();
        lcore::ClockType counter = getPerformanceCounter();
        seed = ((counter<<32) | t);
#endif
        return seed;
    }

    //---------------------------------------------
    //---
    //--- RandXorshift
    //---
    //---------------------------------------------
#define LCORE_RANDOM_XORSHIFT_PROC \
    u32 t = x_^(x_<<11);\
    x_ = y_;\
    y_ = z_;\
    z_ = w_;\
    w_ = (w_^(w_>>19)) ^ (t^(t>>8));\

    RandXorshift::RandXorshift()
        :x_(123459876)
        ,y_(362436069)
        ,z_(521288629)
        ,w_(88675123)
    {
    }

    RandXorshift::RandXorshift(u32 seed)
    {
        srand(seed);
    }

    RandXorshift::~RandXorshift()
    {
    }

    void RandXorshift::srand(u32 seed)
    {
        x_ = seed & 0xFFFFFFFFU;
        y_ = scramble(x_, 1);
        z_ = scramble(y_, 2);
        w_ = scramble(z_, 3);
    }

    u32 RandXorshift::rand()
    {
LCORE_RANDOM_XORSHIFT_PROC
        return w_;
    }

    f32 RandXorshift::frand()
    {
LCORE_RANDOM_XORSHIFT_PROC

        static const u32 m0 = 0x3F800000U;
        static const u32 m1 = 0x007FFFFFU;
        t = m0|(w_&m1);
        return (*(f32*)&t) - 0.999999881f;
    }

    f32 RandXorshift::frand2()
    {
LCORE_RANDOM_XORSHIFT_PROC

        static const u32 m0 = 0x3F800000U;
        static const u32 m1 = 0x007FFFFFU;
        t = m0|(w_&m1);
        return (*(f32*)&t) - 1.000000000f;
    }

    f64 RandXorshift::drand()
    {
        return rand()*(1.0/4294967295.0); 
    }
#undef LCORE_RANDOM_XORSHIFT_PROC


    void RandXorshift::swap(RandXorshift& rhs)
    {
        lcore::swap(x_, rhs.x_);
        lcore::swap(y_, rhs.y_);
        lcore::swap(z_, rhs.z_);
        lcore::swap(w_, rhs.w_);
    }


    //---------------------------------------------
    //---
    //--- RandXorshift64Star
    //---
    //---------------------------------------------
#define LCORE_RANDOM_XORSHIFT64STAR_PROC \
    static const u64 M=2685821657736338717ULL;\
    u64 s = s_;\
    s_ ^= s>>12;\
    s_ ^= s_<<25;\
    s_ ^= s_>>27;\
    const u64 r = s * M;\

    RandXorshift64Star::RandXorshift64Star()
        :s_(0x8a5cd789635d2dffULL)
    {
    }

    RandXorshift64Star::RandXorshift64Star(u64 seed)
    {
        srand(seed);
    }

    RandXorshift64Star::~RandXorshift64Star()
    {
    }

    void RandXorshift64Star::srand(u64 seed)
    {
        s_ = scramble(seed, 1);
    }

    u64 RandXorshift64Star::rand()
    {
        LCORE_RANDOM_XORSHIFT64STAR_PROC
        return r;
    }

    void RandXorshift64Star::swap(RandXorshift64Star& rhs)
    {
        lcore::swap(s_, rhs.s_);
    }
#undef LCORE_RANDOM_XORSHIFT64STAR_PROC

    //---------------------------------------------
    //---
    //--- RandXorshift64Star32
    //---
    //---------------------------------------------
#define LCORE_RANDOM_XORSHIFT64STAR32_PROC \
    static const u64 M=2685821657736338717ULL;\
    u64 s = s_;\
    s_ ^= s>>12;\
    s_ ^= s_<<25;\
    s_ ^= s_>>27;\
    r_ = s * M;\

    RandXorshift64Star32::RandXorshift64Star32()
        :s_(0x8a5cd789635d2dffULL)
        ,r_(0)
        ,flag_(1)
    {
    }

    RandXorshift64Star32::RandXorshift64Star32(u64 seed)
        :r_(0)
        ,flag_(1)
    {
        srand(seed);
    }

    RandXorshift64Star32::~RandXorshift64Star32()
    {
    }

    void RandXorshift64Star32::srand(u64 seed)
    {
        s_ = scramble(seed, 1);
    }

    u32 RandXorshift64Star32::rand()
    {
        if(flag_^=1){
            return static_cast<u32>(r_>>32);
        }
        LCORE_RANDOM_XORSHIFT64STAR32_PROC
        return static_cast<u32>(r_);
    }
#undef LCORE_RANDOM_XORSHIFT64STAR32_PROC

    f32 RandXorshift64Star32::frand()
    {
        u32 t = rand();

        static const u32 m0 = 0x3F800000U;
        static const u32 m1 = 0x007FFFFFU;
        t = m0|(t&m1);
        return (*(f32*)&t)- 0.999999881f;
    }

    f32 RandXorshift64Star32::frand2()
    {
        u32 t = rand();

        static const u32 m0 = 0x3F800000U;
        static const u32 m1 = 0x007FFFFFU;
        t = m0|(t&m1);
        return (*(f32*)&t) - 1.000000000f;
    }

    f64 RandXorshift64Star32::drand()
    {
        return rand()*(1.0/4294967295.0); 
    }

    void RandXorshift64Star32::swap(RandXorshift64Star32& rhs)
    {
        lcore::swap(s_, rhs.s_);
        lcore::swap(r_, rhs.r_);
        lcore::swap(flag_, rhs.flag_);
    }

    //---------------------------------------------
    //---
    //--- RandXorshift128Plus
    //---
    //---------------------------------------------
#define LCORE_RANDOM_XORSHIFT128PLUS_PROC \
    u64 s1 = s0_;\
    const u64 s0 = s1_;\
    s0_ = s0;\
    s1 ^= s1<<23;\
    s1_ = s1^s0^(s1>>18)^(s0>>5);\
    const u64 r = s1_+s0;\

    RandXorshift128Plus::RandXorshift128Plus()
        :s0_(0x8a5cd789635d2dffULL)
        ,s1_(0x121fd2155c472f96ULL)
    {
    }

    RandXorshift128Plus::RandXorshift128Plus(u64 seed)
    {
        srand(seed);
    }

    RandXorshift128Plus::~RandXorshift128Plus()
    {
    }

    void RandXorshift128Plus::srand(u64 seed)
    {
        s0_ = seed;
        s1_ = scramble(s0_, 1);
    }

    u64 RandXorshift128Plus::rand()
    {
LCORE_RANDOM_XORSHIFT128PLUS_PROC
        return r;
    }

    void RandXorshift128Plus::swap(RandXorshift128Plus& rhs)
    {
        lcore::swap(s0_, rhs.s0_);
        lcore::swap(s1_, rhs.s1_);
    }
#undef LCORE_RANDOM_XORSHIFT128PLUS_PROC

    //---------------------------------------------
    //---
    //--- RandXorshift128Plus
    //---
    //---------------------------------------------
#define LCORE_RANDOM_XORSHIFT128PLUS32_PROC \
    u64 s1 = s0_;\
    const u64 s0 = s1_;\
    s0_ = s0;\
    s1 ^= s1<<23;\
    s1_ = s1^s0^(s1>>18)^(s0>>5);\
    r_ = s1_+s0;\

    RandXorshift128Plus32::RandXorshift128Plus32()
        :s0_(0x8a5cd789635d2dff)
        ,s1_(0x121fd2155c472f96)
        ,r_(0)
        ,flag_(1)
    {
    }

    RandXorshift128Plus32::RandXorshift128Plus32(u64 seed)
        :r_(0)
        ,flag_(1)
    {
        srand(seed);
    }

    RandXorshift128Plus32::~RandXorshift128Plus32()
    {
    }

    void RandXorshift128Plus32::srand(u64 seed)
    {
        s0_ = seed;
        s1_ = scramble(s0_, 1);
    }

    u32 RandXorshift128Plus32::rand()
    {
        if(flag_^=1){
            return static_cast<u32>(r_>>32);
        }
        LCORE_RANDOM_XORSHIFT128PLUS32_PROC
        return static_cast<u32>(r_);
    }
#undef LCORE_RANDOM_XORSHIFT128PLUS32_PROC

    f32 RandXorshift128Plus32::frand()
    {
        u32 t = rand();

        static const u32 m0 = 0x3F800000U;
        static const u32 m1 = 0x007FFFFFU;
        t = m0|(t&m1);
        return (*(f32*)&t)- 0.999999881f;
    }

    f32 RandXorshift128Plus32::frand2()
    {
        u32 t = rand();

        static const u32 m0 = 0x3F800000U;
        static const u32 m1 = 0x007FFFFFU;
        t = m0|(t&m1);
        return (*(f32*)&t) - 1.000000000f;
    }

    f64 RandXorshift128Plus32::drand()
    {
        return rand()*(1.0/4294967295.0); 
    }

    void RandXorshift128Plus32::swap(RandXorshift128Plus32& rhs)
    {
        lcore::swap(s0_, rhs.s0_);
        lcore::swap(s1_, rhs.s1_);
        lcore::swap(r_, rhs.r_);
        lcore::swap(flag_, rhs.flag_);
    }

    //---------------------------------------------
    //---
    //--- RandWELL
    //---
    //---------------------------------------------
    RandWELL::RandWELL()
        :index_(0)
    {
    }

    RandWELL::RandWELL(u32 seed)
        :index_(0)
    {
        srand(seed);
    }

    RandWELL::~RandWELL()
    {
    }

    void RandWELL::srand(u32 seed)
    {
        state_[0] = seed;
        for(u32 i=1; i<N; ++i){
            state_[i] = (1812433253 * (state_[i-1]^(state_[i-1] >> 30)) + i); 
        }
    }

    u32 RandWELL::rand()
    {
        u32 a, b, c, d;

        a = state_[index_];
        c = state_[(index_+13)&15];
        b = a^c^(a<<16)^(c<<15);
        c = state_[(index_+9)&15];
        c ^= c>>11;
        a = state_[index_] = b^c;
        d = a^((a<<5)&0xDA442D24UL);
        index_ = (index_ + 15) & 15;
        a = state_[index_];
        state_[index_] = a^b^d^(a<<2)^(b<<18)^(c<<28);
        return state_[index_];
    }

    f32 RandWELL::frand()
    {
        u32 t = rand();

        static const u32 m0 = 0x3F800000U;
        static const u32 m1 = 0x007FFFFFU;
        t = m0|(t&m1);
        return (*(f32*)&t)- 0.999999881f;
    }

    f32 RandWELL::frand2()
    {
        u32 t = rand();

        static const u32 m0 = 0x3F800000U;
        static const u32 m1 = 0x007FFFFFU;
        t = m0|(t&m1);
        return (*(f32*)&t) - 1.000000000f;
    }

    f64 RandWELL::drand()
    {
        return rand()*(1.0/4294967295.0); 
    }

    void RandWELL::swap(RandWELL& rhs)
    {
        for(u32 i=0; i<N; ++i){
            lcore::swap(state_[i], rhs.state_[i]);
        }
        lcore::swap(index_, rhs.index_);
    }


    void cryptRandom(u32 size, void* buffer)
    {
        LASSERT(NULL != buffer);
        HMODULE handle = LoadLibrary("Advapi32.dll");
        if(NULL != handle){
            FARPROC procAddress = GetProcAddress(handle, "SystemFunction036");
            BOOLEAN result = (*(BOOLEAN (*)(PVOID, ULONG))procAddress)(buffer, size);
            FreeLibrary(handle);
            if(result){
                return;
            }
        }

        RandWELL random(lcore::getDefaultSeed());
        u32 count = size/sizeof(u32);
        u32* pu32 = static_cast<u32*>(buffer);
        for(u32 i=0; i<count; ++i){
            *pu32 = random.rand();
            ++pu32;
        }
        count = size - count*sizeof(u32);
        u8* pu8 = reinterpret_cast<u8*>(pu32);
        for(u32 i=0; i<count; ++i){
            *pu8 = static_cast<u8>(random.rand());
            ++pu8;
        }
    }
}

/**
@file Random.cpp
@author t-sakai
@date 2011/09/04
*/
#include "Random.h"
#include "../utility.h"

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
    u32 getStaticSeed()
    {
        return 13249876;
    }

    u32 getDefaultSeed()
    {
        u32 t = getTime();
        u32 counter = getPerformanceCounter();

        u32 other = 0;

#ifdef _WIN32
        LARGE_INTEGER freeBytesAvalable;
        LARGE_INTEGER totalBytes;
        LARGE_INTEGER totalFreeBytes;

        u32 discBytes = 0;
        BOOL ret = FALSE;
        ret = GetDiskFreeSpaceEx(
            NULL,
            (PULARGE_INTEGER)&freeBytesAvalable,
            (PULARGE_INTEGER)&totalBytes,
            (PULARGE_INTEGER)&totalFreeBytes);

        if(ret){
            discBytes = totalFreeBytes.LowPart;
        }

        MEMORYSTATUS memstat;

        GlobalMemoryStatus(&memstat);
        u32 memoryBytes = memstat.dwAvailPhys;
        other = ( (discBytes<<16) | (memoryBytes));
#endif

        u32 seed = ((t<<16) | (counter&0xFFFFU)) ^ other;
        return seed;
    }


    RandomXorshift::RandomXorshift()
        :x_(123459876)
        ,y_(362436069)
        ,z_(521288629)
        ,w_(88675123)
    {
    }

    RandomXorshift::~RandomXorshift()
    {
    }

    void RandomXorshift::srand(u32 seed)
    {
        x_ = seed & 0xFFFFFFFFU;
        y_ = rand(x_);
        z_ = rand(y_);
        w_ = rand(z_);
    }

    u32 RandomXorshift::rand()
    {
        u32 t = x_^(x_<<11);
        x_ = y_;
        y_ = z_;
        z_ = w_;
        w_ = (w_^(w_>>19)) ^ (t^(t>>8));
        return w_;
    }

    f32 RandomXorshift::frand()
    {
        u32 t = x_^(x_<<11);
        x_ = y_;
        y_ = z_;
        z_ = w_;
        w_ = (w_^(w_>>19)) ^ (t^(t>>8));

        static const u32 m0 = 0x3F800000U;
        static const u32 m1 = 0x007FFFFFU;
        t = m0|(w_&m1);
        return (*(f32*)&t)- 0.9999999f;
    }

    f64 RandomXorshift::drand()
    {
        return rand()*(1.0/4294967295.0); 
    }

    u32 RandomXorshift::rand(u32 v)
    {
        return (v*1664525 + 1013904223);
    }



    RandomWELL::RandomWELL()
        :index_(0)
    {
    }

    RandomWELL::~RandomWELL()
    {
    }

    void RandomWELL::srand(u32 seed)
    {
        state_[0] = seed;
        for(u32 i=1; i<N; ++i){
            state_[i] = (1812433253 * (state_[i-1]^(state_[i-1] >> 30)) + i); 
        }
    }

    u32 RandomWELL::rand()
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

    f32 RandomWELL::frand()
    {
        u32 t = rand();

        static const u32 m0 = 0x3F800000U;
        static const u32 m1 = 0x007FFFFFU;
        t = m0|(t&m1);
        return (*(f32*)&t)- 0.9999999f;
    }

    f64 RandomWELL::drand()
    {
        return rand()*(1.0/4294967295.0); 
    }
}

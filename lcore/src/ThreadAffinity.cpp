/**
@file ThreadAffinity.cpp
@author t-sakai
@date 2017/04/05 create
*/
#include "ThreadAffinity.h"
#include "Thread.h"
#include "Random.h"

namespace lcore
{
    //----------------------------------------------------
    //---
    //--- ThreadAffinity
    //---
    //----------------------------------------------------
#define LCORE_RANDOM_XORSHIFT_PROC \
    u32 t = x_^(x_<<11);\
    x_ = y_;\
    y_ = z_;\
    z_ = w_;\
    w_ = (w_^(w_>>19)) ^ (t^(t>>8));\

    ThreadAffinity::Random::Random()
        :x_(123459876)
        ,y_(362436069)
        ,z_(521288629)
        ,w_(88675123)
    {
    }

    ThreadAffinity::Random::~Random()
    {
    }

    void ThreadAffinity::Random::srand(u32 seed)
    {
        x_ = seed & 0xFFFFFFFFU;
        y_ = scramble(x_, 1);
        z_ = scramble(y_, 2);
        w_ = scramble(z_, 3);
    }

    u32 ThreadAffinity::Random::rand()
    {
        LCORE_RANDOM_XORSHIFT_PROC
        return w_;
    }

    ThreadAffinity::ThreadAffinity()
    {
    }

    ThreadAffinity::~ThreadAffinity()
    {
    }

    void ThreadAffinity::initialize()
    {
        cpuInformation_.initialize();
        random_.srand(getDefaultSeed());
        for(s32 i=0; i<cpuInformation_.getNumCores(); ++i){
            flags_[i] = Flag_None;
        }
    }

    s32 ThreadAffinity::setAffinity(LHANDLE thread, bool occupy)
    {
        u8 numAllowed = 0;
        u8 minAllocated = MaxAllocated;
        u8 num;
        for(s32 i=0; i<cpuInformation_.getNumCores(); ++i){
            if(0 != (Flag_Occupied&flags_[i])){
                continue;
            }
            num = flags_[i]>>1;
            minAllocated = minimum(minAllocated, num);
        }
        for(s32 i=0; i<cpuInformation_.getNumCores(); ++i){
            if(0 != (Flag_Occupied&flags_[i])){
                continue;
            }
            num = flags_[i]>>1;
            if(num == minAllocated){
                ++numAllowed;
            }
        }
        LASSERT(0<numAllowed);
        s32 index = static_cast<s32>(random_.rand()&0x7FFFFFFFU)%numAllowed;
        numAllowed = 0;
        s32 allocated = -1;
        for(s32 i=0; i<cpuInformation_.getNumCores(); ++i){
            if(0 != (Flag_Occupied&flags_[i])){
                continue;
            }
            num = flags_[i]>>1;
            if(num == minAllocated){
                if(numAllowed == index){
                    allocated = i;
                    break;
                }
                ++numAllowed;
            }
        }
        if(allocated<0){
            return -1;
        }
        num = flags_[allocated]>>1;
        if(num<MaxAllocated){
            num += 1;
        }
        flags_[allocated] = (num<<1) | (flags_[allocated] & Flag_Occupied);
        setThreadAffinityMask(thread, cpuInformation_.getCore(allocated).groupMask_);
        if(1<cpuInformation_.getNumCores() && occupy){
            flags_[allocated] |= Flag_Occupied;
        }
        return cpuInformation_.getCore(allocated).id_;
    }

    s32 ThreadAffinity::setAffinity(ThreadRaw& thread, bool occupy)
    {
        LASSERT(thread.valid());
        return setAffinity(thread.handle_, occupy);
    }

    s32 ThreadAffinity::setAffinity(Thread& thread, bool occupy)
    {
        LASSERT(thread.valid());
        return setAffinity(thread.handle_, occupy);
    }
}

/**
@file ThreadAffinity.h
@author t-sakai
@date 2017/04/05 create
*/
#include "lcore.h"
#include "CPU.h"

namespace lcore
{
    class ThreadRaw;
    class Thread;

    //----------------------------------------------------
    //---
    //--- ThreadAffinity
    //---
    //----------------------------------------------------
    class ThreadAffinity
    {
    public:
        static const s32 MaxCores = 32;
        static const u8 Flag_None = 0;
        static const u8 Flag_Occupied = 0x01U;
        static const u8 MaxAllocated = 0x7F;

        ThreadAffinity();
        ~ThreadAffinity();

        /**
        */
        void initialize();
        /**
        */
        inline const CPUInformation& getCPUInformation() const;

        /**
        */
        s32 setAffinity(LHANDLE thread, bool occupy);

        /**
        */
        s32 setAffinity(ThreadRaw& thread, bool occupy);

        /**
        */
        s32 setAffinity(Thread& thread, bool occupy);
    private:
        ThreadAffinity(const ThreadAffinity&);
        ThreadAffinity& operator=(const ThreadAffinity&);

        class Random
        {
        public:
            Random();
            ~Random();
            void srand(u32 seed);
            u32 rand();
        private:
            u32 x_;
            u32 y_;
            u32 z_;
            u32 w_;
        };

        CPUInformation cpuInformation_;
        Random random_;
        u8 flags_[MaxCores];
    };

    inline const CPUInformation& ThreadAffinity::getCPUInformation() const
    {
        return cpuInformation_;
    }
}

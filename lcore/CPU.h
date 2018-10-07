﻿#ifndef INC_LCORE_CPU_H_
#define INC_LCORE_CPU_H_
/**
@file CPU.h
@author t-sakai
@date 2012/01/08 create
*/
#include "lcore.h"

namespace lcore
{

#define LCORE_CPUID_BIT_FLAG(v) (0x00000001U << v)

    enum CPUID_FUNC
    {
        CPUID_FUNC_VERSIONID = 0x00000000U,
        CPUID_FUNC_CPUINFO   = 0x00000001U,
    };

    /// CPUID_FUNC_CPUINFOをEAXに指定した場合の、EDXに返ってくるフラグ
    enum CPUINFO_FLAG
    {
        CPUINFO_FPU = LCORE_CPUID_BIT_FLAG(0),
        CPUINFO_VME = LCORE_CPUID_BIT_FLAG(1),
        CPUINFO_DE = LCORE_CPUID_BIT_FLAG(2),
        CPUINFO_PSE = LCORE_CPUID_BIT_FLAG(3),
        CPUINFO_TSC = LCORE_CPUID_BIT_FLAG(4),
        CPUINFO_MSR = LCORE_CPUID_BIT_FLAG(5),
        CPUINFO_PAE = LCORE_CPUID_BIT_FLAG(6),
        CPUINFO_MCE = LCORE_CPUID_BIT_FLAG(7),
        CPUINFO_CX8 = LCORE_CPUID_BIT_FLAG(8),
        CPUINFO_APIC = LCORE_CPUID_BIT_FLAG(9),
        CPUINFO_RESERVED0 = LCORE_CPUID_BIT_FLAG(10),
        CPUINFO_SEP = LCORE_CPUID_BIT_FLAG(11),
        CPUINFO_MTRR = LCORE_CPUID_BIT_FLAG(12),
        CPUINFO_PGE = LCORE_CPUID_BIT_FLAG(13),
        CPUINFO_MCA = LCORE_CPUID_BIT_FLAG(14),
        CPUINFO_CMOV = LCORE_CPUID_BIT_FLAG(15),
        CPUINFO_PAT = LCORE_CPUID_BIT_FLAG(16),
        CPUINFO_PSE36 = LCORE_CPUID_BIT_FLAG(17),
        CPUINFO_PSN = LCORE_CPUID_BIT_FLAG(18),
        CPUINFO_CFLSH = LCORE_CPUID_BIT_FLAG(19),
        CPUINFO_RESERVED1 = LCORE_CPUID_BIT_FLAG(20),
        CPUINFO_DTES = LCORE_CPUID_BIT_FLAG(21),
        CPUINFO_ACPI = LCORE_CPUID_BIT_FLAG(22),
        CPUINFO_MMX = LCORE_CPUID_BIT_FLAG(23),
        CPUINFO_FXSR = LCORE_CPUID_BIT_FLAG(24),
        CPUINFO_SSE = LCORE_CPUID_BIT_FLAG(25),
        CPUINFO_SSE2 = LCORE_CPUID_BIT_FLAG(26),
        CPUINFO_SELFSNOOP = LCORE_CPUID_BIT_FLAG(27),
        CPUINFO_HTT = LCORE_CPUID_BIT_FLAG(28),
        CPUINFO_ACC = LCORE_CPUID_BIT_FLAG(29),
        CPUINFO_IA64 = LCORE_CPUID_BIT_FLAG(30),
        CPUINFO_PBE = LCORE_CPUID_BIT_FLAG(31),
    };


    /// CPUID_FUNC_CPUINFOをEAXに指定した場合の、ECXに返ってくるフラグ
    enum CPUINFO2_FLAG
    {
        CPUINFO2_SSE3 = LCORE_CPUID_BIT_FLAG(0),
        CPUINFO2_PCLMUL = LCORE_CPUID_BIT_FLAG(1),
        CPUINFO2_DTES64 = LCORE_CPUID_BIT_FLAG(2),
        CPUINFO2_MON = LCORE_CPUID_BIT_FLAG(3),
        CPUINFO2_DSCPL = LCORE_CPUID_BIT_FLAG(4),
        CPUINFO2_VMX = LCORE_CPUID_BIT_FLAG(5),
        CPUINFO2_SMX = LCORE_CPUID_BIT_FLAG(6),
        CPUINFO2_EST = LCORE_CPUID_BIT_FLAG(7),
        CPUINFO2_TM2 = LCORE_CPUID_BIT_FLAG(8),
        CPUINFO2_SSSE3 = LCORE_CPUID_BIT_FLAG(9),
        CPUINFO2_CID = LCORE_CPUID_BIT_FLAG(10),
        CPUINFO2_RESERVED0 = LCORE_CPUID_BIT_FLAG(11),
        CPUINFO2_FMA = LCORE_CPUID_BIT_FLAG(12),
        CPUINFO2_CX16 = LCORE_CPUID_BIT_FLAG(13),
        CPUINFO2_ETPRD = LCORE_CPUID_BIT_FLAG(14),
        CPUINFO2_PDCM = LCORE_CPUID_BIT_FLAG(15),
        CPUINFO2_RESERVED1 = LCORE_CPUID_BIT_FLAG(16),
        CPUINFO2_PCID = LCORE_CPUID_BIT_FLAG(17),
        CPUINFO2_DCA = LCORE_CPUID_BIT_FLAG(18),
        CPUINFO2_SSE41 = LCORE_CPUID_BIT_FLAG(19),
        CPUINFO2_SSE42 = LCORE_CPUID_BIT_FLAG(20),
        CPUINFO2_x2APIC = LCORE_CPUID_BIT_FLAG(21),
        CPUINFO2_MOVBE = LCORE_CPUID_BIT_FLAG(22),
        CPUINFO2_POPCNT = LCORE_CPUID_BIT_FLAG(23),
        CPUINFO2_TSCD = LCORE_CPUID_BIT_FLAG(24),
        CPUINFO2_AES = LCORE_CPUID_BIT_FLAG(25),
        CPUINFO2_XSAVE = LCORE_CPUID_BIT_FLAG(26),
        CPUINFO2_OSXSAVE = LCORE_CPUID_BIT_FLAG(27),
        CPUINFO2_AVX = LCORE_CPUID_BIT_FLAG(28),
        CPUINFO2_F16C = LCORE_CPUID_BIT_FLAG(29),
        CPUINFO2_RDRAND = LCORE_CPUID_BIT_FLAG(30),
        CPUINFO2_RESERVED2 = LCORE_CPUID_BIT_FLAG(31),
    };

#undef LCORE_CPUID_BIT_FLAG

#if defined(__GNUC__)
    void cpuid(u32);

#elif defined(_MSC_VER)

    bool isSupportCPUID();

    void cpuid(s32 func, s32& a, s32& b, s32& c, s32& d);

    /**
    @brief 論理プロセッサ数取得
    @return 論理プロセッサ数
    */
    u32 getLogicalCPUCount();

#endif

    struct CPUCore
    {
        u64 groupMask_;
        s32 group_;
        s16 id_;
        s16 numTHreads_;
    };

    class CPUInformation
    {
    public:
        static const s32 MaxCores = 32;
        static const s32 MaxLogicalCores = 64;

        CPUInformation();
        ~CPUInformation();

        void initialize();

        inline s32 getNumCores() const;
        inline const CPUCore& getCore(s32 index) const;
    private:
        s32 numCores_;
        CPUCore cores_[MaxCores];
    };

    inline s32 CPUInformation::getNumCores() const
    {
        return numCores_;
    }

    inline const CPUCore& CPUInformation::getCore(s32 index) const
    {
        return cores_[index];
    }
}
#endif //INC_LCORE_CPU_H_

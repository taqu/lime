/**
@file CPU.cpp
@author t-sakai
@date 2012/01/08 create
*/
#include "CPU.h"

#if defined(_WIN32)
#include <Windows.h>
#include <intrin.h>

#elif defined(__linux__)
#include <unistd.h>
#endif


namespace lcore
{
#if defined(__GNUC__)
    void cpuid(u32);

#elif defined(_MSC_VER)

    bool isSupportCPUID()
    {
#if defined(_WIN64)
        return true;
#else
        u32 eflag = 0;

        _asm
        {
            pushfd
            pop eax  ;load EFLAGS to EAX
            mov ecx,eax
            xor eax,200000h  ;flip ID bit in EFLAGS
            push eax
            popfd
            pushfd
            pop eax
            xor eax,ecx
            push ecx
            popfd
            mov eflag,eax
        }
        return (eflag != 0);
#endif
    }

    void cpuid(s32 func, s32& a, s32& b, s32& c, s32& d)
    {
#if 1
        s32 cpuinfo[4];
        __cpuid(cpuinfo, func);
        a = cpuinfo[0];
        b = cpuinfo[1];
        c = cpuinfo[2];
        d = cpuinfo[3];
#else
        s32 ta = func;
        s32 tb = 0;
        s32 tc = 0;
        s32 td = 0;
        _asm
        {
            pushad
            mov eax,ta
            mov ebx,tb
            mov ecx,tc
            mov edx,td
            cpuid
            mov ta,eax
            mov tb,ebx
            mov tc,ecx
            mov td,edx
            popad
        }
        a = ta;
        b = tb;
        c = tc;
        d = td;
#endif
    }

#if 0
    bool isWin2000()
    {
        OSVERSIONINFOEX versionInfo;
        ZeroMemory(&versionInfo, sizeof(OSVERSIONINFOEX));
        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        versionInfo.dwMajorVersion = 5;

        DWORDLONG conditionMask = 0;
        VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
        return TRUE == VerifyVersionInfo(&versionInfo, VER_MAJORVERSION, conditionMask);
    }
#endif

    /**
    @brief 論理プロセッサ数取得
    @return 論理プロセッサ数
    */
    u32 getLogicalCPUCount()
    {
#if defined(_WIN32)
        SYSTEM_INFO systemInfo;
        GetSystemInfo(&systemInfo);
        return systemInfo.dwNumberOfProcessors;
#elif defined(__linux__)
        return sysconf(_SC_NPROCESSORS_ONLN);
#endif
    }

namespace
{
    s16 calcIDIn2Thread(u64 mask)
    {
        s16 id = 0;
        while(0 != (mask >>= 2)){
            ++id;
        }
        return id;
    }

    s16 calcIDIn1Thread(u64 mask)
    {
        s16 id = 0;
        while(0 != (mask >>= 1)){
            ++id;
        }
        return id;
    }
}

    //---------------------------------------------------------
    CPUInformation::CPUInformation()
        :numCores_(0)
    {
    }

    CPUInformation::~CPUInformation()
    {
    }

    void CPUInformation::initialize()
    {
        numCores_ = 0;
#if defined(_WIN32)

        BYTE* buffer = NULL;
        DWORD length = 0;

        if(FALSE == GetLogicalProcessorInformationEx(RelationProcessorCore, NULL, &length)){
            if(ERROR_INSUFFICIENT_BUFFER == GetLastError()){
                buffer = (BYTE*)LMALLOC(length);
                if(NULL == buffer){
                    return;
                }
                if(FALSE == GetLogicalProcessorInformationEx(RelationProcessorCore, reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(buffer), &length)){
                    LFREE(buffer);
                    return;
                }
            }else{
                return;
            }
        }

        DWORD offset=0;
        while(offset<length){
            SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* info = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(buffer+offset);
            if(length<(offset+info->Size)){
                break;
            }
            for(s32 i=0; i<info->Processor.GroupCount; ++i){
                if(0 == info->Processor.GroupMask[i].Mask){
                    continue;
                }
                cores_[numCores_].groupMask_ = info->Processor.GroupMask[i].Mask;
                cores_[numCores_].group_ = info->Processor.GroupMask[i].Group;
                cores_[numCores_].numTHreads_ = (0 != (LTP_PC_SMT & info->Processor.Flags))? 2 : 1;

                switch(cores_[numCores_].numTHreads_)
                {
                case 1:
                    cores_[numCores_].id_ = calcIDIn1Thread(cores_[numCores_].groupMask_);
                    break;
                case 2:
                    cores_[numCores_].id_ = calcIDIn2Thread(cores_[numCores_].groupMask_);
                    break;
                default:
                    cores_[numCores_].id_ = static_cast<s16>(i);
                    break;
                }

                if(MaxCores<=++numCores_){
                    offset = length;
                    break;
                }
            }
            offset += info->Size;
        }
        LFREE(buffer);

#elif defined(__linux__)
        return;
#endif
    }

#endif
}

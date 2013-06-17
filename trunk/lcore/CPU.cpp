/**
@file CPU.cpp
@author t-sakai
@date 2012/01/08 create
*/
#include "CPU.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <intrin.h>
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

    bool isWin2000()
    {
        OSVERSIONINFO versionInfo;
        ZeroMemory(&versionInfo, sizeof(OSVERSIONINFO));
        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        GetVersionEx(&versionInfo);

        return (versionInfo.dwMajorVersion >= 5);
    }

    /**
    @brief 論理プロセッサ数取得
    @return 論理プロセッサ数
    */
    u32 getLogicalCPUCount()
    {
        //OSバージョンチェック
        if(false == isWin2000()){
            return 1;
        }

#if 0
        HANDLE process = GetCurrentProcess();
        DWORD_PTR pmask, smask;

        if(FALSE == GetProcessAffinityMask(process, &pmask, &smask)){
            return 1;
        }
        if(pmask == 0){
            return 1;
        }

        u32 cpuCount = 0;

        while(pmask != 0){
            pmask >>= 1;
            ++cpuCount;
        }

        return cpuCount;

#else
        SYSTEM_INFO systemInfo;
        GetSystemInfo(&systemInfo);
        return systemInfo.dwNumberOfProcessors;
#endif
    }
#endif
}

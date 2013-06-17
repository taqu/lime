/**
@file utlity.cpp
@author t-sakai
@date 2011/02/05
*/
#include "lcore.h"
#include "utility.h"

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

namespace lcore
{
    //-------------------------------------------------------------
    // 後方から文字探索
    const Char* rFindChr(const Char* src, Char c, u32 size)
    {
        LASSERT(src != NULL);
        src += (size-1);
        for(u32 i=0; i<size; ++i){
            if(*src == c){
                return src;
            }
            --src;
        }
        return NULL;
    }


    //-------------------------------------------------------------
    // パスからディレクトリパス抽出
    u32 extractDirectoryPath(Char* dst, const Char* path, u32 length)
    {
        if(length<=0){
            return 0;
        }

        s32 i = static_cast<s32>(length-1);
        for(; 0<=i; --i){
            if(path[i] == '/' || path[i] == '\\'){
                break;
            }
        }
        u32 dstSize = i+1;
        for(u32 j=0; j<dstSize; ++j){
            dst[j] = path[j];
        }
        dst[dstSize] = '\0';
        return dstSize;
    }


    //---------------------------------------------------------
    //---
    //--- タイム関係
    //---
    //---------------------------------------------------------
    // カウント取得
    ClockType getPerformanceCounter()
    {
#ifdef _WIN32
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
#ifdef _WIN32
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return freq.QuadPart;
#else
        return CLOCKS_PER_SEC;
#endif
    }

    // 秒単位の時間差分計算
    f32 calcTime(ClockType prevTime, ClockType currentTime)
    {
        ClockType d = (currentTime>=prevTime)? currentTime - prevTime : lcore::numeric_limits<ClockType>::maximum() - prevTime + currentTime;
        f64 delta = static_cast<f64>(d)/getPerformanceFrequency();
        return static_cast<f32>(delta);
    }

    // ミリ秒単位の時間を取得
    u32 getTime()
    {
#ifdef _WIN32
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
#if defined(_WIN32)
            return 1;
#else
            rusage t;
            getrusage(RUSAGE_SELF, &t);
            return static_cast<u32>(t.ru_utime.tv_sec * 1000 + t.ru_utime.tv_usec/1000);
#endif
        }
    }
}

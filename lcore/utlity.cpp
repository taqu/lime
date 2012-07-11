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
    u32 getPerformanceCounter()
    {
#ifdef _WIN32
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        return static_cast<u32>( count.QuadPart );
#else
        clock_t t = 0;
        t = clock();
        return static_cast<u32>(t);
#endif
    }

    // 秒間カウント数
    u32 getPerformanceFrequency()
    {
#ifdef _WIN32
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return freq.QuadPart;
#else
        return CLOCKS_PER_SEC;
#endif
    }

    // マイクロ秒単位で時間取得
    u32 getTimeFromPerformanCounter()
    {
#ifdef _WIN32
        LARGE_INTEGER count;
        LARGE_INTEGER freq;
        QueryPerformanceCounter(&count);
        QueryPerformanceFrequency(&freq);
        return static_cast<u32>( (count.QuadPart*(1000*1000))/freq.QuadPart );
#else
        clock_t t = 0;
        t = clock();
        return static_cast<u32>( (t * (1000*1000))/CLOCKS_PER_SEC );
        //rusage t;
        //getrusage(RUSAGE_SELF, &t);
        //return static_cast<u32>(t.ru_utime.tv_usec);
#endif
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

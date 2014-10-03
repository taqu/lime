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
    lcore::f64 calcTime64(lcore::ClockType prevTime, lcore::ClockType currentTime)
    {
        lcore::ClockType d = (currentTime>=prevTime)? currentTime - prevTime : lcore::numeric_limits<lcore::ClockType>::maximum() - prevTime + currentTime;
        lcore::f64 delta = static_cast<lcore::f64>(d)/lcore::getPerformanceFrequency();
        return delta;
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

    //---------------------------------------------------------
    //---
    //--- Character Code
    //---
    //---------------------------------------------------------
    // UTF8 to UTF16
    s32 UTF8toUTF16(u16& utf16, const Char* utf8)
    {
        LASSERT(NULL != utf8);
        u8 firstByte = ~(*((u8*)utf8));
        if(firstByte & 0x80U){
            utf16 = utf8[0];
            return 1;

        }else if(firstByte & 0x40U){

        }else if(firstByte & 0x20U){
            u8 c0 = (utf8[0] & 0x1FU);
            u8 c1 = (utf8[1] & 0x3FU);
            utf16 = (c0<<6) | c1;
            return 2;
        }else if(firstByte & 0x10U){
            u8 c0 = (utf8[0] & 0x0FU);
            u8 c1 = (utf8[1] & 0x3FU);
            u8 c2 = (utf8[2] & 0x3FU);
            utf16 = (c0<<12) | (c1<<6) | c2;
            return 3;
        }
        utf16 = 0;
        return 0;
    }

    // UTF16 to UTF8
    s32 UTF16toUTF8(Char* utf8, u16 utf16)
    {
        LASSERT(NULL != utf8);
        if(utf16 < 0x80U){
            utf8[0] = static_cast<u8>(utf16);
            return 1;
        }else if(utf16 < 0x800U){
            u8 c0 = static_cast<u8>((utf16>>6) | 0xC0U);
            u8 c1 = static_cast<u8>((utf16&0x3FU) | 0x80U);
            *((u8*)(utf8+0)) = c0;
            *((u8*)(utf8+1)) = c1;
            return 2;
        }else{
            u8 c0 = (utf16>>12) | 0xE0U;
            u8 c1 = ((utf16>>6)&0x3FU) | 0x80U;
            u8 c2 = ((utf16>>0)&0x3FU) | 0x80U;
            *((u8*)(utf8+0)) = c0;
            *((u8*)(utf8+1)) = c1;
            *((u8*)(utf8+2)) = c2;
            return 3;
        }
    }
}

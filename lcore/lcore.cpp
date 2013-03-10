/**
@file lcore.cpp
@author t-sakai
@date 2009/01/17 create
*/

#include "lcore.h"

#if defined(ANDROID)
#include <stdio.h>
#include <stdarg.h>

#else
#include <iostream>
#include <sstream>
#endif //defined(ANDROID)


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
    bool isLittleEndian()
    {
        u32 v = 1;
        return *reinterpret_cast<u8*>(&v) != 0;
    }

    u16 toBinary16Float(f32 f)
    {
        U32F32Union t;
        t.f_ = f;

        u16 sign = (t.u_>>16) & 0x8000U;
        s32 exponent = (t.u_>>23) & 0x00FFU;
        u32 fraction = t.u_ & 0x007FFFFFU;

        if(exponent == 0){
            return sign; //符号付き0

        }else if(exponent == 0xFFU){
            if(fraction == 0){
                return sign | 0x7C00U; //符号付きInf
            }else{
                return (fraction>>13) | 0x7C00U; //NaN
            }
        }else {
            exponent += (-127 + 15);
            if(exponent>=0x1F){ //オーバーフロー
                return sign | 0x7C00U;
            }else if(exponent<=0){ //アンダーフロー
                s32 shift = 14 - exponent;
                if(shift>24){ //表現できないほど小さい
                    return sign;
                }else{
                    fraction |= 0x800000U; //隠れた整数ビット足す
                    u16 frac = fraction >> shift;
                    if((fraction>>(shift-1)) & 0x01U){ //１ビット下位を丸める
                        frac += 1;
                    }
                    return sign | frac;
                }
            }
        }

        u16 ret = sign | ((exponent<<10) & 0x7C00U) | (fraction>>13);
        if((fraction>>12) & 0x01U){ //１ビット下位を丸める
            ret += 1;
        }
        return ret;
    }

    f32 fromBinary16Float(u16 s)
    {
        u32 sign = (s & 0x8000U) << 16;
        u32 exponent = ((s & 0x7C00U) >> 10);
        u32 fraction = (s & 0x03FFU);

        U32F32Union t;
        if(exponent == 0){
            if(fraction != 0){
                fraction <<= 1;
                while(0==(fraction & 0x0400U)){
                    ++exponent;
                    fraction <<= 1;
                }
                exponent = (127 - 15) - exponent;
                fraction &= 0x03FFU;
            }

        }else if(exponent == 0x1FU){
            exponent = 0xFFU; //Infinity か NaN

        }else{
            exponent += (127 - 15);
        }

         t.u_ = sign | (exponent<<23) | (fraction<<13);

        return t.f_;
    }


#if defined(_WIN32)
    LeakCheck::LeakCheck()
    {
#ifdef _DEBUG
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    }

    LeakCheck::~LeakCheck()
    {
    }

    System::System()
    {
        timeBeginPeriod(1);
    }

    System::~System()
    {
        timeEndPeriod(1);
    }

#else
    LeakCheck::LeakCheck()
    {
    }

    LeakCheck::~LeakCheck()
    {
    }

    System::System()
    {
    }

    System::~System()
    {
    }
#endif


    void Log(const Char* format, ...)
    {
#if defined(LIME_ENABLE_LOG)

        if(format == NULL){
            return;
        }

        va_list ap;
        va_start(ap, format);

#if defined(ANDROID)
        __android_log_vprint(ANDROID_LOG_DEBUG, "LIME", format, ap);
        //__android_log_vprint(ANDROID_LOG_ERROR, "LIME", format, ap);
#else
#ifdef _DEBUG
        static const u32 MaxBuffer = 256;
#else
        static const u32 MaxBuffer = 64;
#endif
        Char buffer[MaxBuffer+2];
        int count=vsnprintf(buffer, MaxBuffer, format, ap);
        if(count<0){
            count = MaxBuffer;
        }
#if defined(_WIN32)
        buffer[count]='\n';
        buffer[count+1]='\0';
        OutputDebugString(buffer);
#else
        buffer[count]='\n';
        buffer[count+1]='\0';
        std::cerr << buffer;
#endif

#endif

        va_end(ap);
#endif
    }

}

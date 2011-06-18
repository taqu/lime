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
#include <windows.h>
#include <mmsystem.h>
#else
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif


#include "Logger.h"

namespace lcore
{
#if defined(_WIN32)
typedef TLogger<CharTraitsMultiByte, DebugOutputter<CharTraitsMultiByte> > Logger;

#elif defined(ANDROID)
typedef TLogger<CharTraitsMultiByte, DebugOutputterAndroid<CharTraitsMultiByte> > Logger;

#else
typedef TLogger<CharTraitsMultiByte, DebugOutputterStdOut<CharTraitsMultiByte> > Logger;
#endif

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
        Logger::initialize();
    }

    System::~System()
    {
        Logger::terminate();
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
        //Logger::initialize();
    }

    System::~System()
    {
        //Logger::terminate();
    }
#endif


#if defined(_WIN32)
    void LogImpl(LogLevel level, const char* file, s32 line, const char* message)
    {
        std::stringstream ss;
        ss << message << ' ' << file << '(' << line << ')';

        switch(level)
        {
        case LogLevel_Info:
            Logger::info() << ss.str().c_str();
            break;
        case LogLevel_Warn:
            Logger::warn() << ss.str().c_str();
            break;
        case LogLevel_Error:
            Logger::error() << ss.str().c_str();
            break;
        default:
            Logger::error() << ss.str().c_str();
            break;
        }
    }

#else
    void LogImpl(LogLevel level, const char* file, s32 line, const char* format, ...)
    {
        switch(level)
        {
        case LogLevel_Info:
            fprintf(stderr, "[Info]: ");
            break;
        case LogLevel_Warn:
            fprintf(stderr, "[Warn]: ");
            break;
        case LogLevel_Error:
            fprintf(stderr, "[Error]: ");
            break;
        default:
            fprintf(stderr, "[Error]: ");
            break;
        }

        va_list ap;
        va_start(ap, format);

        vfprintf(stderr, format, ap);

        va_end(ap);

        fprintf(stderr, "(%s, %d)\n", file, line);
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
        static const u32 MaxBuffer = 64;
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

    //---------------------------------------------------------
    //---
    //--- タイム関係
    //---
    //---------------------------------------------------------
    // CPUクロック取得
    u32 getPerformanceCounter()
    {
#ifdef _WIN32
        LARGE_INTEGER count;
        LARGE_INTEGER freq;
        QueryPerformanceCounter(&count);
        QueryPerformanceFrequency(&freq);
        return static_cast<u32>( count.QuadPart );
#else
        clock_t t = 0;
        t = clock();
        return static_cast<u32>(t);
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

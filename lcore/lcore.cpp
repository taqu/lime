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

}

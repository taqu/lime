#ifndef INC_CONFIGDLL_H__
#define INC_CONFIGDLL_H__
/**
@file ConfigDLL.h
@author t-sakai
@date 2011/08/17 create
*/

#ifdef CONFIGDLL_EXPORTS
#define CONFIGDLL_DECL __declspec(dllexport)

#else
#define CONFIGDLL_DECL __declspec(dllimport)
#endif

namespace config
{
#ifdef _WIN32
    typedef char Char;
    typedef __int32 s32;
    typedef unsigned __int32 u32;

#elif defined(ANDROID) || defined(__linux__)
    typedef char Char;
    typedef int32_t s32;
    typedef uint32_t u32;

#else
    typedef char Char;
    typedef long s32;
    typedef unsigned long u32;
#endif

    enum BufferFormat
    {
        Buffer_A8R8G8B8 =0,
        Buffer_X8R8G8B8,
        Buffer_R5G6B5,
        Buffer_X1R5G5B5,
        Buffer_A1R5G5B5,
        Buffer_A2R10G10B10,
        Buffer_Num,
        Buffer_FORCEDWORD = 0xFFFFFFFFU,
    };

    struct Config
    {
        u32 width_;
        u32 height_;
        u32 rate_;
        s32 windowed_;
        u32 format_;
    };

    static const s32 Fail = 0;
    static const s32 OK = 1;
    static const s32 Cancel = 2;
}

#ifdef __cplusplus
extern "C"
{
#endif

    CONFIGDLL_DECL long getConfig(config::Config& conf, const config::Char* filepath, bool popup);

#ifdef __cplusplus
}
#endif

#endif //INC_CONFIGDLL_H__

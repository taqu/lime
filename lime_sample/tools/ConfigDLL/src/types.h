#ifndef INC_CONFIG_TYPES_H__
#define INC_CONFIG_TYPES_H__
/**
@file types.h
@author t-sakai
@date 2011/08/18 create
*/
#ifndef WINVER                          // 最低限必要なプラットフォームが Windows Vista であることを指定します。
#define WINVER 0x0600           // これを Windows の他のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_WINNT            // 最低限必要なプラットフォームが Windows Vista であることを指定します。
#define _WIN32_WINNT 0x0600     // これを Windows の他のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_WINDOWS          // 最低限必要なプラットフォームが Windows 98 であることを指定します。
#define _WIN32_WINDOWS 0x0410 // これを Windows Me またはそれ以降のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_IE                       // 最低限必要なプラットフォームが Internet Explorer 7.0 であることを指定します。
#define _WIN32_IE 0x0700        // これを IE の他のバージョン向けに適切な値に変更してください。
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cassert>

#include "../ConfigDLL.h"

#define CONF_NEW new
#define CONF_DELETE(ptr) {delete ptr; ptr=NULL;}
#define CONF_DELETE_ARRAY(ptr) {delete[] ptr; ptr = NULL;}

#ifdef _DEBUG
#define CONF_ASSERT(exp) assert(exp)
#else
#define CONF_ASSERT(exp)
#endif

struct IDirect3D9;

namespace config
{
#ifdef _WIN32
    typedef __int8 s8;
    typedef __int16 s16;
    typedef __int64 s64;

    typedef unsigned __int8 u8;
    typedef unsigned __int16 u16;
    typedef unsigned __int64 u64;

    typedef float f32;
    typedef double f64;

    typedef HINSTANCE HMODULE;

#elif defined(ANDROID) || defined(__linux__)
    typedef int8_t s8;
    typedef int16_t s16;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint64_t u64;

    typedef float f32;
    typedef double f64;

    typedef void* HMODULE;

#else
    typedef char s8;
    typedef short s16;
    typedef long long s64;

    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned long long u64;

    typedef float f32;
    typedef double f64;

    typedef void* HMODULE;
#endif
}

#endif //INC_CONFIG_TYPES_H__

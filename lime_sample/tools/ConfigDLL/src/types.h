#ifndef INC_CONFIG_TYPES_H__
#define INC_CONFIG_TYPES_H__
/**
@file types.h
@author t-sakai
@date 2011/08/18 create
*/
#ifndef WINVER                          // �Œ���K�v�ȃv���b�g�t�H�[���� Windows Vista �ł��邱�Ƃ��w�肵�܂��B
#define WINVER 0x0600           // ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_WINNT            // �Œ���K�v�ȃv���b�g�t�H�[���� Windows Vista �ł��邱�Ƃ��w�肵�܂��B
#define _WIN32_WINNT 0x0600     // ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_WINDOWS          // �Œ���K�v�ȃv���b�g�t�H�[���� Windows 98 �ł��邱�Ƃ��w�肵�܂��B
#define _WIN32_WINDOWS 0x0410 // ����� Windows Me �܂��͂���ȍ~�̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_IE                       // �Œ���K�v�ȃv���b�g�t�H�[���� Internet Explorer 7.0 �ł��邱�Ƃ��w�肵�܂��B
#define _WIN32_IE 0x0700        // ����� IE �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
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

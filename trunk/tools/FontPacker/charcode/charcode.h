#ifndef INC_CHARCODE_CHARCODE_H__
#define INC_CHARCODE_CHARCODE_H__
/**
@file charcode.h
@author t-sakai
@date 2013/08/29 create
*/
namespace charcode
{
#ifdef _WIN32
    typedef __int8 s8;
    typedef __int16 s16;
    typedef __int32 s32;
    typedef __int64 s64;

    typedef unsigned __int8 u8;
    typedef unsigned __int16 u16;
    typedef unsigned __int32 u32;
    typedef unsigned __int64 u64;

    typedef float f32;
    typedef double f64;

    typedef char Char;

#elif defined(ANDROID) || defined(__linux__)
    typedef int8_t s8;
    typedef int16_t s16;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint64_t u64;

    typedef float f32;
    typedef double f64;

    typedef char Char;

#else
    typedef char s8;
    typedef short s16;
    typedef long long s64;

    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned long long u64;

    typedef float f32;
    typedef double f64;

    typedef char Char;
#endif
}
#endif //INC_CHARCODE_CHARCODE_H__

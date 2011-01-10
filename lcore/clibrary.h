#ifndef INC_LCORE_CLIBRARY_H__
#define INC_LCORE_CLIBRARY_H__
/**
@file CLibrary.h
@author t-sakai
@date 2009/02/16 create
*/
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

namespace lcore
{

    template<class T>
    inline const T& maximum(const T& left, const T& right)
    {
        return (left<right)? right : left;
    }

    template<class T>
    inline const T& minimum(const T& left, const T& right)
    {
        return (right<left)? right : left;
    }

    inline double log(double x)
    {
        return ::log(x);
    }

    inline double log2(double x)
    {
        return log(x)/log(2.0);
    }

    inline double ceil(double x)
    {
        return ::ceil(x);
    }

    /// xのy乗を計算する
    inline double pow(double x, double y)
    {
        return ::pow(x, y);
    }

    inline bool isNan(double f)
    {
#if defined(_WIN32)
        return (0 != ::_isnan(f));
#else
        return (0 != isnan(f));
#endif
    }

    inline bool isSpace(Char c)
    {
        return (isspace(c) != 0);
    }

    inline bool isNullChar(Char c)
    {
        return (c == '\0');
    }

    inline int isEOL(const Char* str)
    {
        if(0x0D == *str){
            return (0x0A == *(str+1))? 2 : 1;
        }else if(0x0A == *str){
            return 1;
        }
        return 0;
    }

    inline int strncmp(const Char* str1, const Char* str2, int maxCount)
    {
        return ::strncmp(str1, str2, maxCount);
    }

    inline char* strncpy(Char* dst, size_t dstSize, const Char* src, size_t count)
    {
#if defined(WIN32)
        strncpy_s(dst, dstSize, src, count);
        return dst;
#else
        return ::strncpy(dst, src, count);
#endif
    }

    inline size_t strlen(const Char* str)
    {
        return ::strlen(str);
    }

    inline const Char* strstr(const Char* str, const Char* key)
    {
        return ::strstr(str, key);
    }


    //-----------------------------------------------------
    //
    inline int memcmp(const void* buff0, const void* buff1, unsigned int size)
    {
        return ::memcmp(buff0, buff1, size);
    }

    //-----------------------------------------------------
    //
    inline void* memset(void* dst, int val, unsigned int size)
    {
        return ::memset(dst, val, size);
    }

    //-----------------------------------------------------
    //
    inline void* memcpy(void* dst, const void* src, size_t size)
    {
        return ::memcpy(dst, src, size);
    }
}
#endif //INC_LCORE_CLIBRARY_H__

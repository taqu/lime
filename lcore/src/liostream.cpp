/**
@file liostream.cpp
@author t-sakai
@date 2010/07/26 create
*/
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include "lcore.h"
#include "liostream.h"

namespace lcore
{
//#if defined(ANDROID)
#if 1

    s32 ios::ModeInt[Mode_Num] =
    {
        ios::in,
        ios::out,
        ios::app,
        (ios::in | ios::binary),
        (ios::out | ios::binary),
        (ios::app | ios::out | ios::binary),
        (ios::in | ios::out),
        (ios::in | ios::out| ios::trunc),
        (ios::in | ios::out| ios::app),
        (ios::in | ios::out| ios::binary),
        (ios::in | ios::out| ios::trunc | ios::binary),
        (ios::in | ios::out| ios::app | ios::binary),
    };

    const Char* ios::ModeString[] =
    {
        "r",
        "w",
        "a",
        "rb",
        "wb",
        "ab",
        "r+",
        "w+",
        "a+",
        "rb+",
        "wb+",
        "ab+",
    };


    const Char* ios::getModeString(int mode)
    {
        for(int i=0; i<Mode_Num; ++i){
            if(mode == ModeInt[i]){
                return ModeString[i];
            }
        }
        return NULL;
    }

#if defined(_WIN32) || defined(_WIN64)
    u32 ios::getDesiredAccess(s32 mode)
    {
        u32 access = 0;
        if(mode & ios::in){
            access |= GENERIC_READ;
        }
        if(mode & ios::out){
            access |= GENERIC_WRITE;
        }
        return access;
    }

    u32 ios::getCreationDisposition(s32 mode)
    {
        u32 disposition = CREATE_ALWAYS;
        if(mode == ModeInt[Mode_R]){
            disposition = OPEN_EXISTING;

        }else if(mode == ModeInt[Mode_W]){
            disposition = CREATE_ALWAYS;

        }else if(mode == ModeInt[Mode_A]){
            disposition = OPEN_ALWAYS;

        }else if(mode == ModeInt[Mode_RP]){
            disposition = CREATE_ALWAYS;

        }else if(mode == ModeInt[Mode_WP]){
            disposition = CREATE_ALWAYS;

        }else if(mode == ModeInt[Mode_AP]){
            disposition = OPEN_ALWAYS;
        }
        return disposition;
    }

#else
    u32 getFileSize(FILE* file)
    {
        if(NULL == file){
            return 0;
        }
        s32 fd = fileno(file);
        if(fd<0){
            return 0;
        }
        stat buff;
        if(fstat(fd, &buff)<0){
            return 0;
        }
        return buff.st_size;
    }
#endif

    //----------------------------------------------------------
    //---
    //--- ifstream
    //---
    //----------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
    s32 ifstream::get()
    {
        s32 c = 0;
        DWORD numRead = 0;
        return ReadFile(file_, &c, 1, &numRead, NULL)? c : 0;
    }

    s32 ifstream::peek()
    {
        s32 c = 0;
        DWORD numRead = 0;
        if(ReadFile(file_, &c, 1, &numRead, NULL)){
            if(0<numRead){
                SetFilePointer(file_, -1, 0, FILE_CURRENT);
            }
            return c;
        }else{
            return 0;
        }
    }
#else
    s32 ifstream::get()
    {
        return fgetc(file_);
    }

    s32 ifstream::peek()
    {
        s32 c = getc(file_);
        return c==EOF? 0 : ungetc(c, file_);
    }
#endif

    //----------------------------------------------------------
    //---
    //--- ofstream
    //---
    //----------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
    s32 ofstream::print(const Char* format, ... )
    {
        LASSERT(NULL != file_);
        LASSERT(NULL != format);

        va_list ap;
        va_start(ap, format);

        s32 size = _vscprintf(format, ap) + 1;
        Char* buffer = (Char*)LMALLOC(size*sizeof(Char));
        vsprintf_s(buffer, size, format, ap);
        DWORD written = 0;
        s32 ret = WriteFile(file_, buffer, size-1, &written, NULL);
        LFREE(buffer);

        va_end(ap);
        return ret;
    }

    s32 ofstream::print(const Char16* format, ... )
    {
        LASSERT(NULL != file_);
        LASSERT(NULL != format);

        s32 size_utf8 = 0;
        const Char16* c16 = format;
        while(!isNullChar(*c16)){
            size_utf8 += getUTF8Size(*c16);
            ++c16;
        }
        size_utf8 += 1;
        Char* buffer = (Char*)LMALLOC(size_utf8*sizeof(Char));
        c16 = format;
        Char* c8 = buffer;
        while(isNullChar(*c16)){
            c8 += UTF16toUTF8(c8, *c16);
            ++c16;
        }
        *c8 = CharNull;
        va_list ap;
        va_start(ap, buffer);

        s32 size = _vscprintf(buffer, ap) + 1;
        Char* result = (Char*)LMALLOC(size*sizeof(Char));
        vsprintf_s(result, size, buffer, ap);
        DWORD written = 0;
        s32 ret = WriteFile(file_, result, size-1, &written, NULL);
        LFREE(result);
        LFREE(buffer);

        va_end(ap);
        return ret;
    }
#else

    s32 ofstream::print(const Char* format, ... )
    {
        LASSERT(NULL != file_);
        LASSERT(NULL != format);

        va_list ap;
        va_start(ap, format);

        int ret = vfprintf(file_, format, ap);

        va_end(ap);
        return ret;
    }
#endif

#endif


namespace io
{
    Char16 readAsUTF16(lcore::istream& is)
    {
        Char c[4] = {0};
        if(is.read(c, 1)<=0){
            return 0;
        }
        s32 size = getUTF8Size(c[0]);
        if(1<size){
            size -= 1;
            s32 n = is.read(c+1, size);
            if(n<size){
                return 0;
            }
        }
        Char16 utf16;
        UTF8toUTF16(utf16, c);
        return utf16;
    }

    Char16 peekAsUTF16(lcore::istream& is)
    {
        Char c[4] = {0};
        if(is.read(c, 1)<=0){
            return 0;
        }
        s32 size = getUTF8Size(c[0]);
        if(1<size){
            s32 n = is.read(c+1, size-1);
            if(n<(size-1)){
                is.seekg(-size, lcore::ios::cur);
                return 0;
            }
        }
        Char16 utf16;
        UTF8toUTF16(utf16, c);
        is.seekg(-size, lcore::ios::cur);
        return utf16;
    }

    void writeAsUTF8(lcore::ofstream& os, Char16 c)
    {
        Char buffer[8];
        s32 size = UTF16toUTF8(buffer, c);
        os.write(buffer, size);
    }
}
}

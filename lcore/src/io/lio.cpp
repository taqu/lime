/**
@file lio.cpp
@author t-sakai
@date 2016/10/28 create
*/
#include <stdio.h>
#include "io/lio.h"
#include "LString.h"

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace lcore
{
    //--------------------------------------------
    //---
    //--- Path
    //---
    //--------------------------------------------
    bool Path::isSpecial(u32 flags)
    {
        static const DWORD checks[] =
        {
            FILE_ATTRIBUTE_HIDDEN,
            FILE_ATTRIBUTE_SYSTEM,
            FILE_ATTRIBUTE_ENCRYPTED,
            FILE_ATTRIBUTE_TEMPORARY,
            FILE_ATTRIBUTE_SPARSE_FILE,
        };
        static const s32 Num = sizeof(checks)/sizeof(DWORD);
        for(int i=0; i<Num; ++i){
            DWORD check = checks[i] & flags;
            if(check != 0){
                return true;
            }
        }
        return false;
    }

    bool Path::isSpecial(u32 flags, const Char* name)
    {
        LASSERT(NULL != name);
        return isSpecial(flags) || '.' == name[0];
    }

    bool Path::exists(const Char* path)
    {
        LASSERT(NULL != path);
        DWORD ret = GetFileAttributes(path);
        return ((DWORD)-1) != ret ? !isSpecial(ret) : false;
    }

    bool Path::exists(const lcore::String& path)
    {
        return exists(path.c_str());
    }

    s32 Path::existsType(const Char* path)
    {
        LASSERT(NULL != path);
        DWORD ret = GetFileAttributes(path);
        if(((DWORD)-1)!=ret){
            if(isSpecial(ret)){
                return Exists_No;
            }
            return FILE_ATTRIBUTE_DIRECTORY==(ret&FILE_ATTRIBUTE_DIRECTORY)? Exists_Directory : Exists_File;
        }
        return Exists_No;
    }

    s32 Path::existsType(const lcore::String& path)
    {
        return existsType(path.c_str());
    }

    bool Path::isFile(u32 flags)
    {
        LASSERT(((DWORD)-1) != flags);
        return isSpecial(flags)? false : 0 == (flags&FILE_ATTRIBUTE_DIRECTORY);
    }

    bool Path::isFile(const Char* path)
    {
        LASSERT(NULL != path);
        DWORD ret = GetFileAttributes(path);
        if(((DWORD)-1) == ret){
            return false;
        }
        return isFile(ret);
    }

    bool Path::isFile(const lcore::String& path)
    {
        return isFile(path.c_str());
    }

    bool Path::isDirectory(u32 flags)
    {
        LASSERT(((DWORD)-1) != flags);
        return isSpecial(flags)? false : 0 != (flags&FILE_ATTRIBUTE_DIRECTORY);
    }

    bool Path::isDirectory(const Char* path)
    {
        LASSERT(NULL != path);
        DWORD ret = GetFileAttributes(path);
        if(((DWORD)-1) == ret){
            return false;
        }
        return isDirectory(ret);
    }

    bool Path::isDirectory(const lcore::String& path)
    {
        return isDirectory(path.c_str());
    }

    bool Path::isNormalDirectory(const Char* path)
    {
        LASSERT(NULL != path);
        DWORD ret = GetFileAttributes(path);
        if(((DWORD)-1) == ret){
            return false;
        }
        s32 pathLen = lcore::strlen_s32(path);
        s32 nameLen = extractDirectoryName(NULL, pathLen, path);
        if(nameLen<=0){
            return false;
        }
        const Char* name = path + pathLen - nameLen;
        return isNormalDirectory(ret, name);
    }

    bool Path::isNormalDirectory(u32 flags, const Char* name)
    {
        LASSERT(NULL != name);
        return Path::isDirectory(flags) && '.' != name[0];
    }

    bool Path::isSpecialDirectory(const Char* path)
    {
        LASSERT(NULL != path);
        DWORD ret = GetFileAttributes(path);
        if(((DWORD)-1) == ret){
            return false;
        }
        s32 pathLen = lcore::strlen_s32(path);
        s32 nameLen = extractDirectoryName(NULL, pathLen, path);
        if(nameLen<=0){
            return false;
        }
        const Char* name = path + pathLen - nameLen;
        return isSpecialDirectory(ret, name);
    }

    bool Path::isSpecialDirectory(u32 flags, const Char* name)
    {
        LASSERT(NULL != name);
        return Path::isDirectory(flags) && '.' == name[0];
    }

//    s32 scprintf(const Char* format, ...)
//    {
//        va_list args;
//        va_start(args, format);
//#ifdef _MSC_VER
//        s32 count = ::_vscprintf(format, args);
//#else
//        s32 count = vsnprintf(NULL, 0, format, args);
//#endif
//        va_end(args);
//        return count;
//    }
//
//    s32 vscprintf(const Char* format, va_list args)
//    {
//#ifdef _MSC_VER
//        return ::_vscprintf(format, args);
//#else
//        va_list cargs;
//        va_copy(cargs, args);
//        s32 count = vsnprintf(NULL, 0, format, cargs);
//        va_end(cargs);
//        return count;
//#endif
//    }

    void Path::getCurrentDirectory(lcore::String& path)
    {
        s32 size = GetCurrentDirectory(0, NULL);
        if(size<=0){
            path.clear();
            return;
        }
        path.fill(size-1, CharNull);
        GetCurrentDirectory(size, &path[0]);

#if defined(_WIN32) || defined(_WIN64)
        path.replace('\\', lcore::PathDelimiter);
#endif
    }

    bool Path::setCurrentDirectory(const lcore::String& path)
    {
        return TRUE == SetCurrentDirectory(path.c_str());
    }

    bool Path::setCurrentDirectory(const Char* path)
    {
        LASSERT(NULL != path);
        return TRUE == SetCurrentDirectory(path);
    }

    bool Path::isRoot(const lcore::String& path)
    {
#if defined(_WIN32) || defined(_WIN64)
        if(path.length() != 3){
            return false;
        }
        return isalpha(path[0]) && path[1]==':' && path[2]==lcore::PathDelimiter;

#else
        if(path.length() != 1){
            return false;
        }
        return path[0] == LVFS_PATH_SEPARATOR;
#endif
    }

    bool Path::isRoot(const Char* path)
    {
        LASSERT(NULL != path);
        return isRoot(static_cast<s32>(strlen(path)), path);
    }

    bool Path::isRoot(s32 length, const Char* path)
    {
        LASSERT(NULL != path);
#if defined(_WIN32) || defined(_WIN64)
        if(3 == length){
            return false;
        }
        return (isalpha(path[0]) && path[1]==':' && path[2]==lcore::PathDelimiter);
#else
        if(1 == length){
            return false;
        }
        return path[0] == lcore::PathDelimiter;
#endif
    }

    bool Path::isAbsolute(const lcore::String& path)
    {
#if defined(_WIN32) || defined(_WIN64)
        if(path.length() < 3){
            return false;
        }
        return isalpha(path[0]) && path[1]==':' && path[2]==lcore::PathDelimiter;

#else
        if(path.length() < 1){
            return false;
        }
        return path[0] == lcore::PathDelimiter;
#endif
    }

    bool Path::isAbsolute(const Char* path)
    {
        LASSERT(NULL != path);
        return isAbsolute(static_cast<s32>(lcore::strlen(path)), path);
    }

    bool Path::isAbsolute(s32 length, const Char* path)
    {
        LASSERT(NULL != path);
#if defined(_WIN32) || defined(_WIN64)
        if(length < 3){
            return false;
        }
        return isalpha(path[0]) && path[1] == ':' && path[2] == lcore::PathDelimiter;

#else
        if(length < 1){
            return false;
        }
        return path[0] == lcore::PathDelimiter;
#endif
    }

    void Path::chompPathSeparator(lcore::String& path)
    {
        if(path.length()<=0 || Path::isRoot(path)){
            return;
        }
        if(path[path.length()-1] == lcore::PathDelimiter){
            path.pop_back();
        }
    }

    void Path::chompPathSeparator(Char* path)
    {
        LASSERT(NULL != path);
        Path::chompPathSeparator(static_cast<s32>(strlen(path)), path);
    }

    void Path::chompPathSeparator(s32 length, Char* path)
    {
        LASSERT(NULL != path);
        if(length<=0 || Path::isRoot(length, path)){
            return;
        }
        if(path[length-1] == lcore::PathDelimiter){
            path[length-1] = lcore::CharNull;
        }
    }

    s32 Path::chompPathSeparator(const lcore::String& path)
    {
        if(path.length()<=0 || Path::isRoot(path)){
            return path.length();
        }
        if(path[path.length()-1] == lcore::PathDelimiter){
            return path.length()-1;
        }
        return path.length();
    }

    s32 Path::chompPathSeparator(const Char* path)
    {
        LASSERT(NULL != path);
        return chompPathSeparator(static_cast<s32>(strlen(path)), path);
    }

    s32 Path::chompPathSeparator(s32 length, const Char* path)
    {
        LASSERT(NULL != path);
        if(length<=0 || Path::isRoot(length, path)){
            return length;
        }
        if(path[length-1] == lcore::PathDelimiter){
            return length-1;
        }
        return length;
    }

    void Path::getFilename(lcore::String& filename, s32 length, const Char* path)
    {
        LASSERT(0<=length);
        s32 filenameLength = lcore::extractFileName(NULL, static_cast<u32>(length), path);
        if(filenameLength<=0){
            filename.clear();
            return;
        }
        filename.fill(filenameLength);
        lcore::extractFileName(&filename[0], length, path);
    }

    void Path::getDirectoryname(lcore::String& directoryname, s32 length, const Char* path)
    {
        LASSERT(0<=length);
        s32 directorynameLength = lcore::extractDirectoryName(NULL, static_cast<u32>(length), path);
        if(directorynameLength<=0){
            directoryname.clear();
            return;
        }
        directoryname.fill(directorynameLength);
        lcore::extractDirectoryName(&directoryname[0], length, path);
    }

    s64 File::read(LHANDLE handle, s64 size, void* b)
    {
        LASSERT(handle != INVALID_HANDLE_VALUE);
        u8* buffer = static_cast<u8*>(b);
        s64 total = 0;
        while(0<size){
            DWORD numBytesToRead = (0xFFFFFFFFLL<size)? 0xFFFFFFFFUL : static_cast<DWORD>(size);
            DWORD numBytesRead = 0;
            if(ReadFile(handle, buffer, numBytesToRead, &numBytesRead, NULL)){
                size -= numBytesRead;
                buffer += numBytesRead;
                total += numBytesRead;
            }else{
                break;
            }
        }
        return total;
    }

    s64 File::read(LHANDLE handle, s64 offset, s64 size, void* b)
    {
        LASSERT(handle != INVALID_HANDLE_VALUE);
        LARGE_INTEGER distance;
        distance.QuadPart = offset;
        if(!SetFilePointerEx(handle, distance, NULL, FILE_BEGIN)){
            return 0;
        }

        u8* buffer = static_cast<u8*>(b);
        s64 total = 0;
        while(0<size){
            DWORD numBytesToRead = (0xFFFFFFFFLL<size)? 0xFFFFFFFFUL : static_cast<DWORD>(size);
            DWORD numBytesRead = 0;
            if(ReadFile(handle, buffer, numBytesToRead, &numBytesRead, NULL)){
                size -= numBytesRead;
                buffer += numBytesRead;
                total += numBytesRead;
            }else{
                break;
            }
        }
        return total;
    }

    s64 File::write(LHANDLE handle, s64 size, const void* d)
    {
        LASSERT(handle != INVALID_HANDLE_VALUE);

        const u8* data = static_cast<const u8*>(d);
        s64 total = 0;
        while(0<size){
            DWORD numBytesToWrite = (0xFFFFFFFFLL<size)? 0xFFFFFFFFUL : static_cast<DWORD>(size);
            DWORD numBytesWritten = 0;
            if(WriteFile(handle, data, numBytesToWrite, &numBytesWritten, NULL)){
                size -= numBytesWritten;
                data += numBytesWritten;
                total += numBytesWritten;
            }else{
                break;
            }
        }
        return total;
    }

    s64 File::write(LHANDLE handle, s64 offset, s64 size, const void* d)
    {
        LASSERT(handle != INVALID_HANDLE_VALUE);

        LARGE_INTEGER distance;
        distance.QuadPart = offset;
        if(!SetFilePointerEx(handle, distance, NULL, FILE_BEGIN)){
            return 0;
        }

        const u8* data = static_cast<const u8*>(d);
        s64 total = 0;
        while(0<size){
            DWORD numBytesToWrite = (0xFFFFFFFFLL<size)? 0xFFFFFFFFUL : static_cast<DWORD>(size);
            DWORD numBytesWritten = 0;
            if(WriteFile(handle, data, numBytesToWrite, &numBytesWritten, NULL)){
                size -= numBytesWritten;
                data += numBytesWritten;
                total += numBytesWritten;
            }else{
                break;
            }
        }
        return total;
    }
}

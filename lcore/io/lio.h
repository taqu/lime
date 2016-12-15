#ifndef INC_LCORE_LIO_H__
#define INC_LCORE_LIO_H__
/**
@file lio.h
@author t-sakai
@date 2016/10/28 create
*/
#include "../lcore.h"

namespace lcore
{
    class String;

    //--------------------------------------------
    //---
    //--- Path
    //---
    //--------------------------------------------
    struct Path
    {
    public:
        static const s32 Exists_No = 0;
        static const s32 Exists_File = 1;
        static const s32 Exists_Directory = 2;

        static bool isSpecial(u32 flags);
        static bool isSpecial(u32 flags, const Char* name);
        static bool exists(const Char* path);
        static bool exists(const lcore::String& path);
        static s32 existsType(const Char* path);
        static s32 existsType(const lcore::String& path);
        static bool isFile(u32 flags);
        static bool isFile(const Char* path);
        static bool isFile(const lcore::String& path);
        static bool isDirectory(u32 flags);
        static bool isDirectory(const Char* path);
        static bool isDirectory(const lcore::String& path);

        static bool isNormalDirectory(const Char* path);
        static bool isNormalDirectory(u32 flags, const Char* name);
        static bool isSpecialDirectory(const Char* path);
        static bool isSpecialDirectory(u32 flags, const Char* name);

        static void getCurrentDirectory(lcore::String& path);
        static bool setCurrentDirectory(const lcore::String& path);
        static bool setCurrentDirectory(const Char* path);
        static bool isRoot(const lcore::String& path);
        static bool isRoot(const Char* path);
        static bool isRoot(s32 length, const Char* path);
        static bool isAbsolute(const lcore::String& path);
        static bool isAbsolute(const Char* path);
        static bool isAbsolute(s32 length, const Char* path);
        static void chompPathSeparator(lcore::String& path);
        static void chompPathSeparator(Char* path);
        static void chompPathSeparator(s32 length, Char* path);
        static s32 chompPathSeparator(const lcore::String& path);
        static s32 chompPathSeparator(const Char* path);
        static s32 chompPathSeparator(s32 length, const Char* path);

        static void getFilename(lcore::String& filename, s32 length, const Char* path);
        static void getDirectoryname(lcore::String& directoryname, s32 length, const Char* path);
    };

    struct File
    {
    public:
        static s64 read(LHANDLE handle, s64 size, void* buffer);
        static s64 read(LHANDLE handle, s64 offset, s64 size, void* buffer);
        static s64 write(LHANDLE handle, s64 size, const void* data);
        static s64 write(LHANDLE handle, s64 offset, s64 size, const void* data);
    };
}

#endif //INC_LCORE_LIO_H__

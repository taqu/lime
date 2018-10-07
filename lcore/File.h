#ifndef INC_LCORE_FILE_H_
#define INC_LCORE_FILE_H_
/**
@file File.h
@author t-sakai
@date 2018/10/02 create
*/

#include "lcore.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#endif

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
        static bool exists(const String& path);
        static s32 existsType(const Char* path);
        static s32 existsType(const String& path);
        static bool isFile(u32 flags);
        static bool isFile(const Char* path);
        static bool isFile(const String& path);
        static bool isDirectory(u32 flags);
        static bool isDirectory(const Char* path);
        static bool isDirectory(const String& path);

        static bool isNormalDirectory(const Char* path);
        static bool isNormalDirectory(u32 flags, const Char* name);
        static bool isSpecialDirectory(const Char* path);
        static bool isSpecialDirectory(u32 flags, const Char* name);

        static void getCurrentDirectory(String& path);
        static bool setCurrentDirectory(const String& path);
        static bool setCurrentDirectory(const Char* path);
        static bool isRoot(const String& path);
        static bool isRoot(const Char* path);
        static bool isRoot(s32 length, const Char* path);
        static bool isAbsolute(const String& path);
        static bool isAbsolute(const Char* path);
        static bool isAbsolute(s32 length, const Char* path);
        static void chompPathSeparator(String& path);
        static void chompPathSeparator(Char* path);
        static void chompPathSeparator(s32 length, Char* path);
        static s32 chompPathSeparator(const String& path);
        static s32 chompPathSeparator(const Char* path);
        static s32 chompPathSeparator(s32 length, const Char* path);

        /**
        @brief パスからディレクトリパス抽出. dstがNULLの場合, パスの長さを返す
        @return dstの長さ
        @param dst ... 出力. ヌル文字込みで十分なサイズがあること
        @param length ... パスの長さ
        @param path ... パス
        */
        static s32 extractDirectoryPath(Char* dst, s32 length, const Char* path);

        /**
        @brief パスからディレクトリ名抽出. dstがNULLの場合, ディレクトリ名の長さを返す
        @return dstの長さ
        @param dst ... 出力. ヌル文字込みで十分なサイズがあること
        @param length ... パスの長さ
        @param path ... パス
        */
        static s32 extractDirectoryName(Char* dst, s32 length, const Char* path);

        /**
        @brief パスからファイル名抽出. dstがNULLの場合, ファイル名の長さを返す
        @return dstの長さ
        @param dst ... 出力. ヌル文字込みで十分なサイズがあること
        @param length ... パスの長さ
        @param path ... パス
        */
        static s32 extractFileName(Char* dst, s32 length, const Char* path);

        /**
        @brief パスからファイル名抽出. dstがNULLの場合, ファイル名の長さを返す
        @return dstの長さ
        @param dst ... 出力. ヌル文字込みで十分なサイズがあること
        @param length ... パスの長さ
        @param path ... パス
        */
        static s32 extractFileNameWithoutExt(Char* dst, s32 length, const Char* path);

        /**
        @brief パスから最初のファイル名抽出.
        @param length ... 出力. 抽出したファイル名の長さ
        @param name ... 出力. 抽出したファイル名
        @param pathLength ... パスの長さ
        @param path ... パス
        */
        static const Char* parseFirstNameFromPath(s32& length, Char* name, s32 pathLength, const Char* path);

        /**
        @brief パスから拡張子抽出
        */
        static const Char* getExtension(s32 length, const Char* path);

        static void getFilename(String& filename, s32 length, const Char* path);
        static void getDirectoryname(String& directoryname, s32 length, const Char* path);
    };

    //--------------------------------------------
    //---
    //--- ios
    //---
    //--------------------------------------------
    struct ios
    {
        static const s32 in = 0x01 | 0x20;
        static const s32 out = 0x02 | 0x20;
        static const s32 ate = 0x04;
        static const s32 app = 0x08;
        static const s32 trunc = 0x10;
        static const s32 binary = 0x20;

#if defined(_WIN32)
        static const s32 beg = FILE_BEGIN;
        static const s32 cur = FILE_CURRENT;
        static const s32 end = FILE_END;
#else
        static const s32 beg = SEEK_SET;
        static const s32 cur = SEEK_CUR;
        static const s32 end = SEEK_END;
#endif

	    enum Mode
        {
            Mode_R = 0,
            Mode_W,
            Mode_A,
            Mode_RB,
            Mode_WB,
            Mode_AB,
            Mode_RP,
            Mode_WP,
            Mode_AP,
            Mode_RBP,
            Mode_WBP,
            Mode_ABP,
            Mode_Num,
        };

        static s32 ModeInt[Mode_Num];

        static const Char* ModeString[Mode_Num];
        static const Char* getModeString(s32 mode);

#if defined(_WIN32)
        static s32 getDesiredAccess(s32 mode);
        static s32 getCreationDisposition(s32 mode);
#endif
    };

    //------------------------------------------------------
    //---
    //--- File
    //---
    //------------------------------------------------------
    class File
    {
    public:
        File();
        File(File&& rhs);
        File(const Char* filepath, s32 mode);
        ~File();

        bool open(const Char* filepath, s32 mode);
        bool is_open() const;
        void close();

        bool eof();
        bool good() const;
        bool seek(off_t offset, s32 dir);
        off_t tell();
        s64 size();

        bool write(s64 size, const void* data);
        bool read(s64 size, void* data);

        s32 get();
        s32 peek();

        s32 print(const Char* format, ...);
        s32 print(const Char16* format, ...);

        void swap(File& rhs);
        File& operator=(File&& rhs);

        template<class T>
        bool read(T& x);

        template<class T>
        bool write(const T& x);

        static bool read(LHANDLE handle, s64 size, void* data);
        static bool read(LHANDLE handle, s64 offset, s64 size, void* data);
        static bool write(LHANDLE handle, s64 size, const void* data);
        static bool write(LHANDLE handle, s64 offset, s64 size, const void* data);

    private:
        File(const File&) = delete;
        File& operator=(const File&) = delete;
        
#if defined(_WIN32)
        LHANDLE file_;
#else
        FILE* file_;
#endif
    };

    template<class T>
    bool File::read(T& x)
    {
#if defined(_WIN32)
        return ReadFileEx(file_, &x, sizeof(T), NULL, NULL);
#else
        return 0<fread(&x, sizeof(T), 1, file_);
#endif
    }

    template<class T>
    bool File::write(const T& x)
    {
#if defined(_WIN32)
        return WriteFileEx(file_, &x, sizeof(T), NULL, NULL);
#else
        return 0<fwrite(&x, sizeof(T), 1, file_);
#endif
    }
}

#endif //INC_LCORE_FILE_H_

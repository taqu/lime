/**
@file File.cpp
@author t-sakai
@date 2018/10/02 create
*/
#include "File.h"
#include "LString.h"

#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

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
        for (int i = 0; i<Num; ++i){
            DWORD check = checks[i] & flags;
            if (check != 0){
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

    bool Path::exists(const String& path)
    {
        return exists(path.c_str());
    }

    s32 Path::existsType(const Char* path)
    {
        LASSERT(NULL != path);
        DWORD ret = GetFileAttributes(path);
        if (((DWORD)-1)!=ret){
            if (isSpecial(ret)){
                return Exists_No;
            }
            return FILE_ATTRIBUTE_DIRECTORY==(ret&FILE_ATTRIBUTE_DIRECTORY) ? Exists_Directory : Exists_File;
        }
        return Exists_No;
    }

    s32 Path::existsType(const String& path)
    {
        return existsType(path.c_str());
    }

    bool Path::isFile(u32 flags)
    {
        LASSERT(((DWORD)-1) != flags);
        return isSpecial(flags) ? false : 0 == (flags&FILE_ATTRIBUTE_DIRECTORY);
    }

    bool Path::isFile(const Char* path)
    {
        LASSERT(NULL != path);
        DWORD ret = GetFileAttributes(path);
        if (((DWORD)-1) == ret){
            return false;
        }
        return isFile(ret);
    }

    bool Path::isFile(const String& path)
    {
        return isFile(path.c_str());
    }

    bool Path::isDirectory(u32 flags)
    {
        LASSERT(((DWORD)-1) != flags);
        return isSpecial(flags) ? false : 0 != (flags&FILE_ATTRIBUTE_DIRECTORY);
    }

    bool Path::isDirectory(const Char* path)
    {
        LASSERT(NULL != path);
        DWORD ret = GetFileAttributes(path);
        if (((DWORD)-1) == ret){
            return false;
        }
        return isDirectory(ret);
    }

    bool Path::isDirectory(const String& path)
    {
        return isDirectory(path.c_str());
    }

    bool Path::isNormalDirectory(const Char* path)
    {
        LASSERT(NULL != path);
        DWORD ret = GetFileAttributes(path);
        if (((DWORD)-1) == ret){
            return false;
        }
        s32 pathLen = lcore::strlen_s32(path);
        s32 nameLen = extractDirectoryName(NULL, pathLen, path);
        if (nameLen<=0){
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
        if (((DWORD)-1) == ret){
            return false;
        }
        s32 pathLen = lcore::strlen_s32(path);
        s32 nameLen = extractDirectoryName(NULL, pathLen, path);
        if (nameLen<=0){
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

    void Path::getCurrentDirectory(String& path)
    {
        s32 size = GetCurrentDirectory(0, NULL);
        if (size<=0){
            path.clear();
            return;
        }
        path.fill(size-1, CharNull);
        GetCurrentDirectory(size, &path[0]);

#if defined(_WIN32)
        path.replace('\\', PathDelimiter);
#endif
    }

    bool Path::setCurrentDirectory(const String& path)
    {
        return TRUE == SetCurrentDirectory(path.c_str());
    }

    bool Path::setCurrentDirectory(const Char* path)
    {
        LASSERT(NULL != path);
        return TRUE == SetCurrentDirectory(path);
    }

    bool Path::isRoot(const String& path)
    {
#if defined(_WIN32)
        if (path.length() != 3){
            return false;
        }
        return isalpha(path[0]) && path[1]==':' && path[2]==PathDelimiter;

#else
        if (path.length() != 1){
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
#if defined(_WIN32)
        if (3 == length){
            return false;
        }
        return (isalpha(path[0]) && path[1]==':' && path[2]==PathDelimiter);
#else
        if (1 == length){
            return false;
        }
        return path[0] == PathDelimiter;
#endif
    }

    bool Path::isAbsolute(const String& path)
    {
#if defined(_WIN32)
        if (path.length() < 3){
            return false;
        }
        return isalpha(path[0]) && path[1]==':' && path[2]==PathDelimiter;

#else
        if (path.length() < 1){
            return false;
        }
        return path[0] == PathDelimiter;
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
#if defined(_WIN32)
        if (length < 3){
            return false;
        }
        return isalpha(path[0]) && path[1] == ':' && path[2] == PathDelimiter;

#else
        if (length < 1){
            return false;
        }
        return path[0] == PathDelimiter;
#endif
    }

    void Path::chompPathSeparator(String& path)
    {
        if (path.length()<=0 || Path::isRoot(path)){
            return;
        }
        if (path[path.length()-1] == PathDelimiter){
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
        if (length<=0 || Path::isRoot(length, path)){
            return;
        }
        if (path[length-1] == PathDelimiter){
            path[length-1] = CharNull;
        }
    }

    s32 Path::chompPathSeparator(const String& path)
    {
        if (path.length()<=0 || Path::isRoot(path)){
            return path.length();
        }
        if (path[path.length()-1] == PathDelimiter){
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
        if (length<=0 || Path::isRoot(length, path)){
            return length;
        }
        if (path[length-1] == PathDelimiter){
            return length-1;
        }
        return length;
    }

    //-------------------------------------------------------------
    // パスからディレクトリパス抽出
    s32 Path::extractDirectoryPath(Char* dst, s32 length, const Char* path)
    {
        LASSERT(NULL != path);
        LASSERT(0<=length);
        if (length<=0){
            if (NULL != dst){
                dst[0] = CharNull;
            }
            return 0;
        }

        s32 i = length-1;
        for (; 0<=i; --i){
            if (PathDelimiter == path[i]){
                break;
            }
        }
        s32 dstLen = i+1;
        if (NULL != dst){
            for (s32 j = 0; j<dstLen; ++j){
                dst[j] = path[j];
            }
            dst[dstLen] = CharNull;
        }
        return dstLen;
    }

    // パスからディレクトリ名抽出
    s32 Path::extractDirectoryName(Char* dst, s32 length, const Char* path)
    {
        LASSERT(NULL != path);
        LASSERT(0<=length);
        if (NULL != dst){
            dst[0] = CharNull;
        }
        if (length<=0){
            return 0;
        }

        if (PathDelimiter == path[length-1]){
            --length;
            if (length<=0){
                return 0;
            }
        }
        s32 i = length-1;
        for (; 0<=i; --i){
            if (PathDelimiter == path[i]){
                break;
            }
        }

        s32 dstLen = length-i-1;
        if (NULL != dst){
            for (s32 j = i+1; j<length; ++j){
                dst[j-i-1] = path[j];
            }
            dst[dstLen] = CharNull;
        }
        return dstLen;
    }

    //-------------------------------------------------------------
    // パスからファイル名抽出
    s32 Path::extractFileName(Char* dst, s32 length, const Char* path)
    {
        LASSERT(NULL != path);
        LASSERT(0<=length);
        if (length<=0){
            if (NULL != dst){
                dst[0] = CharNull;
            }
            return 0;
        }

        s32 i = length-1;
        for (; 0<=i; --i){
            if (PathDelimiter == path[i]){
                break;
            }
        }

        s32 dstLen = length-i-1;
        if (NULL != dst){
            for (s32 j = i+1; j<length; ++j){
                dst[j-i-1] = path[j];
            }
            dst[dstLen] = CharNull;
        }
        return dstLen;
    }

    // パスからファイル名抽出
    s32 Path::extractFileNameWithoutExt(Char* dst, s32 length, const Char* path)
    {
        LASSERT(NULL != path);
        LASSERT(0<=length);
        if (length<=0){
            if (NULL != dst){
                dst[0] = CharNull;
            }
            return 0;
        }

        s32 i = length-1;
        for (; 0<=i; --i){
            if (PathDelimiter == path[i]){
                break;
            }
        }

        s32 dstLen = length-i-1;
        if (NULL != dst){
            for (s32 j = length-1; i<j; --j){
                if ('.' == path[j]){
                    dstLen = j-i-1;
                }
                dst[j-i-1] = path[j];
            }
            dst[dstLen] = CharNull;
        }
        return dstLen;
    }

    // パスから最初のファイル名抽出
    const Char* Path::parseFirstNameFromPath(s32& length, Char* name, s32 pathLength, const Char* path)
    {
        LASSERT(NULL != name);
        LASSERT(NULL != path);

        length = 0;
        while (CharNull != *path && length<(pathLength-1)){
            if (PathDelimiter == *path){
                ++path;
                break;
            }
            name[length++] = *path;
            ++path;
        }
        name[length] = CharNull;
        return path;
    }

    // パスから拡張子抽出
    const Char* Path::getExtension(s32 length, const Char* path)
    {
        LASSERT(0<=length);
        LASSERT(NULL != path);
        for (s32 i = length-1; 0<=i; --i){
            if (path[i] == '.'){
                return &path[i+1];
            }
        }
        return &path[length];
    }

    void Path::getFilename(String& filename, s32 length, const Char* path)
    {
        LASSERT(0<=length);
        s32 filenameLength = extractFileName(NULL, static_cast<u32>(length), path);
        if (filenameLength<=0){
            filename.clear();
            return;
        }
        filename.fill(filenameLength);
        extractFileName(&filename[0], length, path);
    }

    void Path::getDirectoryname(String& directoryname, s32 length, const Char* path)
    {
        LASSERT(0<=length);
        s32 directorynameLength = extractDirectoryName(NULL, static_cast<u32>(length), path);
        if (directorynameLength<=0){
            directoryname.clear();
            return;
        }
        directoryname.fill(directorynameLength);
        extractDirectoryName(&directoryname[0], length, path);
    }

    //--------------------------------------------
    //---
    //--- ios
    //---
    //--------------------------------------------
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

    const Char* ios::getModeString(s32 mode)
    {
        for(s32 i = 0; i<Mode_Num; ++i){
            if(mode == ModeInt[i]){
                return ModeString[i];
            }
        }
        return NULL;
    }

#if defined(_WIN32)
    s32 ios::getDesiredAccess(s32 mode)
    {
        s32 access = 0;
        if(mode & ios::in){
            access |= GENERIC_READ;
        }
        if(mode & ios::out){
            access |= GENERIC_WRITE;
        }
        return access;
    }

    s32 ios::getCreationDisposition(s32 mode)
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
#endif

    //------------------------------------------------------
    //---
    //--- File
    //---
    //------------------------------------------------------
    File::File()
        :file_(NULL)
    {
    }

    File::File(File&& rhs)
        :file_(rhs.file_)
    {
        rhs.file_ = NULL;
    }

    File::File(const Char* filepath, s32 mode)
        :file_(NULL)
    {
        open(filepath, mode);
    }

    File::~File()
    {
        close();
    }

    bool File::open(const Char* filepath, s32 mode)
    {
        LASSERT(NULL != filepath);
        close();

#ifdef _WIN32
        mode &= ~ios::binary;
        file_ = CreateFile(
            filepath,
            ios::getDesiredAccess(mode),
            FILE_SHARE_READ,
            NULL,
            ios::getCreationDisposition(mode),
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if(INVALID_HANDLE_VALUE != file_ && (HANDLE)ERROR_ALREADY_EXISTS != file_){
            return true;
        } else{
            file_ = NULL;
            return false;
        }
#else
        file_ = fopen(filepath, ios::getModeString(mode));
        return NULL != file_;
#endif
    }

    bool File::is_open() const
    {
        return NULL != file_;
    }

    void File::close()
    {
        if(NULL != file_){
#ifdef _WIN32
            CloseHandle(file_);
#else
            fclose(file_);
#endif
            file_ = NULL;
        }
    }

    bool File::eof()
    {
        LASSERT(NULL != file_);
#ifdef _WIN32
        LARGE_INTEGER offset;
        offset.QuadPart = 0;
        if(!SetFilePointerEx(file_, offset, &offset, FILE_CURRENT)){
            return true;
        }
        LARGE_INTEGER size;
        return GetFileSizeEx(file_, &size)? size.QuadPart<=offset.QuadPart : true;
#else
        return 0 != feof(file_);
#endif
    }

    bool File::good() const
    {
        return NULL != file_;
    }

    bool File::seek(off_t offset, s32 dir)
    {
        LASSERT(NULL != file_);
        LASSERT(0<=ios::beg && dir<=ios::end);

#ifdef _WIN32
        LARGE_INTEGER loffset;
        loffset.QuadPart = offset;
        return SetFilePointerEx(file_, loffset, NULL, dir);
#else
        return 0 == fseek(file_, offset, dir);
#endif
    }

    off_t File::tell()
    {
        LASSERT(NULL != file_);
#ifdef _WIN32
        LARGE_INTEGER offset;
        offset.QuadPart = 0;
        if(!SetFilePointerEx(file_, offset, &offset, FILE_CURRENT)){
            return 0;
        }
        return offset.QuadPart;
#else
        return ftell(file_);
#endif
    }

    s64 File::size()
    {
        LASSERT(NULL != file_);
#ifdef _WIN32
        LARGE_INTEGER result;
        return GetFileSizeEx(file_, &result)? result.QuadPart : 0;
#else
        s32 fd = fileno(file);
        if(fd<0){
            return 0;
        }
        stat64 buff;
        if(fstat64(fd, &buff)<0){
            return 0;
        }
        return buff.st_size;
#endif
    }

    bool File::write(s64 size, const void* data)
    {
#if _WIN32
        DWORD numByhtesWrote = 0;
        return WriteFile(file_, data, static_cast<DWORD>(size), &numByhtesWrote, NULL);
#else
        return 0<fwrite(ptr, size, 1, file_);
#endif
    }

    bool File::read(s64 size, void* data)
    {
#if _WIN32
        DWORD numBytesRead = 0;
        return ReadFile(file_, data, static_cast<DWORD>(size), &numBytesRead, NULL);
#else
        return 0<fread(ptr, size, 1, file_);
#endif
    }

    s32 File::get()
    {
#if _WIN32
        s32 c = 0;
        DWORD numBytesRead = 0;
        return ReadFile(file_, &c, 1, &numBytesRead, NULL) ? c : EOF;
#else
        return fgetc(file_);
#endif
    }

    s32 File::peek()
    {
#if _WIN32
        s32 c = 0;
        DWORD numBytesRead = 0;
        if(ReadFile(file_, &c, 1, &numBytesRead, NULL)){
            if(0<numBytesRead){
                LARGE_INTEGER offset;
                offset .QuadPart = -1;
                SetFilePointerEx(file_, offset, 0, FILE_CURRENT);
            }
            return c;
        } else{
            return EOF;
        }
#else
        s32 c = getc(file_);
        return c!=EOF? ungetc(c, file_) : EOF;
#endif
    }

    s32 File::print(const Char* format, ...)
    {
        LASSERT(NULL != file_);
        LASSERT(NULL != format);

        va_list ap;
        va_start(ap, format);

        s32 size = _vscprintf(format, ap) + 1;
        Char* buffer = (Char*)LMALLOC(size*sizeof(Char));
        vsprintf_s(buffer, size, format, ap);
        DWORD written = 0;
#ifdef _WIN32
        s32 ret = WriteFile(file_, buffer, size-1, &written, NULL);
#else
        s32 ret = fwrite(buffer, size-1, 1, file_);
#endif
        LFREE(buffer);

        va_end(ap);
        return ret;
    }

    s32 File::print(const Char16* format, ...)
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
#ifdef _WIN32
        s32 ret = 1;
        DWORD written = 0;
        if(!WriteFile(file_, result, size-1, &written, NULL)){
            ret = 0;
        }
#else
        s32 ret = fwrite(result, size-1, 1, file_);
#endif
        LFREE(result);
        LFREE(buffer);

        va_end(ap);
        return ret;
    }

    void File::swap(File& rhs)
    {
        lcore::swap(file_, rhs.file_);
    }

    File& File::operator=(File&& rhs)
    {
        if(&rhs != this){
            close();
            file_ = rhs.file_;
            rhs.file_ = NULL;
        }
        return *this;
    }


    bool File::read(LHANDLE handle, s64 size, void* b)
    {
        LASSERT(handle != INVALID_HANDLE_VALUE);
        u8* buffer = static_cast<u8*>(b);
        s64 total = 0;
        while(0<size){
            DWORD numBytesToRead = (0xFFFFFFFFLL<size) ? 0xFFFFFFFFUL : static_cast<DWORD>(size);
            DWORD numBytesRead = 0;
            if(ReadFile(handle, buffer, numBytesToRead, &numBytesRead, NULL)){
                size -= numBytesRead;
                buffer += numBytesRead;
                total += numBytesRead;
            } else{
                break;
            }
        }
        return size<=total;
    }

    bool File::read(LHANDLE handle, s64 offset, s64 size, void* b)
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
            DWORD numBytesToRead = (0xFFFFFFFFLL<size) ? 0xFFFFFFFFUL : static_cast<DWORD>(size);
            DWORD numBytesRead = 0;
            if(ReadFile(handle, buffer, numBytesToRead, &numBytesRead, NULL)){
                size -= numBytesRead;
                buffer += numBytesRead;
                total += numBytesRead;
            } else{
                break;
            }
        }
        return size<=total;
    }

    bool File::write(LHANDLE handle, s64 size, const void* d)
    {
        LASSERT(handle != INVALID_HANDLE_VALUE);

        const u8* data = static_cast<const u8*>(d);
        s64 total = 0;
        while(0<size){
            DWORD numBytesToWrite = (0xFFFFFFFFLL<size) ? 0xFFFFFFFFUL : static_cast<DWORD>(size);
            DWORD numBytesWritten = 0;
            if(WriteFile(handle, data, numBytesToWrite, &numBytesWritten, NULL)){
                size -= numBytesWritten;
                data += numBytesWritten;
                total += numBytesWritten;
            } else{
                break;
            }
        }
        return size<=total;
    }

    bool File::write(LHANDLE handle, s64 offset, s64 size, const void* d)
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
            DWORD numBytesToWrite = (0xFFFFFFFFLL<size) ? 0xFFFFFFFFUL : static_cast<DWORD>(size);
            DWORD numBytesWritten = 0;
            if(WriteFile(handle, data, numBytesToWrite, &numBytesWritten, NULL)){
                size -= numBytesWritten;
                data += numBytesWritten;
                total += numBytesWritten;
            } else{
                break;
            }
        }
        return size<=total;
    }
}

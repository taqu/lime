/**
@file VirtualFileSystem.cpp
@author t-sakai
@date 2016/11/06 create
*/
#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#endif

#include "io/VirtualFileSystem.h"
#include "io/VFSPack.h"

namespace lcore
{
    //--------------------------------------------
    //---
    //--- FileProxyPack
    //---
    //--------------------------------------------
    FileProxyPack::FileProxyPack(fs::File* file, VirtualFileSystemPack* parent)
        :file_(file)
        ,parent_(parent)
    {
        LASSERT(NULL != file_);
        LASSERT(NULL != parent_);
    }

    FileProxyPack::~FileProxyPack()
    {
    }

    bool FileProxyPack::isCompressed() const
    {
        return file_->isCompressed();
    }

    s64 FileProxyPack::getUncompressedSize() const
    {
        return file_->getUncompressedSize();
    }

    s64 FileProxyPack::getCompressedSize() const
    {
        return file_->getCompressedSize();
    }

    s64 FileProxyPack::read(s64 offset, s64 size, u8* buffer) const
    {
        return file_->read(offset, size, buffer);
    }

    s64 FileProxyPack::write(s64 offset, s64 size, u8* data)
    {
        return file_->write(offset, size, data);
    }

    bool FileProxyPack::thisParent(VirtualFileSystemBase* vfs) const
    {
        return static_cast<VirtualFileSystemBase*>(parent_) == vfs;
    }

    //----------------------------------------------------------
    //---
    //--- FileProxyIStream
    //---
    //----------------------------------------------------------
    FileProxyIStream::FileProxyIStream()
        :file_(NULL)
        ,current_(0)
    {}

    FileProxyIStream::FileProxyIStream(FileProxy* file)
        :file_(file)
        ,current_(0)
    {
        LASSERT(NULL != file_);
    }

    FileProxyIStream::~FileProxyIStream()
    {
    }

    bool FileProxyIStream::eof()
    {
        return file_->getUncompressedSize()<=current_;
    }

    bool FileProxyIStream::good()
    {
        return current_<=file_->getUncompressedSize();
    }

    bool FileProxyIStream::seekg(s32 offset, int dir)
    {
        s64 size = file_->getUncompressedSize();
        switch(dir)
        {
        case lcore::ios::beg:
            current_ = offset;
            break;
        case lcore::ios::cur:
            current_ += offset;
            break;
        case lcore::ios::end:
            current_ = size-offset-1;
            break;
        default:
            return false;
        }
        s64 end = (0<size)? size-1 : 0;
        current_ = lcore::clamp(current_, 0LL, end);
        return true;
    }

    s32 FileProxyIStream::tellg()
    {
        return static_cast<s32>(current_);
    }

    s32 FileProxyIStream::read(void* dst, s32 count)
    {
        LASSERT(0<=count);
        s64 size = file_->getUncompressedSize();
        size -= current_;
        count = lcore::minimum(count, static_cast<s32>(size));
        size = file_->read(current_, count, static_cast<u8*>(dst));
        current_ += size;
        return static_cast<s32>(size);
    }

    u32 FileProxyIStream::getSize()
    {
        return static_cast<u32>(file_->getUncompressedSize());
    }

    //--------------------------------------------
    //---
    //--- DirectoryProxyPack
    //---
    //--------------------------------------------
    DirectoryProxyPack::DirectoryProxyPack(fs::Directory* directory, VirtualFileSystemPack* parent)
        :directory_(directory)
        ,parent_(parent)
    {
        LASSERT(NULL != directory_);
        LASSERT(NULL != parent_);
    }

    DirectoryProxyPack::~DirectoryProxyPack()
    {

    }

    s32 DirectoryProxyPack::getNumChildren() const
    {
        return directory_->getNumChildren();
    }

    s32 DirectoryProxyPack::findChild(s32 length, const Char* name) const
    {
        return directory_->findChild(length, name);
    }

    fs::FileStatus DirectoryProxyPack::getStatus(s32 index) const
    {
        return directory_->getFileStatus(index);
    }

    fs::FileStatus DirectoryProxyPack::getStatus(s32 length, const Char* name) const
    {
        s32 index = directory_->findChild(length, name);
        if(index<0){
            fs::FileStatus status = {0};
            return status;
        }
        return directory_->getFileStatus(index);
    }

    FileProxy* DirectoryProxyPack::openFile(s32 index)
    {
        fs::File* file = directory_->getFile(index);
        if(NULL == file){
            return NULL;
        }
        return parent_->popFile(file);
    }

    FileProxy* DirectoryProxyPack::openFile(s32 length, const Char* name)
    {
        LASSERT(0<=length);
        LASSERT(NULL != name);
        s32 index = directory_->findChild(length, name);
        if(index<0){
            return NULL;
        }
        fs::File* file = directory_->getFile(index);
        if(NULL == file){
            return NULL;
        }
        return parent_->popFile(file);
    }

    DirectoryProxy* DirectoryProxyPack::openDirectory(s32 index)
    {
        fs::Directory* directory = directory_->getDirectory(index);
        if(NULL == directory){
            return NULL;
        }
        return parent_->popDirectory(directory);
    }

    DirectoryProxy* DirectoryProxyPack::openDirectory(s32 length, const Char* name)
    {
        LASSERT(0<=length);
        LASSERT(NULL != name);
        s32 index = directory_->findChild(length, name);
        if(index<0){
            return NULL;
        }
        fs::Directory* directory = directory_->getDirectory(index);
        if(NULL == directory){
            return NULL;
        }
        return parent_->popDirectory(directory);
    }

    bool DirectoryProxyPack::thisParent(VirtualFileSystemBase* vfs) const
    {
        return static_cast<VirtualFileSystemBase*>(parent_) == vfs;
    }

    //--------------------------------------------
    //---
    //--- VirtualFileSystemPack
    //---
    //--------------------------------------------
    VirtualFileSystemPack::VirtualFileSystemPack(VFSPack& vfsPack)
        :localRoot_(NULL)
        ,size_(0)
        ,top_(NULL)
    {
        handle_ = vfsPack.handle_;
        numEntries_ = vfsPack.numEntries_;
        entries_ = vfsPack.entries_;
        stringBuffer_ = vfsPack.stringBuffer_;

        if(0<vfsPack.numEntries_){
            fs::Descriptor* desc = reinterpret_cast<fs::Descriptor*>(entries_);
            switch(desc->getType())
            {
            case fs::Type_File:
                emptyRoot_.set(1, reinterpret_cast<uintptr_t>(entries_), true);
                localRoot_ = &emptyRoot_;
                break;
            case fs::Type_Directory:{
                localRoot_ = reinterpret_cast<fs::Directory*>(entries_);
            }
                break;
            };
        }
    }

    VirtualFileSystemPack::~VirtualFileSystemPack()
    {
        for(s32 i=0; i<size_; ++i){
            LFREE(proxies_[i]);
        }

        if(NULL != handle_){
            CloseHandle(handle_);
            handle_ = NULL;
        }
        fs::Directory().swap(emptyRoot_);
        numEntries_ = 0;
        LDELETE_ARRAY(entries_);
        LDELETE_ARRAY(stringBuffer_);
    }

    s32 VirtualFileSystemPack::getType()
    {
        return fs::VFSType_Pack;
    }

    DirectoryProxy* VirtualFileSystemPack::openRoot()
    {
        return (NULL != localRoot_)? popDirectory(localRoot_) : NULL;
    }

    FileProxy* VirtualFileSystemPack::openFile(const Char* path)
    {
        LASSERT(NULL != path);
        if(lcore::PathDelimiter == *path){
            ++path;
        }
        if(lcore::CharNull == *path){
            return NULL;
        }

        s32 length = lcore::strlen_s32(path);

        //ノード名をパスから取り出す
        path = lcore::parseFirstNameFromPath(length, name_, NameBufferSize, path);

        fs::Directory* current = localRoot_;
        for(;;){
            fs::Directory* next = NULL;

            //一致するノード名の子を検索
            s32 numChildren = current->getNumChildren();
            for(s32 i=0; i<numChildren; ++i){
                fs::FileStatus status = current->getFileStatus(i);
                s32 len = lcore::maximum(length, status.nameLength_);
                if(0 == lcore::strncmp(name_, status.name_, len)){
                    if(fs::Type_File == status.type_){
                        if(*path == CharNull){
                            fs::File* f = current->getFile(i);
                            return popFile(f);
                        }
                    } else{
                        next = current->getDirectory(i);
                        break;
                    }
                }
            } //for(s32 i=0; i<numChildren;
            if(NULL == next){
                break;
            }
            if(lcore::CharNull == *path){
                break;
            }
            //次のノード名をパスから取り出す
            path = lcore::parseFirstNameFromPath(length, name_, NameBufferSize, path);
            current = next;
        } //for(;;){
        return NULL;
    }

    void VirtualFileSystemPack::closeFile(FileProxy* file)
    {
        if(NULL == file){
            return;
        }
        pushFile(static_cast<FileProxyPack*>(file));
    }

    DirectoryProxy* VirtualFileSystemPack::openDirectory(const Char* path)
    {
        LASSERT(NULL != path);
        if(lcore::PathDelimiter == *path){
            ++path;
        }
        if(lcore::CharNull == *path){
            return NULL;
        }

        s32 length = lcore::strlen_s32(path);

        //ノード名をパスから取り出す
        path = lcore::parseFirstNameFromPath(length, name_, NameBufferSize, path);

        fs::Directory* current = localRoot_;
        for(;;){
            fs::Directory* next = NULL;

            //一致するノード名の子を検索
            s32 numChildren = current->getNumChildren();
            for(s32 i=0; i<numChildren; ++i){
                fs::FileStatus status = current->getFileStatus(i);
                s32 len = lcore::maximum(length, status.nameLength_);
                if(0 == lcore::strncmp(name_, status.name_, len)){
                    if(fs::Type_Directory == status.type_){
                        next = current->getDirectory(i);
                        if(*path == CharNull){
                            return popDirectory(next);
                        }
                        break;
                    }
                }
            } //for(s32 i=0; i<numChildren;
            if(NULL == next){
                break;
            }
            if(lcore::CharNull == *path){
                break;
            }
            //次のノード名をパスから取り出す
            path = lcore::parseFirstNameFromPath(length, name_, NameBufferSize, path);
            current = next;
        } //for(;;){
        return NULL;
    }

    void VirtualFileSystemPack::closeDirectory(DirectoryProxy* directory)
    {
        if(NULL == directory){
            return;
        }
        pushDirectory(static_cast<DirectoryProxyPack*>(directory));
    }

    FileProxyPack* VirtualFileSystemPack::popFile(fs::File* file)
    {
        if(NULL == top_){
            expand();
        }
        Proxy* ptr = top_;
        top_ = top_->next_;
        FileProxyPack* f = LPLACEMENT_NEW(ptr) FileProxyPack(file, this);
        return f;
    }

    void VirtualFileSystemPack::pushFile(FileProxyPack* file)
    {
        LASSERT(NULL != file);
        LASSERT(this == file->parent_);

        file->~FileProxyPack();
        Proxy* proxy = reinterpret_cast<Proxy*>(file);
        proxy->next_ = top_;
        top_ = proxy;
    }

    DirectoryProxyPack* VirtualFileSystemPack::popDirectory(fs::Directory* directory)
    {
        if(NULL == top_){
            expand();
        }
        Proxy* ptr = top_;
        top_ = top_->next_;
        DirectoryProxyPack* d = LPLACEMENT_NEW(ptr) DirectoryProxyPack(directory, this);
        return d;
    }

    void VirtualFileSystemPack::pushDirectory(DirectoryProxyPack* directory)
    {
        LASSERT(NULL != directory);
        LASSERT(this == directory->parent_);

        directory->~DirectoryProxyPack();
        Proxy* proxy = reinterpret_cast<Proxy*>(directory);
        proxy->next_ = top_;
        top_ = proxy;
    }

    void VirtualFileSystemPack::expand()
    {
        LASSERT(size_<Bins);
        proxies_[size_] = (Proxy*)LMALLOC(sizeof(Proxy)*ExpandSize);
        for(s32 i=1; i<ExpandSize; ++i){
            proxies_[size_][i-1].next_ = &proxies_[size_][i];
        }
        proxies_[size_][ExpandSize-1].next_ = NULL;
        top_ = proxies_[size_];
        ++size_;
    }

    //--------------------------------------------
    //---
    //--- FileProxyOS
    //---
    //--------------------------------------------
    FileProxyOS::FileProxyOS(LHANDLE handle, VirtualFileSystemOS* parent)
        :handle_(handle)
        ,parent_(parent)
    {
        LASSERT(NULL != handle_);
        LASSERT(NULL != parent_);
    }

    FileProxyOS::~FileProxyOS()
    {
        CloseHandle(handle_);
        handle_ = NULL;
    }

    bool FileProxyOS::isCompressed() const
    {
        return false;
    }

    s64 FileProxyOS::getUncompressedSize() const
    {
        LARGE_INTEGER size;
        size.QuadPart = 0;
        return GetFileSizeEx(handle_, &size)? size.QuadPart : 0;
    }

    s64 FileProxyOS::getCompressedSize() const
    {
        LARGE_INTEGER size;
        size.QuadPart = 0;
        return GetFileSizeEx(handle_, &size)? size.QuadPart : 0;
    }

    s64 FileProxyOS::read(s64 offset, s64 size, u8* buffer) const
    {
        return File::read(handle_, offset, size, buffer);
    }

    s64 FileProxyOS::write(s64 offset, s64 size, u8* data)
    {
        return File::write(handle_, offset, size, data);
    }

    bool FileProxyOS::thisParent(VirtualFileSystemBase* vfs) const
    {
        return static_cast<VirtualFileSystemBase*>(parent_) == vfs;
    }

    //--------------------------------------------
    //---
    //--- DirectoryProxyOS
    //---
    //--------------------------------------------
    DirectoryProxyOS::DirectoryProxyOS(s32 length, Char* path, VirtualFileSystemOS* parent)
        :length_(length)
        ,path_(path)
        ,parent_(parent)
    {
        LASSERT(0<=length_);
        LASSERT(NULL != path_);
        LASSERT(NULL != parent_);
    }

    DirectoryProxyOS::~DirectoryProxyOS()
    {
        LDELETE_ARRAY(path_);
    }

    s32 DirectoryProxyOS::getNumChildren() const
    {
        const String& tmpPath = getSearchPath();

        WIN32_FIND_DATA data;
        HANDLE handle = FindFirstFile(tmpPath.c_str(), &data);
        if(INVALID_HANDLE_VALUE == handle){
            return 0;
        }
        s32 count = 0;
        do{
            if(!Path::isSpecial(data.dwFileAttributes, data.cFileName)){
                ++count;
            }
        }while(FindNextFile(handle, &data));
        FindClose(handle);
        return count;
    }

    s32 DirectoryProxyOS::findChild(s32 /*length*/, const Char* name) const
    {
        //LASSERT(0<=length);
        LASSERT(NULL != name);

        const String& tmpPath = getSearchPath();
        WIN32_FIND_DATA data;
        HANDLE handle = FindFirstFile(tmpPath.c_str(), &data);
        if(INVALID_HANDLE_VALUE == handle){
            return -1;
        }
        s32 count = 0;
        do{
            if(!Path::isSpecial(data.dwFileAttributes, data.cFileName)){
                if(0 == lcore::strncmp(data.cFileName, name, MAX_PATH)){
                    FindClose(handle);
                    return count;
                }
                ++count;
            }
        }while(FindNextFile(handle, &data));
        FindClose(handle);
        return -1;
    }

    fs::FileStatus DirectoryProxyOS::getStatus(s32 index) const
    {
        LASSERT(0<=index);
        fs::FileStatus status;
        lcore::memset(&status, 0, sizeof(fs::FileStatus));

        const String& tmpPath = getSearchPath();

        WIN32_FIND_DATA data;
        HANDLE handle = FindFirstFile(tmpPath.c_str(), &data);
        if(INVALID_HANDLE_VALUE == handle){
            return status;
        }
        s32 count = 0;
        do{
            if(!Path::isSpecial(data.dwFileAttributes, data.cFileName)){
                if(index == count){
                    if(0 != (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
                        status.type_ = fs::Type_Directory;
                    } else{
                        status.type_ = fs::Type_File;
                        status.umcompressedSize_ = data.nFileSizeHigh;
                        status.umcompressedSize_ = (status.umcompressedSize_<<32) | data.nFileSizeLow;
                        status.compressedSize_ = status.umcompressedSize_;
                    }
                    status.nameLength_ = lcore::strlen_s32(data.cFileName);
                    status.name_ = getTemporaryName(status.nameLength_, data.cFileName);
                    break;
                }
                ++count;
            }
        }while(FindNextFile(handle, &data));
        FindClose(handle);
        return status;
    }

    fs::FileStatus DirectoryProxyOS::getStatus(s32 /*length*/, const Char* name) const
    {
        //LASSERT(0<=length);
        LASSERT(NULL != name);
        fs::FileStatus status;
        lcore::memset(&status, 0, sizeof(fs::FileStatus));

        const String& tmpPath = getSearchPath();

        WIN32_FIND_DATA data;
        HANDLE handle = FindFirstFile(tmpPath.c_str(), &data);
        if(INVALID_HANDLE_VALUE == handle){
            return status;
        }
        do{
            if(!Path::isSpecial(data.dwFileAttributes, data.cFileName)
                && 0 == lcore::strncmp(data.cFileName, name, MAX_PATH)){
                if(0 != (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
                    status.type_ = fs::Type_Directory;
                } else{
                    status.type_ = fs::Type_File;
                    status.umcompressedSize_ = data.nFileSizeLow;
                    status.compressedSize_ = data.nFileSizeLow;
                }
                status.nameLength_ = lcore::strlen_s32(data.cFileName);
                status.name_ = getTemporaryName(status.nameLength_, data.cFileName);
                break;
            }
        }while(FindNextFile(handle, &data));
        FindClose(handle);
        return status;
    }

    FileProxy* DirectoryProxyOS::openFile(s32 index)
    {
        String& tmpPath = getSearchPath();

        WIN32_FIND_DATA data;
        HANDLE handle = FindFirstFile(tmpPath.c_str(), &data);
        if(INVALID_HANDLE_VALUE == handle){
            return NULL;
        }
        s32 count = 0;
        do{
            if(!Path::isSpecial(data.dwFileAttributes, data.cFileName)){
                if(index == count){
                    FindClose(handle);
                    if(0 != (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
                        return NULL;
                    } else{
                        HANDLE newFileHandle = CreateFile(
                            getTemporaryPath(data.cFileName),
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
                        if(INVALID_HANDLE_VALUE != newFileHandle){
                            return parent_->popFile(newFileHandle);
                        }
                        return NULL;
                    }
                }
                ++count;
            }
        }while(FindNextFile(handle, &data));
        FindClose(handle);
        return NULL;
    }

    FileProxy* DirectoryProxyOS::openFile(s32 /*length*/, const Char* name)
    {
        //LASSERT(0<=length);
        LASSERT(NULL != name);

        String& tmpPath = getSearchPath();

        WIN32_FIND_DATA data;
        HANDLE handle = FindFirstFile(tmpPath.c_str(), &data);
        if(INVALID_HANDLE_VALUE == handle){
            return NULL;
        }
        do{
            if(!Path::isSpecial(data.dwFileAttributes, data.cFileName)
                && 0 == lcore::strncmp(data.cFileName, name, MAX_PATH)){
                FindClose(handle);
                if(0 != (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
                    return NULL;
                } else{
                    HANDLE newFileHandle = CreateFile(
                        getTemporaryPath(data.cFileName),
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
                    if(INVALID_HANDLE_VALUE != newFileHandle){
                        return parent_->popFile(newFileHandle);
                    }
                    return NULL;
                }
            }
        }while(FindNextFile(handle, &data));
        FindClose(handle);
        return NULL;
    }

    DirectoryProxy* DirectoryProxyOS::openDirectory(s32 index)
    {
        String& tmpPath = getSearchPath();

        WIN32_FIND_DATA data;
        HANDLE handle = FindFirstFile(tmpPath.c_str(), &data);
        if(INVALID_HANDLE_VALUE == handle){
            return NULL;
        }
        s32 count = 0;
        do{
            if(!Path::isSpecial(data.dwFileAttributes, data.cFileName)){
                if(index == count){
                    FindClose(handle);
                    if(0 != (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
                        const String& tmp = getTemporaryDirectoryPath(data.cFileName);
                        Char* newPath = LNEW Char[tmp.length()+1];
                        lcore::memcpy(newPath, tmp.c_str(), tmp.length()+1);
                        return parent_->popDirectory(tmp.length(), newPath);
                    } else{
                        return NULL;
                    }
                }
                ++count;
            }
        }while(FindNextFile(handle, &data));
        FindClose(handle);
        return NULL;
    }

    DirectoryProxy* DirectoryProxyOS::openDirectory(s32 /*length*/, const Char* name)
    {
        //LASSERT(0<=length);
        LASSERT(NULL != name);
        String& tmpPath = getSearchPath();

        WIN32_FIND_DATA data;
        HANDLE handle = FindFirstFile(tmpPath.c_str(), &data);
        if(INVALID_HANDLE_VALUE == handle){
            return NULL;
        }
        do{
            if(!Path::isSpecial(data.dwFileAttributes, data.cFileName)
                && 0 == lcore::strncmp(data.cFileName, name, MAX_PATH)){
                FindClose(handle);
                if(0 != (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
                    const String& tmp = getTemporaryDirectoryPath(data.cFileName);
                    Char* newPath = LNEW Char[tmp.length()+1];
                    lcore::memcpy(newPath, tmp.c_str(), tmp.length()+1);
                    return parent_->popDirectory(tmp.length(), newPath);
                } else{
                    return NULL;
                }
            }
        }while(FindNextFile(handle, &data));
        FindClose(handle);
        return NULL;
    }

    bool DirectoryProxyOS::thisParent(VirtualFileSystemBase* vfs) const
    {
        return static_cast<VirtualFileSystemBase*>(parent_) == vfs;
    }

    inline String& DirectoryProxyOS::getPath() const
    {
        String& tmpPath = parent_->path_;
        tmpPath.assign(length_, path_);
        if(length_<=0){
            return tmpPath;
        }
        if(tmpPath[tmpPath.length()-1] != PathDelimiter){
            tmpPath.append(PathDelimiter);
        }
        return tmpPath;
    }

    inline String& DirectoryProxyOS::getSearchPath() const
    {
        String& tmpPath = getPath();
        tmpPath.append(1, "*");
        return tmpPath;
    }

    inline String& DirectoryProxyOS::getSearchPath(s32 length, const Char* name) const
    {
        String& tmpPath = getPath();
        tmpPath.append(length, name);
        return tmpPath;
    }

    inline const Char* DirectoryProxyOS::getTemporaryName(s32 length, const Char* name) const
    {
        String& tmpPath = parent_->path_;
        tmpPath.assign(length, name);
        return tmpPath.c_str();
    }

    inline const Char* DirectoryProxyOS::getTemporaryPath(const Char* name) const
    {
        String& tmpPath = getPath();
        tmpPath.append(lcore::strlen_s32(name), name);
        return tmpPath.c_str();
    }

    inline String& DirectoryProxyOS::getTemporaryDirectoryPath(const Char* name) const
    {
        String& tmpPath = getPath();
        tmpPath.append(lcore::strlen_s32(name), name);
        if(tmpPath.length()<=0){
            return tmpPath;
        }
        if(tmpPath[tmpPath.length()-1] != PathDelimiter){
            tmpPath.append(PathDelimiter);
        }
        return tmpPath;
    }

    //--------------------------------------------
    //---
    //--- VirtualFileSystemOS
    //---
    //--------------------------------------------
    VirtualFileSystemOS::VirtualFileSystemOS(const Char* path)
        :size_(0)
        ,top_(NULL)
    {
        LASSERT(NULL != path);
        globalRoot_.assign(lcore::strlen_s32(path), path);
        if(0<globalRoot_.length()){
            if(PathDelimiter != globalRoot_[globalRoot_.length()-1]){
                globalRoot_.append(PathDelimiter);
            }
        }
    }

    VirtualFileSystemOS::~VirtualFileSystemOS()
    {
        for(s32 i=0; i<size_; ++i){
            LFREE(proxies_[i]);
        }
    }

    s32 VirtualFileSystemOS::getType()
    {
        return fs::VFSType_OS;
    }

    DirectoryProxy* VirtualFileSystemOS::openRoot()
    {
        Char* path = LNEW Char[globalRoot_.length()+1];
        lcore::memcpy(path, globalRoot_.c_str(), globalRoot_.length()+1);
        return popDirectory(globalRoot_.length(), path);
    }

    FileProxy* VirtualFileSystemOS::openFile(const Char* path)
    {
        LASSERT(NULL != path);
        s32 length = lcore::strlen_s32(path);
        if(length<=0){
            return NULL;
        }
        if(Path::isAbsolute(length, path)){
            path_.assign(length, path);
        } else{
            path_.assign(globalRoot_.length(), globalRoot_.c_str());
            path_.append(length, path);
        }

        HANDLE handle = CreateFile(
            path_.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if(INVALID_HANDLE_VALUE != handle){
            return popFile(handle);
        }
        return NULL;
    }

    void VirtualFileSystemOS::closeFile(FileProxy* file)
    {
        if(NULL == file){
            return;
        }
        pushFile(static_cast<FileProxyOS*>(file));
    }

    DirectoryProxy* VirtualFileSystemOS::openDirectory(const Char* path)
    {
        LASSERT(NULL != path);
        s32 length = lcore::strlen_s32(path);
        if(length<=0){
            return NULL;
        }
        Char* dirPath;
        if(path[length-1] != PathDelimiter){
            --length;
        }
        dirPath = LNEW Char[length+1];
        lcore::memcpy(dirPath, path, sizeof(Char)*(length+1));
        dirPath[length] = CharNull;
        return popDirectory(length, dirPath);
    }

    void VirtualFileSystemOS::closeDirectory(DirectoryProxy* directory)
    {
        if(NULL == directory){
            return;
        }
        pushDirectory(static_cast<DirectoryProxyOS*>(directory));
    }

    FileProxyOS* VirtualFileSystemOS::popFile(LHANDLE handle)
    {
        if(NULL == top_){
            expand();
        }
        Proxy* ptr = top_;
        top_ = top_->next_;
        FileProxyOS* file = LPLACEMENT_NEW(ptr) FileProxyOS(handle, this);
        return file;
    }

    void VirtualFileSystemOS::pushFile(FileProxyOS* file)
    {
        LASSERT(NULL != file);
        LASSERT(this == file->parent_);

        file->~FileProxyOS();
        Proxy* proxy = reinterpret_cast<Proxy*>(file);
        proxy->next_ = top_;
        top_ = proxy;
    }

    DirectoryProxyOS* VirtualFileSystemOS::popDirectory(s32 length, Char* path)
    {
        if(NULL == top_){
            expand();
        }
        Proxy* ptr = top_;
        top_ = top_->next_;
        DirectoryProxyOS* directory = LPLACEMENT_NEW(ptr) DirectoryProxyOS(length, path, this);
        return directory;
    }

    void VirtualFileSystemOS::pushDirectory(DirectoryProxyOS* directory)
    {
        LASSERT(NULL != directory);
        LASSERT(this == directory->parent_);

        directory->~DirectoryProxyOS();
        Proxy* proxy = reinterpret_cast<Proxy*>(directory);
        proxy->next_ = top_;
        top_ = proxy;
    }

    void VirtualFileSystemOS::expand()
    {
        LASSERT(size_<Bins);
        proxies_[size_] = (Proxy*)LMALLOC(sizeof(Proxy)*ExpandSize);
        for(s32 i=1; i<ExpandSize; ++i){
            proxies_[size_][i-1].next_ = &proxies_[size_][i];
        }
        proxies_[size_][ExpandSize-1].next_ = NULL;
        top_ = proxies_[size_];
        ++size_;
    }

    //--------------------------------------------
    //---
    //--- FileProxyOSRaw
    //---
    //--------------------------------------------
    FileProxyOSRaw::FileProxyOSRaw(LHANDLE handle)
        :handle_(handle)
    {
        LASSERT(NULL != handle_);
    }

    FileProxyOSRaw::~FileProxyOSRaw()
    {
        if(NULL != handle_){
            CloseHandle(handle_);
            handle_ = NULL;
        }
    }

    bool FileProxyOSRaw::isCompressed() const
    {
        return false;
    }

    s64 FileProxyOSRaw::getUncompressedSize() const
    {
        LARGE_INTEGER size;
        size.QuadPart = 0;
        return GetFileSizeEx(handle_, &size)? size.QuadPart : 0;
    }

    s64 FileProxyOSRaw::getCompressedSize() const
    {
        LARGE_INTEGER size;
        size.QuadPart = 0;
        return GetFileSizeEx(handle_, &size)? size.QuadPart : 0;
    }

    s64 FileProxyOSRaw::read(s64 offset, s64 size, u8* buffer) const
    {
        return File::read(handle_, offset, size, buffer);
    }

    s64 FileProxyOSRaw::write(s64 offset, s64 size, u8* data)
    {
        return File::write(handle_, offset, size, data);
    }

    bool FileProxyOSRaw::thisParent(VirtualFileSystemBase* vfs) const
    {
        return false;
    }
}

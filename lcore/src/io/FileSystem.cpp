/**
@file FileSystem.cpp
@author t-sakai
@date 2016/10/28 create
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

#include "io/FileSystem.h"
#include "io/VirtualFileSystem.h"
#include "io/VFSPack.h"

namespace lcore
{
namespace fs
{
    //--------------------------------------------
    //---
    //--- Descriptor
    //---
    //--------------------------------------------
    Descriptor::Descriptor()
        :type_(0)
        ,flags_(0)
        ,nameLength_(0)
    {
        name_.small_[0] = CharNull;
    }

    Descriptor::Descriptor(u16 type, u16 flags)
        :type_(type)
        ,flags_(flags)
        ,nameLength_(0)
    {
        name_.small_[0] = CharNull;
    }

    Descriptor::~Descriptor()
    {
        destroyName();
    }

    void Descriptor::setName(s32 length, Char* filename, bool share)
    {
        LASSERT(0<=length);
        LASSERT(NULL != filename);
        destroyName();
        if(share){
            setFlag(Flag_ShareName);
            nameLength_ = length;
            name_.elements_ = filename;

        }else{
            nameLength_ = length;
            if(nameLength_<SmallSize){
                lcore::memcpy(name_.small_, filename, sizeof(Char)*nameLength_);
                name_.small_[nameLength_] = CharNull;
            } else{
                name_.elements_ = LNEW Char[sizeof(Char)*(nameLength_+1)];
                lcore::memcpy(name_.elements_, filename, sizeof(Char)*nameLength_);
                name_.elements_[nameLength_] = CharNull;
            }
        }
    }
    s32 Descriptor::compareName(s32 length, const Char* name) const
    {
        LASSERT(0<=length);
        LASSERT(NULL != name);
        const Char* thisName = getName();
        s32 len = lcore::maximum(length, nameLength_);
        return lcore::strncmp(thisName, name, len);
    }

    s32 Descriptor::compareName(const lcore::String& name) const
    {
        const Char* thisName = getName();
        s32 len = lcore::maximum(name.length(), nameLength_);
        return lcore::strncmp(thisName, name.c_str(), len);
    }

    void Descriptor::swap(Descriptor& rhs)
    {
        lcore::swap(type_, rhs.type_);
        lcore::swap(flags_, rhs.flags_);
        lcore::swap(nameLength_, rhs.nameLength_);
        lcore::swap(name_, rhs.name_);
    }

    void Descriptor::destroyName()
    {
        if(checkFlag(Flag_ShareName)){
            name_.small_[0] = CharNull;
            resetFlag(Flag_ShareName);

        }else{
            if(nameLength_<SmallSize){
                name_.small_[0] = CharNull;
            } else{
                LDELETE_ARRAY(name_.elements_);
            }
        }
        nameLength_ = 0;
    }

    //--------------------------------------------
    //---
    //--- File
    //---
    //--------------------------------------------
    File::File()
        :Descriptor(Type_File, 0)
        ,offset_(0)
        ,uncompressedSize_(0)
        ,compressedSize_(0)
        ,data_(NULL)
    {
    }

    File::~File()
    {
        destroy();
    }

    void File::set(
        s64 offset,
        s64 uncompressedSize,
        s64 compressedSize,
        void* data,
        bool compressed, bool share)
    {
        LASSERT(0<=offset);
        LASSERT(0<=uncompressedSize);
        LASSERT(0<=compressedSize);
        LASSERT(NULL != data);
        destroy();
        uncompressedSize_ = uncompressedSize;
        compressedSize_ = compressedSize;
        if(compressed){
            setFlag(Flag_Compressed);
        }
        if(share){
            offset_ = offset;
            setFlag(Flag_ShareData);
            data_ = data;
        } else{
            offset_ = 0;
            s64 size = (compressed)? compressedSize_ : uncompressedSize_;
            data_ = LNEW u8[size];
            lcore::memcpy(data_, data, size);
        }
    }

    s64 File::read(s64 offset, s64 size, u8* buffer) const
    {
        LASSERT(0<=offset);
        LASSERT(0<=size);
        LASSERT(NULL != buffer);

        if(uncompressedSize_<=offset){
            return 0;
        }
        s64 readSize = lcore::minimum(size, uncompressedSize_-offset);

        if(checkFlag(Flag_Compressed)){
            LASSERT(false && "not implemented");
            return 0;
        } else if(checkFlag(Flag_ShareData)){
            HANDLE handle = static_cast<HANDLE>(data_);
            return lcore::File::read(handle, offset+offset_, readSize, buffer);

        } else{
            u8* data = static_cast<u8*>(data_);
            lcore::memcpy(buffer, data+offset, readSize);
            return readSize;
        }
    }

    s64 File::write(s64, s64 size, u8* data)
    {
        LASSERT(0<=size);
        LASSERT(NULL != data);
        if(checkFlag(Flag_ShareData)){
            resetFlag(Flag_ShareData);
            resetFlag(Flag_Compressed);
            data_ = LNEW u8[size];

        } else if(checkFlag(Flag_Compressed)){
            resetFlag(Flag_Compressed);
            if(compressedSize_<size){
                LDELETE_ARRAY(data_);
                data_ = LNEW u8[size];
            }
        } else{
            if(uncompressedSize_<size){
                LDELETE_ARRAY(data_);
                data_ = LNEW u8[size];
            }
        }
        offset_ = 0;
        uncompressedSize_ = size;
        compressedSize_ = size;
        lcore::memcpy(data_, data, uncompressedSize_);
        return size;
    }

    void File::swap(File& rhs)
    {
        Descriptor::swap(rhs);
        lcore::swap(offset_, rhs.offset_);
        lcore::swap(uncompressedSize_, rhs.uncompressedSize_);
        lcore::swap(compressedSize_, rhs.compressedSize_);
        lcore::swap(data_, rhs.data_);
    }

    void File::destroy()
    {
        offset_ = 0;
        uncompressedSize_ = 0;
        compressedSize_ = 0;
        if(checkFlag(Flag_ShareData)){
            resetFlag(Flag_ShareData);
            data_ = NULL;
        }else{
            LDELETE_ARRAY(data_);
        }
        resetFlag(Flag_Compressed);
    }

    //--------------------------------------------
    //---
    //--- Directory
    //---
    //--------------------------------------------
    Directory::Directory()
        :Descriptor(Type_Directory, 0)
        ,numChildren_(0)
        ,descriptors_(0)
    {
    }

    Directory::~Directory()
    {
        destroy();
    }

    s32 Directory::findChild(s32 length, const Char* name) const
    {
        LASSERT(0<=length);
        LASSERT(NULL != name);

        u8* descs = reinterpret_cast<u8*>(descriptors_);
        u32 stride = fs::getEntrySize();

        for(s32 i=0; i<numChildren_; ++i){
            Descriptor* desc = reinterpret_cast<Descriptor*>(descs);
            if(0 == desc->compareName(length, name)){
                return i;
            }
            descs += stride;
        }
        return -1;
    }

    s32 Directory::findChild(const lcore::String& name) const
    {
        u8* descs = reinterpret_cast<u8*>(descriptors_);
        u32 stride = fs::getEntrySize();

        for(s32 i=0; i<numChildren_; ++i){
            Descriptor* desc = reinterpret_cast<Descriptor*>(descs);
            if(0 == desc->compareName(name)){
                return i;
            }
            descs += stride;
        }
        return -1;
    }

    FileStatus Directory::getFileStatus(s32 index) const
    {
        LASSERT(0<=index && index<numChildren_);
        const Descriptor* descriptor = getDescriptor(index);
        FileStatus status;
        status.type_ = descriptor->type_;
        status.flags_ = descriptor->flags_;
        status.nameLength_ = descriptor->getNameLength();
        status.name_ = descriptor->getName();

        switch(descriptor->type_)
        {
        case Type_Directory:
        {
            status.umcompressedSize_ = 0;
            status.compressedSize_ = 0;
        }
            break;
        case Type_File:
        {
            const File* file = static_cast<const File*>(descriptor);
            status.umcompressedSize_ = file->getUncompressedSize();
            status.compressedSize_ = file->getCompressedSize();
        }
            break;
        }
        return status;
    }

    //void Directory::addFile(File& file)
    //{
    //    if(0<=findChild(file.getNameLength(), file.getName())){
    //        return;
    //    }
    //    u8* descs = static_cast<u8*>(descriptors_);
    //    u32 stride = sizeof(File)<sizeof(Directory)? sizeof(Directory) : sizeof(File);
    //    u8* newDescs = LNEW u8[stride*(numChildren_+1)];
    //    lcore::memcpy(newDescs, descs, stride*numChildren_);
    //    File* f = LPLACEMENT_NEW(newDescs + stride*numChildren_) File;
    //    f->swap(file);

    //    LDELETE_ARRAY(descs);
    //    ++numChildren_;
    //    descriptors_ = static_cast<uintptr_t>(newDescs);
    //}

    //void Directory::addDirectory(Directory& directory)
    //{
    //    if(0<=findChild(directory.getNameLength(), directory.getName())){
    //        return;
    //    }
    //    u8* descs = static_cast<u8*>(descriptors_);
    //    u32 stride = sizeof(File)<sizeof(Directory)? sizeof(Directory) : sizeof(File);
    //    u8* newDescs = LNEW u8[stride*(numChildren_+1)];
    //    lcore::memcpy(newDescs, descs, stride*numChildren_);
    //    Directory* d = LPLACEMENT_NEW(newDescs + stride*numChildren_) Directory;
    //    d->swap(directory);

    //    LDELETE_ARRAY(descs);
    //    ++numChildren_;
    //    descriptors_ = static_cast<uintptr_t>(newDescs);
    //}

    //void Directory::remove(s32 index)
    //{
    //    if(numChildren_<=index){
    //        return;
    //    }
    //    if(numChildren_<=1){
    //        destroy();
    //        return;
    //    }
    //    Descriptor* descriptor = getDescriptor(index);
    //    if(descriptor->type_ == Type_File){
    //        File* f = static_cast<File*>(descriptor);
    //        f->~File();
    //    }else{
    //        Directory* d = static_cast<Directory*>(descriptor);
    //        d->~Directory();
    //    }

    //    u8* descs = static_cast<u8*>(descriptors_);
    //    u32 stride = sizeof(File)<sizeof(Directory)? sizeof(Directory) : sizeof(File);

    //    --numChildren_;
    //    u8* newDescs = LNEW u8[stride*numChildren_];
    //    lcore::memcpy(newDescs, descs, stride*index);

    //    descs += stride*(index+1);
    //    newDescs += stride*index;
    //    lcore::memcpy(newDescs, descs, stride*(numChildren_-index));

    //    LDELETE_ARRAY(descs);
    //    descriptors_ = static_cast<uintptr_t>(newDescs);
    //}

    void Directory::set(s32 numChildren, uintptr_t descriptors, bool share)
    {
        LASSERT(0<=numChildren);
        LASSERT(0 != descriptors);
        destroy();
        if(share){
            setFlag(Flag_ShareDescs);
        }
        numChildren_ = numChildren;
        descriptors_ = descriptors;
    }

    void Directory::swap(Directory& rhs)
    {
        Descriptor::swap(rhs);
        lcore::swap(numChildren_, rhs.numChildren_);
        lcore::swap(descriptors_, rhs.descriptors_);
    }

    void Directory::destroy()
    {
        u8* descs = reinterpret_cast<u8*>(descriptors_);
        u32 stride = fs::getEntrySize();

        for(s32 i=0; i<numChildren_; ++i){
            Descriptor* desc = reinterpret_cast<Descriptor*>(descs);
            switch(desc->type_){
            case Type_File:{
                File* file = static_cast<File*>(desc);
                file->~File();
            }
                break;

            case Type_Directory:{
                Directory* directory = static_cast<Directory*>(desc);
                directory->~Directory();
            }
                break;
            }
            descs += stride;
        }
        if(!checkFlag(Flag_ShareDescs)){
            LDELETE_ARRAY(descs);
            resetFlag(Flag_ShareDescs);
        }
        numChildren_ = 0;
        descriptors_ = 0;
    }
}

    //--------------------------------------------
    //---
    //--- FileSystem
    //---
    //--------------------------------------------
    FileSystem::FileSystem()
        :maxID_(-1)
    {
        for(s32 i=0; i<MaxMounts; ++i){
            vfs_[i] = NULL;
        }
    }

    FileSystem::~FileSystem()
    {
        for(s32 i=0; i<MaxMounts; ++i){
            LDELETE(vfs_[i]);
        }
        maxID_ = -1;
    }

    //
    bool FileSystem::mountOS(s32 id, const Char* path)
    {
        LASSERT(0<=id && id<MaxMounts);
        if(Path::Exists_Directory != Path::existsType(path)){
            return false;
        }
        LDELETE(vfs_[id]);
        vfs_[id] = LNEW VirtualFileSystemOS(path);
        maxID_ = lcore::maximum(maxID_, id);
        return true;
    }

    //
    bool FileSystem::mountPack(s32 id, const Char* path, bool checkHash)
    {
        LASSERT(0<=id && id<MaxMounts);
        VFSPack vfsPack;
        if(!readVFSPack(vfsPack, path, checkHash)){
            return false;
        }
        LDELETE(vfs_[id]);
        vfs_[id] = LNEW VirtualFileSystemPack(vfsPack);
        maxID_ = lcore::maximum(maxID_, id);
        return true;
    }

    //
    void FileSystem::unmount(s32 id)
    {
        LASSERT(0<=id && id<MaxMounts);
        if(NULL == vfs_[id]){
            return;
        }
        LDELETE(vfs_[id]);
        if(id != maxID_){
            return;
        }
        maxID_ = -1;
        for(s32 i=id-1; 0<=i; --i){
            if(NULL != vfs_[i]){
                maxID_ = i;
                return;
            }
        }
    }

    //
    VirtualFileSystemBase* FileSystem::get(s32 id)
    {
        LASSERT(0<=id && id<MaxMounts);
        return vfs_[id];
    }

    //
    FileProxy* FileSystem::openFile(const Char* path)
    {
        LASSERT(NULL != path);
        FileProxy* file = NULL;
        for(s32 i=maxID_; 0<=i; --i){
            if(NULL != vfs_[i]){
                file = vfs_[i]->openFile(path);
                if(NULL != file){
                    break;
                }
            }
        }
        return file;
    }

    //
    void FileSystem::closeFile(FileProxy* file)
    {
        if(NULL == file){
            return;
        }
        for(s32 i=maxID_; 0<=i; --i){
            if(NULL != vfs_[i] && file->thisParent(vfs_[i])){
                vfs_[i]->closeFile(file);
                return;
            }
        }
    }

    //
    DirectoryProxy* FileSystem::openDirectory(const Char* path)
    {
        LASSERT(NULL != path);
        DirectoryProxy* directory = NULL;
        for(s32 i=maxID_; 0<=i; --i){
            if(NULL != vfs_[i]){
                directory = vfs_[i]->openDirectory(path);
                if(NULL != directory){
                    break;
                }
            }
        }
        return directory;
    }

    //
    void FileSystem::closeDirectory(DirectoryProxy* directory)
    {
        if(NULL == directory){
            return;
        }
        for(s32 i=maxID_; 0<=i; --i){
            if(NULL != vfs_[i] && directory->thisParent(vfs_[i])){
                vfs_[i]->closeDirectory(directory);
                return;
            }
        }
    }

    //
    bool FileSystem::packDirectory(const Char* filepath, const Char* path)
    {
        LASSERT(NULL != filepath);
        LASSERT(NULL != path);
        return writeVFSPack(filepath, path);
    }
}

/**
@file Resource.cpp
@author t-sakai
@date 2014/07/15 create
*/
#include "opus/Resource.h"

namespace lsound
{
    //-------------------------------------------
    //---
    //--- File
    //---
    //-------------------------------------------
    File::File()
        :refCount_(0)
        ,file_(NULL)
    {
    }

#if defined(_WIN32)
    File::File(lcore::LHANDLE file)
#else
    File::File(FILE* file)
#endif
        :refCount_(0)
        ,file_(file)
    {
    }

    File::~File()
    {
        if(NULL != file_){
#if defined(_WIN32)
            CloseHandle(file_);
#else
            fclose(file_);
#endif
            file_ = NULL;
        }
    }

    void File::addRef()
    {
        ++refCount_;
    }

    void File::release()
    {
        if(--refCount_ == 0){
            LDELETE_RAW(this);
        }
    }

    void File::seek(s64 pos, s32 origin)
    {
#ifdef _WIN32
        LARGE_INTEGER loffset;
        loffset.QuadPart = pos;
        SetFilePointerEx(file_, loffset, NULL, origin);
#else
        fseek(file_, pos, origin);
#endif
    }

    s32 File::read(s32 size, void* data)
    {
#ifdef _WIN32
        DWORD numBytesRead = 0;
        return ReadFile(file_, data, static_cast<DWORD>(size), &numBytesRead, NULL)
            ? static_cast<s32>(numBytesRead)
            : 0;
#else
        return static_cast<s32>(fread(data, size, 1, file_));
#endif
    }

    void File::swap(File& rhs)
    {
        lcore::swap(refCount_, rhs.refCount_);
        lcore::swap(file_, rhs.file_);
    }

    //-------------------------------------------
    //---
    //--- Memory
    //---
    //-------------------------------------------
    Memory::Memory()
        :refCount_(0)
        ,size_(0)
        ,memory_(NULL)
    {
    }

    Memory::Memory(s64 size, u8* memory)
        :refCount_(0)
        ,size_(size)
        ,memory_(memory)
    {
    }

    Memory::~Memory()
    {
        LDELETE_ARRAY(memory_);
    }

    void Memory::addRef()
    {
        ++refCount_;
    }

    void Memory::release()
    {
        if(--refCount_ == 0){
            LDELETE_RAW(this);
        }
    }

    void Memory::swap(Memory& rhs)
    {
        lcore::swap(refCount_, rhs.refCount_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(memory_, rhs.memory_);
    }

#ifdef ANDROID
    //-------------------------------------------
    //---
    //--- Asset
    //---
    //-------------------------------------------
    Asset::Asset()
        :refCount_(0)
        ,asset_(NULL)
    {
    }

    Asset::Asset(AAsset* asset)
        :refCount_(0)
        ,asset_(asset)
    {
    }

    Asset::~Asset()
    {
        if(NULL != asset_){
            AAsset_close(asset_);
            asset_ = NULL;
        }
    }

    void Asset::addRef()
    {
        ++refCount_;
    }

    void Asset::release()
    {
        if(--refCount_ == 0){
            LDELETE_RAW(this);
        }
    }
#endif

    //-------------------------------------------
    //---
    //--- PackFile
    //---
    //-------------------------------------------
    namespace
    {
#if defined(_WIN32)
        lcore::LHANDLE open_file(const Char* filepath)
        {
            return CreateFile(
                filepath,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
        }

        bool read(lcore::LHANDLE file, s64 size, void* data)
        {
            DWORD numBytesRead = 0;
            return ReadFile(file, data, static_cast<DWORD>(size), &numBytesRead, NULL);
        }

        s64 fsize(lcore::LHANDLE file)
        {
            LARGE_INTEGER result;
            return GetFileSizeEx(file, &result)? result.QuadPart : 0;
        }

        lcore::off_t tell(lcore::LHANDLE file)
        {
            LARGE_INTEGER offset;
            offset.QuadPart = 0;
            if(!SetFilePointerEx(file, offset, &offset, FILE_CURRENT)){
                return 0;
            }
            return offset.QuadPart;
        }

        void seek(lcore::LHANDLE file, lcore::off_t offset, s32 dir)
        {
            LARGE_INTEGER loffset;
            loffset.QuadPart = offset;
            SetFilePointerEx(file, loffset, NULL, dir);
        }

        void close(lcore::LHANDLE file)
        {
            CloseHandle(file);
        }
#else
        FILE* open_file(const Char* filepath)
        {
            return fopen(filepath, "rb");
        }

        bool read(FILE* file, u32 size, void* data)
        {
            return 0<fread(data, size, 1, file);
        }

        s64 fsize(FILE* file)
        {
            s32 fd = fileno(file);
            if(fd<0){
                return 0;
            }
            stat64 buff;
            if(fstat64(fd, &buff)<0){
                return 0;
            }
            return buff.st_size;
        }

        lcore::off_t tell(FILE* file file)
        {
            return ftell(file);
        }

        void seek(FILE* file, lcore::off_t offset, s32 dir)
        {
            fseek(file, offset, dir)
        }

        void close(FILE* file)
        {
            fclose(file);
        }
#endif
    }
    PackFile::PackFile()
        :entries_(NULL)
        ,file_(NULL)
    {
    }

    PackFile::~PackFile()
    {
        LDELETE_ARRAY(entries_);
        if(NULL != file_){
            file_->release();
            file_ = NULL;
        }
    }

    void PackFile::get(s32 index, File*& file, s64& start, s64& end)
    {
        LASSERT(0<=index && index<numFiles_);
        file = file_;
        start = entries_[index].offset_;
        end = start + entries_[index].size_;
    }

    PackFile* PackFile::open(const Char* path)
    {
        LASSERT(NULL != path);
#if defined(_WIN32)
        lcore::LHANDLE file = open_file(path);
#else
        FILE* file = open_file(path);
#endif
        //lcore::Log("PackFile::open %s %s", path, (NULL==f)?"false":"true");
        if(NULL == file){
            return NULL;
        }

        PackHeader header;
        if(!read(file, sizeof(PackHeader), &header)){
            close(file);
            return NULL;
        }
        FileEntry* entries = LNEW FileEntry[header.numFiles_];
        if(!read(file, sizeof(FileEntry)*header.numFiles_, entries)){
            LDELETE_ARRAY(entries);
            close(file);
            return NULL;
        }

        //ファイル先頭からのオフセットに変換
        s64 dataTop = tell(file);
        for(u32 i=0; i<header.numFiles_; ++i){
            entries[i].offset_ += dataTop;
        }

        PackFile* packFile = LNEW PackFile();
        packFile->numFiles_ = header.numFiles_;
        packFile->entries_ = entries;
        packFile->file_ = LNEW File(file);
        packFile->file_->addRef();
        return packFile;
    }

    //-------------------------------------------
    //---
    //--- PackMemory
    //---
    //-------------------------------------------
    PackMemory::PackMemory()
        :entries_(NULL)
        ,memory_(NULL)
    {
    }

    PackMemory::~PackMemory()
    {
        LDELETE_ARRAY(entries_);
        if(NULL != memory_){
            memory_->release();
        }
    }

    void PackMemory::get(s32 index, Memory*& memory, s64& offset, s64& size)
    {
        LASSERT(0<=index && index<numFiles_);
        memory = memory_;
        offset = entries_[index].offset_;
        size = entries_[index].size_;
    }

    PackMemory* PackMemory::open(const Char* path)
    {
        LASSERT(NULL != path);
#if defined(_WIN32)
        lcore::LHANDLE file = open_file(path);
#else
        FILE* file = open_file(path);
#endif
        if(NULL == file){
            return NULL;
        }

        PackHeader header;
        if(!read(file, sizeof(PackHeader), &header)){
            close(file);
            return NULL;
        }
        FileEntry* entries = LNEW FileEntry[header.numFiles_];
        if(!read(file, sizeof(FileEntry)*header.numFiles_, entries)){
            LDELETE_ARRAY(entries);
            close(file);
            return NULL;
        }

        //Calculate data size
        s64 dataTop = tell(file);
        s64 size = fsize(file) - dataTop;

        u8* memory = LNEW u8[size];
        if(!read(file, size, memory)){
            LDELETE_ARRAY(memory);
            LDELETE_ARRAY(entries);
            close(file);
            return NULL;
        }
        close(file);

        PackMemory* packMemory = LNEW PackMemory();
        packMemory->numFiles_ = header.numFiles_;
        packMemory->entries_ = entries;
        packMemory->memory_ = LNEW Memory(size, memory);
        packMemory->memory_->addRef();
        return packMemory;
    }

#ifdef ANDROID
    PackMemory* PackMemory::openFromAsset(AAssetManager* assetManager, const Char* path, s32 mode)
    {
        LASSERT(NULL != assetManager);
        LASSERT(NULL != path);
        AAsset* asset = AAssetManager_open(assetManager, path, mode);

        if(NULL == asset){
            return NULL;
        }

        s32 pos = 0;
        s32 ret;
        PackHeader header;
        ret = AAsset_read(asset, &header, sizeof(PackHeader));
        if(0>=ret){
            AAsset_close(asset);
            return NULL;
        }
        pos += ret;

        FileEntry* entries = LIME_NEW FileEntry[header.numFiles_];
        ret = AAsset_read(asset, entries, sizeof(FileEntry)*header.numFiles_);
        if(0>=ret){
            LIME_DELETE_ARRAY(entries);
            AAsset_close(asset);
            return NULL;
        }
        pos += ret;

        //データサイズ計算
        s32 dataTop = pos;
        u32 size = AAsset_getLength(asset) - dataTop;

        u8* memory = LIME_NEW u8[size];
        ret = AAsset_read(asset, memory, size);
        AAsset_close(asset);
        if(0>=ret){
            LIME_DELETE_ARRAY(memory);
            LIME_DELETE_ARRAY(entries);
            return NULL;
        }

        PackMemory* packMemory = LIME_NEW PackMemory();
        packMemory->numFiles_ = header.numFiles_;
        packMemory->entries_ = entries;
        packMemory->memory_ = LIME_NEW Memory(size, memory);
        packMemory->memory_->addRef();
        return packMemory;
    }

    //-------------------------------------------
    //---
    //--- PackAsset
    //---
    //-------------------------------------------
    PackAsset::PackAsset()
        :entries_(NULL)
        ,asset_(NULL)
    {
    }

    PackAsset::~PackAsset()
    {
        LIME_DELETE_ARRAY(entries_);
        if(NULL != asset_){
            asset_->release();
        }
    }

    void PackAsset::get(s32 index, Asset*& asset, s32& start, s32& end)
    {
        LASSERT(0<=index && index<numFiles_);
        asset = asset_;
        start = entries_[index].offset_;
        end = start + entries_[index].size_;
    }

    PackAsset* PackAsset::open(AAssetManager* assetManager, const Char* path, s32 mode)
    {
        LASSERT(NULL != assetManager);
        LASSERT(NULL != path);

        AAsset* asset = AAssetManager_open(assetManager, path, mode);

        if(NULL == asset){
            return NULL;
        }

        s32 pos = 0;
        s32 ret;
        PackHeader header;
        ret = AAsset_read(asset, &header, sizeof(PackHeader));
        if(0>=ret){
            AAsset_close(asset);
            return NULL;
        }
        pos += ret;

        FileEntry* entries = LIME_NEW FileEntry[header.numFiles_];
        ret = AAsset_read(asset, entries, sizeof(FileEntry)*header.numFiles_);
        if(0>=ret){
            LIME_DELETE_ARRAY(entries);
            AAsset_close(asset);
            return NULL;
        }
        pos += ret;

        //ファイル先頭からのオフセットに変換
        s32 dataTop = pos;
        for(s32 i=0; i<header.numFiles_; ++i){
            entries[i].offset_ += dataTop;
        }

        PackAsset* packAsset = LIME_NEW PackAsset();
        packAsset->numFiles_ = header.numFiles_;
        packAsset->entries_ = entries;
        packAsset->asset_ = LIME_NEW Asset(asset);
        packAsset->asset_->addRef();
        return packAsset;
    }
#endif
}

#ifndef INC_LSOUND_RESOURCE_H_
#define INC_LSOUND_RESOURCE_H_
/**
@file Resource.h
@author t-sakai
@date 2014/07/15 create
*/
#include "../lsound.h"
#include <stdio.h>
#ifdef ANDROID
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

#include "Pack.h"

//#define LSOUND_RESOURCE_ENABLE_SYNC
#ifdef LSOUND_RESOURCE_ENABLE_SYNC
#include <lcore/SyncObject.h>
#endif

namespace lsound
{
    //-------------------------------------------
    //---
    //--- File
    //---
    //-------------------------------------------
    class File
    {
    public:
        File();

#if defined(_WIN32)
        explicit File(lcore::LHANDLE file);
#else
        explicit File(FILE* file);
#endif

        ~File();

        void addRef();
        void release();

        void seek(s64 pos, s32 origin);

        /**
        @return Number of bytes which been read
        */
        s32 read(s32 size, void* data);

        void swap(File& rhs);
    private:
        File(const File&) = delete;
        File(File&&) = delete;
        File& operator=(const File&) = delete;
        File& operator=(File&&) = delete;

        s32 refCount_;
#if defined(_WIN32)
        lcore::LHANDLE file_;
#else
        FILE* file_;
#endif
#ifdef LSOUND_RESOURCE_ENABLE_SYNC
        lcore::CriticalSection cs_;
#endif
    };

    //-------------------------------------------
    //---
    //--- Memory
    //---
    //-------------------------------------------
    class Memory
    {
    public:
        Memory();
        Memory(s64 size, u8* memory);
        ~Memory();

        void addRef();
        void release();

        void seek(s64 pos, s32 dir);

        /**
        @return Number of bytes which been read
        */
        s32 read(s32 size, void* data);

        void swap(Memory& rhs);
    private:
        Memory(const Memory&) = delete;
        Memory(Memory&&) = delete;
        Memory& operator=(const Memory&) = delete;
        Memory& operator=(Memory&&) = delete;

        friend class MemoryStream;

        s32 refCount_;
        s64 size_;
        u8* memory_;
    };

#ifdef ANDROID
    //-------------------------------------------
    //---
    //--- Asset
    //---
    //-------------------------------------------
    class Asset
    {
    public:
        Asset();
        Asset(AAsset* asset);
        ~Asset();

        void addRef();
        void release();
    private:
        Asset(const Asset&) = delete;
        Asset(Asset&&) = delete;
        Asset& operator=(const Asset&) = delete;
        Asset& operator=(Asset&&) = delete;

        friend class AssetStream;
#ifdef LSOUND_USE_WAVE
        friend class Wave;
#endif
        s32 refCount_;
        AAsset* asset_;
#ifdef LSOUND_RESOURCE_ENABLE_SYNC
        lcore::CriticalSection cs_;
#endif
    };
#endif

    //-------------------------------------------
    //---
    //--- PackResource
    //---
    //-------------------------------------------
    class PackResource
    {
    public:
        enum ResourceType
        {
            ResourceType_File,
            ResourceType_Memory,
            ResourceType_Asset,
        };

        virtual ~PackResource()
        {}

        /// ÉtÉ@ÉCÉãêîéÊìæ
        s32 getNumFiles() const{ return numFiles_;}

        virtual s32 getType() const =0;
    protected:
        PackResource()
            :numFiles_(0)
        {}

        s32 numFiles_;
    };

    //-------------------------------------------
    //---
    //--- PackFile
    //---
    //-------------------------------------------
    class PackFile : public PackResource
    {
    public:
        PackFile();
        virtual ~PackFile();

        virtual s32 getType() const{ return PackResource::ResourceType_File;}

        void get(s32 index, File*& file, s64& start, s64& end);

        static PackFile* open(const Char* path);
    private:
        PackFile(const PackFile&) = delete;
        PackFile(PackFile&&) = delete;
        PackFile& operator=(const PackFile&) = delete;
        PackFile& operator=(PackFile&&) = delete;

        FileEntry* entries_;
        File* file_;
    };

    //-------------------------------------------
    //---
    //--- PackMemory
    //---
    //-------------------------------------------
    class PackMemory : public PackResource
    {
    public:
        PackMemory();
        virtual ~PackMemory();

        virtual s32 getType() const{ return PackResource::ResourceType_Memory;}

        void get(s32 index, Memory*& memory, s64& offset, s64& size);

        static PackMemory* open(const Char* path);

#ifdef ANDROID
        static PackMemory* openFromAsset(AAssetManager* assetManager, const Char* path, s32 mode);
#endif

    private:
        PackMemory(const PackMemory&) = delete;
        PackMemory(PackMemory&&) = delete;
        PackMemory& operator=(const PackMemory&) = delete;
        PackMemory& operator=(PackMemory&&) = delete;

        FileEntry* entries_;
        Memory* memory_;
    };

#ifdef ANDROID
    //-------------------------------------------
    //---
    //--- PackAsset
    //---
    //-------------------------------------------
    class PackAsset : public PackResource
    {
    public:
        PackAsset();
        virtual ~PackAsset();

        virtual s32 getType() const{ return PackResource::ResourceType_Asset;}

        void get(s32 index, Asset*& asset, s32& start, s32& end);

        static PackAsset* open(AAssetManager* assetManager, const Char* path, s32 mode);
    private:
        PackAsset(const PackAsset&) = delete;
        PackAsset(PackAsset&&) = delete;
        PackAsset& operator=(const PackAsset&) = delete;
        PackAsset& operator=(PackAsset&&) = delete;

        FileEntry* entries_;
        Asset* asset_;
    };
#endif
}
#endif //INC_LSOUND_RESOURCE_H_

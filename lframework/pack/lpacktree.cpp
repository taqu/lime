/**
@file lpacktree.cpp
@author t-sakai
@date 2015/08/21 create
*/
#include "lpacktree.h"
#include <lcore/clibrary.h>

namespace lpacktree
{
    bool isLittleEndian()
    {
        s32 x = 1;
        return (*(s8*)&x)? true : false;
    }

    //---------------------------------------
    //---
    //--- PackBase
    //---
    //---------------------------------------
    PackBase::PackBase()
        :entries_(NULL)
    {
        header_.endian_ = Endian_Little;
        header_.reserved_ = 0;
        header_.numEntries_ = 0;
    }

    PackBase::PackBase(const PackHeader& header, FileEntry* entries)
        :header_(header)
        ,entries_(entries)
    {
    }

    PackBase::~PackBase()
    {
        LIME_DELETE_ARRAY(entries_);
    }

    const FileEntry* PackBase::find(const Char* path) const
    {
        LASSERT(NULL != path);
        if(lcore::Delimiter == path[0]){
            ++path;
        }

        Char name[MaxFileNameBufferSize];

        s32 start = 0;
        s32 end = header_.numEntries_;

        u32 length;
        path = lcore::parseNextNameFromPath(path, length, name, MaxFileNameLength);
        while(0<=length){
            for(s32 i=start; i<end; ++i){
                if(0 == lcore::memcmp(name, entries_[i].name_, length)){
                    if(entries_[i].type_ == Type_File){
                        return &entries_[i];
                    }
                    start = i+1;
                    end = start + entries_[i].numChildren_;
                    break;

                }else if(entries_[i].type_ == Type_Directory){
                    return NULL;
                }
            }
            path = lcore::parseNextNameFromPath(path, length, name, MaxFileNameLength);
        }
        return NULL;
    }

    void PackBase::swap(PackBase& rhs)
    {
        PackHeader tmp = header_;
        header_ = rhs.header_;
        rhs.header_ = tmp;

        lcore::swap(entries_, rhs.entries_);
    }

    //---------------------------------------
    //---
    //--- PackEntry
    //---
    //---------------------------------------
    PackEntry::PackEntry()
    {
    }

    PackEntry::PackEntry(const PackHeader& header, FileEntry* entries)
        :PackBase(header, entries)
    {
    }

    PackEntry::~PackEntry()
    {
    }

    void PackEntry::swap(PackEntry& rhs)
    {
        PackBase::swap(rhs);
    }

    //---------------------------------------
    //---
    //--- PackMemory
    //---
    //---------------------------------------
    PackMemory::PackMemory()
        :size_(0)
        ,data_(NULL)
    {
    }

    PackMemory::PackMemory(const PackHeader& header, FileEntry* entries, u32 size, u8* data)
        :PackBase(header, entries)
        ,size_(size)
        ,data_(data)
    {
    }

    PackMemory::~PackMemory()
    {
        LIME_DELETE_ARRAY(data_);
    }

    u32 PackMemory::read(u8* dst, u32 index, u32 offset, u32 size) const
    {
        LASSERT(NULL != data_);

        const FileEntry& entry = getEntry(index);
        LASSERT(size<=entry.size_);
        LASSERT((entry.offset_+size)<=size_);

        offset += entry.offset_;
        lcore::memcpy(dst, data_+offset, size);
        return size;
    }

    const u8* PackMemory::get(u32 index, u32 offset) const
    {
        LASSERT(NULL != data_);

        const FileEntry& entry = getEntry(index);

        offset += entry.offset_;
        return data_+offset;
    }

    void PackMemory::swap(PackMemory& rhs)
    {
        PackBase::swap(rhs);
        lcore::swap(size_, rhs.size_);
        lcore::swap(data_, rhs.data_);
    }

    //---------------------------------------
    //---
    //--- PackFile
    //---
    //---------------------------------------
    PackFile::PackFile()
    {
    }

    PackFile::PackFile(const PackHeader& header, FileEntry* entries, lcore::ifstream& file)
        :PackBase(header, entries)
    {
        file_.swap(file);
    }

    PackFile::~PackFile()
    {
        file_.close();
    }

    u32 PackFile::read(u8* dst, u32 index, u32 offset, u32 size)
    {
        const FileEntry& entry = getEntry(index);

        offset += entry.offset_;
        if(!file_.seekg(offset, lcore::ios::beg)){
            return 0;
        }

        return lcore::io::read(file_, dst, size);
    }

    void PackFile::swap(PackFile& rhs)
    {
        PackBase::swap(rhs);
        file_.swap(rhs.file_);
    }
}

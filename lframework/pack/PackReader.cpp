/**
@file PackReader.cpp
@author t-sakai
@date 2012/03/21 create

*/
#include "PackReader.h"

namespace lpacktree
{
    //-------------------------------------------------
    bool PackReader::readEntry(PackEntry& pack, const Char* path)
    {
        if(NULL == path){
            return false;
        }

        lcore::ifstream file(path, lcore::ios::in|lcore::ios::binary);

        if(!file.is_open()){
            return false;
        }

        u32 fileSize = file.getSize(0);

        //PackHeader読み込み
        PackHeader header;
        lcore::lsize_t ret = lcore::io::read(file, header);
        if(0 == ret){
            return false;
        }

        //FileEntry読み込み
        FileEntry* entries = LIME_NEW FileEntry[header.numEntries_];

        u32 size = sizeof(FileEntry) * header.numEntries_;
        ret = lcore::io::read(file, entries, size);
        if(0 == ret){
            LIME_DELETE_ARRAY(entries);
            return false;
        }

        //各エントリのチェック
        u32 dataOffset = sizeof(PackHeader) + size;
        u32 offset = dataOffset;
        for(u32 i=0; i<header.numEntries_; ++i){
            if(offset < entries[i].offset_){ //offset値が不正
                LIME_DELETE_ARRAY(entries);
                return false;
            }
            offset += entries[i].size_;
        }

        if(fileSize<offset){ //ファイルサイズとあわない
            LIME_DELETE_ARRAY(entries);
            return false;
        }

        PackEntry tmp(header, entries);
        pack.swap(tmp);
        return true;
    }

    //-------------------------------------------------
    bool PackReader::readMemory(PackMemory& pack, const Char* path)
    {
        if(NULL == path){
            return false;
        }

        lcore::ifstream file(path, lcore::ios::in|lcore::ios::binary);

        if(!file.is_open()){
            return false;
        }

        u32 fileSize = file.getSize(0);

        //PackHeader読み込み
        PackHeader header;
        lcore::lsize_t ret = lcore::io::read(file, header);
        if(0 == ret){
            return false;
        }

        //FileEntry読み込み
        FileEntry* entries = LIME_NEW FileEntry[header.numEntries_];

        u32 size = sizeof(FileEntry) * header.numEntries_;
        ret = lcore::io::read(file, entries, size);
        if(0 == ret){
            LIME_DELETE_ARRAY(entries);
            return false;
        }

        //各エントリのチェックとoffsetの補正
        u32 dataOffset = sizeof(PackHeader) + size;
        u32 offset = dataOffset;
        for(u32 i=0; i<header.numEntries_; ++i){
            if(offset < entries[i].offset_){ //offset値が不正
                LIME_DELETE_ARRAY(entries);
                return false;
            }
            entries[i].offset_ -= dataOffset; //data先頭からのoffset
            offset += entries[i].size_;
        }

        if(fileSize<offset){ //ファイルサイズとあわない
            LIME_DELETE_ARRAY(entries);
            return false;
        }

        //data読み込み
        u32 dataSize = offset-dataOffset;
        u8* data = LIME_NEW u8[dataSize];
        ret = lcore::io::read(file, data, dataSize);
        if(0 == ret){
            LIME_DELETE_ARRAY(data);
            LIME_DELETE_ARRAY(entries);
            return false;
        }

        PackMemory tmp(header, entries, dataSize, data);
        pack.swap(tmp);
        return true;
    }

    //-------------------------------------------------
    bool PackReader::readFile(PackFile& pack, const Char* path)
    {
        if(NULL == path){
            return false;
        }

        lcore::ifstream file(path, lcore::ios::in|lcore::ios::binary);

        if(!file.is_open()){
            return false;
        }

        u32 fileSize = file.getSize(0);

        //PackHeader読み込み
        PackHeader header;
        lcore::lsize_t ret = lcore::io::read(file, header);
        if(0 == ret){
            return false;
        }

        //FileEntry読み込み
        FileEntry* entries = LIME_NEW FileEntry[header.numEntries_];

        u32 size = sizeof(FileEntry) * header.numEntries_;
        ret = lcore::io::read(file, entries, size);
        if(0 == ret){
            LIME_DELETE_ARRAY(entries);
            return false;
        }

        //各エントリのチェック
        u32 dataOffset = sizeof(PackHeader) + size;
        u32 offset = dataOffset;
        for(u32 i=0; i<header.numEntries_; ++i){
            if(offset < entries[i].offset_){ //offset値が不正
                LIME_DELETE_ARRAY(entries);
                return false;
            }
            offset += entries[i].size_;
        }

        if(fileSize<offset){ //ファイルサイズとあわない
            LIME_DELETE_ARRAY(entries);
            return false;
        }

        PackFile tmp(header, entries, file);
        pack.swap(tmp);
        return true;
    }
}

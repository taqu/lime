/**
@file PackReader.cpp
@author t-sakai
@date 2012/03/21 create

*/
#include "PackReader.h"

namespace pack
{
    //-------------------------------------------------
    PackReader::PackReader()
        :entries_(NULL)
    {
        header_.endian_ = Endian_Little;
        header_.reserved_ = 0;
        header_.numFiles_ = 0;
    }

    //-------------------------------------------------
    PackReader::~PackReader()
    {
        LIME_DELETE_ARRAY(entries_);
    }

    //-------------------------------------------------
    // ヘッダーロード
    bool PackReader::readHeader()
    {
        LASSERT(stream_.is_open());

        PackHeader header;
        lcore::lsize_t ret = lcore::io::read(stream_, header);
        if(ret == 0){
            return false;
        }

        FileEntry* entries = LIME_NEW FileEntry[header.numFiles_];

        u32 size = sizeof(FileEntry) * header.numFiles_;
        ret = lcore::io::read(stream_, entries, size);

        if(ret == 0){
            LIME_DELETE_ARRAY(entries);
            return false;
        }

        header_ = header;
        LIME_DELETE_ARRAY(entries_);
        entries_ = entries;
        return true;
    }

    //-------------------------------------------------
    bool PackReader::read(u32 index, void* dst)
    {
        LASSERT(stream_.is_open());
        LASSERT(0<=index && index<header_.numFiles_);
        LASSERT(NULL != dst);

        const FileEntry& entry = entries_[index];

        if(false == stream_.seekg( entry.offset_, lcore::ios::beg )){
            return false;
        }

        lcore::lsize_t ret = lcore::io::read(stream_, dst, entry.size_);
        return (0 != ret);
    }
}

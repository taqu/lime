/**
@file PackWriter.cpp
@author t-sakai
@date 2012/03/21 create

*/

#include "opus/PackWriter.h"

namespace lsound
{
    //-------------------------------------------------
    PackWriter::PackWriter()
    {
        header_.numFiles_ = 0;
    }

    //-------------------------------------------------
    PackWriter::~PackWriter()
    {
        listStream_.close();
        for(MemPtrArray::iterator itr = mempts_.begin();
            itr != mempts_.end();
            ++itr)
        {
            LDELETE_ARRAY(*itr);
        }
    }

    //-------------------------------------------------
    bool PackWriter::push_back(const Char* name, s64 size, const void* buffer)
    {
        LASSERT(NULL != name);

        FileEntry entry;
        entry.size_ = size;

        entries_.push_back(entry);
        
        u8* tmp = LNEW u8[size];
        lcore::memcpy(tmp, buffer, size);

        mempts_.push_back(tmp);

        if(listStream_.is_open()){
            listStream_.print("%s,\r\n", name);
        }
        return true;
    }

    //-------------------------------------------------
    bool PackWriter::openListFile(const Char* path)
    {
        LASSERT(NULL != path);
        return listStream_.open(path, lcore::ios::out);
    }

    //-------------------------------------------------
    bool PackWriter::write(const Char* path)
    {
        LASSERT(NULL != path);
        lcore::File file;
        if(!file.open(path, lcore::ios::out)){
            return false;
        }
        PackHeader header;
        header.numFiles_ = static_cast<u32>(entries_.size());
        if(!file.write(header)){
            return false;
        }

        s64 offset = 0;
        for(FileEntryArray::iterator itr = entries_.begin();
            itr != entries_.end();
            ++itr)
        {
            (*itr).offset_ = offset;
            offset += (*itr).size_;

            if(!file.write(*itr)){
                return false;
            }
        }

        FileEntryArray::iterator entryItr = entries_.begin();
        for(MemPtrArray::iterator itr = mempts_.begin();
            itr != mempts_.end();
            ++itr, ++entryItr)
        {
            if(!file.write((*entryItr).size_, (*itr))){
                return false;
            }
        }
        file.close();
        return true;
    }
}

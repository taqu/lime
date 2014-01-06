/**
@file PackWriter.cpp
@author t-sakai
@date 2012/03/21 create

*/

#include "PackWriter.h"
#include <lcore/utility.h>

namespace lpack
{
    //-------------------------------------------------
    PackWriter::PackWriter()
    {
        header_.endian_ = Endian_Little;
        header_.reserved_ = 0;
        header_.numFiles_ = 0;
    }

    //-------------------------------------------------
    PackWriter::~PackWriter()
    {
        for(MemPtrArray::iterator itr = mempts_.begin();
            itr != mempts_.end();
            ++itr)
        {
            LIME_DELETE_ARRAY(*itr);
        }
    }

    //-------------------------------------------------
    bool PackWriter::push_back(const Char* name, u32 size, const void* buffer)
    {
        LASSERT(NULL != name);

        FileEntry entry;

        lcore::strncpy(entry.name_, MaxFileNameBufferSize, name, MaxFileNameLength);
        entry.name_[MaxFileNameLength] = '\0';

        entry.size_ = size;

        entries_.push_back(entry);
        
        u8* tmp = LIME_NEW u8[size];
        lcore::memcpy(tmp, buffer, size);

        mempts_.push_back(tmp);
        return true;
    }

    //-------------------------------------------------
    bool PackWriter::write(const Char* path)
    {
        if(false == open(path)){
            return false;
        }

        header_.numFiles_ = static_cast<u16>( entries_.size() );

        lcore::lsize_t ret = lcore::io::write(stream_, header_);
        if(0 == ret){
            return false;
        }

        s32 offset = stream_.tellg() + sizeof(FileEntry) * header_.numFiles_;

        for(FileEntryArray::iterator itr = entries_.begin();
            itr != entries_.end();
            ++itr)
        {
            (*itr).offset_ = offset;
            offset += (*itr).size_;

            ret = lcore::io::write(stream_, (*itr));
            if(0 == ret){
                return false;
            }
        }

        FileEntryArray::iterator entryItr = entries_.begin();
        for(MemPtrArray::iterator itr = mempts_.begin();
            itr != mempts_.end();
            ++itr)
        {
            ret = lcore::io::write( stream_, (*itr), (*entryItr).size_ );
            if(0 == ret){
                return false;
            }
            ++entryItr;
        }

        close();
        return true;
    }

    //-------------------------------------------------
    bool PackWriter::writeList(const Char* path)
    {
        if(false == open(path)){
            return false;
        }

        Char buffer[MaxFileNameLength*2];

        for(FileEntryArray::iterator itr = entries_.begin();
            itr != entries_.end();
            ++itr)
        {
            lcore::lsize_t len = lcore::strlen(itr->name_);

            const Char* dot = lcore::rFindChr(itr->name_, '.', len);

            if(NULL != dot){
                lcore::strncpy(buffer, MaxFileNameLength*2, itr->name_, dot - itr->name_);
            }else{
                lcore::strncpy(buffer, MaxFileNameLength*2, itr->name_, len);
            }
            for(Char* c = buffer; *c != '\0'; ++c){
                *c = toupper(*c);
            }
            stream_.print("%s,\n", buffer);
        }

        close();
        return true;
    }
}

/**
@file PackReader.cpp
@author t-sakai
@date 2013/02/12 create
*/
#include "opus/PackReader.h"
#include <lcore/File.h>

namespace lsound
{
    //------------------------------------------
    //---
    //--- PackReader
    //---
    //------------------------------------------
    bool PackReader::read(Pack& pack, const Char* path)
    {
        LASSERT(NULL != path);
        lcore::File stream;
        if(!stream.open(path, lcore::ios::in)){
            return false;
        }

        Pack tmp;
        if(!stream.read(tmp.header_)){
            return false;
        }

        tmp.entries_ = LNEW FileEntry[tmp.header_.numFiles_];

        s64 entrySize = tmp.header_.numFiles_ * sizeof(FileEntry);
        if(!stream.read(entrySize, tmp.entries_)){
            return false;
        }

        //Calcurate data size
        s64 dataTop = stream.tell();
        s64 dataSize = stream.size() - dataTop;
        stream.seek(dataTop, lcore::ios::beg);

        tmp.data_ = LNEW u8[dataSize];
        if(!stream.read(dataSize, tmp.data_)){
            return false;
        }
        pack.swap(tmp);
        return true;
    }
}

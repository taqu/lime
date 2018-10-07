#ifndef INC_LSOUND_PACKWRITER_H_
#define INC_LSOUND_PACKWRITER_H_
/**
@file PackWriter.h
@author t-sakai
@date 2012/03/20 create

*/
#include <lcore/File.h>
#include <lcore/Array.h>
#include "Pack.h"

namespace lsound
{
    class PackWriter
    {
    public:
        PackWriter();
        ~PackWriter();

        bool push_back(const Char* name, s64 size, const void* buffer);

        bool write(const Char* path);

        /**
        @brief Open a list file
        */
        bool openListFile(const Char* path);

    private:
        PackWriter(const PackWriter&) = delete;
        PackWriter(PackWriter&&) = delete;
        PackWriter& operator=(const PackWriter&) = delete;
        PackWriter& operator=(PackWriter&&) = delete;

        typedef lcore::Array<FileEntry> FileEntryArray;
        typedef lcore::Array<void*> MemPtrArray;

        lcore::File listStream_;
        PackHeader header_;
        FileEntryArray entries_;
        MemPtrArray mempts_;
    };
}
#endif //INC_LSOUND_PACKWRITER_H_

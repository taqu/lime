#ifndef INC_LSOUND_PACK_H_
#define INC_LSOUND_PACK_H_
/**
@file Pack.h
@author t-sakai
@date 2013/02/11 create
*/
#include "../lsound.h"

namespace lsound
{
    struct PackHeader
    {
        u32 numFiles_;
    };

    struct FileEntry
    {
        s64 size_;
        s64 offset_;
    };

    class Pack
    {
    public:
        Pack();
        ~Pack();

        /**
        @return Number of files
        */
        u32 getNumFiles() const;

        s64 getSize(u32 index) const;
        const u8* getData(u32 index) const;

        s64 getFileOffset(u32 index) const;

        void swap(Pack& rhs);
    private:
        Pack(const Pack&) = delete;
        Pack(Pack&&) = delete;
        Pack& operator=(const Pack&) = delete;
        Pack& operator=(Pack&&) = delete;

        friend class PackReader;

        PackHeader header_;
        FileEntry* entries_;
        s64 dataTopOffset_;
        u8* data_;
    };
}
#endif //INC_LSOUND_PACK_H_

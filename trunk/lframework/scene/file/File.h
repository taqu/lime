#ifndef INC_LSCENE_LFILE_FILE_H__
#define INC_LSCENE_LFILE_FILE_H__
/**
@file File.h
@author t-sakai
@date 2014/12/08 create
*/
#include "../lscene.h"
#include <lcore/liostream.h>

namespace lscene
{
namespace lfile
{
    class FileDesc;

    //------------------------------------------------
    //---
    //--- SharedFile
    //---
    //------------------------------------------------
    class SharedFile : public lcore::istream
    {
    public:
        SharedFile();
        SharedFile(const SharedFile& rhs);
        SharedFile(u64 id, FileDesc* file, s32 offset, s32 size);
        virtual ~SharedFile();

        inline u64 getID() const;
        inline bool is_open() const;
        inline s32 getSize() const;
        inline s32 tellg() const;
        inline void seekg(s32 pos);

        virtual lsize_t read(void* dst, u32 size);

        template<class T>
        inline s32 read(T* dst)
        {
            return read(dst, sizeof(T));
        }

        template<class T>
        inline s32 read(T& dst)
        {
            return read(&dst, sizeof(T));
        }

        void swap(SharedFile& rhs);
        SharedFile& operator=(const SharedFile& rhs);

        //--- istream interface
        virtual bool eof();
        virtual bool good();
        
        virtual bool seekg(s32 offset, int dir);
        virtual s32 tellg();
        
        virtual u32 getSize();
        virtual u32 getSize(s32 afterOffset);
    private:
        u64 id_;
        FileDesc* file_;
        s32 offset_;
        s32 size_;
        s32 current_;
    };

    inline u64 SharedFile::getID() const
    {
        return id_;
    }

    inline bool SharedFile::is_open() const
    {
        return (NULL != file_);
    }

    inline s32 SharedFile::getSize() const
    {
        return size_;
    }

    inline s32 SharedFile::tellg() const
    {
        return current_;
    }

    inline void SharedFile::seekg(s32 pos)
    {
        current_ = lcore::clamp(pos, 0, size_);
    }
}
}
#endif //INC_LSCENE_LFILE_FILE_H__

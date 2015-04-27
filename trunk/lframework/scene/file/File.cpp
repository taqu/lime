/**
@file File.cpp
@author t-sakai
@date 2014/12/08 create
*/
#include "FileSystem.h"

namespace lscene
{
namespace lfile
{
    //------------------------------------------------
    //---
    //--- SharedFile
    //---
    //------------------------------------------------
    SharedFile::SharedFile()
        :id_(0)
        ,file_(NULL)
        ,offset_(0)
        ,size_(0)
        ,current_(0)
    {
    }

    SharedFile::SharedFile(const SharedFile& rhs)
        :id_(rhs.id_)
        ,file_(rhs.file_)
        ,offset_(rhs.offset_)
        ,size_(rhs.size_)
        ,current_(rhs.current_)
    {
        if(file_){
            ++file_->refCount_;
        }
    }

    SharedFile::SharedFile(u64 id, FileDesc* file, s32 offset, s32 size)
        :id_(id)
        ,file_(file)
        ,offset_(offset)
        ,size_(size)
        ,current_(0)
    {
        if(file_){
            ++file_->refCount_;
        }
    }

    SharedFile::~SharedFile()
    {
        if(file_){
            if(--file_->refCount_==0){
                file_->~FileDesc();
                LSCENE_FREE(file_);
            }
            file_ = NULL;
        }
    }

    lsize_t SharedFile::read(void* dst, u32 size)
    {
        s32 current = current_ + size;
        if(size_<current){
            return 0;
        }

        file_->file_.seekg(current_+offset_, lcore::ios::beg);

        if(0<file_->file_.read(dst, size)){
            current_ = current;
            return 1;
        }
        return 0;
    }

    void SharedFile::swap(SharedFile& rhs)
    {
        lcore::swap(id_, rhs.id_);
        lcore::swap(file_, rhs.file_);
        lcore::swap(offset_, rhs.offset_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(current_, rhs.current_);
    }

    SharedFile& SharedFile::operator=(const SharedFile& rhs)
    {
        SharedFile(rhs).swap(*this);
        return *this;
    }

    bool SharedFile::eof()
    {
        return (size_<=current_);
    }

    bool SharedFile::good()
    {
        return (0<=current_ && current_<size_);
    }

    bool SharedFile::seekg(s32 offset, int dir)
    {
        switch(dir)
        {
        case lcore::ios::beg:
            {
                if(0<size_){
                    current_ = lcore::clamp(offset, 0, size_-1);
                }else{
                    current_ = 0;
                }
                return true;
            }
            break;
        case lcore::ios::cur:
            break;
        case lcore::ios::end:
            if(0<size_){
                current_ = lcore::clamp(offset, 0, size_-1);
            }else{
                current_ = 0;
            }
            return true;
            break;
        }
        return false;
    }

    s32 SharedFile::tellg()
    {
        return current_;
    }

    u32 SharedFile::getSize()
    {
        return size_;
    }

    u32 SharedFile::getSize(s32 afterOffset)
    {
        seekg(afterOffset, lcore::ios::beg);
        return size_;
    }
}
}

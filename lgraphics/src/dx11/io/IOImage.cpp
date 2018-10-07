/**
@file IOImage.cpp
@author t-sakai
@date 2018/10/06 create
*/
#include "io/IOImage.h"
#define SZLIB_IMPLEMENTATION
#include "io/szlib.h"
#define CPPIMG_IMPLEMENTATION
#include "io/cppimg.h"

namespace lgfx
{
    void rgb2rgba(s32 width, s32 height, u8* buffer)
    {
        s32 size = width*height;
        s32 src = 3*size-3;
        s32 dst = 4*size-4;
        for(s32 i = 0; i<size; ++i){
            buffer[dst+3] = 0xFFU;
            buffer[dst+2] = buffer[src+2];
            buffer[dst+1] = buffer[src+1];
            buffer[dst+0] = buffer[src+0];
            src -= 3;
            dst -= 4;
        }
    }

    void toLinearGray(s32 width, s32 height, u8* buffer)
    {
        s32 size = width*height;
        for(s32 i = 0; i<size; ++i){
            buffer[i] = lcore::toLinear(buffer[i]);
        }
    }

    //----------------------------------------------------
    //---
    //--- FStream
    //---
    //----------------------------------------------------
    FStream::FStream()
        :file_(NULL)
    {}

    FStream::FStream(lcore::File* file)
        :file_(file)
    {
        LASSERT(NULL != file_);
    }

    FStream::~FStream()
    {
        file_ = NULL;
    }

    bool FStream::valid() const
    {
        return CPPIMG_NULL != file_;
    }

    bool FStream::seek(cppimg::off_t pos, s32 whence)
    {
        LASSERT(NULL != file_);
        return file_->seek(pos, whence);
    }

    cppimg::off_t FStream::tell()
    {
        LASSERT(NULL != file_);
        return file_->tell();
    }

    s64 FStream::size()
    {
        LASSERT(NULL != file_);
        return file_->size();
    }

    s32 FStream::read(cppimg::size_t size, void* dst)
    {
        LASSERT(NULL != file_);
        return file_->read(static_cast<s64>(size), dst);
    }

    s32 FStream::write(cppimg::size_t size, const void* dst)
    {
        LASSERT(NULL != file_);
        return file_->write(static_cast<s64>(size), dst);
    }

    //----------------------------------------------------
    //---
    //--- ISStream
    //---
    //----------------------------------------------------
    ISStream::ISStream()
        :size_(0)
        ,current_(0)
        ,bytes_(CPPIMG_NULL)
    {}

    ISStream::ISStream(s64 size, const u8* bytes)
        :size_(size)
        ,current_(0)
        ,bytes_(bytes)
    {
        CPPIMG_ASSERT(0<=size_);
    }

    ISStream::~ISStream()
    {
        size_ = 0;
        current_ = 0;
        bytes_ = CPPIMG_NULL;
    }

    bool ISStream::valid() const
    {
        return CPPIMG_NULL != bytes_;
    }

    bool ISStream::seek(cppimg::off_t pos, s32 whence)
    {
        switch(whence)
        {
        case SEEK_SET:
            current_ = lcore::clamp(pos, 0LL, size_);
            break;
        case SEEK_CUR:
            current_ = lcore::clamp(current_+pos, 0LL, size_);
            break;
        case SEEK_END:
            current_ = lcore::clamp(size_+pos, 0LL, size_);
            break;
        default:
            return false;
        }
        return true;
    }

    cppimg::off_t ISStream::tell()
    {
        return current_;
    }

    s64 ISStream::size()
    {
        return size_;
    }

    s32 ISStream::read(size_t size, void* dst)
    {
        s64 end = lcore::clamp(current_+static_cast<s64>(size), 0LL, size_);
        u8* d = REINTERPRET_CAST(u8*, dst);
        s64 count = 0;
        for(; current_<end; ++count, ++current_){
            d[count] = bytes_[current_];
        }
        return static_cast<s64>(size)<=count? 1 : 0;
    }

    s32 ISStream::write(size_t , const void* )
    {
        return 0;
    }

    //----------------------------------------------------
    //---
    //--- OSStream
    //---
    //----------------------------------------------------
    OSStream::OSStream()
        :size_(0)
        ,current_(0)
        ,bytes_(CPPIMG_NULL)
    {}

    OSStream::OSStream(s64 size, u8* bytes)
        :size_(size)
        , current_(0)
        , bytes_(bytes)
    {
        CPPIMG_ASSERT(0<=size_);
    }

    OSStream::~OSStream()
    {
        size_ = 0;
        current_ = 0;
        bytes_ = CPPIMG_NULL;
    }

    bool OSStream::valid() const
    {
        return CPPIMG_NULL != bytes_;
    }

    bool OSStream::seek(cppimg::off_t pos, s32 whence)
    {
        switch(whence)
        {
        case SEEK_SET:
            current_ = lcore::clamp(pos, 0LL, size_);
            break;
        case SEEK_CUR:
            current_ = lcore::clamp(current_+pos, 0LL, size_);
            break;
        case SEEK_END:
            current_ = lcore::clamp(size_+pos, 0LL, size_);
            break;
        default:
            return false;
        }
        return true;
    }

    cppimg::off_t OSStream::tell()
    {
        return current_;
    }

    s64 OSStream::size()
    {
        return size_;
    }

    s32 OSStream::read(size_t , void* )
    {
        return 0;
    }

    s32 OSStream::write(size_t size, const void* dst)
    {
        s64 end = lcore::clamp(current_+static_cast<s64>(size), 0LL, size_);
        const u8* d = REINTERPRET_CAST(const u8*, dst);
        s64 count = 0;
        for(; current_<end; ++count, ++current_){
            bytes_[current_] = d[count];
        }
        return static_cast<s64>(size)<=count ? 1 : 0;
    }
}

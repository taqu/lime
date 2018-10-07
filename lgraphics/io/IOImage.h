#ifndef INC_LGRAPHICS_IOIMAGE_H_
#define INC_LGRAPHICS_IOIMAGE_H_
/**
@file IOImage.h
@author t-sakai
@date 2018/10/06 create
*/
#include "../lgraphics.h"
#include <lcore/File.h>
#include "cppimg.h"
#include "../Enumerations.h"

namespace lgfx
{
    using cppimg::ColorType;
    using cppimg::Stream;

    void rgb2rgba(s32 width, s32 height, u8* buffer);
    void toLinearGray(s32 width, s32 height, u8* buffer);

    template<class T>
    void transposeGray(s32 width, s32 height, T* bytes)
    {
        LASSERT(0<=width);
        LASSERT(0<=height);

        s32 step = width*4;
        s32 half = height>>1;
        for(s32 i=0; i<half; ++i){
            T* row0 = bytes + i*step;
            T* row1 = bytes + (height-i-1)*step;
            for(s32 j=0; j<step; ++j){
                lcore::swap(row0[j], row1[j]);
            }
        }
    }

    template<class T>
    void transposeRGBA(s32 width, s32 height, T* bytes)
    {
        LASSERT(0<=width);
        LASSERT(0<=height);

        s32 step = width*4;
        s32 half = height>>1;
        for(s32 i=0; i<half; ++i){
            T* row0 = bytes + i*step;
            T* row1 = bytes + (height-i-1)*step;
            for(s32 j=0; j<step; ++j){
                lcore::swap(row0[j], row1[j]);
            }
        }
    }

    //----------------------------------------------------
    //---
    //--- FStream
    //---
    //----------------------------------------------------
    class FStream : public Stream
    {
    public:
        FStream();
        explicit FStream(lcore::File* file);
        ~FStream();

        virtual bool valid() const;
        virtual bool seek(cppimg::off_t pos, s32 whence);
        virtual cppimg::off_t tell();
        virtual s64 size();

        virtual s32 read(cppimg::size_t size, void* dst);
        virtual s32 write(cppimg::size_t size, const void* dst);
    private:
        FStream(const FStream&) = delete;
        FStream(FStream&&) = delete;
        FStream& operator=(const FStream&) = delete;
        FStream& operator=(FStream&&) = delete;

        lcore::File* file_;
    };

    //----------------------------------------------------
    //---
    //--- ISStream
    //---
    //----------------------------------------------------
    class ISStream : public Stream
    {
    public:
        ISStream();
        ISStream(s64 size, const u8* bytes);
        ~ISStream();

        virtual bool valid() const;
        virtual bool seek(cppimg::off_t pos, s32 whence);
        virtual cppimg::off_t tell();
        virtual s64 size();

        virtual s32 read(size_t size, void* dst);
        virtual s32 write(size_t size, const void* dst);

        template<class T>
        bool read(T& x)
        {
            return 0<read(sizeof(T), &x);
        }
    private:
        ISStream(const ISStream&) = delete;
        ISStream(ISStream&&) = delete;
        ISStream& operator=(const ISStream&) = delete;
        ISStream& operator=(ISStream&&) = delete;

        s64 size_;
        s64 current_;
        const u8* bytes_;
    };

    //----------------------------------------------------
    //---
    //--- OSStream
    //---
    //----------------------------------------------------
    class OSStream : public Stream
    {
    public:
        OSStream();
        OSStream(s64 size, u8* bytes);
        ~OSStream();

        virtual bool valid() const;
        virtual bool seek(cppimg::off_t pos, s32 whence);
        virtual cppimg::off_t tell();
        virtual s64 size();

        virtual s32 read(size_t size, void* dst);
        virtual s32 write(size_t size, const void* dst);

        template<class T>
        bool write(const T& x)
        {
            return 0<write(sizeof(T), &x);
        }
    private:
        OSStream(const OSStream&) = delete;
        OSStream(OSStream&&) = delete;
        OSStream& operator=(const OSStream&) = delete;
        OSStream& operator=(OSStream&&) = delete;

        s64 size_;
        s64 current_;
        u8* bytes_;
    };
}
#endif //INC_LGRAPHICS_IOIMAGE_H_

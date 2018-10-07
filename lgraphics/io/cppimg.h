#ifndef INC_CPPIMG_H_
#define INC_CPPIMG_H_
/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/
/**
@file cppimg.h
@author t-sakai

USAGE:
Put '#define CPPIMG_IMPLEMENTATION' before including this file to create the implementation.
Put '#define CPPIMG_DISABLE_PNG' to disable support for PNG.
*/
#include <cassert>
#include <cstdint>
#include <utility>
#include <cstdio>
#include <sys/stat.h>
#include <cmath>

//#define CPPIMG_IMPLEMENTATION

#if !defined(CPPIMG_DISABLE_PNG) && !defined(CPPIMG_DISABLE_OPENEXR)

#ifdef CPPIMG_IMPLEMENTATION
#define SZLIB_IMPLEMENTATION
#endif

#include "szlib.h"
#endif

namespace cppimg
{

#ifndef CPPIMG_NULL
#ifdef __cplusplus
#   if 201103L<=__cplusplus || 1900<=_MSC_VER
#       define CPPIMG_CPP11 1
#   endif
#endif

#ifdef __cplusplus
#   ifdef CPPIMG_CPP11
#       define CPPIMG_NULL nullptr
#   else
#       define CPPIMG_NULL 0
#   endif
#else
#   define CPPIMG_NULL (void*)0
#endif
#endif

#ifndef CPPIMG_TYPES
#define CPPIMG_TYPES
    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef float f32;
    typedef double f64;

    typedef char Char;

    typedef ::size_t size_t;
#endif

#ifndef CPPIMG_NULL
#ifdef __cplusplus
#   if 201103L<=__cplusplus || 1900<=_MSC_VER
#       define CPPIMG_CPP11 1
#   endif
#endif

#ifdef __cplusplus
#   ifdef CPPIMG_CPP11
#       define CPPIMG_NULL nullptr
#   else
#       define CPPIMG_NULL 0
#   endif
#else
#   define CPPIMG_NULL (void*)0
#endif
#endif

#ifndef CPPIMG_FUNC
#define CPPIMG_FUNC
    using std::move;

#ifndef CPPIMG_ASSERT
#define CPPIMG_ASSERT(exp) assert(exp)
#endif

#ifdef _MSC_VER

#ifndef CPPIMG_OFF_T
#define CPPIMG_OFF_T
    typedef s64 off_t;
#endif

#ifndef CPPIMG_FSEEK
#define CPPIMG_FSEEK(f,p,o) _fseeki64((f),(p),(o))
#endif

#ifndef CPPIMG_FTELL
#define CPPIMG_FTELL(f) _ftelli64((f))
#endif

#ifndef CPPIMG_SPRINTF
#define CPPIMG_SPRINTF(b,f, ...) sprintf_s(b, f, __VA_ARGS__)
#endif

    //----------------------------------------------------------------
    inline s64 CPPIMG_FSIZE(FILE* file)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file);
        struct _stat64 stat;
        return (0 == _fstat64(_fileno(file), &stat))? stat.st_size : 0;
    }

    inline FILE* CPPIMG_FOPEN(const Char* filepath, const Char* mode)
    {
        FILE* file = CPPIMG_NULL;
        return 0 == ::fopen_s(&file, filepath, mode) ? file : CPPIMG_NULL;
    }

    inline void CPPIMG_FCLOSE(FILE*& file)
    {
        if(CPPIMG_NULL != file){
            fclose(file);
            file = CPPIMG_NULL;
        }
    }

#else
#ifndef CPPIMG_OFF_T
#define CPPIMG_OFF_T
    typedef s64 off_t;
#endif

#ifndef CPPIMG_FSEEK
#define CPPIMG_FSEEK(f,p,o) fseeko64((f),(p),(o))
#endif

#ifndef CPPIMG_FTELL
#define CPPIMG_FTELL(f) ftello64((f))
#endif

#ifndef CPPIMG_SPRINTF
#define CPPIMG_SPRINTF(b,f, ...) sprintf(b, f, __VA_ARGS__)
#endif

    inline s64 CPPIMG_FSIZE(FILE* file)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file);
        struct stat64 stat;
        return (0 == fstat64(fileno(file), &stat))? stat.st_size : 0;
    }

    inline FILE* CPPIMG_FOPEN(const Char* filepath, const Char* mode)
    {
        return fopen(filepath, mode);
    }

    inline void CPPIMG_FCLOSE(FILE*& file)
    {
        if(CPPIMG_NULL != file){
            fclose(file);
            file = CPPIMG_NULL;
        }
    }
#endif

#ifndef CPPIMG_MALLOC
#define CPPIMG_MALLOC(size) malloc(size)
#endif

#ifndef CPPIMG_FREE
#define CPPIMG_FREE(ptr) free(ptr); (ptr) = CPPIMG_NULL
#endif

#ifndef CPPIMG_NEW
#define CPPIMG_NEW new
#endif

#ifndef CPPIMG_PLACEMENT_NEW
#define CPPIMG_PLACEMENT_NEW(ptr) new(ptr)
#endif

#ifndef CPPIMG_DELETE
#define CPPIMG_DELETE(ptr) delete (ptr); (ptr) = CPPIMG_NULL
#endif

#ifndef CPPIMG_DELETE_ARRAY
#define CPPIMG_DELETE_ARRAY(ptr) delete[] (ptr); (ptr) = CPPIMG_NULL
#endif

#ifndef CPPIMG_MEMSET
#define CPPIMG_MEMSET(ptr, value, size) memset((ptr), (value), (size))
#endif

    template<class T>
    inline T absolute(T x)
    {
        return 0<=x? x : -x;
    }

    template<class T>
    inline T minimum(T x0, T x1)
    {
        return x0<x1? x0 : x1;
    }

    template<class T>
    inline T maximum(T x0, T x1)
    {
        return x0<x1? x1 : x0;
    }

    template<class T>
    inline T clamp(T x, T minv, T maxv)
    {
        x = maximum(x, minv);
        return minimum(x, maxv);
    }

    template<class T>
    inline void swap(T& x0, T& x1)
    {
        T t = cppimg::move(x0);
        x0 = cppimg::move(x1);
        x1 = cppimg::move(t);
    }
#endif


    inline s32 fixed32_construct(f32 x, s32 shift)
    {
        x = x*(f32)(0x01U<<shift);
        return (s32)x;
    }

    union UnionS32F32
    {
        s32 s32_;
        f32 f32_;
    };

    union UnionU32F32
    {
        u32 u32_;
        f32 f32_;
    };

    /**
    @brief clamp float value [0 1]
    */
    f32 clamp01(f32 x);

    /**
    @brief Convert IEEE 754 single precision float to half precision float
    */
    u16 toFloat16(f32 f);
    /**
    @brief Convert IEEE 754 half precision float to single precision float
    */
    f32 toFloat32(u16 h);

    static const Char CPPIMG_NULLCHAR = '\0';
    static const u32 MaxWidth = 0x7FFFFFFFU;
    static const u32 MaxHeight = 0x7FFFFFFFU;
    static const s32 MaxChannels = 4;

    enum ColorType
    {
        ColorType_GRAY,
        ColorType_RGB,
        ColorType_RGBA,
    };

    enum Type
    {
        Type_UINT = 0,
        Type_HALF = 1,
        Type_FLOAT = 2,
    };

    enum Channel
    {
        Channel_R =0,
        Channel_G,
        Channel_B,
        Channel_A,
        Channel_Y,
        Channel_Num,
    };

    extern const Char* ChannelNames[Channel_Num];

    inline s32 getBytesPerPixel(ColorType colorType)
    {
        switch(colorType)
        {
        case ColorType_GRAY:
            return 1;
        case ColorType_RGB:
            return 3;
        case ColorType_RGBA:
        default:
            return 4;
        };
    }

    s32 getBytesPerPixel(s32 channels, const s32* types);
    s32 getSize(Type type);
    s32 getNumChannels(ColorType type);

    struct F32ToF16
    {
        u16 operator()(f32 x) const
        {
            return toFloat16(x);
        }
    };

    struct F16ToF32
    {
        static f32 convert(u16 x)
        {
            return toFloat32(x);
        }

        f32 operator()(u16 x) const
        {
            return toFloat32(x);
        }
    };

    struct F32ToU32
    {
        u32 operator()(f32 x) const
        {
            return static_cast<u32>(0xFFFFFFFFU*clamp01(x));
        }
    };

    struct U32ToF32
    {
        static f32 convert(u32 x)
        {
            constexpr f32 inv = 1.0/static_cast<f32>(0xFFFFFFFFU);
            return static_cast<f32>(x) * inv;
        }

        f32 operator()(u32 x) const
        {
            constexpr f32 inv = 1.0/static_cast<f32>(0xFFFFFFFFU);
            return static_cast<f32>(x) * inv;
        }
    };

    struct F16ToU32
    {
        u32 operator()(u16 x) const
        {
            return static_cast<u32>(0xFFFFFFFFU*clamp01(toFloat32(x)));
        }
    };

    struct U32ToF16
    {
        u16 operator()(u32 x) const
        {
            constexpr f32 inv = 1.0/static_cast<f32>(0xFFFFFFFFU);
            return toFloat16(static_cast<f32>(x)*inv);
        }
    };


    template<class T, class U, class V>
    void convertType(s32 width, s32 height, s32 channels, T* dst, const U* src, V func)
    {
        CPPIMG_ASSERT(0<=width);
        CPPIMG_ASSERT(0<=height);
        CPPIMG_ASSERT(0<=channels);
        CPPIMG_ASSERT(CPPIMG_NULL != dst);
        CPPIMG_ASSERT(CPPIMG_NULL != src);

        T* dr = dst;
        const U* sr = src;
        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                s32 c = j*channels;
                for(s32 k=0; k<channels; ++k){
                    dst[c+k] = func(src[c+k]);
                }
            }
            dr += width;
            sr += width;
        }
    }

    /**
    */
    inline void convertF32ToF16(s32 width, s32 height, s32 channels, u16* dst, const f32* src)
    {
        convertType(width, height, channels, dst, src, F32ToF16());
    }
    /**
    */
    inline void convertF16ToF32(s32 width, s32 height, s32 channels, f32* dst, const u16* src)
    {
        convertType(width, height, channels, dst, src, F16ToF32());
    }

    /**
    */
    inline void convertF32ToU32(s32 width, s32 height, s32 channels, u32* dst, const f32* src)
    {
        convertType(width, height, channels, dst, src, F32ToU32());
    }
    /**
    */
    inline void convertU32ToF32(s32 width, s32 height, s32 channels, f32* dst, const u32* src)
    {
        convertType(width, height, channels, dst, src, U32ToF32());
    }

    /**
    */
    inline void convertF16ToU32(s32 width, s32 height, s32 channels, u32* dst, const u16* src)
    {
        convertType(width, height, channels, dst, src, F16ToU32());
    }
    /**
    */
    inline void convertU32ToF16(s32 width, s32 height, s32 channels, u16* dst, const u32* src)
    {
        convertType(width, height, channels, dst, src, U32ToF16());
    }


    //----------------------------------------------------------------
    struct F32ToU8
    {
        static u8 convert(f32 x)
        {
            return minimum(static_cast<u8>(clamp01(x)*0xFFU), static_cast<u8>(0xFFU));
        }

        u8 operator()(f32 x) const
        {
            return F32ToU8::convert(x);
        }
    };

    struct U32ToU8
    {
        u8 operator()(u32 x) const
        {
            return F32ToU8::convert(U32ToF32::convert(x));
        }
    };

    struct F16ToU8
    {
        u8 operator()(u16 x) const
        {
            return F32ToU8::convert(F16ToF32::convert(x));
        }
    };

    void convert(s32 width, s32 height, s32 channels, u8* dst, const void* src, const s32* types);


    //----------------------------------------------------
    //---
    //--- Auto
    //---
    //----------------------------------------------------
    template<class T>
    class Auto
    {
    public:
        Auto()
            :pointer_(CPPIMG_NULL)
        {}

        explicit Auto(T* pointer)
            :pointer_(pointer)
        {}

        ~Auto()
        {
            CPPIMG_DELETE(pointer_);
        }

        const T* operator->() const
        {
            return pointer_;
        }

        T* operator->()
        {
            return pointer_;
        }
    private:
        Auto& operator=(const Auto&) = delete;
        Auto& operator=(Auto&&) = delete;

        T* pointer_;
    };

    //----------------------------------------------------
    //---
    //--- Stream
    //---
    //----------------------------------------------------
    class Stream
    {
    public:
        virtual bool valid() const =0;
        virtual bool seek(off_t pos, s32 whence) =0;
        virtual off_t tell() =0;
        virtual s64 size() =0;

        virtual s32 read(size_t size, void* dst) =0;
        virtual s32 write(size_t size, const void* dst) =0;
    protected:
        Stream(){}
        ~Stream(){}
    private:
        Stream(const Stream&) = delete;
        Stream& operator=(const Stream&) = delete;
        Stream& operator=(Stream&&) = delete;
    };

    //----------------------------------------------------
    //---
    //--- FStream
    //---
    //----------------------------------------------------
    template<class T>
    class FStream : public T
    {
    public:
        void close();

        virtual bool valid() const;
        virtual bool seek(off_t pos, s32 whence);
        virtual off_t tell();
        virtual s64 size();
    protected:
        FStream(const FStream&) = delete;
        FStream& operator=(const FStream&) = delete;
        FStream& operator=(FStream&&) = delete;

        bool open(const Char* filepath, const Char* mode);

        FStream();
        FStream(FStream&& rhs);
        ~FStream();

        FILE* file_;
    };

    template<class T>
    FStream<T>::FStream()
        :file_(CPPIMG_NULL)
    {}

    template<class T>
    FStream<T>::FStream(FStream&& rhs)
        :file_(rhs.file_)
    {
        rhs.file_ = CPPIMG_NULL;
    }

    template<class T>
    FStream<T>::~FStream()
    {
        close();
    }

    template<class T>
    bool FStream<T>::open(const Char* filepath, const Char* mode)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != filepath);
        CPPIMG_ASSERT(CPPIMG_NULL != mode);
        FILE* file = CPPIMG_FOPEN(filepath, mode);
        if(CPPIMG_NULL == file){
            return false;
        }
        close();
        file_ = file;
        return true;
    }

    template<class T>
    void FStream<T>::close()
    {
        if(CPPIMG_NULL != file_){
            fclose(file_);
            file_ = CPPIMG_NULL;
        }
    }

    template<class T>
    bool FStream<T>::valid() const
    {
        return CPPIMG_NULL != file_;
    }

    template<class T>
    bool FStream<T>::seek(off_t pos, s32 whence)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file_);
        return 0<=CPPIMG_FSEEK(file_, pos, whence);
    }

    template<class T>
    off_t FStream<T>::tell()
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file_);
        return CPPIMG_FTELL(file_);
    }

    template<class T>
    s64 FStream<T>::size()
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file_);
        return CPPIMG_FSIZE(file_);
    }

    //----------------------------------------------------
    //---
    //--- IFStream
    //---
    //----------------------------------------------------
    class IFStream : public FStream<Stream>
    {
    public:
        typedef FStream<Stream> parent_type;

        IFStream()
        {}
        IFStream(IFStream&& rhs)
            :parent_type(cppimg::move(rhs))
        {}
        ~IFStream()
        {
            close();
        }

        bool open(const Char* filepath)
        {
            return parent_type::open(filepath, "rb");
        }

        virtual s32 read(size_t size, void* dst);
        virtual s32 write(size_t ,const void*){ return 0;}

        IFStream& operator=(IFStream&& rhs);
    private:
        IFStream(const IFStream&) = delete;
        IFStream& operator=(const IFStream&) = delete;
    };

    //----------------------------------------------------
    //---
    //--- OFStream
    //---
    //----------------------------------------------------
    class OFStream : public FStream<Stream>
    {
    public:
        typedef FStream<Stream> parent_type;

        OFStream()
        {}
        OFStream(OFStream&& rhs)
            :parent_type(cppimg::move(rhs))
        {}
        ~OFStream()
        {
            close();
        }

        bool open(const Char* filepath)
        {
            return parent_type::open(filepath, "wb");
        }

        virtual s32 read(size_t, void*){ return 0;}
        virtual s32 write(size_t size, const void* dst);

        OFStream& operator=(OFStream&& rhs);
    private:
        OFStream(const OFStream&) = delete;
        OFStream& operator=(const OFStream&) = delete;
    };

    //----------------------------------------------------
    //---
    //--- BMP
    //---
    //----------------------------------------------------
    class BMP
    {
    public:
        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool read(s32& width, s32& height, ColorType& colorType, void* image, Stream& stream);

        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool write(Stream& stream, s32 width, s32 height, ColorType colorType, const void* image);

    private:
        static const u16 MAGIC = 0x4D42U;//'MB';

        static const u32 Compression_RGB = 0;
        static const u32 Compression_RLE8 = 1;
        static const u32 Compression_RLE4 = 2;
        static const u32 Compression_BitFields = 3;
        static const u32 Compression_JPEG = 4;
        static const u32 Compression_PNG = 5;

        typedef u32 FXPT2DOT30;
        struct CIEXYZ
        {
            FXPT2DOT30 x_;
            FXPT2DOT30 y_;
            FXPT2DOT30 z_;
        };

        struct CIEXYZTRIPLE
        {
            CIEXYZ r_;
            CIEXYZ g_;
            CIEXYZ b_;
        };

        struct HEADER
        {
            //u16 type_; /// magic number
            u32 size_; /// file size
            u16 reserve1_;
            u16 reserve2_;
            u32 offset_; /// offset from file head to data

        };

        struct INFOHEADER
        {
            u32 infoSize_;
            s32 width_;
            s32 height_;
            u16 planes_;
            u16 bitCount_;
            u32 compression_;
            u32 sizeImage_;
            s32 xPixPerMeter_;
            s32 yPixPerMeter_;
            u32 colorUsed_;
            u32 colorImportant_;
        };

        struct INFOHEADERV4 : public INFOHEADER
        {
            u32 maskR_;
            u32 maskG_;
            u32 maskB_;
            u32 maskA_;
            u32 colorSpace_;
            CIEXYZTRIPLE endpoints_;
            u32 gammaR_;
            u32 gammaG_;
            u32 gammaB_;
        };

        struct INFOHEADERV5 : public INFOHEADERV4
        {
            u32 intent_;
            u32 profileData_;
            u32 profileSize_;
            u32 reserved_;
        };

        static bool read24(u8* image, s32 width, s32 height, bool leftBottom, Stream& stream);
        static bool read32(u8* image, s32 width, s32 height, bool leftBottom, Stream& stream);
    };

    //----------------------------------------------------
    //---
    //--- TGA
    //---
    //----------------------------------------------------
    class TGA
    {
    public:
        static const s32 Option_None = 0;
        static const s32 Option_Compress = (0x01<<0);

        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool read(s32& width, s32& height, ColorType& colorType, void* image, Stream& stream);

        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool write(Stream& stream, s32 width, s32 height, ColorType colorType, const void* image, s32 options = Option_Compress);

    private:
        enum Type
        {
            Type_None = 0x00U,
            Type_IndexColor = 0x01U,
            Type_FullColor = 0x02U,
            Type_Gray = 0x03U,
            Type_IndexColorRLE = 0x09U,
            Type_FullColorRLE = 0x0AU,
            Type_GrayRLE = 0x0BU,
        };

        enum Offset
        {
            Offset_IDLeng = 0,
            Offset_ColorMapType,
            Offset_ImageType,

            Offset_ColorMapOriginL,
            Offset_ColorMapOriginH,

            Offset_ColorMapLengL,
            Offset_ColorMapLengH,

            Offset_ColorMapSize,

            Offset_OriginXL,
            Offset_OriginXH,
            Offset_OriginYL,
            Offset_OriginYH,

            Offset_WidthL,
            Offset_WidthH,
            Offset_HeightL,
            Offset_HeightH,
            Offset_BitPerPixel,
            Offset_Discripter,
        };
        static const u32 TGA_HEADER_SIZE = 18;

        static inline u16 getU16(u8 low, u8 high)
        {
            return (static_cast<u16>(high)<<8) + low;
        }
        static inline bool check(u8 bpp, const u8* x0, const u8* x1)
        {
            for(u8 i=0; i<bpp; ++i){
                if(x0[i] != x1[i]){
                    return false;
                }
            }
            return true;
        }

        static bool readInternal(s32 width, s32 height, u8 bpp, u8* image, Stream& stream);
        static bool readRLEInternal(s32 width, s32 height, u8 bpp, u8* image, Stream& stream);
        static u8 calcRunLength(u8 bpp, s32 x, s32 y, s32 width, const u8* image);
        static bool writeUncompress(Stream& stream, s32 width, s32 height, u8 bpp, const u8* image);
        static bool writeRLE(Stream& stream, s32 width, s32 height, u8 bpp, const u8* image);
    };

    //----------------------------------------------------
    //---
    //--- PPM
    //---
    //----------------------------------------------------
    class PPM
    {
    public:
        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool write(Stream& stream, s32 width, s32 height, ColorType colorType, const void* image);
    };

#if !defined(CPPIMG_DISABLE_PNG)
    //----------------------------------------------------
    //---
    //--- PNG
    //---
    //----------------------------------------------------
    class PNG
    {
    public:
        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool read(s32& width, s32& height, ColorType& colorType, void* image, Stream& stream);

#if 0
        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool write(Stream& stream, s32 width, s32 height, ColorType colorType, const void* image);
#endif
    private:
        static const u64 Signature = 0x0A1A0A0D474E5089U;

        static const u32 ColorType_Gray = 0;
        static const u32 ColorType_True = 2;
        static const u32 ColorType_Index = 3;
        static const u32 ColorType_GrayAlpha = 4;
        static const u32 ColorType_TrueAlpha = 6;

        static const u8 FilterType_None = 0;
        static const u8 FilterType_Sub = 1;
        static const u8 FilterType_Up = 2;
        static const u8 FilterType_Avg = 3;
        static const u8 FilterType_Paeth = 4;

        struct Chunk
        {
            u32 length_;
            u32 type_;
        };

        struct ChunkIHDR : public Chunk
        {
            static const u32 Type = 0x52444849U;//'RDHI';
            static const u32 Size = 13;
            bool read(Stream& stream);

            u32 width_;
            u32 height_;
            u8 bitDepth_;
            u8 colorType_;
            u8 compression_;
            u8 filter_;
            u8 interlace_;
        };

        struct ChunkPLTE : public Chunk
        {
            static const u32 Type = 0x45544C50U;//'ETLP';
            static const u32 MaxSize = 256;
            bool read(Stream& stream);

            u32 size_;
            u8 r_[MaxSize];
            u8 g_[MaxSize];
            u8 b_[MaxSize];
        };

        struct ChunkIDAT : public Chunk
        {
            static const u32 Type = 0x54414449U;//'TADI';
            static const u32 BufferSize = 1024;

            ChunkIDAT(u32 totalSrcSize, u32 width, u32 height, s32 color, s32 alpha);
            ~ChunkIDAT();

            bool initialize();
            bool terminate();
            bool read(Stream& stream);
            bool decode(void* image);
            void filter(u32 scanlineSize, s32 filterFlag, u8* scanline, u8* image);

            u8* src_;
            u32 totalSrcSize_;
            u32 srcSize_;
            u32 totalSize_;
            u32 totalCount_;
            u32 color_;
            u32 alpha_;
            u32 width_;
        };

        struct ChunkIEND : public Chunk
        {
            static const u32 Type = 0x444E4549U;//'DNEI';
        };

        static inline u16 reverse(u16 x);
        static inline u32 reverse(u32 x);
        static bool readHeader(Stream& stream);
        static bool checkCRC32(const Chunk& chunk, Stream& stream);
        static bool skipChunk(const Chunk& chunk, Stream& stream);

        template<class T>
        static void setChunkHeader(T& dst, const Chunk& src);

        template<class T>
        static bool readHeader(T& chunk, Stream& stream);
        static bool writeChunk(Stream& stream, u32 type, u32 size, const void* data);
    };
#endif

    //----------------------------------------------------
    //---
    //--- JPEG
    //---
    //----------------------------------------------------
    class JPEG
    {
    public:
        static const s32 FIXED_POINT_SHIFT = 12;
        static const s32 FIXED_POINT_SHIFT2 = 6;

        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        static bool read(s32& width, s32& height, ColorType& colorType, void* image, Stream& stream);

        /**
        @brief
        @return Success:true, Fail:false
        @param width
        @param height
        @param image
        @param colorType
        @param stream
        */
        //static bool write(Stream& stream, s32 width, s32 height, ColorType colorType, const void* image);
    private:
        static const s32 QT_SIZE = 64;
        static const s32 QT_NUM = 4;
        static const s32 HT_NUM = 4;
        static const s32 HT_CLASS = 2;
        static const s32 HT_BITS_TABLE = 16;
        static const s32 HT_MAX_SIZE = 256;
        static const s32 FRAME_NUM = 16;
        static const s32 BLOCK_WIDTH = 8;
        static const s32 BLOCK_SIZE = BLOCK_WIDTH*BLOCK_WIDTH;
        static const s32 BLOCK_SHIFT = 3;
        static const s32 MAX_COMPONENTS = 4;
        static const s32 MAX_COMPONENT_SIZE = MAX_COMPONENTS*BLOCK_SIZE;

        static const s32 Flag_SOI = 0x01<<0;
        static const s32 Flag_DQT = 0x01<<1;
        static const s32 Flag_DHT = 0x01<<2;
        static const s32 Flag_SOS = 0x01<<3;
        static const s32 Flag_NEEDS = (Flag_SOI | Flag_DQT | Flag_DHT | Flag_SOS);

        static inline s32 blocks(s32 size)
        {
            return (size+0x07)>>3;
        }

        struct Segment;

        struct ByteStream
        {
            ByteStream()
                :bits_(0)
                ,byte_(0)
                ,stream_(CPPIMG_NULL)
            {}

            ByteStream(Stream* stream)
                :bits_(0)
                ,byte_(0)
                ,stream_(stream)
            {}

            inline void reset();
            inline s32 read(size_t size, void* bytes);
            inline s32 write(size_t size, void* bytes);
            inline bool skip(size_t size);

            s32 readByte();

            s32 readBits(s32 bits);
            s32 writeBits(s32 bits, s32 value);
            s32 fillByte();

            bool readSegment(Segment& segment);
            bool readMarker(u8& marker);
            bool read16(u16& x);

            inline bool skipSegment(const Segment& segment);

            s32 bits_;
            u8 byte_;
            Stream* stream_;
        };

        struct Segment
        {
            u8 ff_;
            u8 marker_;
            u16 length_;
        };

        struct QuantizationTable
        {
            inline s32 getPrecision() const
            {
                return (precisionAndNumber_>>4) & 0xFU;
            }

            inline s32 getNumber() const
            {
                return precisionAndNumber_ & 0xFU;
            }

            u8 precisionAndNumber_; ///< upper:precision, lower:number
            u16 factors_[QT_SIZE];
        };

        struct HuffmanTable
        {
            //s8 class_; ///< table class 0=DC, 1=AC
            //s8 id_; ///< table number 0-3
            s32 number_;
            u16 code_[HT_MAX_SIZE];
            u8 size_[HT_MAX_SIZE];
            u8 value_[HT_MAX_SIZE];
        };

        struct Component
        {
            /**
            @brief Horizontal sampling factor from 1 to 4
            */
            inline u8 getHorizontal() const
            {
                return (sampling_>>4) & 0x0FU;
            }

            /**
            @brief Vertical sampling factor from 1 to 4
            */
            inline u8 getVertical() const
            {
                return sampling_ & 0x0FU;
            }

            u8 id_; ///< component identifier 0-255
            u8 sampling_; ///< sampling factor 1-4
            u8 quantizationTable_; ///< quantization table destination selector 0-4
        };

        struct FrameHeader
        {
            bool getMaxSampling();

            u16 height_; ///< number of lines
            u16 width_; ///< number of samples per line
            u8 precision_; ///< sample precision
            u8 numComponents_; ///< number of image components, gray=1, 3=YCbCr or YIQ, 4=CMYK
            u8 maxVerticalSampling_;
            u8 maxHorizontalSampling_;
            Component components_[MAX_COMPONENTS];
        };

        struct Scan
        {
            struct Component
            {
                inline s32 getDCHuffman() const
                {
                    return (huffman_>>4) & 0xFU;
                }

                inline s32 getACHuffman() const
                {
                    return huffman_ & 0xFU;
                }

                u8 id_; ///< component identifier
                u8 huffman_;
            };

            inline s32 getApproxBitPositionHigh() const
            {
                return (pointTransform_>>4) & 0xFU;
            }

            inline s32 getApproxBitPositionLow() const
            {
                return pointTransform_ & 0xFU;
            }

            u8 numComponents_;
            Component components_[MAX_COMPONENTS];
            u8 startSpectra_; ///< start of spectral selection
            u8 endSpectra_; ///< end of spectral selection
            u8 pointTransform_;
        };

        struct JFXX
        {
            struct IF
            {
                u16 version_;
                u8 unit_;
                u16 xdensity_;
                u16 ydensity_;
                u8 xthumb_;
                u8 ythumb_;
            };

            struct XX
            {
                u8 type_; ///< 10:jpeg, 11:256 pallet, 12:rgb bitmap
            };

            u8 id_[5];
            union Format
            {
                IF jfif_;
                XX jfxx_;
            };
            Format format_;
        };

        struct Context
        {
            void createCosTable()
            {
                const f32 isqrt2 = 0.70710678118f;
                const f32 pi = 3.14159265359f;
                for(s32 i=0; i<BLOCK_WIDTH; ++i){
                    cosine_[i][0] = static_cast<s16>(isqrt2 * (f32)(1<<FIXED_POINT_SHIFT));
                    for(s32 j=1; j<BLOCK_WIDTH; ++j){
                        f32 f = cosf( ((i*2 + 1)*j) * (pi/16.0f) );
                        cosine_[i][j] = static_cast<s16>(f * (f32)(1<<FIXED_POINT_SHIFT));
                    }
                }
            }

            s32 flags_;
            QuantizationTable quantization_[QT_NUM]; ///< quantization table
            HuffmanTable huffman_[HT_CLASS][HT_NUM]; ///< huffman table
            u16 restartInterval_; ///< restart interval
            u16 numberOfLines_; ///< number of lines
            FrameHeader frame_;
            Scan scan_;
            JFXX jfxx_;
            s16 cosine_[BLOCK_WIDTH][BLOCK_WIDTH];

            s16 components_[MAX_COMPONENTS][MAX_COMPONENT_SIZE];
            s16 directCurrents_[MAX_COMPONENTS];
            s16 block_[BLOCK_SIZE];
            s16 dct_[BLOCK_SIZE];

            ByteStream byteStream_;
            s16* work_;
            u8* rgb_;
        };

        class AutoFree
        {
        public:
            AutoFree(Context* pointer)
                :pointer_(pointer)
            {}

            ~AutoFree()
            {
                if(CPPIMG_NULL != pointer_){
                    CPPIMG_FREE(pointer_->work_);
                    CPPIMG_FREE(pointer_);
                }
            }

        private:
            Context* pointer_;
        };

        static const u8 MARKER_SOI = 0xD8U;
        static const u8 MARKER_EOI = 0xD9U;

        static const u8 MARKER_DQT  = 0xDBU;
        static const u8 MARKER_DHT  = 0xC4U;
        static const u8 MARKER_DRI = 0xDDU;
        static const u8 MARKER_DNL = 0xDCU;

        static const u8 MARKER_SOF0 = 0xC0U;
        static const u8 MARKER_SOF1 = 0xC1U;
        static const u8 MARKER_SOF2 = 0xC2U;
        static const u8 MARKER_SOF3 = 0xC3U;
        static const u8 MARKER_SOF5 = 0xC5U;
        static const u8 MARKER_SOF6 = 0xC6U;
        static const u8 MARKER_SOF7 = 0xC7U;
        static const u8 MARKER_SOF9 = 0xC9U;
        static const u8 MARKER_SOFA = 0xCAU;
        static const u8 MARKER_SOFB = 0xCBU;
        static const u8 MARKER_SOFD = 0xCDU;
        static const u8 MARKER_SOFE = 0xCEU;
        static const u8 MARKER_SOFF = 0xCFU;

        static const u8 MARKER_SOS  = 0xDAU;

        static const u8 MARKER_APP0 = 0xE0U;

        //
        //static const u8 MARKER_COM   = 0xFEU;
        //static const u8 MARKER_APP00 = 0xE0U;
        //static const u8 MARKER_APP01 = 0xE1U;
        //static const u8 MARKER_APP02 = 0xE2U;
        //static const u8 MARKER_APP03 = 0xE3U;
        //static const u8 MARKER_APP04 = 0xE4U;
        //static const u8 MARKER_APP05 = 0xE5U;
        //static const u8 MARKER_APP06 = 0xE6U;
        //static const u8 MARKER_APP07 = 0xE7U;
        //static const u8 MARKER_APP08 = 0xE8U;
        //static const u8 MARKER_APP09 = 0xE9U;
        //static const u8 MARKER_APP10 = 0xEAU;
        //static const u8 MARKER_APP11 = 0xEBU;
        //static const u8 MARKER_APP12 = 0xECU;
        //static const u8 MARKER_APP13 = 0xEDU;
        //static const u8 MARKER_APP14 = 0xEEU;
        //static const u8 MARKER_APP15 = 0xEFU;

        //static const u8 MARKER_APP00_JFIF = MARKER_APP00;
        //static const u8 MARKER_APP01_EXIF = MARKER_APP01;
        //static const u8 MARKER_APP02_ICC = MARKER_APP02;
        //static const u8 MARKER_APP14_ADOBE = MARKER_APP14;

        static const u8 MARKER_RST0 = 0xD0U;
        static const u8 MARKER_RST1 = 0xD1U;
        static const u8 MARKER_RST2 = 0xD2U;
        static const u8 MARKER_RST3 = 0xD3U;
        static const u8 MARKER_RST4 = 0xD4U;
        static const u8 MARKER_RST5 = 0xD5U;
        static const u8 MARKER_RST6 = 0xD6U;
        static const u8 MARKER_RST7 = 0xD7U;

        static const u8 ZigZag[BLOCK_SIZE];

        /**
        @biref big endian
        */
        static inline u16 toU16B(u8 x0, u8 x1)
        {
            return (x0<<8) | x1;
        }

        static inline u16 swapEndian(u16 x)
        {
            return (x>>8) | (x<<8);
        }

        static inline s32 fixedPointShift(s32 x, s32 shift)
        {
            return x >> shift;
        }

        template<typename T=s32>
        static inline T fixedPointRound(s32 x, s32 shift)
        {
            return static_cast<T>((x + ((1<<shift)-1)) >> shift);
        }

        static inline u8 clamp8bits(s32 x)
        {
            static const u8 u0 = 0;
            static const u8 u255 = 255;
            return 0<=x? ((x<=255)? static_cast<u8>(x) : u255) : u0;
        }

        /**
        @brief Read Define Quantization Table
        */
        static bool readDQT(Context& context, const Segment& segment);
        /**
        @brief Read Define Huffman Table
        */
        static bool readDHT(Context& context, const Segment& segment);
        /**
        @brief Read Define Restart Interval
        */

        static bool readDRI(Context& context, const Segment& segment);
        /**
        @brief Read Define Number of Lines
        */

        static bool readDNL(Context& context, const Segment& segment);
        /**
        @brief Read Start Of Frame
        */

        static bool readSOF(Context& context, const Segment& segment);
        /**
        @brief Read Start Of Scan
        */
        static bool readSOS(Context& context, const Segment& segment);

        static bool readJFXX(Context& context, const Segment& segment);

        static bool decode(Context& context);
        static bool decodeMCU(Context& context);
        static s32 decodeHuffmanCode(Context& context, s32 type, s32 table);
        static bool decodeHuffmanBlock(Context& context, s32 component);
        static void inverseQuantization(Context& context, s32 component);
        static void inverseDCT(Context& context);
        static void copyComponents(Context& context, s32 ux, s32 uy);
        static void toGray(Context& context);
        static void toRGB(Context& context);
        static void filter(Context& context, s32 component);
    };


#if !defined(CPPIMG_DISABLE_OPENEXR)
    //----------------------------------------------------
    //---
    //--- OpenEXR
    //---
    //----------------------------------------------------
    class OpenEXR
    {
    public:
        struct Information
        {
            s32 getBytesPerPixel() const
            {
                return cppimg::getBytesPerPixel(numChannels_, types_);
            }

            s32 getSize(s32 channel) const
            {
                return cppimg::getSize(STATIC_CAST(Type, types_[channel]));
            }

            s32 width_;
            s32 height_;
            s32 numChannels_;
            ColorType colorType_;
            s32 types_[MaxChannels];
        };

        /**
        @brief
        @return Success:true, Fail:false
        @param information
        @param image
        @param stream
        */
        static bool read(Information& information, void* image, Stream& stream);

        /**
        @brief
        @return Success:true, Fail:false
        @param stream
        @param width
        @param height
        @param colorType
        @param pixelType
        @param image
        */
        static bool write(Stream& stream, s32 width, s32 height, ColorType colorType, Type pixelType, const void* image);

    private:
        static const u32 MAGIC = 0x01312F76U;
        static const s32 MinStringBufferSize = 16;
        static const s32 MaxStringSize = 256;
        static const s32 MaxInChannels = 8;
        static const s32 MaxOutChannels = 4;
        static const s32 LinesPerBlock = 16;

        enum Compression
        {
            NO_COMPRESSION = 0,
            RLE_COMPRESSION = 1,
            ZIPS_COMPRESSION = 2,
            ZIP_COMPRESSION = 3,
            PIZ_COMPRESSION = 4,
            PXR24_COMPRESSION = 5,
            B44_COMPRESSION = 6,
            B44A_COMPRESSION = 7,
        };

        enum EnvMap
        {
            ENVMAP_LATLONG = 0,
            ENVMAP_CUBE = 1,
        };

        enum LineOrder
        {
            INCREASING_Y = 0,
            DECREASING_Y = 1,
            RANDOM_Y = 2,
        };

        enum LevelMode
        {
            ONE_LEVEL = 0,
            MIPMAP_LEVELS = 1,
            RIPMAP_LEVELS = 2,
        };

        enum RoundingMode
        {
            ROUND_DOWN = 0,
            ROUND_UP = 1,
        };

        enum AttributeName
        {
            AttrName_Channels,
            AttrName_Compression,
            AttrName_DataWindow,
            AttrName_DisplayWindow,
            AttrName_LineOrder,
            AttrName_PixelAspectRatio,
            AttrName_ScreenWindowCenter,
            AttrName_ScreenWindowWidth,
            AttrName_Tiles,
            AttrName_View,
            AttrName_Name,
            AttrName_Type,
            AttrName_Version,
            AttrName_ChunkCount,
            AttrName_MaxSamplesPerPixel,
            AttrName_Chromaticities,
            AttrName_End,
            AttrName_Max,
        };

        static const Char* AttributeNames[AttrName_Max];

        enum AttributeType
        {
            AttrType_Box2i,
            AttrType_Box2f,
            AttrType_Chlist,
            AttrType_Chromaticities,
            AttrType_Compression,
            AttrType_Double,
            AttrType_Envmap,
            AttrType_Float,
            AttrType_Int,
            AttrType_Keycode,
            AttrType_LineOrder,
            AttrType_M33f,
            AttrType_M44f,
            AttrType_Preview,
            AttrType_Rational,
            AttrType_String,
            AttrType_StringVector,
            AttrType_TileDesc,
            AttrType_TimeCode,
            AttrType_V2i,
            AttrType_V2f,
            AttrType_V3i,
            AttrType_V3f,
            AttrType_Max,
        };

        static const Char* TypeNames[AttrType_Max];

        struct Version 
        {
            static const u32 Flag_TileFormat = 0x1U<<9;
            static const u32 Flag_LongName = 0x1U<<10;
            static const u32 Flag_DeepData = 0x1U<<11;
            static const u32 Flag_MultiPart = 0x1U<<12;

            inline bool isTile() const
            {
                return Flag_TileFormat == (version_&Flag_TileFormat);
            }

            inline bool isLongName() const
            {
                return Flag_LongName == (version_&Flag_LongName);
            }

            inline bool isDeepData() const
            {
                return Flag_DeepData == (version_&Flag_DeepData);
            }

            inline bool isMultiPart() const
            {
                return Flag_MultiPart == (version_&Flag_MultiPart);
            }

            static Version create(
                bool tile,
                bool longName,
                bool deepData,
                bool multiPart);

            u32 version_;
        };

        struct Box2i
        {
            s32 xMin_;
            s32 yMin_;
            s32 xMax_;
            s32 yMax_;
        };

        struct Box2f
        {
            f32 xMin_;
            f32 yMin_;
            f32 xMax_;
            f32 yMax_;
        };

        struct Channel
        {
            Char name_[MaxStringSize];
            s32 pixelType_;
            u8 flags_[4]; //flags_[0]: it is linear or not
            s32 xSampling_;
            s32 ySampling_;
        };

        struct Chromaticities
        {
            f32 redX_;
            f32 redY_;
            f32 greenX_;
            f32 greenY_;
            f32 blueX_;
            f32 blueY_;
            f32 whiteX_;
            f32 whiteY_;
        };

        struct KeyCode
        {
            s32 code_[7];
        };

        struct Matrix33
        {
            f32 m_[9];
        };

        struct Matrix44
        {
            f32 m_[16];
        };

        class Preview
        {
        public:
            Preview();
            ~Preview();

            bool read(Stream& stream);
            void swap(Preview& rhs);
        private:
            Preview(const Preview&) = delete;
            Preview(Preview&&) = delete;
            Preview& operator=(const Preview&) = delete;
            Preview& operator=(Preview&&) = delete;

            s32 width_;
            s32 height_;
            u8* image_;
        };

        struct Rational
        {
            s32 numerator_;
            u32 denominator_;
        };

        struct TiledDesc
        {
            u32 xSize_;
            u32 ySize_;
            u8 levelMode_;
            u8 roundingMode_[16];
        };

        struct TimeCode
        {
            u32 timeAndFlags_;
            u32 userData_;
        };

        struct Vector2i
        {
            s32 x_;
            s32 y_;
        };

        struct Vector2f
        {
            f32 x_;
            f32 y_;
        };

        struct Vector3i
        {
            s32 x_;
            s32 y_;
            s32 z_;
        };

        struct Vector3f
        {
            f32 x_;
            f32 y_;
            f32 z_;
        };

        class Buffer
        {
        public:
            Buffer();
            explicit Buffer(s64 capacity);
            ~Buffer();

            bool reserve(s64 capacity);
            bool expand(s64 newCapacity);

            s64 capacity() const
            {
                return capacity_;
            }

            u8 operator[](s64 index) const
            {
                CPPIMG_ASSERT(0<=index && index<capacity_);
                return buffer_[index];
            }

            u8& operator[](s64 index)
            {
                CPPIMG_ASSERT(0<=index && index<capacity_);
                return buffer_[index];
            }

            const u8* begin() const
            {
                return buffer_;
            }

            u8* begin()
            {
                return buffer_;
            }
        private:
            s64 capacity_;
            u8* buffer_;
        };

        class StreamBuffer
        {
        public:
            StreamBuffer();
            explicit StreamBuffer(s64 increments);
            StreamBuffer(s64 increments, s64 capacity);
            ~StreamBuffer();

            s64 size() const
            {
                return size_;
            }

            s64 capacity() const
            {
                return buffer_.capacity();
            }

            void setIncrements(s64 increments)
            {
                CPPIMG_ASSERT(0<increments);
                increments_ = increments;
            }

            bool reserve(s64 capacity)
            {
                return buffer_.reserve(capacity);
            }

            const u8* begin() const
            {
                return buffer_.begin();
            }

            bool write(s64 size, const void* data);
        private:
            s64 size_;
            s64 increments_;
            Buffer buffer_;
        };

        struct Header
        {
            void initialize();

            bool getColorType(ColorType& colorType) const;
            void getTypes(s32 types[MaxInChannels]) const;
            const Channel* findChannel(const Char* name) const;
            void sortChannels();
            void getChannelAssign(s32 assign[MaxInChannels]) const;

            s32 numChannels_;
            Channel channels_[MaxInChannels];
            u8 compression_;
            Box2i dataWindow_;
            Box2i displayWindow_;
            u8 lineOrder_;
            f32 pixelAspectRatio_;
            Vector2f screenWindowCenter_;
            f32 screenWindowWidth_;

            Chromaticities chromaticities_;

            //Tile Header
            TiledDesc tiles_;

            //Multi-Part Header
            Char view_[MaxStringSize];

            //Multi-Part and Deep Data Header
            Char name_[MaxStringSize];
            Char type_[MaxStringSize];
            s32 version_;
            s32 chunkCount_;

            //Deep Data Header
            s32 maxSamplesPerPixel_;

            bool hasChromaticities_;
        };

        class Context
        {
        public:
            enum Status
            {
                Success,
                End,
                Error,
            };
            Context();
            ~Context();

            bool readVersion(Stream& stream);
            bool readHeader(Stream& stream);
            Status readAttribute(u32& flags, Stream& stream);

            void skipUnknownName(Stream& stream);
            void skipUnknownType(Stream& stream);

            static bool readChannel(Channel& channel, Stream& stream);
            bool readChannels(Stream& stream, s32 valueSize);

            bool readOffsetTable(Stream& stream);
            bool readScanlines(Stream& stream);
            bool readScanlines_NO_COMPRESSION(Stream& stream);
            bool readScanlines_RLE_COMPRESSION(Stream& stream);
            bool readScanlines_ZIP_COMPRESSION(Stream& stream);

            void getChannelInformation(s32 sizes[MaxInChannels], s32 offsets[MaxInChannels]) const;
            bool uncompressRLE(Stream& stream, s32 dstSize, u8* dst, u8* tmp, s32 srcSize);

            Version version_;
            Header header_;

            u64* offsetTable_;
            Information* information_;
            void* image_;
        };

        class WriteContext
        {
        public:
            WriteContext();
            ~WriteContext();

            s64 size() const
            {
                return streamBuffer_.size();
            }

            s64 capacity() const
            {
                return streamBuffer_.capacity();
            }

            void setIncrements(s64 increments)
            {
                streamBuffer_.setIncrements(increments);
            }

            bool reserve(s64 capacity)
            {
                return streamBuffer_.reserve(capacity);
            }

            const u8* begin() const
            {
                return streamBuffer_.begin();
            }

            bool write(s64 size, const void* data)
            {
                return streamBuffer_.write(size, data);
            }

            s32 numScanlines_;
            u64* offsetTable_;
            StreamBuffer streamBuffer_;
        };

        static AttributeName findAttributeName(const Char* str);
        static AttributeType findAttributeType(const Char* str);
        static s32 read(Char str[MaxStringSize], Stream& stream);
        static s32 readString(Char str[MaxStringSize], Stream& stream);
        static s32 writeAttribute(Stream& stream, AttributeName name, AttributeType type, s32 size, const void* data);
        static s32 writeChannels(Stream& stream, ColorType colorType, Type pixelType);
        static s32 writeChannels(Stream& stream, s32 size, const Char** names, Type pixelType, u8 linear, s32 xSampling, s32 ySampling);
        static s32 writeNull(Stream& stream);

        static void getChannelInformation(s32 offsets[MaxOutChannels], ColorType colorType, Type pixelType);

        static void preprocess(s32 size, u8* dst, const u8* src);
        static void postprocess(s32 size, u8* dst, u8* src);

        static s32 compressZlib(szlib::szContext& context, Buffer& dst, Buffer& tmp, s32 srcSize, const u8* src);
        static s32 uncompressZlib(szlib::szContext& context, Buffer& dst, Buffer& tmp, s32 srcSize, const u8* src);

        static bool writeScanlines_NO_COMPRESSION(WriteContext& context, u64 offset, s32 width, s32 height, ColorType colorType, Type pixelType, const void* data);
        static bool writeScanlines_ZIP_COMPRESSION(WriteContext& context, u64 offset, s32 width, s32 height, ColorType colorType, Type pixelType, s32 linesPerBlock, const void* data);
    };
#endif

}
#endif //INC_CPPIMG_H_

#ifdef CPPIMG_IMPLEMENTATION

#ifdef _MSC_VER
#define ALIGNED(N) __declspec(align(N))
#else
#define ALIGNED(N) __attribute__((aligned(N)))
#endif

//Enable the use of F16C intrinsic functions
#if !defined(_MSC_VER)
#define CPPIMG_DISABLE_F16C
#endif

#if !defined(CPPIMG_DISABLE_F16C)
#include <immintrin.h>
#include <emmintrin.h>
#endif

namespace cppimg
{
    f32 clamp01(f32 x)
    {
        UnionS32F32 u;
        u.f32_ = x;

        s32 s = u.s32_ >> 31;
        s = ~s;
        u.s32_ &= s;

        u.f32_ -= 1.0f;
        s = u.s32_ >> 31;
        u.s32_ &= s;
        u.f32_ += 1.0f;
        return u.f32_;
    }

    u16 toFloat16(f32 f)
    {
#if defined(CPPIMG_DISABLE_F16C)
        UnionU32F32 t;
        t.f32_ = f;

        u16 sign = (t.u32_>>16) & 0x8000U;
        s32 exponent = (t.u32_>>23) & 0x00FFU;
        u32 fraction = t.u32_ & 0x007FFFFFU;

        if(exponent == 0){
            return sign; //Signed zero

        }else if(exponent == 0xFFU){
            if(fraction == 0){
                return sign | 0x7C00U; //Signed infinity
            }else{
                return static_cast<u16>((fraction>>13) | 0x7C00U); //NaN
            }
        }else {
            exponent += (-127 + 15);
            if(exponent>=0x1F){ //Overflow
                return sign | 0x7C00U;
            }else if(exponent<=0){ //Underflow
                s32 shift = 14 - exponent;
                if(shift>24){ //Too small
                    return sign;
                }else{
                    fraction |= 0x800000U; //Add hidden bit
                    u16 frac = static_cast<u16>(fraction >> shift);
                    if((fraction>>(shift-1)) & 0x01U){ //Round lowest 1 bit
                        frac += 1;
                    }
                    return sign | frac;
                }
            }
        }

        u16 ret = static_cast<u16>(sign | ((exponent<<10) & 0x7C00U) | (fraction>>13));
        if((fraction>>12) & 0x01U){ //Round lower 1 bit
            ret += 1;
        }
        return ret;
#else
        ALIGNED(16) u16 result[8];
        _mm_store_si128((__m128i*)result, _mm_cvtps_ph(_mm_set1_ps(f), 0)); //round to nearest
        return result[0];
#endif
    }

    f32 toFloat32(u16 h)
    {
#if defined(CPPIMG_DISABLE_F16C)
        u32 sign = (h & 0x8000U) << 16;
        u32 exponent = ((h & 0x7C00U) >> 10);
        u32 fraction = (h & 0x03FFU);

        if(exponent == 0){
            if(fraction != 0){
                fraction <<= 1;
                while(0==(fraction & 0x0400U)){
                    ++exponent;
                    fraction <<= 1;
                }
                exponent = (127 - 15) - exponent;
                fraction &= 0x03FFU;
            }

        }else if(exponent == 0x1FU){
            exponent = 0xFFU; //Infinity or NaN

        }else{
            exponent += (127 - 15);
        }

        UnionU32F32 t;
        t.u32_ = sign | (exponent<<23) | (fraction<<13);

        return t.f32_;
#else
        ALIGNED(16) f32 result[4];
        _mm_store_ps(result, _mm_cvtph_ps(_mm_set1_epi16(*(s16*)&h)));
        return result[0];
#endif
    }

    const Char* ChannelNames[Channel_Num]
    {
        "R",
        "G",
        "B",
        "A",
        "Y",
    };

    s32 getBytesPerPixel(s32 channels, const s32* types)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != types);
        s32 size = 0;
        for(s32 i=0; i<channels; ++i){
            size += getSize(STATIC_CAST(Type, types[i]));
        }
        return size;
    }

    s32 getSize(Type type)
    {
        switch(type){
        case Type_UINT:
            return sizeof(u32);
        case Type_HALF:
            return sizeof(u16);
        case Type_FLOAT:
        default:
            return sizeof(f32);
        }
    }

    s32 getNumChannels(ColorType type)
    {
        switch(type){
        case ColorType_GRAY:
            return 1;
        case ColorType_RGB:
            return 3;
        case ColorType_RGBA:
            return 4;
        default:
            CPPIMG_ASSERT(false);
            return 0;
        }
    }

    void convert(s32 width, s32 height, s32 channels, u8* dst, const void* src, const s32* types)
    {
        CPPIMG_ASSERT(0<=width);
        CPPIMG_ASSERT(0<=height);
        CPPIMG_ASSERT(0<=channels);
        CPPIMG_ASSERT(CPPIMG_NULL != dst);
        CPPIMG_ASSERT(CPPIMG_NULL != src);
        CPPIMG_ASSERT(CPPIMG_NULL != types);

        s32 sizes[MaxChannels];
        for(s32 i=0; i<channels; ++i){
            sizes[i] = getSize(STATIC_CAST(Type, types[i]));
        }
        s32 srcBytesPerPixel = getBytesPerPixel(channels, types);

        F32ToU8 f32ToU8;
        U32ToU8 u32ToU8;
        F16ToU8 f16ToU8;

        s32 dstStep = width*channels;
        u8* dr = reinterpret_cast<u8*>(dst);
        s32 srcStep = width*srcBytesPerPixel;
        const u8* sr = reinterpret_cast<const u8*>(src);
        for(s32 i=0; i<height; ++i){
            u8* d = dr;
            const u8* s = sr;
            for(s32 j=0; j<width; ++j){
                for(s32 k=0; k<channels; ++k){
                    switch(types[k]){
                    case Type_UINT:
                        *d = u32ToU8(*reinterpret_cast<const u32*>(s));
                        break;
                    case Type_HALF:
                        *d = f16ToU8(*reinterpret_cast<const u16*>(s));
                        break;
                    case Type_FLOAT:
                        *d = f32ToU8(*reinterpret_cast<const f32*>(s));
                        break;
                    default:
                        CPPIMG_ASSERT(false);
                        break;
                    }
                    ++d;
                    s += sizes[k];
                }
            }
            dr += dstStep;
            sr += srcStep;
        }
    }

namespace
{
    //----------------------------------------------------
    //--- CRC32
    //----------------------------------------------------
    u32 CRC32Table[] =
    {
        0x0U,0x77073096U,0xEE0E612CU,0x990951BAU,0x76DC419U,0x706AF48FU,0xE963A535U,0x9E6495A3U,0xEDB8832U,0x79DCB8A4U,0xE0D5E91EU,0x97D2D988U,0x9B64C2BU,0x7EB17CBDU,0xE7B82D07U,0x90BF1D91U,0x1DB71064U,0x6AB020F2U,0xF3B97148U,0x84BE41DEU,0x1ADAD47DU,0x6DDDE4EBU,0xF4D4B551U,0x83D385C7U,0x136C9856U,0x646BA8C0U,0xFD62F97AU,0x8A65C9ECU,0x14015C4FU,0x63066CD9U,0xFA0F3D63U,0x8D080DF5U,0x3B6E20C8U,0x4C69105EU,0xD56041E4U,0xA2677172U,0x3C03E4D1U,0x4B04D447U,0xD20D85FDU,0xA50AB56BU,0x35B5A8FAU,0x42B2986CU,0xDBBBC9D6U,0xACBCF940U,0x32D86CE3U,0x45DF5C75U,0xDCD60DCFU,0xABD13D59U,0x26D930ACU,0x51DE003AU,0xC8D75180U,0xBFD06116U,0x21B4F4B5U,0x56B3C423U,0xCFBA9599U,0xB8BDA50FU,0x2802B89EU,0x5F058808U,0xC60CD9B2U,0xB10BE924U,0x2F6F7C87U,0x58684C11U,0xC1611DABU,0xB6662D3DU,
        0x76DC4190U,0x1DB7106U,0x98D220BCU,0xEFD5102AU,0x71B18589U,0x6B6B51FU,0x9FBFE4A5U,0xE8B8D433U,0x7807C9A2U,0xF00F934U,0x9609A88EU,0xE10E9818U,0x7F6A0DBBU,0x86D3D2DU,0x91646C97U,0xE6635C01U,0x6B6B51F4U,0x1C6C6162U,0x856530D8U,0xF262004EU,0x6C0695EDU,0x1B01A57BU,0x8208F4C1U,0xF50FC457U,0x65B0D9C6U,0x12B7E950U,0x8BBEB8EAU,0xFCB9887CU,0x62DD1DDFU,0x15DA2D49U,0x8CD37CF3U,0xFBD44C65U,0x4DB26158U,0x3AB551CEU,0xA3BC0074U,0xD4BB30E2U,0x4ADFA541U,0x3DD895D7U,0xA4D1C46DU,0xD3D6F4FBU,0x4369E96AU,0x346ED9FCU,0xAD678846U,0xDA60B8D0U,0x44042D73U,0x33031DE5U,0xAA0A4C5FU,0xDD0D7CC9U,0x5005713CU,0x270241AAU,0xBE0B1010U,0xC90C2086U,0x5768B525U,0x206F85B3U,0xB966D409U,0xCE61E49FU,0x5EDEF90EU,0x29D9C998U,0xB0D09822U,0xC7D7A8B4U,0x59B33D17U,0x2EB40D81U,0xB7BD5C3BU,0xC0BA6CADU,
        0xEDB88320U,0x9ABFB3B6U,0x3B6E20CU,0x74B1D29AU,0xEAD54739U,0x9DD277AFU,0x4DB2615U,0x73DC1683U,0xE3630B12U,0x94643B84U,0xD6D6A3EU,0x7A6A5AA8U,0xE40ECF0BU,0x9309FF9DU,0xA00AE27U,0x7D079EB1U,0xF00F9344U,0x8708A3D2U,0x1E01F268U,0x6906C2FEU,0xF762575DU,0x806567CBU,0x196C3671U,0x6E6B06E7U,0xFED41B76U,0x89D32BE0U,0x10DA7A5AU,0x67DD4ACCU,0xF9B9DF6FU,0x8EBEEFF9U,0x17B7BE43U,0x60B08ED5U,0xD6D6A3E8U,0xA1D1937EU,0x38D8C2C4U,0x4FDFF252U,0xD1BB67F1U,0xA6BC5767U,0x3FB506DDU,0x48B2364BU,0xD80D2BDAU,0xAF0A1B4CU,0x36034AF6U,0x41047A60U,0xDF60EFC3U,0xA867DF55U,0x316E8EEFU,0x4669BE79U,0xCB61B38CU,0xBC66831AU,0x256FD2A0U,0x5268E236U,0xCC0C7795U,0xBB0B4703U,0x220216B9U,0x5505262FU,0xC5BA3BBEU,0xB2BD0B28U,0x2BB45A92U,0x5CB36A04U,0xC2D7FFA7U,0xB5D0CF31U,0x2CD99E8BU,0x5BDEAE1DU,
        0x9B64C2B0U,0xEC63F226U,0x756AA39CU,0x26D930AU,0x9C0906A9U,0xEB0E363FU,0x72076785U,0x5005713U,0x95BF4A82U,0xE2B87A14U,0x7BB12BAEU,0xCB61B38U,0x92D28E9BU,0xE5D5BE0DU,0x7CDCEFB7U,0xBDBDF21U,0x86D3D2D4U,0xF1D4E242U,0x68DDB3F8U,0x1FDA836EU,0x81BE16CDU,0xF6B9265BU,0x6FB077E1U,0x18B74777U,0x88085AE6U,0xFF0F6A70U,0x66063BCAU,0x11010B5CU,0x8F659EFFU,0xF862AE69U,0x616BFFD3U,0x166CCF45U,0xA00AE278U,0xD70DD2EEU,0x4E048354U,0x3903B3C2U,0xA7672661U,0xD06016F7U,0x4969474DU,0x3E6E77DBU,0xAED16A4AU,0xD9D65ADCU,0x40DF0B66U,0x37D83BF0U,0xA9BCAE53U,0xDEBB9EC5U,0x47B2CF7FU,0x30B5FFE9U,0xBDBDF21CU,0xCABAC28AU,0x53B39330U,0x24B4A3A6U,0xBAD03605U,0xCDD70693U,0x54DE5729U,0x23D967BFU,0xB3667A2EU,0xC4614AB8U,0x5D681B02U,0x2A6F2B94U,0xB40BBE37U,0xC30C8EA1U,0x5A05DF1BU,0x2D02EF8DU,
    };

    u32 updateCRC32(u32 crc, u32 len, const u8* buffer)
    {
        for(u32 i=0; i<len; ++i){
            crc = CRC32Table[(crc ^ buffer[i]) & 0xFFU] ^ (crc >> 8);
        }
        return crc;
    }

#if 0 //unused
    u32 CRC32(u32 len, const u8* buffer)
    {
        return updateCRC32(0xFFFFFFFFUL, len, buffer)^0xFFFFFFFFUL;
    }
#endif

    //----------------------------------------------------
    //--- SeekSet
    //----------------------------------------------------
    class SeekSet
    {
    public:
        SeekSet(off_t pos, Stream* stream)
            :pos_(pos)
            ,stream_(stream)
        {}
        ~SeekSet()
        {
            if(CPPIMG_NULL != stream_){
                stream_->seek(pos_, SEEK_SET);
            }
        }

        void clear()
        {
            stream_ = CPPIMG_NULL;
        }
    private:
        off_t pos_;
        Stream* stream_;
    };
}

    //----------------------------------------------------
    //---
    //--- IFStream
    //---
    //----------------------------------------------------
    s32 IFStream::read(size_t size, void* dst)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file_);
        size_t num = 0<size? 1 : 0;
        return num == fread(dst, size, num, file_)? 1 : -1;
    }

    IFStream& IFStream::operator=(IFStream&& rhs)
    {
        if(this != &rhs){
            if(CPPIMG_NULL != file_){
                fclose(file_);
            }
            file_ = rhs.file_;
            rhs.file_ = CPPIMG_NULL;
        }
        return *this;
    }

    //----------------------------------------------------
    //---
    //--- OFStream
    //---
    //----------------------------------------------------
    s32 OFStream::write(size_t size, const void* dst)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != file_);
        size_t num = 0<size? 1 : 0;
        return num == fwrite(dst, size, num, file_)? 1 : -1;
    }

    OFStream& OFStream::operator=(OFStream&& rhs)
    {
        if(this != &rhs){
            if(CPPIMG_NULL != file_){
                fclose(file_);
            }
            file_ = rhs.file_;
            rhs.file_ = CPPIMG_NULL;
        }
        return *this;
    }

    //----------------------------------------------------
    //---
    //--- BMP
    //---
    //----------------------------------------------------
    bool BMP::read24(u8* image, s32 width, s32 height, bool leftBottom, Stream& stream)
    {
        s32 pitch = width * 3;
        s32 diff = (pitch + 0x03U) & (~0x03U);
        diff -= pitch;
        u8 tmp[3];

        if(leftBottom){
            image += pitch * (height-1);
            for(s32 i = 0; i<height; ++i){
                u8 *b = image;
                for(s32 j = 0; j<width; ++j){
                    if(stream.read(3, tmp)<=0){
                        return false;
                    }
                    b[0] = tmp[2];
                    b[1] = tmp[1];
                    b[2] = tmp[0];
                    b += 3;
                }
                if(stream.read(diff, tmp)<=0){
                    return false;
                }
                image -= pitch;
            }

        }else{
            for(s32 i = 0; i<height; ++i){
                for(s32 j = 0; j<width; ++j){
                    if(stream.read(3, tmp)<=0){
                        return false;
                    }
                    image[0] = tmp[2];
                    image[1] = tmp[1];
                    image[2] = tmp[0];
                    image += 3;
                }
                if(stream.read(diff, tmp)<=0){
                    return false;
                }
            }
        }
        return true;
    }

    bool BMP::read32(u8* image, s32 width, s32 height, bool leftBottom, Stream& stream)
    {
        s32 pitch = width * 4;

        if(leftBottom){
            image += pitch * (height-1);
            for(s32 i = 0; i<height; ++i){
                if(stream.read(pitch, image)<=0){
                    return false;
                }
                u8 *b = image;
                for(s32 j = 0; j<width; ++j){
                    cppimg::swap(b[0], b[3]);
                    cppimg::swap(b[1], b[2]);
                    b += 4;
                }
                image -= pitch;
            }

        } else{
            for(s32 i = 0; i<height; ++i){
                if(stream.read(pitch, image)<=0){
                    return false;
                }
                u8 *b = image;
                for(s32 j = 0; j<width; ++j){
                    cppimg::swap(b[0], b[3]);
                    cppimg::swap(b[1], b[2]);
                    b += 4;
                }
                image += pitch;
            }
        }
        return true;
    }

    bool BMP::read(s32& width, s32& height, ColorType& colorType, void* image, Stream& stream)
    {
        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);

        u16 magic;
        if(stream.read(sizeof(u16), &magic)<=0){
            return false;
        }
        if(magic != MAGIC){
            return false;
        }

        HEADER header;
        if(stream.read(sizeof(HEADER), &header)<=0){
            return false;
        }

        INFOHEADERV5 infoHeader;
        if(stream.read(sizeof(u32), &infoHeader.infoSize_)<=0){
            return false;
        }
        switch(infoHeader.infoSize_)
        {
        case 40:
            if(stream.read(40-sizeof(u32), &infoHeader.width_)<=0){
                return false;
            }
            break;
        case 108:
            if(stream.read(108-sizeof(u32), &infoHeader.width_)<=0){
                return false;
            }
            break;
        case 124:
            if(stream.read(124-sizeof(u32), &infoHeader.width_)<=0){
                return false;
            }
            break;
        default:
            if(16<=infoHeader.infoSize_ && infoHeader.infoSize_<=64){
                if(stream.read(infoHeader.infoSize_-sizeof(u32), &infoHeader.width_)<=0){
                    return false;
                }
            }else{
                return false;
            }
            break;
        }

        //Support only rgb no-compression
        if(!(infoHeader.compression_ == Compression_RGB && 24 == infoHeader.bitCount_)
            && !(infoHeader.compression_ == Compression_BitFields && 32 == infoHeader.bitCount_)){
            return false;
        }

        //Support only 24 or 32 bits
        if(24 != infoHeader.bitCount_
            && 32 != infoHeader.bitCount_)
        {
            return false;
        }

        width = static_cast<u32>(infoHeader.width_);
        height = static_cast<u32>(infoHeader.height_);
        colorType = 24 == infoHeader.bitCount_? ColorType_RGB : ColorType_RGBA;
        if(CPPIMG_NULL == image){
            return true;
        }

        bool leftBottom = (0<=infoHeader.height_);
        switch(infoHeader.bitCount_){
        case 24:
            if(!read24(reinterpret_cast<u8*>(image), width, height, leftBottom, stream)){
                return false;
            }
            break;
        case 32:
            if(!read32(reinterpret_cast<u8*>(image), width, height, leftBottom, stream)){
                return false;
            }
            break;
        default:
            CPPIMG_ASSERT(false);
            break;
        }
        seekSet.clear();
        return true;
    }

    bool BMP::write(Stream& stream, s32 width, s32 height, ColorType colorType, const void* image)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != image);
        if(!stream.valid()){
            return false;
        }
        SeekSet seekSet(stream.tell(), &stream);
        HEADER header = {0};
        INFOHEADER infoHeader = {0};
        u32 fileSize = sizeof(MAGIC);
        fileSize += sizeof(HEADER);
        fileSize += sizeof(INFOHEADER);

        header.reserve1_ = 0;
        header.reserve2_ = 0;
        header.size_ = sizeof(MAGIC)+sizeof(INFOHEADER);
        header.offset_ = fileSize;

        infoHeader.infoSize_ = 40;
        infoHeader.width_ = width;
        infoHeader.height_ = height;
        infoHeader.planes_ = 1;
        infoHeader.compression_ = Compression_RGB;
        infoHeader.sizeImage_ = 0;
        infoHeader.xPixPerMeter_ = 0;
        infoHeader.yPixPerMeter_ = 0;
        infoHeader.colorUsed_ = 0;
        infoHeader.colorImportant_ = 0;

        s32 pitch, dstPitch;
        switch(colorType)
        {
        case ColorType_GRAY:
        case ColorType_RGB:
        {
            infoHeader.bitCount_ = 24;
            pitch = 3*width;
            dstPitch = (pitch + 0x03U) & (~0x03U);
            infoHeader.sizeImage_ = dstPitch * height;
        }
        break;

        case ColorType_RGBA:
            infoHeader.bitCount_ = 32;
            pitch = dstPitch = width * 4;
            infoHeader.sizeImage_ = 4 * width * height;
            break;
        default:
            return false;
            break;
        };
        header.size_ += infoHeader.sizeImage_;
        u16 magic = MAGIC;
        if(stream.write(sizeof(magic), &magic)<=0){
            return false;
        }
        if(stream.write(sizeof(header), &header)<=0){
            return false;
        }
        if(stream.write(sizeof(infoHeader), &infoHeader)<=0){
            return false;
        }

        s32 diff = dstPitch - pitch;
        u8 tmp[4];
        const u8* buffer = reinterpret_cast<const u8*>(image);
        switch(colorType)
        {
        case ColorType_GRAY:
            buffer += width * (height-1);
            for(s32 i = 0; i<height; ++i){
                const u8* b = buffer;
                for(s32 j = 0; j<width; ++j){
                    tmp[0] = tmp[1] = tmp[2] = b[0];
                    if(stream.write(3, tmp)<=0){
                        return false;
                    }
                    ++b;
                }
                tmp[0] = tmp[1] = tmp[2] = 0;
                if(stream.write(diff, tmp)<=0){
                    return false;
                }
                buffer -= width;
            }
            break;
        case ColorType_RGB:
            buffer += pitch * (height-1);
            for(s32 i = 0; i<height; ++i){
                const u8* b = buffer;
                for(s32 j = 0; j<width; ++j){
                    tmp[0] = b[2]; tmp[1] = b[1]; tmp[2] = b[0];
                    if(stream.write(3, tmp)<=0){
                        return false;
                    }
                    b += 3;
                }
                tmp[0] = tmp[1] = tmp[2] = 0;
                if(stream.write(diff, tmp)<=0){
                    return false;
                }
                buffer -= pitch;
            }
            break;
        case ColorType_RGBA:
            buffer += pitch * (height-1);
            for(s32 i = 0; i<height; ++i){
                const u8* b = buffer;
                for(s32 j = 0; j<width; ++j){
                    tmp[0] = b[2];
                    tmp[1] = b[1];
                    tmp[2] = b[0];
                    tmp[3] = b[3];
                    if(stream.write(4, tmp)<=0){
                        return false;
                    }
                    b += 4;
                }
                buffer -= pitch;
            }
            break;
        default:
            break;
        };
        seekSet.clear();
        return true;
    }
    
    //----------------------------------------------------
    //---
    //--- TGA
    //---
    //----------------------------------------------------
    bool TGA::readInternal(s32 width, s32 height, u8 bpp, u8* image, Stream& stream)
    {
        //transpose
        s32 rowBytes = width*bpp;
        image += rowBytes*(height-1);

        u8 tmp[4];
        for(s32 i = 0; i<height; ++i){
            u8* row = image;
            for(s32 j = 0; j<width; ++j){
                if(stream.read(bpp, tmp)<=0){
                    return false;
                }
                row[0] = tmp[2];
                row[1] = tmp[1];
                row[2] = tmp[0];
                if(3<bpp){
                    row[3] = tmp[3];
                }
                row += bpp;
            }
            image -= rowBytes;
        }
        return true;
    }

    bool TGA::readRLEInternal(s32 width, s32 height, u8 bpp, u8* image, Stream& stream)
    {
        //transpose
        s32 rowBytes = width*bpp;
        image += rowBytes*(height-1);
        u8* row = image;
        s32 x=0;
        s32 pixels = width*height;
        u8 tmp[4];
        for(s32 i = 0; i<pixels;){
            u8 byte;
            if(stream.read(1, &byte)<=0){
                return false;
            }
            //If MSB is 1 then consecutive, else no-consecutive data
            s32 count = (byte & 0x7FU) + 1;
            if(0 != (byte & 0x80U)){
                //consecutive data
                if(stream.read(bpp, tmp)<=0){
                    return false;
                }
                for(s32 j = 0; j<count; ++j){
                    row[0] = tmp[2];
                    row[1] = tmp[1];
                    row[2] = tmp[0];
                    if(3<bpp){
                        row[3] = tmp[3];
                    }
                    row += bpp;
                    if(width<=++x){
                        x = 0;
                        image -= rowBytes;
                        row = image;
                    }
                }
            } else{
                //no-consecutive data
                for(s32 j = 0; j<count; ++j){
                    if(stream.read(bpp, tmp)<=0){
                        return false;
                    }
                    row[0] = tmp[2];
                    row[1] = tmp[1];
                    row[2] = tmp[0];
                    if(3<bpp){
                        row[3] = tmp[3];
                    }
                    row += bpp;
                    if(width<=++x){
                        x = 0;
                        image -= rowBytes;
                        row = image;
                    }
                }
            }
            i += count;
        }
        return true;
    }

    u8 TGA::calcRunLength(u8 bpp, s32 x, s32 y, s32 width, const u8* image)
    {
        static const u32 maxCount = 0x80U;
        if(0==y && x==(width-1)){
            return 0x80U;
        }
        u8 count = 1;
        s32 nx = x+1;
        s32 ny = (width<=nx)? y-1 : y;

        const u8* current = image+(x+y*width)*bpp;
        const u8* next = image+(nx+ny*width)*bpp;
        if(check(bpp, current, next)){
            while(0<=ny){
                x = nx;
                y = ny;
                current = next;
                if(width<=++nx){
                    if(--ny<0){
                        break;
                    }
                    nx = 0;
                }
                next = image+(nx+ny*width)*bpp;
                if(!check(bpp, current, next)){
                    break;
                }
                if(maxCount<=++count){
                    break;
                }
            }
            return 0x80U | (count-1);

        } else{
            while(0<=ny){
                x = nx;
                y = ny;
                current = next;
                if(width<=++nx){
                    if(--ny<0){
                        break;
                    }
                    nx = 0;
                }
                next = image+(nx+ny*width)*bpp;
                if(check(bpp, current, next)){
                    break;
                }
                if(maxCount<=++count){
                    break;
                }
            }
            return count-1;
        }
    }
#if 0
    bool TGA::write32(Stream& stream, s32 width, s32 height, const u8* image)
    {
        static const u32 bpp = 4;
        s32 pixels = width*height;
        u8 tmp[bpp];
        for(s32 i = 0; i<pixels; ++i){
            tmp[0] = image[2];
            tmp[1] = image[1];
            tmp[2] = image[0];
            tmp[3] = image[3];
            if(stream.write(bpp, tmp)<=0){
                return false;
            }
            image += bpp;
        }
        return true;
    }
#endif

    bool TGA::writeUncompress(Stream& stream, s32 width, s32 height, u8 bpp, const u8* image)
    {
        u8 tmp[4];
        s32 line = width*bpp;
        image += line*(height-1);
        if(bpp<4){
            for(s32 i = height-1; 0<=i; --i){
                const u8* scan = image;
                for(s32 j = 0; j<width; ++j){
                    tmp[0] = scan[2];
                    tmp[1] = scan[1];
                    tmp[2] = scan[0];
                    if(stream.write(bpp, tmp)<=0){
                        return false;
                    }
                    scan += bpp;
                }
                image -= line;
            }
        } else{
            for(s32 i = height-1; 0<=i; --i){
                const u8* scan = image;
                for(s32 j = 0; j<width; ++j){
                    tmp[0] = scan[2];
                    tmp[1] = scan[1];
                    tmp[2] = scan[0];
                    tmp[3] = scan[3];
                    if(stream.write(bpp, tmp)<=0){
                        return false;
                    }
                    scan += bpp;
                }
                image -= line;
            }
        }
        return true;
    }

    bool TGA::writeRLE(Stream& stream, s32 width, s32 height, u8 bpp, const u8* image)
    {
        u8 tmp[4];
        s32 x=0,y=height-1;
        while(0<=y){
            u8 run = calcRunLength(bpp, x, y, width, image);
            u8 count = (run & 0x7FU)+1;

            if(stream.write(1, &run)<=0){
                return false;
            }

            if(0 != (run & 0x80U)){
                const u8* dst = image+(x+y*width)*bpp;
                tmp[0] = dst[2];
                tmp[1] = dst[1];
                tmp[2] = dst[0];
                if(4<=bpp){
                    tmp[3] = dst[3];
                }
                if(stream.write(bpp, tmp)<=0){
                    return false;
                }
                x += count;
                while(width<=x){
                    x -= width;
                    --y;
                }
            } else{
                for(u8 i=0; i<count; ++i){
                    const u8* dst = image+(x+y*width)*bpp;
                    tmp[0] = dst[2];
                    tmp[1] = dst[1];
                    tmp[2] = dst[0];
                    if(4<=bpp){
                        tmp[3] = dst[3];
                    }
                    if(stream.write(bpp, tmp)<=0){
                        return false;
                    }
                    if(width<=++x){
                        x -= width;
                        --y;
                    }
                }
            }//if(0 != (run & 0x80U)
        }//while(0<=y)
        return true;
    }

    bool TGA::read(s32& width, s32& height, ColorType& colorType, void* image, Stream& stream)
    {
        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);

        u8 tgaHeader[TGA_HEADER_SIZE];
        if(stream.read(TGA_HEADER_SIZE, tgaHeader)<=0){
            return false;
        }
        width = static_cast<s32>(getU16(tgaHeader[Offset_WidthL], tgaHeader[Offset_WidthH]));
        height = static_cast<s32>(getU16(tgaHeader[Offset_HeightL], tgaHeader[Offset_HeightH]));

        u8 bpp = tgaHeader[Offset_BitPerPixel];
        u8 type = tgaHeader[Offset_ImageType];

        //Support only full color
        if(Type_FullColor != type
            && Type_FullColorRLE != type)
        {
            return false;
        }

        //Support either 24 or 32 bit 
        if(24 != bpp
            && 32 != bpp)
        {
            return false;
        }

        colorType = (24 == bpp)? ColorType_RGB : ColorType_RGBA;
        if(CPPIMG_NULL == image){
            return true;
        }
        //Skip string, if it exists
        if(0<tgaHeader[Offset_IDLeng]){
            stream.seek(tgaHeader[Offset_IDLeng], SEEK_CUR);
        }
        //SKip color map, if it exists
        if(1<=tgaHeader[Offset_ColorMapType]){
            stream.seek(tgaHeader[Offset_IDLeng], SEEK_CUR);
            u32 length = static_cast<u32>(getU16(tgaHeader[Offset_ColorMapLengL], tgaHeader[Offset_ColorMapLengH]));
            length *= (tgaHeader[Offset_ColorMapSize] >> 3);
            stream.seek(length, SEEK_CUR);
        }

        switch(bpp)
        {
        case 24:
        {
            if(Type_FullColorRLE == type){
                readRLEInternal(width, height, 3, reinterpret_cast<u8*>(image), stream);
            } else{
                readInternal(width, height, 3, reinterpret_cast<u8*>(image), stream);
            }
        }
        break;
        case 32:
        {
            if(Type_FullColorRLE == type){
                readRLEInternal(width, height, 4, reinterpret_cast<u8*>(image), stream);
            } else{
                readInternal(width, height, 4, reinterpret_cast<u8*>(image), stream);
            }
        }
        break;
        default:
            return false;
        };
        seekSet.clear();
        return true;
    }

    bool TGA::write(Stream& stream, s32 width, s32 height, ColorType colorType, const void* image, s32 options)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != image);
        if(!stream.valid()){
            return false;
        }
        SeekSet seekSet(stream.tell(), &stream);
        u8 tgaHeader[TGA_HEADER_SIZE] = {0};

        tgaHeader[Offset_WidthL] = width & 0xFFU;
        tgaHeader[Offset_WidthH] = (width>>8) & 0xFFU;

        tgaHeader[Offset_HeightL] = height & 0xFFU;
        tgaHeader[Offset_HeightH] = (height>>8) & 0xFFU;

        tgaHeader[Offset_BitPerPixel] = (ColorType_RGB == colorType)? 24 : 32;

        Type type = (0 != (options & Option_Compress))? Type_FullColorRLE : Type_FullColor;
        tgaHeader[Offset_ImageType] = static_cast<u8>(type);

        tgaHeader[Offset_Discripter] = (ColorType_RGB == colorType)? 0 : 8; //bit depth of alpha channel

        if(stream.write(TGA_HEADER_SIZE, tgaHeader)<=0){
            return false;
        }
        if(Type_FullColorRLE == tgaHeader[Offset_ImageType]){
            switch(colorType){
            case ColorType_RGB:
                if(!writeRLE(stream, width, height, 3, reinterpret_cast<const u8*>(image))){
                    return false;
                }
                break;
            case ColorType_RGBA:
                if(!writeRLE(stream, width, height, 4, reinterpret_cast<const u8*>(image))){
                    return false;
                }
                break;
            default:
                return false;
            }
        }else{
            switch(colorType){
            case ColorType_RGB:
                if(!writeUncompress(stream, width, height, 3, reinterpret_cast<const u8*>(image))){
                    return false;
                }
                break;
            case ColorType_RGBA:
                if(!writeUncompress(stream, width, height, 4, reinterpret_cast<const u8*>(image))){
                    return false;
                }
                break;
            default:
                return false;
            }
        }
        seekSet.clear();
        return true;
    }

    //----------------------------------------------------
    //---
    //--- PPM
    //---
    //----------------------------------------------------
    bool PPM::write(Stream& stream, s32 width, s32 height, ColorType colorType, const void* image)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != image);
        if(!stream.valid()){
            return false;
        }
        Char buffer[64];
        s32 scomponents, dcomponents;
        switch(colorType){
        case ColorType_GRAY:
            if(stream.write(3, "P2\n")<=0){
                return false;
            }
            scomponents = dcomponents = 1;
            break;
        case ColorType_RGB:
            if(stream.write(3, "P3\n")<=0){
                return false;
            }
            scomponents = dcomponents = 3;
            break;
        case ColorType_RGBA:
            if(stream.write(3, "P3\n")<=0){
                return false;
            }
            scomponents = 4;
            dcomponents = 3;
            break;
        default:
            return false;
        }
        s32 len;
        len = CPPIMG_SPRINTF(buffer, "%d %d\n255\n", width, height);
        if(stream.write(len, buffer)<=0){
            return false;
        }
        for(s32 i=0; i<height; ++i){
            if(0<i && stream.write(1, "\n")<=0){
                return false;
            }
            for(s32 j=0; j<width; ++j){
                s32 index = scomponents*(i*width+j);
                for(s32 k=0; k<dcomponents; ++k){
                    len = CPPIMG_SPRINTF(buffer, "%d ", reinterpret_cast<const u8*>(image)[index+k]);
                    if(stream.write(len, buffer)<=0){
                        return false;
                    }
                }
            }
        }
        return true;
    }

#if !defined(CPPIMG_DISABLE_PNG)
    //----------------------------------------------------
    //---
    //--- PNG
    //---
    //----------------------------------------------------
    bool PNG::read(s32& width, s32& height, ColorType& colorType, void* image, Stream& stream)
    {
        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);
        if(!readHeader(stream)){
            return false;
        }

        ChunkIHDR chunkIHDR;
        if(!readHeader(chunkIHDR, stream)
            || !chunkIHDR.read(stream)
            || MaxWidth<chunkIHDR.width_
            || MaxHeight<chunkIHDR.height_){
            return false;
        }
        width = static_cast<s32>(chunkIHDR.width_);
        height = static_cast<s32>(chunkIHDR.height_);

        s32 color;
        s32 alpha = 0;
        switch(chunkIHDR.colorType_){
        case PNG::ColorType_Gray:
            colorType = ColorType_GRAY;
            color = 1;
            break;
        case PNG::ColorType_True:
            colorType = ColorType_RGB;
            color = 3;
            break;
        case PNG::ColorType_Index:
            colorType = ColorType_RGB;
            color = 1;
            break;
        case PNG::ColorType_GrayAlpha:
            colorType = ColorType_RGBA;
            color = 1;
            alpha = 1;
            break;
        case PNG::ColorType_TrueAlpha:
            colorType = ColorType_RGBA;
            color = 3;
            alpha = 1;
            break;
        default:
            return false;
        }
        if(CPPIMG_NULL == image){
            return true;
        }

        Chunk chunk;
        cppimg::off_t start = stream.tell();
        bool loop = true;
        u32 totalIDAT = 0;
        //sum size of IDAT
        do{
            if(!readHeader(chunk, stream)){
                return false;
            }
            //Support only critical chunks
            switch(chunk.type_)
            {
            case ChunkIDAT::Type:
                totalIDAT += chunk.length_;
                if(!skipChunk(chunk, stream)){
                    return false;
                }
                break;
            case ChunkIEND::Type:
                loop = false;
                break;
            default:
                if(!skipChunk(chunk, stream)){
                    return false;
                }
                break;
            }
        }while(loop);

        ChunkPLTE chunkPLTE;
        ChunkIDAT chunkIDAT(totalIDAT, chunkIHDR.width_, chunkIHDR.height_, color, alpha);
        if(!chunkIDAT.initialize()){
            return false;
        }
        stream.seek(start, SEEK_SET);
        loop = true;
        do{
            if(!readHeader(chunk, stream)){
                return false;
            }
            //Support only critical chunks
            switch(chunk.type_)
            {
            case ChunkPLTE::Type:
                setChunkHeader(chunkPLTE, chunk);
                if(!chunkPLTE.read(stream)){
                    return false;
                }
                break;
            case ChunkIDAT::Type:
                setChunkHeader(chunkIDAT, chunk);
                if(!chunkIDAT.read(stream)){
                    return false;
                }
                break;
            case ChunkIEND::Type:
                loop = false;
                break;
            default:
                if(!skipChunk(chunk, stream)){
                    return false;
                }
                break;
            }
        }while(loop);

        bool result = chunkIDAT.decode(image);
        chunkIDAT.terminate();

        if(result){
            u8* uimage = reinterpret_cast<u8*>(image);
            switch(chunkIHDR.colorType_){
            case PNG::ColorType_Index:
                for(s32 i = 0; i<height; ++i){
                    for(s32 j = 0; j<width; ++j){
                        s32 index = (i*width+j)*3;
                        uimage[index+0] = chunkPLTE.r_[uimage[index+0]];
                        uimage[index+1] = chunkPLTE.g_[uimage[index+1]];
                        uimage[index+2] = chunkPLTE.b_[uimage[index+2]];
                    }
                }
                break;
            }
        }
        seekSet.clear();
        return result;
    }

#if 0
    bool PNG::write(Stream& stream, s32 width, s32 height, ColorType colorType, const void* image)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != image);
        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);

        //Write signature
        u64 signature = Signature;
        if(stream.write(sizeof(u64), &signature)<0){
            return false;
        }

        //Write header
        ChunkIHDR chunkIHDR;
        chunkIHDR.width_ = reverse(static_cast<u32>(width));
        chunkIHDR.height_ = reverse(static_cast<u32>(height));
        chunkIHDR.bitDepth_ = 8;
        chunkIHDR.compression_ = 0;
        chunkIHDR.filter_ = 0;
        chunkIHDR.interlace_ = 0;

        s32 color;
        s32 alpha = 0;
        switch(colorType)
        {
        case ColorType_GRAY:
            chunkIHDR.colorType_ = PNG::ColorType_Gray;
            color = 1;
            break;
        case ColorType_RGB:
            chunkIHDR.colorType_ = PNG::ColorType_True;
            color = 3;
            break;
        case ColorType_RGBA:
            chunkIHDR.colorType_ = PNG::ColorType_TrueAlpha;
            color = 3;
            alpha = 1;
            break;
        default:
            return false;
        }
        if(!writeChunk(stream, ChunkIHDR::Type, ChunkIHDR::Size, &chunkIHDR.width_)){
            return false;
        }

        ChunkIDAT chunkIDAT(true, width, height, color, alpha);
        if(!chunkIDAT.initialize()){
            return false;
        }
        if(!chunkIDAT.write(stream, height, reinterpret_cast<const u8*>(image))){
            return false;
        }
        chunkIDAT.terminate();
        if(!writeChunk(stream, ChunkIEND::Type, 0, CPPIMG_NULL)){
            return false;
        }
        seekSet.clear();
        return true;
    }
#endif

    //--- PNG::ChunkIHDR
    //----------------------------------------------------
    bool PNG::ChunkIHDR::read(Stream& istream)
    {
        if(istream.read(Size, &width_)<0){
            return false;
        }
        u32 crc;
        if(istream.read(sizeof(u32), &crc)<0){
            return false;
        }
        width_ = reverse(width_);
        height_ = reverse(height_);

        //Support only 8 bit depth
#if 0
        switch(color_){
        case PNG::ColorType_Gray:
            if(1!=bitDepth_
                && 2 != bitDepth_
                && 4 != bitDepth_
                && 8 != bitDepth_
                && 16 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_True:
            if(8 != bitDepth_ && 16 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_Index:
            if(1!=bitDepth_
                && 2 != bitDepth_
                && 4 != bitDepth_
                && 8 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_GrayAlpha:
            if(8 != bitDepth_ && 16 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_TrueAlpha:
            if(8 != bitDepth_ && 16 != bitDepth_){
                return false;
            }
            break;
        default:
            return false;
        }
#else
        switch(colorType_){
        case PNG::ColorType_Gray:
            if(8 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_True:
            if(8 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_Index:
            if(8 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_GrayAlpha:
            if(8 != bitDepth_){
                return false;
            }
            break;
        case PNG::ColorType_TrueAlpha:
            if(8 != bitDepth_){
                return false;
            }
            break;
        default:
            return false;
        }
#endif

        //Don't support interlace
        if(0<interlace_){
            return false;
        }
        return true;
    }

    //--- PNG::ChunkPLTE
    //----------------------------------------------------
    bool PNG::ChunkPLTE::read(Stream& stream)
    {
        //It's assumed that (chunk size mod 3) equals zero
        if(0 != (length_%3)){
            return false;
        }

        u8 rgb[3];
        u32 length=length_/3;
        for(u32 i=0; i<length; ++i){
            if(stream.read(3, rgb)<0){
                return false;
            }
            r_[i] = rgb[0];
            g_[i] = rgb[1];
            b_[i] = rgb[2];
        }
        size_ = length;
        u32 crc;
        if(stream.read(sizeof(u32), &crc)<0){
            return false;
        }
        return true;
    }

    //--- ChunkIDAT
    //----------------------------------------------------
    PNG::ChunkIDAT::ChunkIDAT(u32 totalSrcSize, u32 width, u32 height, s32 color, s32 alpha)
        :src_(CPPIMG_NULL)
        ,totalSrcSize_(totalSrcSize)
        ,srcSize_(0)
        ,totalSize_(width*height*(color+alpha))
        ,totalCount_(0)
        ,color_(color)
        ,alpha_(alpha)
        ,width_(width)
    {
    }

    PNG::ChunkIDAT::~ChunkIDAT()
    {
        terminate();
    }

    bool PNG::ChunkIDAT::initialize()
    {
        srcSize_ = 0;
        CPPIMG_FREE(src_);
        src_ = reinterpret_cast<u8*>(CPPIMG_MALLOC(totalSrcSize_));
        if(CPPIMG_NULL == src_){
            return false;
        }
        return true;
    }

    bool PNG::ChunkIDAT::terminate()
    {
        CPPIMG_FREE(src_);
        return true;
    }


    bool PNG::ChunkIDAT::read(Stream& stream)
    {
        u8* src = src_ + srcSize_;
        if(stream.read(length_, src)<0){
            return false;
        }
        u32 crc;
        if(stream.read(sizeof(u32), &crc)<0){
            return false;
        }

        srcSize_ += length_;
        return srcSize_<=totalSrcSize_;
    }

    bool PNG::ChunkIDAT::decode(void* image)
    {
        szlib::szContext context;
        if(szlib::SZ_OK != szlib::initInflate(&context, totalSrcSize_, src_)){
            return false;
        }

        u32 scanlineSize = width_*(color_+alpha_);
        u8* scanline = reinterpret_cast<u8*>(image);
        u32 scanlineOffset = 0;
        s32 filterFlag = FilterType_None;
        u8 buffer[BufferSize];
        s32 result;
        do{
            context.availOut_ = BufferSize;
            context.nextOut_ = buffer;
            result = szlib::inflate(&context);

            switch(result)
            {
            case szlib::SZ_ERROR_MEMORY:
            case szlib::SZ_ERROR_FORMAT:
                szlib::termInflate(&context);
                return false;
            default:
                break;
            };
            u32 outSize = static_cast<u32>(context.thisTimeOut_);
            u8* dst = buffer;
            //Copy inflated data to scanline
            while(0<outSize && totalCount_<totalSize_){
                if(scanlineOffset<=0){
                    --outSize;
                    filterFlag = dst[0];
                    ++dst;
                }

                u32 copySize = ((scanlineOffset+outSize) < scanlineSize)? outSize : scanlineSize-scanlineOffset;
                memcpy(scanline+scanlineOffset, dst, copySize);
                scanlineOffset += copySize;

                //Scanline have been filled up, apply filter
                if(scanlineSize<=scanlineOffset){
                    CPPIMG_ASSERT(scanlineSize == scanlineOffset);
                    filter(scanlineSize, filterFlag, scanline, reinterpret_cast<u8*>(image));
                    filterFlag = -1;
                    scanline += scanlineSize;
                    scanlineOffset = 0;
                }
                totalCount_ += copySize;
                outSize -= copySize;
                dst += copySize;
            }
        }while(szlib::SZ_END != result && totalCount_<totalSize_);
        szlib::termInflate(&context);
        return true;
    }

#if 0
    bool PNG::ChunkIDAT::write(Stream& stream, s32 height, const u8* image)
    {
        struct ReleaseMemory
        {
            ReleaseMemory(u8* ptr)
                :ptr_(ptr)
            {}
            ~ReleaseMemory()
            {
                CPPIMG_FREE(ptr_);
            }
            u8* ptr_;
        };

        s32 lenPos = stream.tell();
        u32 outCount = 0;
        if(stream.write(sizeof(u32), &outCount)<=0){
            return false;
        }
        u32 type = Type;
        if(stream.write(sizeof(u32), &type)<=0){
            return false;
        }
        u32 crc = 0xFFFFFFFFUL;
        crc = updateCRC32(crc, sizeof(u32), reinterpret_cast<const u8*>(&type));

        //Add 1 byte of filter type for each scanlines
        //---------------------------------
        u32 totalSize = totalSize_ + height;

        u8* in = reinterpret_cast<u8*>(CPPIMG_MALLOC(totalSize));
        ReleaseMemory releaseInMemory(in);

        {
            u8 filter = 0;
            const u8* src = image;
            u8* dst = in;
            for(s32 i=0; i<height; ++i){
                dst[0] = filter;
                ++dst;
                memcpy(dst, src, scanlineSize_);
                src += scanlineSize_;
                dst += scanlineSize_;
            }
        }

        //---------------------------------
        u32 inCount = 0;
        s32 result;
        s32 flush;
        do{
            if(totalSize<=inCount){
                break;
            }
            u32 size = totalSize;
            if(totalSize<(inCount+size)){
                size = totalSize-inCount;
            }
            stream_.avail_in = size;
            stream_.next_in = in+inCount;
            inCount += size;
            flush = (totalSize<=inCount)? MZ_FINISH : MZ_NO_FLUSH;
            do{
                stream_.avail_out = BufferSize;
                stream_.next_out = dst_;
                u64 totalOut = stream_.total_out;
                result = mz_deflate(&stream_, flush);
                if(MZ_OK != result && MZ_STREAM_END != result){
                    return false;
                }
                u32 outSize = static_cast<u32>(stream_.total_out - totalOut);
                if(0<outSize){
                    crc = updateCRC32(crc, outSize, dst_);
                    if(stream.write(outSize, dst_)<=0){
                        return false;
                    }
                }
                outCount += outSize;
            }while(MZ_STREAM_END != result);
        }while(flush != MZ_FINISH);

        s32 crcPos = stream.tell();
        stream.seek(lenPos, SEEK_SET);
        outCount = reverse(outCount);
        if(stream.write(sizeof(u32), &outCount)<=0){
            return false;
        }
        stream.seek(crcPos, SEEK_SET);
        crc ^= 0xFFFFFFFFUL;
        crc = reverse(crc);
        return stream.write(sizeof(u32), &crc)<=0;
    }
#endif

    void PNG::ChunkIDAT::filter(u32 scanlineSize, s32 filterFlag, u8* scanline, u8* image)
    {
        CPPIMG_ASSERT(image<=scanline && scanline<(image+totalSize_));
        u32 components = color_+alpha_;
        switch(filterFlag)
        {
        case FilterType_Sub:
            for(u32 i=1; i<width_; ++i){
                u32 p = i*components;
                for(u32 j = 0; j<components; ++j, ++p){
                    s32 x = scanline[p] + scanline[p-components];
                    scanline[p] = static_cast<u8>(x&255);
                }
            }
            break;
        case FilterType_Up:
            if(image<scanline){
                u8* upper = scanline-scanlineSize;
                for(u32 i=0; i<width_; ++i){
                    u32 p = i*components;
                    for(u32 j = 0; j<components; ++j, ++p){
                        s32 x = upper[p] + scanline[p];
                        scanline[p] = static_cast<u8>(x&255);
                    }
                }
            }
            break;
        case FilterType_Avg:
            if(image<scanline){
                u8* upper = scanline-scanlineSize;
                for(u32 i=1; i<width_; ++i){
                    u32 p = i*components;
                    for(u32 j = 0; j<components; ++j, ++p){
                        s32 x = ((scanline[p-components] + upper[p])>>1) + scanline[p];
                        scanline[p] = static_cast<u8>(x&255);
                    }
                }
            }else{
                for(u32 i=1; i<width_; ++i){
                    u32 p = i*components;
                    for(u32 j = 0; j<components; ++j, ++p){
                        s32 x = (scanline[p-components]>>1) + scanline[p];
                        scanline[p] = static_cast<u8>(x&255);
                    }
                }
            }
            break;
        case FilterType_Paeth:
        {
            u8* upper = scanline-scanlineSize;
            for(u32 i=0; i<width_; ++i){
                u32 p = i*components;
                for(u32 j = 0; j<components; ++j, ++p){
                    s32 a = (0<i)? scanline[p-components] : 0;
                    s32 b = (image<scanline)? upper[p] : 0;
                    s32 c = (0<i && image<scanline)? upper[p-components] : 0;
                    s32 x = a+b-c;
                    s32 pa = absolute(x-a);
                    s32 pb = absolute(x-b);
                    s32 pc = absolute(x-c);
                    if(pa<=pb && pa<=pc){
                        x = a;
                    } else{
                        x = (pb<=pc)? b : c;
                    }
                    x += scanline[p];
                    scanline[p] = static_cast<u8>(x&255);
                }
            }
        }
        break;
        default:
            break;
        }
    }

    //--- PNG
    //----------------------------------------------------
    inline u16 PNG::reverse(u16 x)
    {
        return ((x>>8)&0xFFU) | ((x<<8)&0xFF00U);
    }

    inline u32 PNG::reverse(u32 x)
    {
        return ((x>>24)) | ((x>>8)&0xFF00U) | ((x&0xFF00U)<<8) | ((x&0xFFU)<<24);
    }

    bool PNG::readHeader(Stream& stream)
    {
        u64 signature;
        if(stream.read(sizeof(u64), &signature)<0){
            return false;
        }
        if(Signature != signature){
            return false;
        }
        return true;
    }

    bool PNG::checkCRC32(const Chunk& chunk, Stream& stream)
    {
        static const u32 BufferSize = 1024;
        u8 buffer[BufferSize];

        u32 crc = 0xFFFFFFFFUL;
        crc = updateCRC32(crc, sizeof(u32), reinterpret_cast<const u8*>(&chunk.type_));
        off_t pos = stream.tell();
        u32 remain=chunk.length_;
        while(0<remain){
            u32 size = (remain<BufferSize)? remain : BufferSize;
            if(stream.read(size, buffer)<0){
                break;
            }
            crc = updateCRC32(crc, size, buffer);
            remain -= size;
        }

        //Check whether calculated crc equals loaded crc
        u32 readCrc = 0;
        s32 ret = (remain<=0)? stream.read(sizeof(u32), &readCrc) : 0;
        stream.seek(pos, SEEK_SET);
        if(ret<0){
            return false;
        }
        crc ^= 0xFFFFFFFFUL;
        readCrc = reverse(readCrc);
        return readCrc == crc;
    }

    bool PNG::skipChunk(const Chunk& chunk, Stream& stream)
    {
        u32 size = chunk.length_ + 4;//chunk data size + crc size
        return stream.seek(size, SEEK_CUR);
    }

    template<class T>
    void PNG::setChunkHeader(T& dst, const Chunk& src)
    {
        dst.type_ = src.type_;
        dst.length_ = src.length_;
    }

    template<class T>
    bool PNG::readHeader(T& chunk, Stream& stream)
    {
        if(stream.read(sizeof(u32)*2, &chunk)<0){
            return false;
        }
        chunk.length_ = reverse(chunk.length_);
        if(!checkCRC32(chunk, stream)){
            return false;
        }
        return true;
    }

    bool PNG::writeChunk(Stream& stream, u32 type, u32 size, const void* data)
    {
        u32 crc = 0xFFFFFFFFUL;
        crc = updateCRC32(crc, sizeof(u32), reinterpret_cast<const u8*>(&type));
        crc = updateCRC32(crc, size, reinterpret_cast<const u8*>(data));
        crc ^= 0xFFFFFFFFUL;

        u32 rcrc = reverse(crc);
        u32 rsize = reverse(size);

        if(stream.write(sizeof(u32), &rsize)<=0){
            return false;
        }
        if(stream.write(sizeof(u32), &type)<=0){
            return false;
        }
        if(0<size){
            if(stream.write(size, data)<=0){
                return false;
            }
        }
        if(stream.write(sizeof(u32), &rcrc)<=0){
            return false;
        }
        return true;
    }
#endif

    //----------------------------------------------------
    //---
    //--- JPEG
    //---
    //----------------------------------------------------
    inline void JPEG::ByteStream::reset()
    {
        bits_ = 0;
        byte_ = 0;
    }

    inline s32 JPEG::ByteStream::read(size_t size, void* bytes)
    {
        return stream_->read(size, bytes);
    }

    inline s32 JPEG::ByteStream::write(size_t size, void* bytes)
    {
        return stream_->write(size, bytes);
    }

    inline bool JPEG::ByteStream::skip(size_t size)
    {
        return stream_->seek(size, SEEK_CUR);
    }

    s32 JPEG::ByteStream::readByte()
    {
        u8 b = 0;
        if(stream_->read(1, &b)<=0){
            return -1;
        }
        if(b != 0xFFU){
            return b;
        }
        do{
            if(stream_->read(1, &b)<=0){
                return -1;
            }
        }while(b == 0xFFU);
        if(0 == b){
            return 0xFFU;
        }
        return b;
    }

    s32 JPEG::ByteStream::readBits(s32 bits)
    {
        u16 result = 0;
        while(0<bits){
            if(bits_<=0){
                s32 b = readByte();
                if(b<0){
                    return -1;
                }
                byte_ = static_cast<u8>(b);
                bits_ = 8;
            }
            s32 n = minimum(bits_, bits);
            bits -= n;
            bits_ -= n;
            result = (result<<n) | (byte_>>(8-n));
            byte_ <<= n;
        }
        return result;
    }

    //----------------------------------------------------
    s32 JPEG::ByteStream::writeBits(s32 bits, s32 value)
    {
        while(0<bits){
            if(bits_<=0){
                if(stream_->write(1, &byte_)<=0){
                    return -1;
                }
                if(0xFFU == byte_){
                    byte_ = 0;
                    if(stream_->write(1, &byte_)<=0){
                        return -1;
                    }
                } else{
                    byte_ = 0;
                }
                bits_ = 8;
            }
            s32 n = minimum(bits_, bits);
            bits -= n;
            bits_ -= n;
            u8 mask = static_cast<u8>((1<<n)-1);
            byte_ = (byte_<<n) | (value & mask);
            value >>= n;
        }
        return 1;
    }

    s32 JPEG::ByteStream::fillByte()
    {
        if(bits_<=0){
            return 1;
        }
        s32 n = bits_;
        s32 value = (1<<n)-1;
        return writeBits(n, value);
    }

    bool JPEG::ByteStream::readSegment(Segment& segment)
    {
        if(stream_->read(4, &segment)<=0){
            return false;
        }
        if(0xFFU != segment.ff_){
            return false;
        }
        segment.length_ = swapEndian(segment.length_);
        return 2<=segment.length_;
    }

    bool JPEG::ByteStream::readMarker(u8& marker)
    {
        if(stream_->read(sizeof(u8), &marker)<0){
            return false;
        }
        if(0xFFU != marker){
            return false;
        }
        return 0<=stream_->read(sizeof(u8), &marker);
    }

    bool JPEG::ByteStream::read16(u16& x)
    {
        if(0<=stream_->read(sizeof(u16), &x)){
            x = swapEndian(x);
            return true;
        }
        return false;
    }

    inline bool JPEG::ByteStream::skipSegment(const Segment& segment)
    {
        CPPIMG_ASSERT(2<=segment.length_);
        return stream_->seek(segment.length_-2, SEEK_CUR);
    }

    //----------------------------------------------------
    bool JPEG::FrameHeader::getMaxSampling()
    {
        maxVerticalSampling_ = 0;
        maxHorizontalSampling_ = 0;
        for(s32 i=0; i<numComponents_; ++i){
            maxVerticalSampling_ = maximum(maxVerticalSampling_, components_[i].getVertical());
            maxHorizontalSampling_ = maximum(maxHorizontalSampling_, components_[i].getHorizontal());
        }
        return 0<maxVerticalSampling_ && 0<maxHorizontalSampling_;
    }

    //----------------------------------------------------
    const u8 JPEG::ZigZag[BLOCK_SIZE] =
    {
        0,  1,  8, 16,  9,  2,  3, 10,
        17, 24, 32, 25, 18, 11,  4,  5,
        12, 19, 26, 33, 40, 48, 41, 34,
        27, 20, 13,  6,  7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36,
        29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46,
        53, 60, 61, 54, 47, 55, 62, 63,
    };

    bool JPEG::read(s32& width, s32& height, ColorType& colorType, void* image, Stream& stream)
    {
        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);
        {
            Segment soi;
            if(stream.read(sizeof(u16), &soi.ff_)<=0){
                return false;
            }
            if(MARKER_SOI != soi.marker_){
                return false;
            }
        }

        colorType = ColorType_RGB;
        Context* context = reinterpret_cast<Context*>(CPPIMG_MALLOC(sizeof(Context)));
        AutoFree autoFree(context);
        CPPIMG_MEMSET(context, 0, sizeof(Context));
        context->flags_ |= Flag_SOI;
        context->byteStream_ = ByteStream(&stream);
        bool loop = true;
        Segment segment;
        while(loop){
            if(!context->byteStream_.readSegment(segment)){
                return false;
            }
            if(MARKER_EOI == segment.marker_){
                loop = false;
                break;
            }

            switch(segment.marker_){
            case MARKER_SOI:
                return false;

            case MARKER_DQT:
                if(!readDQT(*context, segment)){
                    return false;
                }
                context->flags_ |= Flag_DQT;
                break;
            case MARKER_DHT:
                if(!readDHT(*context, segment)){
                    return false;
                }
                context->flags_ |= Flag_DHT;
                break;
            case MARKER_DRI:
                if(!readDRI(*context, segment)){
                    return false;
                }
                break;
            case MARKER_DNL:
                if(!readDNL(*context, segment)){
                    return false;
                }
                break;
            case MARKER_SOF0:
            case MARKER_SOF1:
            case MARKER_SOF2:
            case MARKER_SOF3:
            case MARKER_SOF5:
            case MARKER_SOF6:
            case MARKER_SOF7:
            case MARKER_SOF9:
            case MARKER_SOFA:
            case MARKER_SOFB:
            case MARKER_SOFD:
            case MARKER_SOFE:
            case MARKER_SOFF:
                if(!readSOF(*context, segment)){
                    return false;
                }
                break;
            case MARKER_APP0:
                if(!readJFXX(*context, segment)){
                    return false;
                }
                break;
            case MARKER_SOS:
                if(!readSOS(*context, segment)){
                    return false;
                }
                loop = false;
                context->flags_ |= Flag_SOS;
                break;
            default:
                if(!context->byteStream_.skipSegment(segment)){
                    return false;
                }
                break;
            }
        };
        if(Flag_NEEDS != (context->flags_ & Flag_NEEDS)){
            return false;
        }

        width = context->frame_.width_;
        height = context->frame_.height_;
        // Support only gray and YCrCb formats
        switch(context->frame_.numComponents_){
        case 1:
            colorType = ColorType_GRAY;
            break;
        case 3:
            colorType = ColorType_RGB;
            break;
        default:
            return false;
        }
        if(CPPIMG_NULL == image){
            return true;
        }

        context->rgb_ = reinterpret_cast<u8*>(image);
        context->createCosTable();
        context->work_ = reinterpret_cast<s16*>(CPPIMG_MALLOC(sizeof(s16)*width*height*context->frame_.numComponents_));
        if(!decode(*context)){
            return false;
        }
        switch(context->frame_.numComponents_){
        case 1:
            toGray(*context);
            break;
        case 3:
            toRGB(*context);
            break;
        default:
            return false;
        }
        seekSet.clear();
        return true;
    }

    bool JPEG::readDQT(Context& context, const Segment& segment)
    {
        ByteStream& stream = context.byteStream_;

        s32 size = 2;
        while(size<segment.length_){
            u8 precisionAndNumber;
            if(stream.read(1, &precisionAndNumber)<=0){
                return false;
            }
            ++size;
            s32 precision = (precisionAndNumber>>4) & 0xFU;
            if(1<precision){
                return false;
            }
            s32 id = precisionAndNumber & 0xFU;
            if(4<=id){
                return false;
            }
            
            QuantizationTable& quantization = context.quantization_[id];
            quantization.precisionAndNumber_ = precisionAndNumber;

            if(precision<1){
                u8 factors8[QT_SIZE];
                if(stream.read(QT_SIZE, factors8)<=0){
                    return false;
                }
                for(s32 i=0; i<QT_SIZE; ++i){
                    quantization.factors_[ZigZag[i]] = factors8[i];
                }
                size += QT_SIZE;
            }else{
                u16 factors16[QT_SIZE];
                if(stream.read(QT_SIZE*2, factors16)<=0){
                    return false;
                }
                for(s32 i=0; i<QT_SIZE; ++i){
                    quantization.factors_[ZigZag[i]] = swapEndian(factors16[i]);
                }
                size += QT_SIZE*2;
            }
        }
        return size==segment.length_;
    }

    bool JPEG::readDHT(Context& context, const Segment& segment)
    {
        ByteStream& stream = context.byteStream_;

        u8 bits[HT_BITS_TABLE];
        s32 size = 2;
        while(size<segment.length_){
            u8 classAndId; //upper:precision, lower:id of huffman table
            if(stream.read(1, &classAndId)<=0){
                return false;
            }
            ++size;
            s32 huffmanClass = (classAndId>>4) & 0xFU;
            if(2<=huffmanClass){
                return false;
            }
            s32 id = classAndId & 0xFU;
            if(4<=id){
                return false;
            }

            if(stream.read(HT_BITS_TABLE, bits)<=0){
                return false;
            }
            size += HT_BITS_TABLE;

            HuffmanTable& table = context.huffman_[huffmanClass][id];
            //Count number of elements
            table.number_ = 0;
            for(s32 i=0; i<HT_BITS_TABLE; ++i){
                table.number_ += bits[i];
            }
            if(HT_MAX_SIZE<table.number_){
                return false;
            }

            //Generate size table
            s32 k=0;
            for(u8 i=1; i<=HT_BITS_TABLE; ++i){
                for(u8 j=1; j<=bits[i-1]; ++j, ++k){
                    table.size_[k] = i;
                }
            }

            //Generate code table
            k=0;
            u16 code = 0;
            u8 si = table.size_[0];
            for(;;){
                for(;table.size_[k] == si; ++k,++code){
                    table.code_[k] = code;
                }
                if(table.number_<=k){
                    break;
                }
                do{
                    code <<= 1;
                    ++si;
                }while(table.size_[k] != si);
            }
            if(stream.read(table.number_, table.value_)<=0){
                return false;
            }
            size += table.number_;

        } //while(size<segment.length_)
        return size==segment.length_;
    }

    bool JPEG::readDRI(Context& context, const Segment& segment)
    {
        ByteStream& stream = context.byteStream_;

        s32 size = 2;
        while(size<segment.length_){
            if(!stream.read16(context.restartInterval_)){
                return false;
            }
            size += 2;
        }
        return size==segment.length_;
    }

    bool JPEG::readDNL(Context& context, const Segment& segment)
    {
        ByteStream& stream = context.byteStream_;
        s32 size = 2;
        while(size<segment.length_){
            if(!stream.read16(context.numberOfLines_)){
                return false;
            }
            size += 2;
        }
        return size==segment.length_;
    }

    bool JPEG::readSOF(Context& context, const Segment& segment)
    {
        ByteStream& stream = context.byteStream_;
        if(stream.read(1, &context.frame_.precision_)<=0){
            return false;
        }
        if(!stream.read16(context.frame_.height_)){
            return false;
        }
        if(!stream.read16(context.frame_.width_)){
            return false;
        }
        if(stream.read(1, &context.frame_.numComponents_)<=0){
            return false;
        }

        if(context.frame_.numComponents_<=0 || MAX_COMPONENTS<context.frame_.numComponents_){
            return false;
        }
        s32 count = 0;
        s32 size = 8;
        while(size<segment.length_ && count<context.frame_.numComponents_){
            if(stream.read(3, &context.frame_.components_[count])<=0){
                return false;
            }
            if(context.frame_.components_[count].getVertical()<=0 || context.frame_.components_[count].getHorizontal()<=0){
                return false;
            }
            ++count;
            size += 3;
        }
        return size==segment.length_;
    }

    bool JPEG::readSOS(Context& context, const Segment& segment)
    {
        ByteStream& stream = context.byteStream_;
        if(stream.read(1, &context.scan_.numComponents_)<=0){
            return false;
        }
        s32 size = 3;
        s32 segmentSize = segment.length_ - 3;
        s32 count = 0;
        while(size<segmentSize && count<context.scan_.numComponents_){
            if(stream.read(sizeof(Scan::Component), &context.scan_.components_[count])<=0){
                return false;
            }
            ++count;
            size += sizeof(Scan::Component);
        }
        if(stream.read(1, &context.scan_.startSpectra_)<=0){
            return false;
        }
        if(stream.read(1, &context.scan_.endSpectra_)<=0){
            return false;
        }
        if(stream.read(1, &context.scan_.pointTransform_)<=0){
            return false;
        }
        size += 3;
        return size==segment.length_;
    }

    bool JPEG::readJFXX(Context& context, const Segment& segment)
    {
        ByteStream& stream = context.byteStream_;
        s32 size = 2;
        if(stream.read(5, context.jfxx_.id_)<=0){
            return false;
        }
        context.jfxx_.id_[4] = CPPIMG_NULLCHAR;
        size += 5;

        if(0==strcmp(reinterpret_cast<const char*>(context.jfxx_.id_), "JFIF")){
            JFXX::IF& jfif = context.jfxx_.format_.jfif_;
            if(stream.read(2, &jfif.version_)<=0){
                return false;
            }
            if(stream.read(1, &jfif.unit_)<=0){
                return false;
            }
            if(stream.read(2, &jfif.xdensity_)<=0){
                return false;
            }
            if(stream.read(2, &jfif.ydensity_)<=0){
                return false;
            }
            if(stream.read(1, &jfif.xthumb_)<=0){
                return false;
            }
            if(stream.read(1, &jfif.ythumb_)<=0){
                return false;
            }
            size += 9;
            s32 thumbSize = 3 * jfif.xthumb_ * jfif.ythumb_;
            if(!stream.skip(thumbSize)){
                return false;
            }
            size += thumbSize;

        }else if(0==strcmp(reinterpret_cast<const char*>(context.jfxx_.id_), "JFXX")){
            JFXX::XX& jfxx = context.jfxx_.format_.jfxx_;
            if(stream.read(1, &jfxx.type_)<=0){
                return false;
            }
            size += (segment.length_ - (2+5+1));
        }
        return size == segment.length_;
    }

    bool JPEG::decode(Context& context)
    {
        s32 vblocks = blocks(context.frame_.height_);
        s32 hblocks = blocks(context.frame_.width_);
        if(!context.frame_.getMaxSampling()){
            return false;
        }

        s32 maxVSampling = context.frame_.maxVerticalSampling_;
        s32 maxHSampling = context.frame_.maxHorizontalSampling_;
        s32 vUnits = vblocks/maxVSampling;
        s32 hUnits = hblocks/maxHSampling;
        if(0<(vblocks - (maxVSampling*vUnits))){
            ++vUnits;
        }
        if(0<(hblocks - (maxHSampling*hUnits))){
            ++hUnits;
        }

        s32 unitSize = maxVSampling * maxHSampling * BLOCK_SIZE;
        CPPIMG_MEMSET(&context.components_[0], 0x00U, sizeof(u8)*unitSize);
        CPPIMG_MEMSET(&context.components_[1], 0x00U, sizeof(s16)*unitSize);
        CPPIMG_MEMSET(&context.components_[2], 0x00U, sizeof(s16)*unitSize);
        CPPIMG_MEMSET(&context.components_[3], 0x00U, sizeof(s16)*unitSize);

        context.directCurrents_[0] = 0;
        context.directCurrents_[1] = 0;
        context.directCurrents_[2] = 0;
        context.directCurrents_[3] = 0;

        ByteStream& stream = context.byteStream_;

        s32 restartCount = 0;
        for(s32 uy=0; uy<vUnits; ++uy){
            for(s32 ux=0; ux<hUnits; ++ux){
                if(!decodeMCU(context)){
                    return false;
                }
                copyComponents(context, ux, uy);
                if(0<context.restartInterval_){
                    if(context.restartInterval_ <= ++restartCount){
                        restartCount = 0;
                        stream.reset();
                        u8 marker;
                        if(!stream.readMarker(marker)){
                            return false;
                        }
                        if(MARKER_RST0<=marker && marker<=MARKER_RST7){
                            //Reset DC
                            context.directCurrents_[0] = 0;
                            context.directCurrents_[1] = 0;
                            context.directCurrents_[2] = 0;
                            context.directCurrents_[3] = 0;
                        }
                    }
                }
            }
        }
        return true;
    }

    bool JPEG::decodeMCU(Context& context)
    {
        s32 unitWidth = context.frame_.maxHorizontalSampling_ << BLOCK_SHIFT;

        for(s32 i=0; i<context.frame_.numComponents_; ++i){
            s32 vSampling = context.frame_.components_[i].getVertical();
            s32 hSampling = context.frame_.components_[i].getHorizontal();
            s32 vDuplicate = context.frame_.maxVerticalSampling_/vSampling;
            s32 hDuplicate = context.frame_.maxHorizontalSampling_/hSampling;
            s32 vCopy = vDuplicate << BLOCK_SHIFT;
            s32 hCopy = hDuplicate << BLOCK_SHIFT;
            for(s32 v=0; v<vSampling; ++v){
                for(s32 h=0; h<hSampling; ++h){
                    if(!decodeHuffmanBlock(context, i)){
                        return false;
                    }
                    inverseQuantization(context, i);
                    inverseDCT(context);
                    s16* component = context.components_[i] + unitWidth*(v << BLOCK_SHIFT) + (h << BLOCK_SHIFT);
                    for(s32 j=0; j<vCopy; ++j){
                        for(s32 k=0; k<hCopy; ++k){
                            component[j*unitWidth + k] = context.block_[((j/vDuplicate) << BLOCK_SHIFT) + (k/hDuplicate)];
                        }
                    }
                }
            }
        }
        return true;
    }

    s32 JPEG::decodeHuffmanCode(Context& context, s32 type, s32 table)
    {
        ByteStream& stream = context.byteStream_;

        const HuffmanTable& huffmanTable = context.huffman_[type][table];
        u16 code = 0;
        u16 length = 0;
        s32 next = 0;
        s32 k = 0;
        while(k<huffmanTable.number_ && length<HT_BITS_TABLE){
            ++length;
            code <<= 1;
            next = stream.readBits(1);
            if(next<0){
                return -1;
            }
            code |= next;
            for(;huffmanTable.size_[k] == length; ++k){
                if(huffmanTable.code_[k] == code){
                    return huffmanTable.value_[k];
                }
            }
        }
        return -1;
    }

    bool JPEG::decodeHuffmanBlock(Context& context, s32 component)
    {
        ByteStream& stream = context.byteStream_;

        //DC
        s32 category = decodeHuffmanCode(context, 0, context.scan_.components_[component].getDCHuffman());
        if(category<0){
            return false;
        }else if(0<category){
            s16 difference = static_cast<s16>(stream.readBits(category));
            if(0 == (difference & (1<<(category-1)))){
                difference -= (1<<category) - 1;
            }
            context.directCurrents_[component] += difference;
        }
        context.dct_[0] = context.directCurrents_[component];

        //AC
        for(s32 k=1; k<BLOCK_SIZE;){
            s32 runCategory = decodeHuffmanCode(context, 1, context.scan_.components_[component].getACHuffman());
            if(runCategory<0){
                return false;
            }else if(0==runCategory){
                while(k<BLOCK_SIZE){
                    context.dct_[ZigZag[k++]] = 0;
                }
                break;
            }
            s32 runLength = runCategory>>4;
            category = runCategory & 0x0FU;
            s16 ac = 0;
            if(0 != category){
                ac = static_cast<s16>(stream.readBits(category));
                if(0 == (ac & (1<<(category-1)))){
                    ac -= (1<<category) - 1;
                }
            }else if(15 != runLength){
                return false;
            }
            if(BLOCK_SIZE<=(runLength+k)){
                //runLength = (BLOCK_SIZE - k - 1);
                return false;
            }
            while(0<runLength){
                context.dct_[ZigZag[k++]] = 0;
                --runLength;
            }
            context.dct_[ZigZag[k++]] = ac;
        }
        return true;
    }

    void JPEG::inverseQuantization(Context& context, s32 component)
    {
        s32 qt = context.frame_.components_[component].quantizationTable_;
        const QuantizationTable& table = context.quantization_[qt];

        for(s32 i=0; i<BLOCK_SIZE; ++i){
            context.dct_[i] *= table.factors_[i];
        }
    }

    void JPEG::inverseDCT(Context& context)
    {
        s32 levelShift = context.frame_.precision_<=8? 128 : 2048;
        levelShift <<= FIXED_POINT_SHIFT2;
        for(s32 y=0; y<BLOCK_WIDTH; ++y){
            s16* block = context.block_ + y*BLOCK_WIDTH;
            for(s32 x=0; x<BLOCK_WIDTH; ++x){
                const s16* csy = &context.cosine_[y][0];
                const s16* csx = &context.cosine_[x][0];
                s32 sum = 0;
                for(s32 v=0; v<BLOCK_WIDTH; ++v){
                    s32 tcsy = csy[v];
                    const s16* dct = context.dct_ + v*BLOCK_WIDTH;
                    for(s32 u=0; u<BLOCK_WIDTH; ++u){
                        s32 c = fixedPointRound(tcsy*csx[u], FIXED_POINT_SHIFT);
                        sum += dct[u] * c;
                    }
                }
                block[x] = static_cast<s16>(fixedPointRound(sum, FIXED_POINT_SHIFT2+2) + levelShift);
            }
        }
    }

    void JPEG::copyComponents(Context& context, s32 ux, s32 uy)
    {
        s16* C[4];
        C[0] = context.components_[0];
        C[1] = context.components_[1];
        C[2] = context.components_[2];
        C[3] = context.components_[3];

        s32 pixelV = uy * (context.frame_.maxVerticalSampling_ << 3);
        s32 pixelH = ux * (context.frame_.maxHorizontalSampling_ << 3);

        CPPIMG_ASSERT(pixelV<context.frame_.height_);
        CPPIMG_ASSERT(pixelH<context.frame_.width_);

        s32 endy = context.frame_.maxVerticalSampling_ << 3;
        s32 endx = context.frame_.maxHorizontalSampling_ << 3;
        s32 stepx = endx;
        if(context.frame_.height_<=(pixelV+endy)){
            endy = context.frame_.height_ - pixelV;
        }
        if(context.frame_.width_<=(pixelH+endx)){
            endx = context.frame_.width_ - pixelH;
        }

        s32 offset = (pixelV * context.frame_.width_ + pixelH) * context.frame_.numComponents_;
        s16* work = context.work_ + offset;

        for(s32 y=0; y<endy; ++y){
            for(s32 x=0; x<endx; ++x){
                s32 index = (y * context.frame_.width_ + x) * context.frame_.numComponents_;
                for(s32 c=0; c<context.frame_.numComponents_; ++c){
                    work[index + c] = C[c][x];
                }
            }
            C[0] += stepx;
            C[1] += stepx;
            C[2] += stepx;
            C[3] += stepx;
        }
    }

    void JPEG::toGray(Context& context)
    {
        s32 repeatH = context.frame_.maxHorizontalSampling_ / context.frame_.components_[0].getHorizontal();
        s32 repeatV = context.frame_.maxVerticalSampling_ / context.frame_.components_[0].getVertical();
        s32 endX = (context.frame_.width_/repeatH) * repeatH;
        s32 endY = (context.frame_.height_/repeatV) * repeatV;
        s16 ratioH = static_cast<s16>(1.0f/repeatH * (f32)(1<<FIXED_POINT_SHIFT));
        s16 ratioV = static_cast<s16>(1.0f/repeatV * (f32)(1<<FIXED_POINT_SHIFT));
        s32 step = context.frame_.width_;

        if(1<repeatH || 1<repeatV){
            s16* Y = context.work_;
            for(s32 y=0; y<context.frame_.height_; y+=repeatV){
                for(s32 x=0; x<endX; x+=repeatH){
                    for(s32 c=1; c<repeatH; ++c){
                        s32 value = Y[x] * ratioH * (repeatH-c) + Y[x+repeatH] * ratioH * (c);
                        Y[x+c] = fixedPointRound<s16>(value, FIXED_POINT_SHIFT);
                    }
                }
                if(y<endY){
                    for(s32 x=0; x<context.frame_.width_; ++x){
                        s16* NY = Y + step*repeatV;
                        for(s32 c=1; c<repeatV; ++c){
                            s32 value = Y[x] * ratioV * (repeatV-c) + NY[x]* ratioV * (c);
                            Y[x+c*step] = fixedPointRound<s16>(value, FIXED_POINT_SHIFT);
                        }
                    }
                }
            }
        }
        for(s32 y=0; y<context.frame_.height_; ++y){
            for(s32 x=0; x<context.frame_.width_; ++x){
                s32 index = (y * context.frame_.width_ + x);
                s32 ty = fixedPointRound(context.work_[index], FIXED_POINT_SHIFT2);
                context.rgb_[index] = clamp8bits(ty);
            }
        }
    }

    void JPEG::toRGB(Context& context)
    {
        for(s32 c=0; c<context.frame_.numComponents_; ++c){
            filter(context, c);
        }

        s32 step = context.frame_.width_ * context.frame_.numComponents_;
        s16* C = context.work_;
        for(s32 y=0; y<context.frame_.height_; ++y){
            s32 row = y * step;
            for(s32 x=0; x<context.frame_.width_; ++x){
                s32 index = row + x * context.frame_.numComponents_;
                s32 ty = C[index+0]<<FIXED_POINT_SHIFT2;
                s32 tcb = C[index+1];
                s32 tcr = C[index+2];
                s32 tr = ty + fixedPointShift((tcr - 0x2000)*0x166E, FIXED_POINT_SHIFT2); //Y + (Cr-128)*1.4020
                s32 tg = ty - fixedPointShift((tcb - 0x2000)*0x0581 + (tcr - 0x2000)*0x0B6C, FIXED_POINT_SHIFT2); //Y - (Cb-128)*0.3441 - (Cbr-128)*0.7139;
                s32 tb = ty + fixedPointShift((tcb - 0x2000)*0x1C59, FIXED_POINT_SHIFT2); //Y - (Cb-128)*1.7718;
                tr = fixedPointRound(tr, FIXED_POINT_SHIFT);
                tg = fixedPointRound(tg, FIXED_POINT_SHIFT);
                tb = fixedPointRound(tb, FIXED_POINT_SHIFT);

                context.rgb_[index+0] = clamp8bits(tr);
                context.rgb_[index+1] = clamp8bits(tg);
                context.rgb_[index+2] = clamp8bits(tb);
            }
        }
    }

    void JPEG::filter(Context& context, s32 component)
    {
        s32 repeatH = context.frame_.maxHorizontalSampling_ / context.frame_.components_[component].getHorizontal();
        s32 repeatV = context.frame_.maxVerticalSampling_ / context.frame_.components_[component].getVertical();
        if(repeatH<=1 && repeatV<=1){
            return;
        }
        s32 endX = (context.frame_.width_/repeatH) * repeatH;
        s32 endY = (context.frame_.height_/repeatV) * repeatV;
        s16 ratioH = static_cast<s16>(1.0f/repeatH * (f32)(1<<FIXED_POINT_SHIFT));
        s16 ratioV = static_cast<s16>(1.0f/repeatV * (f32)(1<<FIXED_POINT_SHIFT));
        s32 numComponents = context.frame_.numComponents_;
        s32 step = context.frame_.width_ * numComponents;

        s16* C = context.work_ + component;
        for(s32 y=0; y<context.frame_.height_; y+=repeatV){
            s32 row = y*step;
            for(s32 x=0; x<endX; x+=repeatH){
                s32 i0 = row + x*numComponents;
                s32 i1 = i0 + repeatH*numComponents;
                for(s32 c=1; c<repeatH; ++c){
                    s32 value = C[i0] * ratioH * (repeatH-c) + C[i1] * ratioH * (c);
                    C[i0+c*numComponents] = fixedPointRound<s16>(value, FIXED_POINT_SHIFT);
                }
            }
            if(y<endY){
                for(s32 x=0; x<context.frame_.width_; ++x){
                    s32 i0 = row + x*numComponents;
                    s32 i1 = i0 + repeatV * step;
                    for(s32 c=1; c<repeatV; ++c){
                        s32 value = C[i0] * ratioV * (repeatV-c) + C[i1] * ratioV * (c);
                        C[i0+c*step] = fixedPointRound<s16>(value, FIXED_POINT_SHIFT);
                    }
                }
            }
        }//for(s32 y=0;
    }

    //----------------------------------------------------
    //---
    //--- OpenEXR
    //---
    //----------------------------------------------------
    OpenEXR::Version OpenEXR::Version::create(
        bool tile,
        bool longName,
        bool deepData,
        bool multiPart)
    {
        Version version;
        version.version_ = 0x02U;
        if(tile){
            version.version_ |= Flag_TileFormat;
        }
        if(longName){
            version.version_ |= Flag_LongName;
        }
        if(deepData){
            version.version_ |= Flag_DeepData;
        }
        if(multiPart){
            version.version_ |= Flag_MultiPart;
        }
        return version;
    }

    OpenEXR::Preview::Preview()
        :width_(0)
        ,height_(0)
        ,image_(CPPIMG_NULL)
    {}

    OpenEXR::Preview::~Preview()
    {
        CPPIMG_FREE(image_);
    }

    bool OpenEXR::Preview::read(Stream& stream)
    {
        if(stream.read(sizeof(s32), &width_)<=0){
            return false;
        }
        if(stream.read(sizeof(s32), &height_)<=0){
            return false;
        }
        s32 size = width_ * height_ * 4;
        CPPIMG_DELETE_ARRAY(image_);
        image_ = reinterpret_cast<u8*>(CPPIMG_MALLOC(size));
        if(stream.read(size, image_)<=0){
            return false;
        }
        return true;
    }

    void OpenEXR::Preview::swap(Preview& rhs)
    {
        cppimg::swap(width_, rhs.width_);
        cppimg::swap(height_, rhs.height_);
        cppimg::swap(image_, rhs.image_);
    }

    //----------------------------------------------------
    void OpenEXR::Header::initialize()
    {
        numChannels_ = 0;
        channels_[0].name_[0] = CPPIMG_NULLCHAR;
        chromaticities_ = {0.6400f, 0.3300f, 0.3000f, 0.6000f, 0.1500f, 0.0600f, 0.3127f, 0.3290f};
        hasChromaticities_ = false;
    }

    bool OpenEXR::Header::getColorType(ColorType& colorType) const
    {
        const Channel* channels[4];
        channels[0] = findChannel(ChannelNames[Channel_R]);
        channels[1] = findChannel(ChannelNames[Channel_G]);
        channels[2] = findChannel(ChannelNames[Channel_B]);
        channels[3] = findChannel(ChannelNames[Channel_A]);

        if(CPPIMG_NULL != channels[0]
            && CPPIMG_NULL != channels[1]
            && CPPIMG_NULL != channels[2]){

            colorType = (CPPIMG_NULL != channels[3])? ColorType_RGBA : ColorType_RGB;

        }else{
            channels[0] = findChannel(ChannelNames[Channel_Y]);
            colorType = ColorType_GRAY;
            if(CPPIMG_NULL == channels[0]){
                return false;
            }
        }
        return true;
    }

    void OpenEXR::Header::getTypes(s32 types[MaxInChannels]) const
    {
        CPPIMG_ASSERT(numChannels_<=MaxInChannels);
        for(s32 i=0; i<numChannels_; ++i){
            types[i] = channels_[i].pixelType_;
        }
    }

    const OpenEXR::Channel* OpenEXR::Header::findChannel(const Char* name) const
    {
        CPPIMG_ASSERT(CPPIMG_NULL != name);
        for(s32 i=0; i<numChannels_; ++i){
            if(0 == strcmp(channels_[i].name_, name)){
                return &channels_[i];
            }
        }
        return CPPIMG_NULL;
    }

    void OpenEXR::Header::sortChannels()
    {
        for(s32 i=0; i<numChannels_; ++i){
            s32 index = i;
            for(s32 j=i+1; j<numChannels_; ++j){
                if(strcmp(channels_[j].name_, channels_[index].name_)<0){
                    index = j;
                }
            }
            if(index != i){
                cppimg::swap(channels_[i], channels_[index]);
            }
        }
    }

    void OpenEXR::Header::getChannelAssign(s32 assign[MaxInChannels]) const
    {
        for(s32 i=0; i<numChannels_; ++i){
            if(0 == strcmp(channels_[i].name_, ChannelNames[Channel_R])){
                assign[i] = 0;
            }else if(0 == strcmp(channels_[i].name_, ChannelNames[Channel_G])){
                assign[i] = 1;
            }else if(0 == strcmp(channels_[i].name_, ChannelNames[Channel_B])){
                assign[i] = 2;
            }else if(0 == strcmp(channels_[i].name_, ChannelNames[Channel_A])){
                assign[i] = 3;
            }else if(0 == strcmp(channels_[i].name_, ChannelNames[Channel_Y])){
                assign[i] = 0;
            }
        }
    }

    //----------------------------------------------------
    OpenEXR::Buffer::Buffer()
        :capacity_(0)
        ,buffer_(CPPIMG_NULL)
    {}

    OpenEXR::Buffer::Buffer(s64 capacity)
        :capacity_(capacity)
    {
        buffer_ = reinterpret_cast<u8*>(CPPIMG_MALLOC(capacity_));
    }

    OpenEXR::Buffer::~Buffer()
    {
        CPPIMG_FREE(buffer_);
    }

    bool OpenEXR::Buffer::reserve(s64 capacity)
    {
        if(capacity<=capacity_){
            return true;
        }
        CPPIMG_FREE(buffer_);
        capacity_ = capacity;
        buffer_ = reinterpret_cast<u8*>(CPPIMG_MALLOC(capacity_));
        return CPPIMG_NULL != buffer_;
    }

    bool OpenEXR::Buffer::expand(s64 newCapacity)
    {
        CPPIMG_ASSERT(0<newCapacity);
        CPPIMG_ASSERT(capacity_<=newCapacity);
        newCapacity = (newCapacity+7) & ~7;
        u8* buffer = REINTERPRET_CAST(u8*, CPPIMG_MALLOC(newCapacity));
        if(CPPIMG_NULL == buffer){
            return false;
        }
        memcpy(buffer, buffer_, newCapacity);
        CPPIMG_FREE(buffer_);
        capacity_ = newCapacity;
        buffer_ = buffer;
        return true;
    }

    //----------------------------------------------------
    OpenEXR::StreamBuffer::StreamBuffer()
        :size_(0)
        ,increments_(1024)
    {}

    OpenEXR::StreamBuffer::StreamBuffer(s64 increments)
        :size_(0)
        ,increments_(increments)
    {}

    OpenEXR::StreamBuffer::StreamBuffer(s64 increments, s64 capacity)
        :size_(0)
        ,increments_(increments)
        ,buffer_(capacity)
    {}

    OpenEXR::StreamBuffer::~StreamBuffer()
    {
    }

    bool OpenEXR::StreamBuffer::write(s64 size, const void* data)
    {
        CPPIMG_ASSERT(0<=size);
        CPPIMG_ASSERT(CPPIMG_NULL != data);
        CPPIMG_ASSERT(0<increments_);
        s64 capacity = size_+size;
        if(buffer_.capacity()<capacity){
            while(capacity<buffer_.capacity()){
                capacity += increments_;
            }
            if(!buffer_.expand(capacity)){
                return false;
            }
        }
        memcpy(buffer_.begin()+size_, data, size);
        size_ += size;
        return true;
    }

    //----------------------------------------------------
    OpenEXR::Context::Context()
        :offsetTable_(CPPIMG_NULL)
        ,information_(CPPIMG_NULL)
        ,image_(CPPIMG_NULL)
    {
        version_.version_ = 0;
        memset(&header_, 0, sizeof(Header));
        header_.initialize();
    }

    OpenEXR::Context::~Context()
    {
        CPPIMG_FREE(offsetTable_);
    }

    bool OpenEXR::Context::readVersion(Stream& stream)
    {
        return 0<stream.read(4, &version_);
    }

    bool OpenEXR::Context::readHeader(Stream& stream)
    {
        static const u32 CommonFlags = (0x01U<<AttrName_Channels)
            | (0x01U<<AttrName_Compression)
            | (0x01U<<AttrName_DataWindow)
            | (0x01U<<AttrName_DisplayWindow)
            | (0x01U<<AttrName_LineOrder)
            | (0x01U<<AttrName_PixelAspectRatio)
            | (0x01U<<AttrName_ScreenWindowCenter)
            | (0x01U<<AttrName_ScreenWindowWidth);

        static const u32 TiledFlags = (0x01U<<AttrName_Tiles);

        static const u32 MultiPartFlags = (0x01U<<AttrName_View)
            | (0x01U<<AttrName_Name)
            | (0x01U<<AttrName_Type)
            | (0x01U<<AttrName_Version)
            | (0x01U<<AttrName_ChunkCount)
            | (0x01U<<AttrName_Tiles);

        static const u32 DeepDataFlags = (0x01U<<AttrName_Name)
            | (0x01U<<AttrName_Type)
            | (0x01U<<AttrName_Version)
            | (0x01U<<AttrName_ChunkCount)
            | (0x01U<<AttrName_Tiles)
            | (0x01U<<AttrName_MaxSamplesPerPixel);

        u32 flags = 0;
        for(;;){
            Status status = readAttribute(flags, stream);
            if(Error == status){
                return false;
            }
            if(End == status){
                break;
            }
        }

        if(CommonFlags != (flags & CommonFlags)){
            return false;
        }
        if(version_.isTile() && (TiledFlags != (flags & TiledFlags))){
            return false;
        }
        if(version_.isMultiPart() && (MultiPartFlags != (flags & MultiPartFlags))){
            return false;
        }
        if(version_.isDeepData() && (DeepDataFlags != (flags & DeepDataFlags))){
            return false;
        }

        if(0 == (flags & (0x01U<<AttrName_ChunkCount))){
            if(0 == (flags & (0x01U<<AttrName_Tiles))){
                switch(header_.compression_){
                case NO_COMPRESSION:
                case RLE_COMPRESSION:
                case ZIPS_COMPRESSION:
                    header_.chunkCount_ = (header_.dataWindow_.yMax_ - header_.dataWindow_.yMin_ + 1);
                    break;
                case ZIP_COMPRESSION:
                {
                    s32 lines = (header_.dataWindow_.yMax_ - header_.dataWindow_.yMin_ + 1);
                    header_.chunkCount_ = (lines + (LinesPerBlock-1))/LinesPerBlock;
                }
                    break;
                case PIZ_COMPRESSION:
                case PXR24_COMPRESSION:
                case B44_COMPRESSION:
                case B44A_COMPRESSION:
                    header_.chunkCount_ = 0;
                    return false;
                }
            }else{
                header_.chunkCount_ = header_.tiles_.xSize_ * header_.tiles_.ySize_;
            }
        }
        return true; 
    }

    OpenEXR::Context::Status OpenEXR::Context::readAttribute(u32& flags, Stream& stream)
    {
        AttributeName attributeName;
        AttributeType attributeType;
        Char str[MaxStringSize];

        //Read attribute name
        if(OpenEXR::read(str, stream)<=0){
            return Error;
        }
        attributeName = findAttributeName(str);
        //End of an attribute list
        if(AttrName_End == attributeName){
            return End;
        }
        if(AttrName_Max == attributeName){
            skipUnknownName(stream);
            return Success;
        }

        //Read attribute type
        if(OpenEXR::read(str, stream)<=0){
            return Error;
        }
        attributeType = findAttributeType(str);
        if(AttrType_Max == attributeType){
            skipUnknownType(stream);
            return Success;
        }

        s32 size;
        if(stream.read(sizeof(s32), &size)<=0){
            return Error;
        }
        switch(attributeName){
        case AttrName_Channels:
            if(!readChannels(stream, size)){
                return Error;
            }
            break;
        case AttrName_Compression:
            if(stream.read(sizeof(u8), &header_.compression_)<=0){
                return Error;
            }
            break;
        case AttrName_DataWindow:
            if(stream.read(sizeof(Box2i), &header_.dataWindow_)<=0){
                return Error;
            }
            break;
        case AttrName_DisplayWindow:
            if(stream.read(sizeof(Box2i), &header_.displayWindow_)<=0){
                return Error;
            }
            break;
        case AttrName_LineOrder:
            if(stream.read(sizeof(u8), &header_.lineOrder_)<=0){
                return Error;
            }
            break;
        case AttrName_PixelAspectRatio:
            if(stream.read(sizeof(f32), &header_.pixelAspectRatio_)<=0){
                return Error;
            }
            break;
        case AttrName_ScreenWindowCenter:
            if(stream.read(sizeof(Vector2f), &header_.screenWindowCenter_)<=0){
                return Error;
            }
            break;
        case AttrName_ScreenWindowWidth:
            if(stream.read(sizeof(f32), &header_.screenWindowWidth_)<=0){
                return Error;
            }
            break;
        case AttrName_Tiles:
            if(stream.read(sizeof(TiledDesc), &header_.tiles_)<=0){
                return Error;
            }
            break;
        case AttrName_View:
            if(readString(header_.view_, stream)<0){
                return Error;
            }
            break;
        case AttrName_Name:
            if(readString(header_.name_, stream)<0){
                return Error;
            }
            break;
        case AttrName_Type:
            if(readString(header_.type_, stream)<0){
                return Error;
            }
            break;
        case AttrName_Version:
            if(stream.read(sizeof(s32), &header_.version_)<=0){
                return Error;
            }
            break;
        case AttrName_ChunkCount:
            if(stream.read(sizeof(s32), &header_.chunkCount_)<=0){
                return Error;
            }
            break;
        case AttrName_MaxSamplesPerPixel:
            if(stream.read(sizeof(s32), &header_.maxSamplesPerPixel_)<=0){
                return Error;
            }
            break;
        case AttrName_Chromaticities:
            if(stream.read(sizeof(Chromaticities), &header_.chromaticities_)<=0){
                return Error;
            }
            header_.hasChromaticities_ = true;
            break;
        default:
            return Success;
        }
        flags |= 0x01U << attributeName;
        return Success;
    }

    void OpenEXR::Context::skipUnknownName(Stream& stream)
    {
        //Skip type string
        s32 length = 0;
        Char c;
        while(length<MaxStringSize){
            if(stream.read(1, &c)<=0){
                return;
            }
            if(CPPIMG_NULLCHAR==c){
                break;
            }
            ++length;
        }

        skipUnknownType(stream);
    }

    void OpenEXR::Context::skipUnknownType(Stream& stream)
    {
        s32 size = 0;
        if(stream.read(sizeof(s32), &size)<=0){
            return;
        }
        stream.seek(size, SEEK_CUR);
    }

    bool OpenEXR::Context::readChannel(Channel& channel, Stream& stream)
    {
        s32 length = 0;
        channel.name_[length] = CPPIMG_NULLCHAR;
        while(length<MaxStringSize){
            Char c;
            if(stream.read(1, &c)<=0){
                return false;
            }
            channel.name_[length] = c;
            if(c == CPPIMG_NULLCHAR){
                break;
            }
            ++length;
        }
        if(MaxStringSize<=length){
            return false;
        }
        if(length<=0){
            return true;
        }
        if(stream.read(sizeof(s32), &channel.pixelType_)<=0){
            return false;
        }
        if(stream.read(4, channel.flags_)<=0){
            return false;
        }
        if(stream.read(sizeof(s32), &channel.xSampling_)<=0){
            return false;
        }
        if(stream.read(sizeof(s32), &channel.ySampling_)<=0){
            return false;
        }
        return true;
    }

    bool OpenEXR::Context::readChannels(Stream& stream, s32 valueSize)
    {
        s32 count = 0;
        s32 size = 0;
        Channel tmp;
        for(;;){
            Channel* channel = (count<MaxInChannels)? &header_.channels_[count] : &tmp;
            if(!readChannel(*channel, stream)){
                return false;
            }

            if(channel->name_[0] == CPPIMG_NULLCHAR){
                size += 1;
                break;
            }
            size += static_cast<s32>( sizeof(s32)*4 + sizeof(Char)*(strlen(channel->name_)+1) );
            ++count;
        }
        header_.numChannels_ = minimum(count, MaxInChannels);
        header_.sortChannels();
        return size == valueSize;
    }

    bool OpenEXR::Context::readOffsetTable(Stream& stream)
    {
        CPPIMG_FREE(offsetTable_);
        u32 size = sizeof(u64)*header_.chunkCount_;
        offsetTable_ = reinterpret_cast<u64*>(CPPIMG_MALLOC(size));
        return 0<stream.read(size, offsetTable_);
    }

    bool OpenEXR::Context::readScanlines(Stream& stream)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != image_);
        bool result = false;
        switch(header_.compression_){
        case NO_COMPRESSION:
            result = readScanlines_NO_COMPRESSION(stream);
            break;
        case RLE_COMPRESSION:
            result = readScanlines_RLE_COMPRESSION(stream);
            break;
        case ZIPS_COMPRESSION:
        case ZIP_COMPRESSION:
            result = readScanlines_ZIP_COMPRESSION(stream);
            break;
        case PIZ_COMPRESSION:
        case PXR24_COMPRESSION:
        case B44_COMPRESSION:
        case B44A_COMPRESSION:
        default:
            break;
        }
        return result;
    }

    bool OpenEXR::Context::readScanlines_NO_COMPRESSION(Stream& stream)
    {
        s32 bytesPerPixel = information_->getBytesPerPixel();
        s32 lineSize = information_->width_ * bytesPerPixel;
        s32 sizes[MaxInChannels];
        s32 offsets[MaxInChannels];
        getChannelInformation(sizes, offsets);

        s32 y=0;
        s32 dataSize=0;
        for(s32 i=0; i<header_.chunkCount_; ++i){
            stream.seek(offsetTable_[i], SEEK_SET);
            if(stream.read(sizeof(s32), &y)<=0){
                return false;
            }
            if(stream.read(sizeof(s32), &dataSize)<=0){
                return false;
            }

            for(s32 j=0; j<information_->numChannels_; ++j){
                u8* pixel = reinterpret_cast<u8*>(image_) + y*lineSize + offsets[j];
                for(s32 k=0; k<information_->width_; ++k){
                    if(stream.read(sizes[j], pixel)<=0){
                        return false;
                    }
                    pixel += bytesPerPixel;
                    dataSize -= sizes[j];
                }
            }
            CPPIMG_ASSERT(0 == dataSize);
        }
        return true;
    }

    bool OpenEXR::Context::readScanlines_RLE_COMPRESSION(Stream& stream)
    {
        s32 bytesPerPixel = information_->getBytesPerPixel();
        s32 lineSize = information_->width_ * bytesPerPixel;
        s32 sizes[MaxInChannels];
        s32 offsets[MaxInChannels];
        getChannelInformation(sizes, offsets);

        Buffer dst(lineSize*2);
        u8* tmp = &dst[0] + lineSize;

        s32 y=0;
        s32 dataSize=0;
        for(s32 i=0; i<header_.chunkCount_; ++i){
            stream.seek(offsetTable_[i], SEEK_SET);
            if(stream.read(sizeof(s32), &y)<=0){
                return false;
            }
            if(stream.read(sizeof(s32), &dataSize)<=0){
                return false;
            }
            if(!uncompressRLE(stream, lineSize, &dst[0], tmp, dataSize)){
                return false;
            }

            const u8* src = &dst[0];
            for(s32 j=0; j<information_->numChannels_; ++j){
                u8* pixel = reinterpret_cast<u8*>(image_) + y*lineSize + offsets[j];
                for(s32 k=0; k<information_->width_; ++k){
                    memcpy(pixel, src, sizes[j]);
                    pixel += bytesPerPixel;
                    src += sizes[j];
                }
            }
        }
        return true;
    }

    bool OpenEXR::Context::readScanlines_ZIP_COMPRESSION(Stream& stream)
    {
        szlib::szContext context;
        if(szlib::SZ_OK != szlib::createInflate(&context)){
            return false;
        }
        s32 numBlocks = header_.chunkCount_;
        s32 lines = (header_.dataWindow_.yMax_ - header_.dataWindow_.yMin_ + 1);
        s32 linesPerBlock = (ZIP_COMPRESSION == header_.compression_)? LinesPerBlock : 1;
        s32 bytesPerPixel = information_->getBytesPerPixel();
        s32 lineSize = information_->width_ * bytesPerPixel;
        s32 blockSize = lineSize * linesPerBlock;
        s32 sizes[MaxInChannels];
        s32 offsets[MaxInChannels];
        getChannelInformation(sizes, offsets);

        Buffer src(blockSize);
        Buffer dst(blockSize*2);
        Buffer tmp(blockSize*2);

        bool result = true;
        s32 y=0;
        s32 dataSize=0;
        for(s32 i=0, next=0; i<numBlocks; ++i){
            s32 prev = next;
            next += linesPerBlock;
            u64 offset = offsetTable_[i];
            stream.seek(offset, SEEK_SET);
            if(stream.read(sizeof(s32), &y)<=0){
                result = false;
                break;
            }
            if(stream.read(sizeof(s32), &dataSize)<=0){
                result = false;
                break;
            }
            src.reserve(dataSize);
            if(stream.read(dataSize, &src[0])<=0){
                result = false;
                break;
            }
            if(!uncompressZlib(context, dst, tmp, dataSize, &src[0])){
                result = false;
                break;
            }

            s32 currentLines = (next<lines)? linesPerBlock : lines-prev;
            const u8* s = &dst[0];
            u8* line = reinterpret_cast<u8*>(image_) + y*lineSize;
            for(s32 j=0; j<currentLines; ++j){
                for(s32 k=0; k<information_->numChannels_; ++k){
                    u8* pixel = line + offsets[k];
                    for(s32 l=0; l<information_->width_; ++l){
                        for(s32 m=0; m<sizes[k]; ++m){
                            pixel[m] = s[m];
                        }
                        s += sizes[k];
                        pixel += bytesPerPixel;
                    }
                }
                line += lineSize;
            }
            CPPIMG_ASSERT((lineSize*currentLines) == static_cast<s32>(s-&dst[0]));
        }
        szlib::termInflate(&context);
        return result;
    }

    void OpenEXR::Context::getChannelInformation(s32 sizes[MaxInChannels], s32 offsets[MaxInChannels]) const
    {
        s32 tmp_offsets[MaxInChannels];
        s32 channelAssign[MaxInChannels];
        header_.getChannelAssign(channelAssign);
        sizes[0] = information_->getSize(0);
        tmp_offsets[0] = 0;
        for(s32 i=1; i<information_->numChannels_; ++i){
            sizes[i] = information_->getSize(i);
            tmp_offsets[i] = tmp_offsets[i-1] + sizes[i-1];
        }
        for(s32 i=0; i<information_->numChannels_; ++i){
            offsets[i] = tmp_offsets[channelAssign[i]];
        }
    }

    bool OpenEXR::Context::uncompressRLE(Stream& stream, s32 dstSize, u8* dst, u8* tmp, s32 srcSize)
    {
        u8* s = tmp;
        s32 in;
        for(in=0; in<srcSize;){
            s8 c;
            if(stream.read(1, &c)<=0){
                return false;
            }
            ++in;
            if(0<=c){ //consecutive
                s32 count = c+1;
                u8 byte;
                if(stream.read(1, &byte)<=0){
                    return false;
                }
                ++in;
                for(s32 k=0; k<count; ++k,++s){
                    s[0] = byte;
                }

            }else{ //non-consecutive
                s32 count = -c;
                if(stream.read(count, s)<=0){
                    return false;
                }
                s += count;
                in += count;
            }
        }
        if(in != srcSize){
            return false;
        }
        if(dstSize != static_cast<s32>(s-tmp)){
            return false;
        }
        postprocess(dstSize, dst, tmp);
        return true;
    }

    //----------------------------------------------------
    OpenEXR::WriteContext::WriteContext()
        :numScanlines_(0)
        ,offsetTable_(CPPIMG_NULL)
    {
    }

    OpenEXR::WriteContext::~WriteContext()
    {
        CPPIMG_FREE(offsetTable_);
    }

    //----------------------------------------------------
    const Char* OpenEXR::AttributeNames[AttrName_Max] =
    {
        "channels",
        "compression",
        "dataWindow",
        "displayWindow",
        "lineOrder",
        "pixelAspectRatio",
        "screenWindowCenter",
        "screenWindowWidth",
        "tiles",
        "view",
        "name",
        "type",
        "version",
        "chunkCount",
        "maxSamplesPerPixel",
        "chromaticities",
        "",
    };

    const Char* OpenEXR::TypeNames[AttrType_Max] =
    {
        "box2i",
        "box2f",
        "chlist",
        "chromaticities",
        "compression",
        "double",
        "envmap",
        "float",
        "int",
        "keycode",
        "lineOrder",
        "m33f",
        "m44f",
        "preview",
        "rational",
        "string",
        "stringvector",
        "tiledesc",
        "timecode",
        "v2i",
        "v2f",
        "v3i",
        "v3f",
    };

    OpenEXR::AttributeName OpenEXR::findAttributeName(const Char* str)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != str);
        for(s32 name = AttrName_Channels; name<AttrName_Max; ++name){
            if(0 == strcmp(AttributeNames[name], str)){
                return (AttributeName)name;
            }
        }
        return AttrName_Max;
    }

    OpenEXR::AttributeType OpenEXR::findAttributeType(const Char* str)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != str);
        for(s32 type = AttrType_Box2i; type<AttrType_Max; ++type){
            if(0 == strcmp(TypeNames[type], str)){
                return (AttributeType)type;
            }
        }

        return AttrType_Max;
    }

    s32 OpenEXR::read(Char str[MaxStringSize], Stream& stream)
    {
        s32 length = 0;
        str[length] = CPPIMG_NULLCHAR;
        Char c;
        while(length<MaxStringSize){
            if(stream.read(1, &c)<=0){
                return -1;
            }
            str[length] = c;
            if(CPPIMG_NULLCHAR==c){
                return 1;
            }
            ++length;
        }
        return -1;
    }

    s32 OpenEXR::readString(Char str[MaxStringSize], Stream& stream)
    {
        s32 length = 0;
        if(stream.read(sizeof(s32), &length)<=0){
            return -1;
        }
        s32 l = 0;
        str[l] = CPPIMG_NULLCHAR;
        Char c;
        while(l<MaxStringSize){
            if(stream.read(1, &c)<=0){
                return -1;
            }
            str[l] = c;
            if(CPPIMG_NULLCHAR==c){
                return 1;
            }
            ++l;
        }
        return -1;
    }

    s32 OpenEXR::writeAttribute(Stream& stream, AttributeName name, AttributeType type, s32 size, const void* data)
    {
        if(stream.write(strlen(AttributeNames[name])+1, AttributeNames[name])<=0){
            return 0;
        }
        if(stream.write(strlen(TypeNames[type])+1, TypeNames[type])<=0){
            return 0;
        }
        if(stream.write(sizeof(s32), &size)<=0){
            return 0;
        }
        return stream.write(size, data);
    }

    s32 OpenEXR::writeChannels(Stream& stream, ColorType colorType, Type pixelType)
    {
        static const Char* Names[5] = {"A", "B", "G", "R", "Y"};
        switch(colorType){
        case ColorType_GRAY:
            return writeChannels(stream, 1, &Names[4], pixelType, 0, 1, 1);
        case ColorType_RGB:
            return writeChannels(stream, 3, &Names[1], pixelType, 0, 1, 1);
        case ColorType_RGBA:
            return writeChannels(stream, 4, &Names[0], pixelType, 0, 1, 1);
        default:
            CPPIMG_ASSERT(false);
            return 0;
        }
    }

    s32 OpenEXR::writeChannels(Stream& stream, s32 size, const Char** names, Type pixelType, u8 linear, s32 xSampling, s32 ySampling)
    {
        CPPIMG_ASSERT(CPPIMG_NULL != names);
        if(stream.write(strlen(AttributeNames[AttrName_Channels])+1, AttributeNames[AttrName_Channels])<=0){
            return 0;
        }
        if(stream.write(strlen(TypeNames[AttrType_Chlist])+1, TypeNames[AttrType_Chlist])<=0){
            return 0;
        }

        static const s32 SizePerChannel = 18;
        s32 total = SizePerChannel*size + 1;
        if(stream.write(sizeof(s32), &total)<=0){
            return 0;
        }
        for(s32 i=0; i<size; ++i){
            if(stream.write(strlen(names[i])+1, names[i])<=0){
                return 0;
            }
            s32 channel[4] = {pixelType, linear, xSampling, ySampling};
            if(stream.write(4*sizeof(s32), channel)<=0){
                return 0;
            }
        }
        return writeNull(stream);
    }

    s32 OpenEXR::writeNull(Stream& stream)
    {
        u8 nul = 0x00U;
        return stream.write(1, &nul);
    }

    bool OpenEXR::read(Information& information, void* image, Stream& stream)
    {
        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);

        u32 magic;
        if(stream.read(4, &magic)<=0){
            return false;
        }
        if(MAGIC != magic){
            return false;
        }
        Auto<Context> context(CPPIMG_NEW Context);
        if(!context->readVersion(stream)){
            return false;
        }
        if(!context->readHeader(stream)){
            return false;
        }

        information.width_ = context->header_.displayWindow_.xMax_ - context->header_.displayWindow_.xMin_ + 1;
        information.height_ = context->header_.displayWindow_.yMax_ - context->header_.displayWindow_.yMin_ + 1;
        information.numChannels_ = context->header_.numChannels_;
        if(!context->header_.getColorType(information.colorType_)){
            return false;
        }
        context->header_.getTypes(information.types_);
        if(CPPIMG_NULL == image){
            return true;
        }
        if(!context->readOffsetTable(stream)){
            return false;
        }
        context->image_ = image;
        context->information_ = &information;

        if(context->version_.isMultiPart()){
        }else if(context->version_.isTile()){
        }else{//Scanline
            if(!context->readScanlines(stream)){
                return false;
            }
        }
        seekSet.clear();
        return true;
    }

    bool OpenEXR::write(Stream& stream, s32 width, s32 height, ColorType colorType, Type pixelType, const void* image)
    {
//#define CPPIMG_OPENEXR_USE_NOCOMPRESSION
        CPPIMG_ASSERT(1<=width);
        CPPIMG_ASSERT(1<=height);
        CPPIMG_ASSERT(CPPIMG_NULL != image);

        if(!stream.valid()){
            return false;
        }

        SeekSet seekSet(stream.tell(), &stream);

        u64 begin = stream.tell();

        //Write common header
        u32 magic = MAGIC;
        if(stream.write(4, &magic)<=0){
            return false;
        }
        Version version = Version::create(false, false, false, false);
        if(stream.write(4, &version.version_)<=0){
            return false;
        }

        //Write channels
        if(writeChannels(stream, colorType, pixelType)<=0){
            return false;
        }

        //Write compression
#ifdef CPPIMG_OPENEXR_USE_NOCOMPRESSION
        u8 compression = NO_COMPRESSION;
#else
        u8 compression = ZIP_COMPRESSION;
#endif
        if(writeAttribute(stream, AttrName_Compression, AttrType_Compression, 1, &compression)<=0){
            return false;
        }

        //Write dataWindow
        Box2i dataWindow = {0, 0, width-1, height-1};
        if(writeAttribute(stream, AttrName_DataWindow, AttrType_Box2i, sizeof(Box2i), &dataWindow)<=0){
            return false;
        }

        //Write displayWindow
        Box2i displayWindow = {0, 0, width-1, height-1};
        if(writeAttribute(stream, AttrName_DisplayWindow, AttrType_Box2i, sizeof(Box2i), &displayWindow)<=0){
            return false;
        }

        //Write lineOrder
        u8 lineOrder = INCREASING_Y;
        if(writeAttribute(stream, AttrName_LineOrder, AttrType_LineOrder, sizeof(u8), &lineOrder)<=0){
            return false;
        }

        //Write pixelAspectRatio
        f32 pixelRatio = 1.0f;
        if(writeAttribute(stream, AttrName_PixelAspectRatio, AttrType_Float, sizeof(f32), &pixelRatio)<=0){
            return false;
        }

        //Write screenWindowCenter
        Vector2f screenWindowCenter = {0.0f, 0.0f};
        if(writeAttribute(stream, AttrName_ScreenWindowCenter, AttrType_V2f, sizeof(Vector2f), &screenWindowCenter)<=0){
            return false;
        }

        //Write screenWindowWidth
        f32 screenWindotWidth = 1.0f;
        if(writeAttribute(stream, AttrName_ScreenWindowWidth, AttrType_Float, sizeof(f32), &screenWindotWidth)<=0){
            return false;
        }

        if(writeNull(stream)<=0){
            return false;
        }
        WriteContext writeContext;
        u64 offset = stream.tell() - begin;
#ifdef CPPIMG_OPENEXR_USE_NOCOMPRESSION
        if(!writeScanlines_NO_COMPRESSION(writeContext, offset ,width, height, colorType, pixelType, image)){
            return false;
        }
#else
        if(!writeScanlines_ZIP_COMPRESSION(writeContext, offset, width, height, colorType, pixelType, LinesPerBlock, image)){
            return false;
        }
#endif
        if(stream.write(sizeof(u64)*writeContext.numScanlines_, writeContext.offsetTable_)<=0){
            return false;
        }
        if(stream.write(writeContext.size(), writeContext.begin())<=0){
            return false;
        }
        seekSet.clear();
        return true;
    }

    void OpenEXR::getChannelInformation(s32 offsets[MaxOutChannels], ColorType colorType, Type pixelType)
    {
        s32 bytesPerChannel = getSize(pixelType);
        switch(colorType){
        case ColorType_GRAY:
            offsets[0] = 0;
            break;
        case ColorType_RGB:
            offsets[0] = (bytesPerChannel<<1);
            offsets[1] = bytesPerChannel;
            offsets[2] = 0;
            break;
        case ColorType_RGBA:
            offsets[0] = (bytesPerChannel<<1) + bytesPerChannel;
            offsets[1] = (bytesPerChannel<<1);
            offsets[2] = bytesPerChannel;
            offsets[3] = 0;
            break;
        default:
            return;
        }
    }

    void OpenEXR::preprocess(s32 size, u8* dst, const u8* src)
    {
        const u8* s = src;
        const u8* end = s + size;
        u8* t0 = dst;
        u8* t1 = dst + ((size+1)>>1);
        for(;;){
            if(end<=s){
                break;
            }
            *t0 = *s;
            ++t0; ++s;
            if(end<=s){
                break;
            }
            *t1 = *s;
            ++t1; ++s;
        }

        s32 p = dst[0];
        for(u8* t = dst+1; t<(dst+size); ++t){
            s32 d = static_cast<s32>(t[0]) - p + (128+256);
            p = t[0];
            t[0] = static_cast<u8>(d);
        }
    }

    void OpenEXR::postprocess(s32 size, u8* dst, u8* src)
    {
        for(u8* t = src+1; t<(src+size); ++t){
            t[0] = static_cast<u8>(static_cast<s32>(t[-1]) + static_cast<s32>(t[0]) - 128);
        }
        const u8* t0 = src;
        const u8* t1 = src + ((size+1)>>1);
        u8* d = dst;
        u8* end = d + size;
        for(;;){
            if(end<=d){
                break;
            }
            *d = *t0;
            ++d; ++t0;
            if(end<=d){
                break;
            }
            *d = *t1;
            ++d; ++t1;
        }
    }

    s32 OpenEXR::compressZlib(szlib::szContext& context, Buffer& dst, Buffer& tmp, s32 srcSize, const u8* src)
    {
        if(!tmp.reserve(srcSize)){
            return -1;
        }
        preprocess(srcSize, &tmp[0], src);
        szlib::resetDeflate(&context, srcSize, &tmp[0], szlib::SZ_Level_Fixed);
        static const s32 ChunkSize = 512;
        u8 chunk[ChunkSize];

        s32 total = 0;
        for(;;){
            context.availOut_ = ChunkSize;
            context.nextOut_ = chunk;
            s32 ret = szlib::deflate(&context);
            switch(ret)
            {
            case szlib::SZ_ERROR_MEMORY:
            case szlib::SZ_ERROR_FORMAT:
                return -1;
            default:
                s32 outCount = total;
                total += context.thisTimeOut_;
                if(dst.capacity()<total){
                    dst.expand(1024);
                }
                memcpy(&dst[outCount], chunk, context.thisTimeOut_);
                if(szlib::SZ_END!=ret){
                    continue;
                }
                break;
            };
            break;
        }
        return total;
    }

    s32 OpenEXR::uncompressZlib(szlib::szContext& context, Buffer& dst, Buffer& tmp, s32 srcSize, const u8* src)
    {
        szlib::resetInflate(&context, srcSize, src);
        static const s32 ChunkSize = 512;
        u8 chunk[ChunkSize];

        s32 total = 0;
        for(;;){
            context.availOut_ = ChunkSize;
            context.nextOut_ = chunk;
            s32 ret = szlib::inflate(&context);
            switch(ret)
            {
            case szlib::SZ_ERROR_MEMORY:
            case szlib::SZ_ERROR_FORMAT:
                return -1;
            default:
                s32 current = total;
                total += context.thisTimeOut_;
                if(tmp.capacity()<total){
                    tmp.expand(1024);
                }
                memcpy(&tmp[current], chunk, context.thisTimeOut_);
                if(szlib::SZ_END!=ret){
                    continue;
                }
                break;
            };
            break;
        }
        if(!dst.reserve(tmp.capacity())){
            return -1;
        }
        postprocess(total, &dst[0], &tmp[0]);
        return total;
    }

    bool OpenEXR::writeScanlines_NO_COMPRESSION(WriteContext& context, u64 offset, s32 width, s32 height, ColorType colorType, Type pixelType, const void* data)
    {
        s32 numChannels = getNumChannels(colorType);
        s32 bytesPerChannel = getSize(pixelType);
        s32 bytesPerPixel = numChannels * bytesPerChannel;
        s32 bytesPerLine = bytesPerPixel * width;
        s32 bytesPerChunk = bytesPerLine;
        s32 offsets[MaxOutChannels];
        getChannelInformation(offsets, colorType, pixelType);

        context.numScanlines_ = height;
        context.offsetTable_ = REINTERPRET_CAST(u64*, CPPIMG_MALLOC(sizeof(u64)*context.numScanlines_));
        offset += sizeof(u64)*context.numScanlines_;
        for(s32 i=0; i<height; ++i){
            context.offsetTable_[i] = offset;
            offset += bytesPerLine + sizeof(s32);
        }
        context.streamBuffer_.setIncrements(bytesPerLine);
        context.streamBuffer_.reserve((STATIC_CAST(s64, bytesPerLine) + sizeof(s32)*2)*height);

        for(s32 i=0; i<height; ++i){
            if(!context.write(sizeof(s32), &i)){
                return false;
            }
            if(!context.write(sizeof(s32), &bytesPerChunk)){
                return false;
            }

            const u8* line = REINTERPRET_CAST(const u8*, data) + i*bytesPerChunk;
            for(s32 j=0; j<numChannels; ++j){
                const u8* pixel = line + offsets[j];
                for(s32 k=0; k<width; ++k){
                    if(!context.write(bytesPerChannel, pixel)){
                        return false;
                    }
                    pixel += bytesPerPixel;
                }
            }
        }
        return true;
    }

    bool OpenEXR::writeScanlines_ZIP_COMPRESSION(WriteContext& context, u64 offset, s32 width, s32 height, ColorType colorType, Type pixelType, s32 linesPerBlock, const void* data)
    {
//#define CPPIMG_OPENEXR_DEBUG_ZIP
        static const s8 ChannelOrder_GRAY[] = {0};
        static const s8 ChannelOrder_RGB[] = {0, 1, 2};
        static const s8 ChannelOrder_RGBA[] = {0, 1, 2, 3};

        szlib::szContext zcontext;
        if(szlib::SZ_OK != szlib::createDeflate(&zcontext)){
            return false;
        }
#ifdef CPPIMG_OPENEXR_DEBUG_ZIP
        szlib::szContext zuncompress;
        if(szlib::SZ_OK != szlib::createInflate(&zuncompress)){
            return false;
        }
#endif
        s32 mask = linesPerBlock-1;
        s32 numBlocks = (height + mask)/linesPerBlock;
        s32 numChannels = getNumChannels(colorType);
        s32 bytesPerChannel = getSize(pixelType);
        s32 bytesPerPixel = numChannels * bytesPerChannel;
        s32 bytesPerLine = bytesPerPixel * width;
        s32 bytesPerChannelLine = bytesPerChannel * width;
        s32 bytesPerChunk = bytesPerLine * linesPerBlock;
        const s8* channelOrder = CPPIMG_NULL;
        switch(colorType){
        case ColorType_GRAY:
            channelOrder = ChannelOrder_GRAY;
            break;
        case ColorType_RGB:
            channelOrder = ChannelOrder_RGB;
            break;
        case ColorType_RGBA:
            channelOrder = ChannelOrder_RGBA;
            break;
        default:
            CPPIMG_ASSERT(false);
            break;
        }

        context.numScanlines_ = numBlocks;
        context.offsetTable_ = REINTERPRET_CAST(u64*, CPPIMG_MALLOC(sizeof(u64)*context.numScanlines_));
        offset += sizeof(u64)*context.numScanlines_;
        context.streamBuffer_.setIncrements(bytesPerChunk);
        context.streamBuffer_.reserve((STATIC_CAST(s64, bytesPerChunk) + sizeof(s32)*2)*numBlocks);

        s32 offsets[MaxOutChannels];
        getChannelInformation(offsets, colorType, pixelType);

        bool result = true;
        Buffer tmp0(bytesPerChunk);
        Buffer tmp1(bytesPerChunk);
        Buffer dst(bytesPerChunk);
#ifdef CPPIMG_OPENEXR_DEBUG_ZIP
        Buffer dst2(bytesPerChunk);
#endif
        const u8* src = REINTERPRET_CAST(const u8*, data);
        for(s32 i=0, next=0; i<numBlocks; ++i){
            s32 prev = next;
            next += linesPerBlock;
            s32 size = (next<height)? bytesPerChunk : (height-prev) * bytesPerLine;
            s32 lines = (next<height)? linesPerBlock : (height-prev);

            const u8* src_line = src;
            u8* dst_line = &tmp0[0];
            for(s32 j=0; j<lines; ++j){
                for(s32 k=0; k<numChannels; ++k){
                    const u8* src_pixel = src_line + offsets[k];
                    u8* dst_pixel = dst_line + channelOrder[k] * bytesPerChannelLine;
                    for(s32 l=0; l<width; ++l){
                        for(s32 m=0; m<bytesPerChannel; ++m){
                            dst_pixel[m] = src_pixel[m];
                        }
                        dst_pixel += bytesPerChannel;
                        src_pixel += bytesPerPixel;
                    }
                }

                src_line += bytesPerLine;
                dst_line += bytesPerLine;
            }
            CPPIMG_ASSERT(static_cast<s32>(dst_line-&tmp0[0]) == size);

            s32 compressed = compressZlib(zcontext, dst, tmp1, size, &tmp0[0]);
            if(compressed<0){
                result = false;
                break;
            }

#ifdef CPPIMG_OPENEXR_DEBUG_ZIP
            s32 uncompressed = uncompressZlib(zuncompress, dst2, tmp1, compressed, &dst[0]);
            CPPIMG_ASSERT(size == uncompressed);
            for(s32 j=0; j<size; ++j){
                CPPIMG_ASSERT(dst2[j] == tmp0[j]);
            }
#endif
            context.offsetTable_[i] = offset;
            src += size;
            if(!context.write(sizeof(s32), &prev)){
                result = false;
                break;
            }
            if(!context.write(sizeof(s32), &compressed)){
                result = false;
                break;
            }
            if(!context.write(sizeof(u8)*compressed, &dst[0])){
                result = false;
                break;
            }
            offset += compressed + sizeof(s32)*2;
        }
#ifdef CPPIMG_OPENEXR_DEBUG_ZIP
        szlib::termInflate(&zuncompress);
#endif
        szlib::termDeflate(&zcontext);
        return result;
    }
}
#endif

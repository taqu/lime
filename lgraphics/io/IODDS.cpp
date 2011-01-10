/**
@file IODDS.cpp
@author t-sakai
@date 2010/05/06 create
*/
#include "../lgraphicsAPIInclude.h"
#include "../lgraphicscore.h"

#include "IODDS.h"
#include "../api/Enumerations.h"
#include "../api/TextureRef.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        enum DDSD
        {
            DDSD_CAPS = 0x00000001,
            DDSD_HEIGHT = 0x00000002,
            DDSD_WIDTH = 0x00000004,
            DDSD_PITCH = 0x00000008,
            DDSD_PIXELFORMAT = 0x00001000,
            DDSD_MIPMAPCOUNT = 0x00020000,
            DDSD_LINEARSIZE = 0x00080000,
            DDSD_DEPTH = 0x00800000,
        };

        enum DDPF
        {
            DDPF_ALPHAPIXELS = 0x00000001,
            DDPF_FOURCC = 0x00000004,
            DDPF_RGB = 0x00000040,
            DDPF_RGBA = 0x00000041,
            DDPF_YUV = 0x200,
            DDPF_LUMINANCE = 0x20000,
        };

        enum DDSCAPS
        {
            DDSCAPS_COMPLEX = 0x00000008,
            DDSCAPS_TEXTURE = 0x00001000,
            DDSCAPS_MIPMAP = 0x00400000,
        };

        enum DDSCAPS2
        {
            DDSCAPS2_CUBEMAP = 0x00000200,
            DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
            DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
            DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
            DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
            DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
            DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
            DDSCAPS2_VOLUME = 0x00200000,
        };

        /**
        @param ddpf ... ピクセルフォーマット

        対応フォーマットはBufferFormatに入っているもののみ
        */
        BufferFormat selectFormat(const IODDS::DDS_PIXELFORMAT& ddpf)
        {
            BufferFormat format = Buffer_Unknown;
            switch(ddpf.flags_)
            {
            case DDPF_RGBA:
                {
                    if((ddpf.RBitMask_ == 0xffffU)
                        && (ddpf.GBitMask_ == 0xffff0000U))
                    {
                        format = Buffer_G16R16;

                    }else if((ddpf.RBitMask_ == 0x3ff00000U)
                        && (ddpf.GBitMask_ == 0xffc00U)
                        && (ddpf.BBitMask_ == 0x3ffU))
                    {
                        format = Buffer_A2R10G10B10;

                    }else if((ddpf.RBitMask_ == 0x3ffU)
                        && (ddpf.GBitMask_ == 0xffc00U)
                        && (ddpf.BBitMask_ == 0x3ff00000U))
                    {
                        format = Buffer_A2B10G10R10;

                    }else if((ddpf.RBitMask_ == 0xffU)
                        && (ddpf.GBitMask_ == 0xff00U)
                        && (ddpf.BBitMask_ == 0xff0000U))
                    {
                        format = Buffer_A8B8G8R8;

                    }else if((ddpf.RBitMask_ == 0xff0000U)
                        && (ddpf.GBitMask_ == 0xff00U)
                        && (ddpf.BBitMask_ == 0xffU))
                    {
                        format = Buffer_A8R8G8B8;
                    }
                }
                break;

            case DDPF_RGB:
                {
                    if((ddpf.RBitMask_ == 0xffffU)
                        && (ddpf.GBitMask_ == 0xffff0000U))
                    {
                        format = Buffer_G16R16;

                    }else if((ddpf.RBitMask_ == 0xff0000U)
                        && (ddpf.GBitMask_ == 0xff00U)
                        && (ddpf.BBitMask_ == 0xffU))
                    {
                        if(ddpf.RGBBitCount_ == 24){
                            format = Buffer_R8G8B8;

                        }else if(ddpf.RGBBitCount_ == 32){
                            format = Buffer_X8R8G8B8;
                        }

                    }else if((ddpf.RBitMask_ == 0xffU)
                        && (ddpf.GBitMask_ == 0xff00U)
                        && (ddpf.BBitMask_ == 0xff0000U))
                    {
                        format = Buffer_X8B8G8R8;
                    }
                }
                break;

            case DDPF_ALPHAPIXELS:
                {
                    format = Buffer_A8;
                }
                break;

            case DDPF_LUMINANCE:
                {
                    if(ddpf.RGBBitCount_ == 8){
                        format = Buffer_L8;

                    }else if(ddpf.RGBBitCount_ == 16){
                        format = Buffer_L16;
                    }
                }
                break;

            case DDPF_FOURCC:
                {
                    //圧縮形式
                    format = static_cast<lgraphics::BufferFormat>( ddpf.fourCC_ );
#if 0
                    switch(ddpf.fourCC_)
                    {
                    case Buffer_A16B16G16R16:
                        format = Buffer_A16B16G16R16;
                        break;

                    case Buffer_DXT1:
                        format = Buffer_DXT1;
                        break;
                    case Buffer_DXT2:
                        format = Buffer_DXT2;
                        break;
                    case Buffer_DXT3:
                        format = Buffer_DXT3;
                        break;
                    case Buffer_DXT4:
                        format = Buffer_DXT4;
                        break;
                    case Buffer_DXT5:
                        format = Buffer_DXT5;
                        break;
                    };
#endif
                }
                break;
            }

            return format;
        }

        /**
        @param ddpf ... ピクセルフォーマット
        @param format ... 

        対応フォーマットはBufferFormatに入っているもののみ
        */
        bool setFormat(IODDS::DDS_PIXELFORMAT& ddpf, BufferFormat format)
        {
            ddpf.size_ = sizeof(IODDS::DDS_PIXELFORMAT);
            switch(format)
            {
            case Buffer_G16R16:
                ddpf.RBitMask_ = 0xffffU;
                ddpf.GBitMask_ = 0xffff0000U;
                ddpf.flags_ = DDPF_RGB;
                ddpf.RGBBitCount_ = 32;
                break;

            case Buffer_A2R10G10B10:
                ddpf.RBitMask_ = 0x3ff00000U;
                ddpf.GBitMask_ = 0xffc00U;
                ddpf.BBitMask_ = 0x3ffU;
                ddpf.ABitMask_ = 0xc0000000U;
                ddpf.flags_ = DDPF_RGBA;
                ddpf.RGBBitCount_ = 32;
                break;

            case Buffer_A2B10G10R10:
                ddpf.RBitMask_ = 0x3ffU;
                ddpf.GBitMask_ = 0xffc00U;
                ddpf.BBitMask_ = 0x3ff00000U;
                ddpf.ABitMask_ = 0xc0000000U;
                ddpf.flags_ = DDPF_RGBA;
                ddpf.RGBBitCount_ = 32;
                break;

            case Buffer_A8B8G8R8:
                ddpf.RBitMask_ = 0xffU;
                ddpf.GBitMask_ = 0xff00U;
                ddpf.BBitMask_ = 0xff0000U;
                ddpf.ABitMask_ = 0xff000000U;
                ddpf.flags_ = DDPF_RGBA;
                ddpf.RGBBitCount_ = 32;
                break;

            case Buffer_A8R8G8B8:
                ddpf.RBitMask_ = 0xff0000U;
                ddpf.GBitMask_ = 0xff00U;
                ddpf.BBitMask_ = 0xffU;
                ddpf.ABitMask_ = 0xff000000U;
                ddpf.flags_ = DDPF_RGBA;
                ddpf.RGBBitCount_ = 32;
                break;

            case Buffer_R8G8B8:
                ddpf.RBitMask_ = 0xff0000U;
                ddpf.GBitMask_ = 0xff00U;
                ddpf.BBitMask_ = 0xffU;
                ddpf.flags_ = DDPF_RGB;
                ddpf.RGBBitCount_ = 24;
                break;

            case Buffer_X8R8G8B8:
                ddpf.RBitMask_ = 0xff0000U;
                ddpf.GBitMask_ = 0xff00U;
                ddpf.BBitMask_ = 0xffU;
                ddpf.flags_ = DDPF_RGB;
                ddpf.RGBBitCount_ = 32;
                break;

            case Buffer_X8B8G8R8:
                ddpf.RBitMask_ = 0xffU;
                ddpf.GBitMask_ = 0xff00U;
                ddpf.BBitMask_ = 0xff0000U;
                ddpf.flags_ = DDPF_RGB;
                ddpf.RGBBitCount_ = 32;
                break;

            case Buffer_A8:
                ddpf.ABitMask_ = 0xffU;
                ddpf.flags_ = DDPF_ALPHAPIXELS;
                ddpf.RGBBitCount_ = 8;
                break;

            case Buffer_L8:
                ddpf.RBitMask_ = 0xffU;
                ddpf.flags_ = DDPF_LUMINANCE;
                ddpf.RGBBitCount_ = 8;
                break;

            case Buffer_L16:
                ddpf.RBitMask_ = 0xffffU;
                ddpf.flags_ = DDPF_LUMINANCE;
                ddpf.RGBBitCount_ = 16;
                break;

            case Buffer_DXT1:
                ddpf.flags_ = DDPF_FOURCC;
                ddpf.fourCC_ = Buffer_DXT1;
                break;

            case Buffer_DXT2:
                ddpf.flags_ = DDPF_FOURCC;
                ddpf.fourCC_ = Buffer_DXT2;
                break;

            case Buffer_DXT3:
                ddpf.flags_ = DDPF_FOURCC;
                ddpf.fourCC_ = Buffer_DXT3;
                break;

            case Buffer_DXT4:
                ddpf.flags_ = DDPF_FOURCC;
                ddpf.fourCC_ = Buffer_DXT4;
                break;

            case Buffer_DXT5:
                ddpf.flags_ = DDPF_FOURCC;
                ddpf.fourCC_ = Buffer_DXT5;
                break;

            case Buffer_A16B16G16R16:
                ddpf.flags_ = DDPF_FOURCC;
                ddpf.fourCC_ = Buffer_A16B16G16R16;
                break;

            default:
                return false;
            }

            return true;
        }

        void readUncompressed(TextureRef& dst, lcore::istream& src, u32 mipmapLevel, const IODDS::DDS_HEADER& header)
        {
            s32 diff = 0;
            char tmp[4];

            u32 height = header.height_;

            s32 pitchSize = 0;
            if(header.flags_ & DDSD_PITCH){
                pitchSize = static_cast<s32>( header.linearSize_ );
            }else{
                u32 pixelByte = header.ddpf_.RGBBitCount_ >> 3;
                u32 pitch = pixelByte * header.width_;
                //4バイトアライメント
                pitch = (pitch + 0x03U) & (0xFFFFFCU);
                pitchSize = static_cast<s32>( pitch );
            }

#if defined(LIME_GL)
            //GL版
            s32 pitch = dst.getBytePerPixel() * header.width_;
            u8* buffer = LIME_NEW u8[pitch*height];

            for(u32 i=0; i<mipmapLevel; ++i){
                diff = (pitchSize < pitch)? 0 : pitchSize - pitch;
                LASSERT(diff<=4);

                char *b = reinterpret_cast<char*>(buffer);
                for(u32 j=0; j<height; ++j){
                    src.read(b, pitch);
                    src.read(tmp, diff);

                    b += pitch;
                }

                dst.blit(i, buffer);

                height >>= 1;
                height = (height<=0)? 1 : height;

                pitchSize >>= 1;
                pitchSize = (pitchSize<=0)? 1 : pitchSize;
                pitch >>= 1;
                pitch = (pitch<=0)? 1 : pitch;
            }
            LIME_DELETE_ARRAY(buffer);
#else
            //DX版
            bool ret = false;
            TextureRef::LockedRect rect;
            for(u32 i=0; i<mipmapLevel; ++i){
                ret = dst.lock(i, rect);
                LASSERT(ret);

                diff = (pitchSize < rect.pitch_)? 0 : pitchSize - rect.pitch_;
                LASSERT(diff<=4);

                char *buffer = reinterpret_cast<char*>(rect.bits_);
                for(u32 j=0; j<height; ++j){
                    src.read(buffer, rect.pitch_);
                    src.read(tmp, diff);

                    buffer += rect.pitch_;
                }

                dst.unlock(i);

                height >>= 1;
                height = (height<=0)? 1 : height;

                pitchSize >>= 1;
                pitchSize = (pitchSize<=0)? 1 : pitchSize;
            }
#endif
        }

        void getFourCCSize(u32& minSize, u32& linearSize, const IODDS::DDS_HEADER& header, BufferFormat format)
        {
            switch(format)
            {
            case Buffer_A16B16G16R16:
                minSize = 8;
                linearSize = header.width_ * header.height_ * minSize;
                break;

            case Buffer_DXT1:
                {
                    minSize = 8;
                    u32 w = (header.width_>=4)? (header.width_>>2) : 1;
                    u32 h = (header.height_>=4)? (header.height_>>2) : 1;
                    linearSize = w * h * minSize;
                }
                break;

            case Buffer_DXT2:
            case Buffer_DXT3:
            case Buffer_DXT4:
            case Buffer_DXT5:
                {
                    minSize = 16;
                    u32 w = (header.width_>=4)? (header.width_>>2) : 1;
                    u32 h = (header.height_>=4)? (header.height_>>2) : 1;
                    linearSize = w * h * minSize;
                }
                break;

            default:
                LASSERT(false);
            };
        }

        void readFourCC(TextureRef& dst, lcore::istream& src, u32 mipmapLevel, const IODDS::DDS_HEADER& header, BufferFormat format)
        {
            u32 minSize = 0;
            u32 linearSize =0;
            getFourCCSize(minSize, linearSize, header, format);
            if(header.flags_ & DDSD_LINEARSIZE){
                linearSize = header.linearSize_;
            }

#if defined(LIME_GL)
            //GL版
            u8* buffer = LIME_NEW u8[linearSize];

            for(u32 i=0; i<mipmapLevel; ++i){
                lcore::io::read(src, buffer, linearSize);

                dst.blit(i, buffer);

                linearSize >>= 2;
                linearSize = lcore::maximum(linearSize, minSize);
            }
            LIME_DELETE_ARRAY(buffer);
#else
            //DX版
            TextureRef::LockedRect rect;
            bool ret = false;
            for(u32 i=0; i<mipmapLevel; ++i){
                ret = dst.lock(i, rect);
                LASSERT(ret);

                char *buffer = reinterpret_cast<char*>(rect.bits_);
                src.read(buffer, linearSize);

                dst.unlock(i);

                linearSize >>= 2;
                linearSize = lcore::maximum(linearSize, minSize);
            }
#endif
        }

#if defined(LIME_GL)
        void writeUncompressed(lcore::ostream&, TextureRef&, u32, IODDS::DDS_HEADER&)
        {
            //TODO:GL版作成
            return;
        
        }
#else
        void writeUncompressed(lcore::ostream& dst, TextureRef& src, u32 mipmapLevel, IODDS::DDS_HEADER& header)
        {
            TextureRef::LockedRect rect;
            bool ret = false;
            s32 diff = 0;
            char tmp[4] = {0,0,0,0};

            u32 pixelByte = header.ddpf_.RGBBitCount_ >> 3;
            u32 pitchSize = pixelByte * header.width_;
            //4バイトアライメント
            pitchSize = (pitchSize + 0x03U) & (0xFFFFFCU);
            //header.flags_ |= DDSD_PITCH;
            //header.linearSize_ = pitchSize;

            u32 height = header.height_;
            s32 pitch = static_cast<s32>( pitchSize );

            lcore::io::write(dst, header);
            for(u32 i=0; i<mipmapLevel; ++i){
                ret = src.lock(i, rect);
                LASSERT(ret);

                diff = (pitch < rect.pitch_)? 0 : pitch - rect.pitch_;
                LASSERT(diff<=4);

                char *buffer = reinterpret_cast<char*>(rect.bits_);
                for(u32 j=0; j<height; ++j){
                    dst.write(buffer, rect.pitch_);
                    dst.write(tmp, diff);

                    buffer += rect.pitch_;
                }

                src.unlock(i);

                height >>= 1;
                height = (height<=0)? 1 : height;

                pitch >>= 1;
                pitch = (pitch<=0)? 1 : pitch;
            }
        
        }
#endif

#if defined(LIME_GL)
        void writeFourCC(lcore::ostream&, TextureRef&, u32, IODDS::DDS_HEADER&, BufferFormat)
        {
            //TODO:GL版作成
            return;
        }
#else
        void writeFourCC(lcore::ostream& dst, TextureRef& src, u32 mipmapLevel, IODDS::DDS_HEADER& header, BufferFormat format)
        {
            TextureRef::LockedRect rect;
            bool ret = false;

            u32 minSize = 0;
            u32 linearSize =0;
            getFourCCSize(minSize, linearSize, header, format);

            header.flags_ |= DDSD_LINEARSIZE;
            header.linearSize_ = linearSize;

            lcore::io::write(dst, header);
            for(u32 i=0; i<mipmapLevel; ++i){
                ret = src.lock(i, rect);
                LASSERT(ret);

                char *buffer = reinterpret_cast<char*>(rect.bits_);
                dst.write(buffer, linearSize);

                src.unlock(i);

                linearSize >>= 2;
                linearSize = lcore::maximum(linearSize, minSize);
            }
        }
#endif
    }

    bool IODDS::read(lcore::istream& is, TextureRef& texture)
    {
        u32 magic;
        lcore::io::read(is, magic);
        if(magic != DDS_MAGIC){
            return false;
        }

        DDS_HEADER header;
        lcore::io::read(is, header);

        if(is.good() == false){
            return false;
        }

        if(header.size_ != DDS_HEADER_SIZE){
            return false;
        }

        BufferFormat format = selectFormat(header.ddpf_);
        if(format == Buffer_Unknown){
            return false;
        }

        bool mipmap = ((header.flags_ & DDSD_MIPMAPCOUNT))
            && ((header.caps_ & DDSCAPS_MIPMAP))
            && header.mipmap_;

        u32 mipmapLevel = (mipmap)? header.mipmap_ : 1;

        bool fourCC = ((header.ddpf_.flags_ & DDPF_FOURCC) != 0);


        texture = Texture::create(header.width_, header.height_, mipmapLevel, Usage_None, format, Pool_Managed);
        if(false == texture.valid()){
            return false;
        }

        if(fourCC){
            readFourCC(texture, is, mipmapLevel, header, format);
        }else{
            readUncompressed(texture, is, mipmapLevel, header);
        }
        return true;
    }

    bool IODDS::write(lcore::ostream& os, TextureRef& texture)
    {
        u32 magic = DDS_MAGIC;

        TextureRef::SurfaceDesc desc;
        texture.getLevelDesc(0, desc);


        BufferFormat format = desc.format_;

        DDS_HEADER header;
        lcore::memset(&header, 0, sizeof(DDS_HEADER));
        bool ret = setFormat(header.ddpf_, format);
        if(ret == false){
            return false;
        }

        header.size_ = sizeof(DDS_HEADER);
        header.width_ = desc.width_;
        header.height_ = desc.height_;

        u32 mipmapLevel = texture.getLevels();

        bool fourCC = ((header.ddpf_.flags_ & DDPF_FOURCC) != 0);

        header.size_ = sizeof(DDS_HEADER);

        header.flags_ |= DDSD_HEIGHT;
        header.flags_ |= DDSD_WIDTH;
        header.flags_ |= DDSD_PIXELFORMAT;
        header.flags_ |= DDSD_CAPS;

        header.caps_ |= DDSCAPS_TEXTURE;

        if(mipmapLevel > 1){
            header.flags_ |= DDSD_MIPMAPCOUNT;
            header.caps_ |= DDSCAPS_MIPMAP;
            header.caps_ |= DDSCAPS_COMPLEX;
            header.mipmap_ = mipmapLevel;
        }

        lcore::io::write(os, magic);
        if(fourCC){
            writeFourCC(os, texture, mipmapLevel, header, format);
        }else{
            writeUncompressed(os, texture, mipmapLevel, header);
        }

        return true;
    }
}
}

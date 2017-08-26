/**
@file IOBMP.cpp
@author t-sakai
@date 2010/05/13 create

*/
#include "lgraphics.h"
#include <lcore/liostream.h>
#include "TextureRef.h"
#include "io/IOBMP.h"

namespace lgfx
{
namespace io
{
    namespace
    {
        void read32(u8* buffer, lcore::istream& src, u32 width, u32 height, bool leftTop)
        {
            //DX版
            u32 pitch = width * 4;

            if(leftTop){
                buffer += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    lcore::io::read(src, buffer, pitch);
                    u8 *b = buffer;
                    for(u32 j=0; j<width; ++j){
                        lcore::swap(b[0],b[2]);
                        b += 4;
                    }
                    buffer -= pitch;
                }

            }else{
                for(u32 i=0; i<height; ++i){
                    lcore::io::read(src, buffer, pitch);
                    u8 *b = buffer;
                    for(u32 j=0; j<width; ++j){
                        lcore::swap(b[0],b[2]);
                        b += 4;
                    }
                    buffer += pitch;
                }
            }
        }


        void read24(u8* buffer, lcore::istream& src, u32 width, u32 height, bool leftTop)
        {
            //DX版
            u32 pitch = width * 3;
            u32 diff = (pitch + 0x03U) & (~0x03U);
            diff -= pitch;

            u8 tmp[4];

            if(leftTop){
                buffer += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    u8 *b = buffer;
                    for(u32 j=0; j<width; ++j){
                        lcore::io::read(src, tmp, 3);
                        b[0] = tmp[2];
                        b[1] = tmp[1];
                        b[2] = tmp[0];
                        b[3] = 0xFFU;
                        b += 4;
                    }
                    lcore::io::read(src, tmp, diff);
                    buffer -= pitch;
                }

            }else{
                for(u32 i=0; i<height; ++i){
                    for(u32 j=0; j<width; ++j){
                        lcore::io::read(src, tmp, 3);
                        buffer[0] = tmp[2];
                        buffer[1] = tmp[1];
                        buffer[2] = tmp[0];
                        buffer[3] = 0xFFU;
                        buffer += 4;
                    }
                    lcore::io::read(src, tmp, diff);
                }
            }
        }


        void write32(lcore::ostream& dst, const u8* buffer, u32 width, u32 height, bool leftTop)
        {
            u32 pitch = width * 4;
            u8 tmp[4];
            if(leftTop){
                buffer += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    const u8* b = buffer;
                    for(u32 j=0; j<width; ++j){
                        tmp[0] = b[2];
                        tmp[1] = b[1];
                        tmp[2] = b[0];
                        tmp[3] = b[3];
                        lcore::io::write(dst, tmp, 4);
                        b += 4;
                    }
                    buffer -= pitch;
                }

            }else{
                for(u32 i=0; i<height; ++i){
                    for(u32 j=0; j<width; ++j){
                        tmp[0] = buffer[2];
                        tmp[1] = buffer[1];
                        tmp[2] = buffer[0];
                        tmp[3] = buffer[3];
                        lcore::io::write(dst, tmp, 4);
                        buffer += 4;
                    }
                }
            }
        }


        void write24(lcore::ostream& dst, const u8* buffer, u32 width, u32 height, bool leftTop)
        {
            u32 pitch = 3*width;
            u32 dstPitch = (pitch + 0x03U) & (~0x03U);
            u32 diff = dstPitch - pitch;

            u8 tmp[3];

            if(leftTop){
                buffer += pitch * (height-1);

                for(u32 i=0; i<height; ++i){
                    const u8* b = buffer;
                    for(u32 j=0; j<width; ++j){
                        tmp[0] = b[2];
                        tmp[1] = b[1];
                        tmp[2] = b[0];
                        lcore::io::write(dst, tmp, 3);
                        b += 3;
                    }
                    tmp[0] = 0;
                    tmp[1] = 0;
                    tmp[2] = 0;
                    lcore::io::write(dst, tmp, diff);
                    buffer -= pitch;
                }

            }else{
                for(u32 i=0; i<height; ++i){
                    for(u32 j=0; j<width; ++j){
                        tmp[0] = buffer[2];
                        tmp[1] = buffer[1];
                        tmp[2] = buffer[0];
                        lcore::io::write(dst, tmp, 3);
                        buffer += 3;
                    }
                    tmp[0] = 0;
                    tmp[1] = 0;
                    tmp[2] = 0;
                    lcore::io::write(dst, tmp, diff);
                }
            }
        }
    }

    //-----------------------------------------
    //マジックナンバー
    const u16 IOBMP::BMP_MAGIC = 'MB';

    bool IOBMP::read(
        lcore::istream& is,
        u8* buffer,
        bool sRGB,
        bool transpose,
        u32& width, u32& height, DataFormat& format)
    {
        s32 startPos = is.tellg();

        u16 magic;
        lcore::io::read(is, magic);

        BMP_HEADER header;
        if(lcore::io::read(is, header)<=0){
            return false;
        }

        if(magic != BMP_MAGIC){
            return false;
        }

        BMP_INFOHEADERV5 infoHeader = {0};
        if(lcore::io::read(is, &infoHeader, sizeof(u32))<=0){
            return false;
        }
        switch(infoHeader.infoSize_)
        {
        case 40:
            if(lcore::io::read(is, &infoHeader.width_, 40-sizeof(u32))<=0){
                return false;
            }
            break;
        case 108:
            if(lcore::io::read(is, &infoHeader.width_, 108-sizeof(u32))<=0){
                return false;
            }
            break;
        case 124:
            if(lcore::io::read(is, &infoHeader.width_, 124-sizeof(u32))<=0){
                return false;
            }
            break;
        default:
            return false;
        }


        //無圧縮だけ受け入れる
        if(infoHeader.compression_ != Compression_RGB){
            return false;
        }

        //24bitか32bitのみ受け入れる
        if(infoHeader.bitCount_ != 24
            && infoHeader.bitCount_ != 32)
        {
            return false;
        }

        width = static_cast<u32>(infoHeader.width_);
        height = static_cast<u32>(infoHeader.height_);


        format = (sRGB)? Data_R8G8B8A8_UNorm_SRGB : Data_R8G8B8A8_UNorm;
        if(NULL == buffer){
            is.seekg(startPos, lcore::ios::beg); //ファイルポインタを元の位置に戻す
            return true;
        }

        bool leftTop = (infoHeader.height_<0);
        if(transpose){
            leftTop = !leftTop;
        }

        is.seekg(startPos+header.offset_, lcore::ios::beg); //ファイルポインタをデータ位置に進める

        //DX版
        switch(infoHeader.bitCount_)
        {
        case 32:
            {
                read32(buffer, is, width, height, leftTop);
            }
            break;

        case 24:
            {
                read24(buffer, is, width, height, leftTop);
            }
            break;

        default:
            return false;
        };

        return true;
    }

    //---------------------------------------------------------------------------------------------------
    bool IOBMP::read(Texture2DRef& texture,
        lcore::istream& is,
        Usage usage,
        BindFlag bindFlag,
        CPUAccessFlag access,
        ResourceMisc misc,
        bool sRGB,
        bool transpose,
        u32& width, u32& height, DataFormat& format)
    {
        if(false == read(is, NULL, sRGB, transpose, width, height, format)){
            return false;
        }

        u32 rowBytes = getBitsPerPixel(format) * width;
        u32 size = rowBytes * height;
        u8* buffer = LNEW u8[size];

        if(false == read(is, buffer, sRGB, transpose, width, height, format)){
            LDELETE_ARRAY(buffer);
            return false;
        }

        SubResourceData initData;
        initData.pitch_ = rowBytes;
        initData.slicePitch_ = 0;
        initData.mem_ = buffer;

        texture = lgfx::Texture::create2D(
                width,
                height,
                1,
                1,
                format,
                usage,
                bindFlag,
                access,
                misc,
                &initData);

        LDELETE_ARRAY(buffer);
        return texture.valid();
    }

    //---------------------------------------------------------------------------------------------------
    bool IOBMP::write(lcore::ostream& os, const u8* buffer, u32 width, u32 height, WriteFormat format)
    {
        BMP_HEADER header = {0};
        BMP_INFOHEADER infoHeader = {0};
        u32 fileSize = sizeof(BMP_MAGIC);
        fileSize += sizeof(BMP_HEADER);
        fileSize += sizeof(BMP_INFOHEADER);

        header.reserve1_ = 0;
        header.reserve2_ = 0;
        header.size_ = sizeof(BMP_MAGIC)+sizeof(BMP_INFOHEADER);
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

        switch(format)
        {
        case WriteFormat_RGB:
            {
                infoHeader.bitCount_ = 24;
                u32 dstPitch = (3*width + 0x03U) & (~0x03U);
                infoHeader.sizeImage_ = dstPitch * height;
            }
            break;

        case WriteFormat_RGBA:
            infoHeader.bitCount_ = 32;
            infoHeader.sizeImage_ = 4 * width * height;
            break;

        default:
            return false;
            break;
        };
        header.size_ += infoHeader.sizeImage_;

        lcore::io::write(os, BMP_MAGIC);
        lcore::io::write(os, header);
        lcore::io::write(os, infoHeader);

        switch(format)
        {
        case WriteFormat_RGB:
            write24(os, buffer, width, height, true);
            break;

        case WriteFormat_RGBA:
            write32(os, buffer, width, height, true);
            break;

        default:
            break;
        };

        return true;
    }
}
}

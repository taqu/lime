/**
@file IOBMP.cpp
@author t-sakai
@date 2015/09/23 create
*/
#include "loader/IOBMP.h"
#include "core/Spectrum.h"

namespace lrender
{
    namespace
    {
        void read32(Color4* buffer, lcore::istream& src, s32 width, s32 height, bool leftTop)
        {
            //DX版
            u8 tmp[4];
            if(leftTop){
                buffer += width * (height-1);
                for(s32 i=0; i<height; ++i){
                    Color4 *b = buffer;
                    for(s32 j=0; j<width; ++j){
                        lcore::io::read(src, tmp, 4);
                        b->setRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                        b += 1;
                    }
                    buffer -= width;
                }

            }else{
                for(s32 i=0; i<height; ++i){
                    Color4 *b = buffer;
                    for(s32 j=0; j<width; ++j){
                        lcore::io::read(src, tmp, 4);
                        b->setRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                        b += 1;
                    }
                    buffer += width;
                }
            }
        }


        void read24(Color4* buffer, lcore::istream& src, s32 width, s32 height, bool leftTop)
        {
            //DX版
            s32 pitch = width * 3;
            s32 diff = (pitch + 0x03U) & (~0x03U);
            diff -= pitch;

            u8 tmp[3];

            if(leftTop){
                buffer += width * (height-1);
                for(s32 i=0; i<height; ++i){
                    Color4 *b = buffer;
                    for(s32 j=0; j<width; ++j){
                        lcore::io::read(src, tmp, 3);
                        b->setRGBA(tmp[0], tmp[1], tmp[2], 0xFFU);
                        b += 1;
                    }
                    lcore::io::read(src, tmp, diff);
                    buffer -= width;
                }

            }else{
                for(s32 i=0; i<height; ++i){
                    for(s32 j=0; j<width; ++j){
                        lcore::io::read(src, tmp, 3);
                        buffer->setRGBA(tmp[0], tmp[1], tmp[2], 0xFFU);
                        buffer += 1;
                    }
                    lcore::io::read(src, tmp, diff);
                }
            }
        }


        void write32(lcore::ostream& dst, const Color4* buffer, s32 width, s32 height, bool leftTop)
        {
            u8 tmp[4];
            if(leftTop){
                buffer += width * (height-1);
                for(s32 i=0; i<height; ++i){
                    const Color4* b = buffer;
                    for(s32 j=0; j<width; ++j){
                        b->getRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                        lcore::io::write(dst, tmp, 4);
                        b += 1;
                    }
                    buffer -= width;
                }

            }else{
                for(s32 i=0; i<height; ++i){
                    for(s32 j=0; j<width; ++j){
                        buffer->getRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                        lcore::io::write(dst, tmp, 4);
                        buffer += 1;
                    }
                }
            }
        }


        void write24(lcore::ostream& dst, const Color4* buffer, s32 width, s32 height, bool leftTop)
        {
            s32 pitch = 3*width;
            s32 dstPitch = (pitch + 0x03U) & (~0x03U);
            s32 diff = dstPitch - pitch;

            u8 tmp[4];

            if(leftTop){
                buffer += width * (height-1);

                for(s32 i=0; i<height; ++i){
                    const Color4* b = buffer;
                    for(s32 j=0; j<width; ++j){
                        b->getRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                        lcore::io::write(dst, tmp, 3);
                        b += 1;
                    }
                    tmp[0] = 0;
                    tmp[1] = 0;
                    tmp[2] = 0;
                    lcore::io::write(dst, tmp, diff);
                    buffer -= width;
                }

            }else{
                for(s32 i=0; i<height; ++i){
                    for(s32 j=0; j<width; ++j){
                        buffer->getRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                        lcore::io::write(dst, tmp, 3);
                        buffer += 1;
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

    bool IOBMP::read(lcore::istream& is, Color4* buffer, s32& width, s32& height, bool transpose)
    {
        s32 startPos = is.tellg();

        u16 magic;
        lcore::io::read(is, magic);

        BMP_HEADER header;
        lcore::io::read(is, header);

        if(magic != BMP_MAGIC){
            return false;
        }

        //無圧縮だけ受け入れる
        if(header.compression_ != Compression_RGB){
            return false;
        }

        //24bitか32bitのみ受け入れる
        if(header.bitCount_ != 24
            && header.bitCount_ != 32)
        {
            return false;
        }

        width = header.width_;
        height = header.height_;

        if(NULL == buffer){
            is.seekg(startPos, lcore::ios::beg); //ファイルポインタを元の位置に戻す
            return true;
        }

        bool leftTop = (header.height_<0);
        if(transpose){
            leftTop = !leftTop;
        }

        //DX版
        switch(header.bitCount_)
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
    bool IOBMP::write(lcore::ostream& os, const Color4* buffer, s32 width, s32 height, WriteFormat format)
    {
        BMP_HEADER header = {0};
        u32 fileSize = sizeof(BMP_MAGIC);
        fileSize += sizeof(BMP_HEADER);

        header.reserve1_ = 0;
        header.reserve2_ = 0;
        header.size_ = fileSize;
        header.offset_ = fileSize;
        header.infoSize_ = INFO_SIZE;
        header.width_ = width;
        header.height_ = height;
        header.planes_ = 1;
        header.cirImportant_ = Compression_RGB;
        header.sizeImage_ = 0;
        header.xPixPerMeter_ = 0;
        header.yPixPerMeter_ = 0;
        header.clrUsed_ = 0;
        header.cirImportant_ = 0;

        switch(format)
        {
        case WriteFormat_RGB:
            {
                header.bitCount_ = 24;
                u32 dstPitch = (3*width + 0x03U) & (~0x03U);
                header.sizeImage_ = dstPitch * height;
            }
            break;

        case WriteFormat_RGBA:
            header.bitCount_ = 32;
            header.sizeImage_ = 4 * width * height;
            break;

        default:
            return false;
            break;
        };
        header.size_ += header.sizeImage_;

        lcore::io::write(os, BMP_MAGIC);
        lcore::io::write(os, header);

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

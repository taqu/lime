/**
@file IOBMP.cpp
@author t-sakai
@date 2010/05/13 create

*/
#include "../lgraphicsAPIInclude.h"
#include "../lgraphicscore.h"

#include "IOBMP.h"
#include "../api/Enumerations.h"
#include "../api/TextureRef.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        void read32(u8* buffer, lcore::istream& src, u32 width, u32 height, bool leftTop)
        {
#if defined(LIME_GL)
            //GL版
            u32 pitch = width * 4;

            u8* b = buffer;
            if(leftTop){
                for(u32 i=0; i<height; ++i){
                    lcore::io::read(src, b, pitch);

                    u8* tmp = b;
                    for(u32 j=0; j<width; ++j){
                        lcore::swap(tmp[0], tmp[2]);
                        tmp += 4;
                    }
                    b += pitch;
                }

            }else{
                b += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    lcore::io::read(src, b, pitch);
                    u8* tmp = b;
                    for(u32 j=0; j<width; ++j){
                        lcore::swap(tmp[0], tmp[2]);
                        tmp += 4;
                    }
                    b -= pitch;
                }
            }
#else
            //DX版
            u32 pitch = width * 4;

            if(leftTop){
                buffer += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    lcore::io::read(src, buffer, pitch);
                    buffer -= pitch;
                }

            }else{
                for(u32 i=0; i<height; ++i){
                    lcore::io::read(src, buffer, pitch);
                    buffer += pitch;
                }
            }
#endif
        }


        void read24(u8* buffer, lcore::istream& src, u32 width, u32 height, bool leftTop)
        {
#if defined(LIME_GL)
            //GL版
            u32 pitch = width * 3;

            u32 diff = (width*3) & 0x03U;

            u8 tmp[4];
            u8* b = buffer;
            if(leftTop){
                for(u32 i=0; i<height; ++i){
                    for(u32 j=0; j<width; ++j){
                        lcore::io::read(src, tmp, 3);
                        b[0] = tmp[2];
                        b[1] = tmp[1];
                        b[2] = tmp[0];
                        b += 3;
                    }
                    lcore::io::read(src, tmp, diff);
                }

            }else{
                b += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    u8 *b2 = b;
                    for(u32 j=0; j<width; ++j){
                        lcore::io::read(src, tmp, 3);
                        b2[0] = tmp[2];
                        b2[1] = tmp[1];
                        b2[2] = tmp[0];
                        b2 += 3;
                    }
                    lcore::io::read(src, tmp, diff);
                    b -= pitch;
                }
            }

#else
            //DX版
            u32 pitch = width * 4;
            u32 diff = (width*3) & 0x03U;

            u8 tmp[4];

            if(leftTop){
                buffer += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    u8 *b = buffer;
                    for(u32 j=0; j<width; ++j){
                        lcore::io::read(src, b, 3);
                        b[3] = 0xFFU;
                        b += 4;
                    }
                    lcore::io::read(src, tmp, diff);
                    buffer -= pitch;
                }

            }else{
                for(u32 i=0; i<height; ++i){
                    for(u32 j=0; j<width; ++j){
                        lcore::io::read(src, buffer, 3);
                        buffer[3] = 0xFFU;
                        buffer += 4;
                    }
                    lcore::io::read(src, tmp, diff);
                }
                
            }
#endif
        }
    }

    //-----------------------------------------
    //マジックナンバー
    const u16 IOBMP::BMP_MAGIC = 'MB';

    //-----------------------------------------
    bool IOBMP::read(lcore::istream& is, u8** ppBuffer, u32& width, u32& height, BufferFormat& format, bool transpose)
    {
        LASSERT(ppBuffer != NULL);

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

        bool leftTop = (header.height_<0);
        if(transpose){
            leftTop = !leftTop;
        }

        width = header.width_;
        height = header.height_;

        //TODO:RGBテクスチャ対応
        switch(header.bitCount_)
        {
        case 32:
            {
#if defined(LIME_GL)
                format = Buffer_A8B8G8R8;
#else
                format = Buffer_A8R8G8B8;
#endif
                *ppBuffer = LIME_NEW u8[4*width*height];
                read32(*ppBuffer, is, width, height, leftTop);
            }
            break;

        case 24:
            {
#if defined(LIME_GL)
                format = Buffer_B8G8R8;
                *ppBuffer = LIME_NEW u8[3*width*height];
#else
                format = Buffer_X8R8G8B8;
                *ppBuffer = LIME_NEW u8[4*width*height];
#endif
                read24(*ppBuffer, is, width, height, leftTop);
            }
            break;

        default:
            return false;
        };

        return true;
    }

    bool IOBMP::read(lcore::istream& is, TextureRef& texture, bool transpose)
    {
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

        bool leftTop = (header.height_<0);
        if(transpose){
            leftTop = !leftTop;
        }

#if defined(LIME_GL)
            //GL版
        switch(header.bitCount_)
        {
        case 32:
            {
                texture = Texture::create(header.width_, header.height_, 1, Usage_None, Buffer_A8B8G8R8, Pool_Managed);
                u8* buffer = LIME_NEW u8[4*header.width_*header.height_];
                read32(buffer, is, header.width_, header.height_, leftTop);

                texture.blit(buffer);

                LIME_DELETE_ARRAY(buffer);
            }
            break;

        case 24:
            {
                texture = Texture::create(header.width_, header.height_, 1, Usage_None, Buffer_B8G8R8, Pool_Managed);
                u8* buffer = LIME_NEW u8[3*header.width_*header.height_];
                read24(buffer, is, header.width_, header.height_, leftTop);

                texture.blit(buffer);

                LIME_DELETE_ARRAY(buffer);
            }
            break;

        default:
            return false;
        };
#else
        //DX版
        switch(header.bitCount_)
        {
        case 32:
            {
                texture = Texture::create(header.width_, header.height_, 1, Usage_None, Buffer_A8R8G8B8, Pool_Managed);

                LockedRect rect;
                if(texture.lock(0, rect) == false){
                    return false;
                }

                u8* buffer = reinterpret_cast<u8*>( rect.bits_ );

                read32(buffer, is, header.width_, header.height_, leftTop);
                texture.unlock(0);
            }
            break;

        case 24:
            {
                texture = Texture::create(header.width_, header.height_, 1, Usage_None, Buffer_X8R8G8B8, Pool_Managed);

                LockedRect rect;
                if(texture.lock(0, rect) == false){
                    return false;
                }

                u8* buffer = reinterpret_cast<u8*>( rect.bits_ );

                read24(buffer, is, header.width_, header.height_, leftTop);
                texture.unlock(0);
            }
            break;

        default:
            return false;
        };
#endif

        return true;
    }



    //---------------------------------------------------------------------------------------------------
    bool IOBMP::write(lcore::ostream& os, const u8* buffer, u32 width, u32 height, BufferFormat format)
    {
        BMP_HEADER header = {0};
        u32 fileSize = sizeof(BMP_MAGIC);
        fileSize += sizeof(BMP_HEADER);

        header.reserve1_ = 0;
        header.reserve2_ = 0;
        header.offset_ = fileSize;
        header.infoSize_ = INFO_SIZE;
        header.width_ = width;
        header.height_ = height;
        header.planes_ = 0;
        header.cirImportant_ = Compression_RGB;
        header.sizeImage_ = 0;
        header.xPixPerMeter_ = 0;
        header.yPixPerMeter_ = 0;
        header.clrUsed_ = 0;
        header.cirImportant_ = 0;

        switch(format)
        {
        case Buffer_R8G8B8:
            header.bitCount_ = 24;
            header.sizeImage_ = 3 * width * height;
            break;

        case Buffer_A8R8G8B8:
            header.bitCount_ = 32;
            header.sizeImage_ = 4 * width * height;
            break;

        case Buffer_X8R8G8B8:
            header.bitCount_ = 24;
            header.sizeImage_ = 3 * width * height;
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
        case Buffer_R8G8B8:
        case Buffer_A8R8G8B8:
            lcore::io::write(os, buffer, header.sizeImage_);
            break;

        case Buffer_X8R8G8B8:
            {
                u32 diff = (width*3) & 0x03U;
                u8 padding[4] = {0,0,0,0};
                for(u32 i=0; i<height; ++i){
                    for(u32 j=0; j<width; ++j){
                        lcore::io::write(os, buffer, 3);
                        buffer += 4;
                    }
                    lcore::io::write(os, padding, diff);
                }
            }
            break;

        default:
            break;
        };
        return true;
    }
}
}

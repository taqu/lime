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
        void read32(TextureRef& dst, lcore::istream& src, u32 width, u32 height, bool leftTop)
        {
#if defined(LIME_GL)
            //GL版
            u32 pitch = width * 4;
            u8* buffer = LIME_NEW u8[pitch*height];

            u8* b = buffer;
            if(leftTop){
                for(u32 i=0; i<height; ++i){
                    lcore::io::read(src, b, pitch);
                    b += pitch;
                }

            }else{
                b += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    lcore::io::read(src, b, pitch);
                    b -= pitch;
                }
            }
            dst.blit(0, buffer);
            LIME_DELETE_ARRAY(buffer);
#else
            //DX版
            u32 pitch = width * 4;
            TextureRef::LockedRect rect;
            if(dst.lock(0, rect) == false){
                return;
            }
            s8* buffer = reinterpret_cast<s8*>( rect.bits_ );
            if(leftTop){
                for(u32 i=0; i<height; ++i){
                    src.read(buffer, pitch);
                    buffer += pitch;
                }

            }else{
                buffer += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    src.read(buffer, pitch);
                    buffer -= pitch;
                }
            }
            dst.unlock(0);
#endif
        }


        void read24(TextureRef& dst, lcore::istream& src, u32 width, u32 height, bool leftTop)
        {
#if defined(LIME_GL)
            //GL版
            u32 pitch = width * 4;
            u8* buffer = LIME_NEW u8[pitch*height];

            u32 diff = (width*3) & 0x03U;

            s8 tmp[4];
            u8* b = buffer;
            if(leftTop){
                for(u32 i=0; i<height; ++i){
                    for(u32 j=0; j<width; ++j){
                        lcore::io::read(src, b, 3);
                        b[3] = 0xFFU;
                        b += 4;
                    }
                    lcore::io::read(src, tmp, diff);
                }

            }else{
                b += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    u8 *b2 = b;
                    for(u32 j=0; j<width; ++j){
                        lcore::io::read(src, b2, 3);
                        b2[3] = 0xFFU;
                        b2 += 4;
                    }
                    lcore::io::read(src, tmp, diff);
                    b -= pitch;
                }
            }
            dst.blit(0, buffer);
            LIME_DELETE_ARRAY(buffer);

#else
            //DX版
            u32 pitch = width * 4;
            u32 diff = (width*3) & 0x03U;

            TextureRef::LockedRect rect;
            if(dst.lock(0, rect) == false){
                return;
            }
            s8 tmp[4];
            s8* buffer = reinterpret_cast<s8*>( rect.bits_ );
            if(leftTop){
                for(u32 i=0; i<height; ++i){
                    for(u32 j=0; j<width; ++j){
                        src.read(buffer, 3);
                        buffer[3] = 0xFFU;
                        buffer += 4;
                    }
                    src.read(tmp, diff);
                }

            }else{
                buffer += pitch * (height-1);
                for(u32 i=0; i<height; ++i){
                    s8 *b = buffer;
                    for(u32 j=0; j<width; ++j){
                        src.read(b, 3);
                        b[3] = 0xFFU;
                        b += 4;
                    }
                    src.read(tmp, diff);
                    buffer -= pitch;
                }
            }
            dst.unlock(0);
#endif
        }
    }

    IOBMP::IOBMP()
    {
    }

    IOBMP::~IOBMP()
    {
    }

    bool IOBMP::read(lcore::istream& is, TextureRef& texture)
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

        BufferFormat format;
        switch(header.bitCount_)
        {
        case 32:
            format = Buffer_A8R8G8B8;
            texture = Texture::create(header.width_, header.height_, 1, Usage_None, format, Pool_Managed);
            read32(texture, is, header.width_, header.height_, (header.height_<0));
            break;

        case 24:
            format = Buffer_X8R8G8B8;
            texture = Texture::create(header.width_, header.height_, 1, Usage_None, format, Pool_Managed);
            read24(texture, is, header.width_, header.height_, (header.height_<0));
            break;

        default:
            return false;
        };

        return true;
    }

}
}

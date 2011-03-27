/**
@file IOTGA.cpp
@author t-sakai
@date 2010/05/20 create
*/
#include "../lgraphicsAPIInclude.h"

#include "IOTGA.h"
#include "../api/Enumerations.h"
#include "../api/TextureRef.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        /**
        @brief メモリ上の連続したバイトを、2バイト整数に変換
        @param low ... メモリアドレス的に下位
        @param high ... メモリアドレス的に上位
        */
        inline u16 getU16(u8 low, u8 high)
        {
            return (static_cast<u16>(high)<<8) + low;
        }


        // Direct3D用
        //-------------------------------------------------------------------------
        void read32DX(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
        {
            LockedRect rect;
            if(dst.lock(0, rect) == false){
                return;
            }
            u8* buffer = reinterpret_cast<u8*>( rect.bits_ );

            static const u32 bpp = 4;

            u32 pitch = width * bpp;

            u32 size = pitch*height;
            lcore::io::read(src, buffer, size);

            dst.unlock(0);
        }

        void read32RLEDX(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
        {
            LockedRect rect;
            if(dst.lock(0, rect) == false){
                return;
            }
            u8* buffer = reinterpret_cast<u8*>( rect.bits_ );
            
            static const u32 bpp = 4;

            u32 pixels = width*height;
            u8 tmp[bpp];
            for(u32 i=0; i<pixels;){
                u8 byte;
                lcore::io::read(src, byte);
                //最上位ビットが０なら連続しないデータ数
                //１なら連続するデータ数
                bool b = ((byte & 0x80U) == 0)? false : true;
                u32 count = (byte & 0x7FU) + 1;
                if(b){
                    //連続するデータ
                    lcore::io::read(src, tmp, bpp);
                    for(u32 j=0; j<count; ++j){
                        for(u32 k=0; k<bpp; ++k){
                            *buffer = tmp[k];
                            ++buffer;
                        }
                    }
                }else{
                    //連続しないデータ
                    for(u32 j=0; j<count; ++j){
                        lcore::io::read(src, buffer, bpp);
                        buffer += bpp;
                    }
                }
                i += count;
            }

            dst.unlock(0);
        }


        void read24DX(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
        {
            LockedRect rect;
            if(dst.lock(0, rect) == false){
                return;
            }
            u8* buffer = reinterpret_cast<u8*>( rect.bits_ );
            
            static const u32 bpp = 3;
            static const u32 dstBpp = 4;

            u32 pixels = width*height;

            for(u32 i=0; i<pixels; ++i){
                lcore::io::read(src, buffer, bpp);
                buffer[bpp] = 0xFFU;
                buffer += dstBpp;
            }

            dst.unlock(0);
        }

        void read24RLEDX(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
        {
            static const u32 bpp = 3;
            static const u32 dstBpp = 4;

            LockedRect rect;
            if(dst.lock(0, rect) == false){
                return;
            }
            u8* buffer = reinterpret_cast<u8*>( rect.bits_ );

            u32 pixels = width*height;
            u8 tmp[bpp];
            for(u32 i=0; i<pixels;){
                u8 byte;
                lcore::io::read(src, byte);
                //最上位ビットが０なら連続しないデータ数
                //１なら連続するデータ数
                bool b = ((byte & 0x80U) == 0)? false : true;
                u32 count = (byte & 0x7FU) + 1;
                if(b){
                    //連続するデータ
                    lcore::io::read(src, tmp, bpp);
                    for(u32 j=0; j<count; ++j){
                        buffer[0] = tmp[0];
                        buffer[1] = tmp[1];
                        buffer[2] = tmp[2];
                        buffer[3] = 0xFFU;
                        buffer += dstBpp;
                    }
                }else{
                    //連続しないデータ
                    for(u32 j=0; j<count; ++j){
                        lcore::io::read(src, tmp, bpp);
                        buffer[0] = tmp[0];
                        buffer[1] = tmp[1];
                        buffer[2] = tmp[2];
                        buffer[3] = 0xFFU;
                        buffer += dstBpp;
                    }
                }
                i += count;
            }

            dst.unlock(0);
        }


        // OpenGL用
        //-------------------------------------------------------------------------
        void read32GL(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
        {
            static const u32 bpp = 4;

            //バッファにロードして転送
            u32 rowBytes = width * bpp;
            u8* buffer = LIME_NEW u8[rowBytes*height];

            //転地する
            u8* tmpBuffer = buffer + rowBytes * (height-1);
            u8 tmp[bpp];
            for(u32 i=0; i<height; ++i){

                u8* row = tmpBuffer;
                for(u32 j=0; j<width; ++j){
                    lcore::io::read(src, tmp, bpp);
                    row[0] = tmp[2];
                    row[1] = tmp[1];
                    row[2] = tmp[0];
                    row[3] = tmp[3];
                    row += bpp;
                }
                tmpBuffer -= rowBytes;
            }

            dst.blit(0, buffer);
            LIME_DELETE_ARRAY(buffer);
        }

        void read32RLEGL(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
        {
            static const u32 bpp = 4;

            //バッファにロードして転送
            u32 rowBytes = width * bpp;
            u8* buffer = LIME_NEW u8[rowBytes*height];

            //転地する
            u32 pixels = width*height;
            u8* tmpBuffer = buffer + rowBytes * (height-1);
            u8* row = tmpBuffer;
            u32 x = 0;
            u32 count = 0;
            u8 byte = 0;
            u8 tmp[bpp];
            for(u32 i=0; i<pixels;){

                lcore::io::read(src, byte);
                //最上位ビットが０なら連続しないデータ数
                //１なら連続するデータ数
                count = (byte & 0x7FU) + 1;
                bool b = ((byte & 0x80U) == 0)? false : true;
                if(b){
                    //連続するデータ
                    lcore::io::read(src, tmp, bpp);
                    for(u32 j=0; j<count; ++j){

                        row[0] = tmp[2];
                        row[1] = tmp[1];
                        row[2] = tmp[0];
                        row[3] = tmp[3];

                        row += bpp;

                        if(++x>=width){
                            x = 0;
                            tmpBuffer -= rowBytes;
                            row = tmpBuffer;
                        }
                    }
                }else{
                    //連続しないデータ
                    for(u32 j=0; j<count; ++j){
                        lcore::io::read(src, tmp, bpp);
                        row[0] = tmp[2];
                        row[1] = tmp[1];
                        row[2] = tmp[0];
                        row[3] = tmp[3];

                        row += bpp;

                        if(++x>=width){
                            x = 0;
                            tmpBuffer -= rowBytes;
                            row = tmpBuffer;
                        }
                    }
                }
                i += count;
            }

            dst.blit(0, buffer);
            LIME_DELETE_ARRAY(buffer);
        }


        void read24GL(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
        {
            static const u32 bpp = 3;

            //バッファにロードして転送
            u32 rowBytes = width * bpp;
            u8* buffer = LIME_NEW u8[rowBytes*height];

            //転地する
            u8* tmpBuffer = buffer + rowBytes * (height-1);
            u8 tmp[bpp];
            for(u32 i=0; i<height; ++i){

                u8* row = tmpBuffer;
                for(u32 j=0; j<width; ++j){
                    lcore::io::read(src, tmp, bpp);
                    row[0] = tmp[0];
                    row[1] = tmp[1];
                    row[2] = tmp[2];
                    row += bpp;
                }
                tmpBuffer -= rowBytes;
            }

            dst.blit(0, buffer);
            LIME_DELETE_ARRAY(buffer);
        }

        void read24RLEGL(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
        {
            static const u32 bpp = 3;

            //バッファにロードして転送
            u32 rowBytes = width * bpp;
            u8* buffer = LIME_NEW u8[rowBytes*height];

            //転地する
            u32 pixels = width*height;
            u8* tmpBuffer = buffer + rowBytes * (height-1);
            u8* row = tmpBuffer;
            u32 x = 0;
            u32 count = 0;
            u8 byte = 0;
            u8 tmp[bpp];
            for(u32 i=0; i<pixels;){
                lcore::io::read(src, byte);
                //最上位ビットが０なら連続しないデータ数
                //１なら連続するデータ数
                count = (byte & 0x7FU) + 1;
                bool b = ((byte & 0x80U) == 0)? false : true;

                if(b){
                    //連続するデータ
                    lcore::io::read(src, tmp, bpp);
                    for(u32 j=0; j<count; ++j){
                        row[0] = tmp[0];
                        row[1] = tmp[1];
                        row[2] = tmp[2];
                        row += bpp;

                        if(++x>=width){
                            x = 0;
                            tmpBuffer -= rowBytes;
                            row = tmpBuffer;
                        }
                    }

                }else{
                    //連続しないデータ
                    for(u32 j=0; j<count; ++j){
                        lcore::io::read(src, row, bpp);
                        row += bpp;

                        if(++x>=width){
                            x = 0;
                            tmpBuffer -= rowBytes;
                            row = tmpBuffer;
                        }
                    }
                }
                i += count;
            }

            dst.blit(0, buffer);
            LIME_DELETE_ARRAY(buffer);
        }

    }

    IOTGA::IOTGA()
    {
    }

    IOTGA::~IOTGA()
    {
    }

    bool IOTGA::read(lcore::istream& is, TextureRef& texture)
    {
        ///GL版作る
        u8 tgaHeader[TGA_HEADER_SIZE];

        lcore::io::read(is, tgaHeader, TGA_HEADER_SIZE);

        u32 width = static_cast<u32>( getU16(tgaHeader[Offset_WidthL], tgaHeader[Offset_WidthH]) );
        u32 height = static_cast<u32>( getU16(tgaHeader[Offset_HeightL], tgaHeader[Offset_HeightH]) );

        u32 bpp = static_cast<u32>( tgaHeader[Offset_BitPerPixel] );


        u8 type = tgaHeader[Offset_ImageType];

        // フルカラーのみ受け入れる
        if(type != Type_FullColor
            && type != Type_FullColorRLE)
        {
            return false;
        }

        //24bitか32bitのみ受け入れる
        if(bpp != 24
            && bpp != 32)
        {
            return false;
        }

        // ID文字列があればスキップ
        if(tgaHeader[Offset_IDLeng] > 0){
            is.seekg(tgaHeader[Offset_IDLeng], lcore::ios::cur);
        }

        // カラーマップがあればスキップ
        if(tgaHeader[Offset_ColorMapType] == 1){
            u32 length = static_cast<u32>( getU16(tgaHeader[Offset_ColorMapLengL], tgaHeader[Offset_ColorMapLengH]) );
            length *= (tgaHeader[Offset_ColorMapSize] >> 3);
            is.seekg(length, lcore::ios::cur);
        }

        BufferFormat format;

#if defined(LIME_GL)
        switch(bpp)
        {
        case 32:
            format = Buffer_A8B8G8R8;
            texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
            if(type == Type_FullColorRLE){
                read32RLEGL(texture, is, width, height);
            }else{
                read32GL(texture, is, width, height);
            }
            break;

        case 24:
            format = Buffer_R8G8B8;
            texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
            if(type == Type_FullColorRLE){
                read24RLEGL(texture, is, width, height);
            }else{
                read24GL(texture, is, width, height);
            }
            break;

        default:
            return false;
        };

#else
        switch(bpp)
        {
        case 32:
            format = Buffer_A8R8G8B8;
            texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
            if(type == Type_FullColorRLE){
                read32RLEDX(texture, is, width, height);
            }else{
                read32DX(texture, is, width, height);
            }
            break;

        case 24:
            format = Buffer_X8R8G8B8;
            texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
            if(type == Type_FullColorRLE){
                read24RLEDX(texture, is, width, height);
            }else{
                read24DX(texture, is, width, height);
            }
            break;

        default:
            return false;
        };
#endif

        return true;
    }

}
}

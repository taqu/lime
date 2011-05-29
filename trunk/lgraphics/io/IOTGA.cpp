/**
@file IOTGA.cpp
@author t-sakai
@date 2010/05/20 create
*/
#include "../lgraphicsAPIInclude.h"

#include "IOTGA.h"
#include "../api/Enumerations.h"
#include "../api/TextureRef.h"

#if defined(LIME_GL)
#include "IOTGAGL.cpp"
#else
#include "IOTGADX.cpp"
#endif

namespace lgraphics
{
    namespace io
    {

        bool IOTGA::read(lcore::istream& is, u8** ppBuffer, u32& width, u32& height, BufferFormat& format, bool transpose)
        {
            u8 tgaHeader[TGA_HEADER_SIZE];

            lcore::io::read(is, tgaHeader, TGA_HEADER_SIZE);

            width = static_cast<u32>( getU16(tgaHeader[Offset_WidthL], tgaHeader[Offset_WidthH]) );
            height = static_cast<u32>( getU16(tgaHeader[Offset_HeightL], tgaHeader[Offset_HeightH]) );

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

            if(transpose){
                switch(bpp)
                {
                case 32:
                    {
#if defined(LIME_GL)
                        format = Buffer_A8B8G8R8;
#else
                        format = Buffer_A8R8G8B8;
#endif
                        *ppBuffer = LIME_NEW u8[4*width*height];

                        if(type == Type_FullColorRLE){
                            read32RLETranspose(*ppBuffer, is, width, height);
                        }else{
                            read32(*ppBuffer, is, width, height);
                        }
                    }
                    break;

                case 24:
                    {
#if defined(LIME_GL)
                        format = Buffer_B8G8R8;
#else
                        format = Buffer_X8R8G8B8;
#endif
                        *ppBuffer = LIME_NEW u8[3*width*height];
                        if(type == Type_FullColorRLE){
                            read24RLE(*ppBuffer, is, width, height);
                        }else{
                            read24(*ppBuffer, is, width, height);
                        }
                    }
                    break;

                default:
                    return false;
                };


            }else{
                switch(bpp)
                {
                case 32:
                    {
#if defined(LIME_GL)
                        format = Buffer_A8B8G8R8;
#else
                        format = Buffer_A8R8G8B8;
#endif
                        *ppBuffer = LIME_NEW u8[4*width*height];
                        if(type == Type_FullColorRLE){
                            read32RLETranspose(*ppBuffer, is, width, height);
                        }else{
                            read32Transpose(*ppBuffer, is, width, height);
                        }
                    }
                    break;

                case 24:
                    {
#if defined(LIME_GL)
                        format = Buffer_B8G8R8;
#else
                        format = Buffer_X8R8G8B8;
#endif
                        *ppBuffer = LIME_NEW u8[3*width*height];
                        if(type == Type_FullColorRLE){
                            read24RLETranspose(*ppBuffer, is, width, height);
                        }else{
                            read24Transpose(*ppBuffer, is, width, height);
                        }
                    }
                    break;

                default:
                    return false;
                };
            }


            return true;
        }

        bool IOTGA::read(lcore::istream& is, TextureRef& texture, bool transpose)
        {
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

#if defined(LIME_GL)
            if(transpose){
                switch(bpp)
                {
                case 32:
                    {
                        texture = Texture::create(width, height, 1, Usage_None, Buffer_A8B8G8R8, Pool_Managed);
                        u8* buffer = LIME_NEW u8[4*width*height];

                        if(type == Type_FullColorRLE){
                            read32RLETranspose(buffer, is, width, height);
                        }else{
                            read32(buffer, is, width, height);
                        }
                        texture.blit(buffer);
                        LIME_DELETE_ARRAY(buffer);
                    }
                    break;

                case 24:
                    {
                        texture = Texture::create(width, height, 1, Usage_None, Buffer_B8G8R8, Pool_Managed);
                        u8* buffer = LIME_NEW u8[3*width*height];
                        if(type == Type_FullColorRLE){
                            read24RLE(buffer, is, width, height);
                        }else{
                            read24(buffer, is, width, height);
                        }
                        texture.blit(buffer);
                        LIME_DELETE_ARRAY(buffer);
                    }
                    break;

                default:
                    return false;
                };


            }else{
                switch(bpp)
                {
                case 32:
                    {
                        texture = Texture::create(width, height, 1, Usage_None, Buffer_A8B8G8R8, Pool_Managed);
                        u8* buffer = LIME_NEW u8[4*width*height];
                        if(type == Type_FullColorRLE){
                            read32RLETranspose(buffer, is, width, height);
                        }else{
                            read32Transpose(buffer, is, width, height);
                        }
                        texture.blit(buffer);
                        LIME_DELETE_ARRAY(buffer);
                    }
                    break;

                case 24:
                    {
                        texture = Texture::create(width, height, 1, Usage_None, Buffer_B8G8R8, Pool_Managed);
                        u8* buffer = LIME_NEW u8[3*width*height];
                        if(type == Type_FullColorRLE){
                            read24RLETranspose(buffer, is, width, height);
                        }else{
                            read24Transpose(buffer, is, width, height);
                        }
                        texture.blit(buffer);
                        LIME_DELETE_ARRAY(buffer);
                    }
                    break;

                default:
                    return false;
                };
            }

#else
            LockedRect rect;
            if(transpose){
                switch(bpp)
                {
                case 32:
                    {
                        texture = Texture::create(width, height, 1, Usage_None, Buffer_A8R8G8B8, Pool_Managed);
                        if(texture.lock(0, rect) == false){
                            return false;
                        }

                        u8* buffer = reinterpret_cast<u8*>( rect.bits_ );
                        if(type == Type_FullColorRLE){
                            read32RLETranspose(buffer, is, width, height);
                        }else{
                            read32Transpose(buffer, is, width, height);
                        }
                        texture.unlock(0);
                    }
                    break;

                case 24:
                    {
                        texture = Texture::create(width, height, 1, Usage_None, Buffer_X8R8G8B8, Pool_Managed);
                        if(texture.lock(0, rect) == false){
                            return false;
                        }

                        u8* buffer = reinterpret_cast<u8*>( rect.bits_ );
                        if(type == Type_FullColorRLE){
                            read24RLETranspose(buffer, is, width, height);
                        }else{
                            read24Transpose(buffer, is, width, height);
                        }
                        texture.unlock(0);
                    }
                    break;

                default:
                    return false;
                };


            }else{
                switch(bpp)
                {
                case 32:
                    {
                        texture = Texture::create(width, height, 1, Usage_None, Buffer_A8R8G8B8, Pool_Managed);
                        if(texture.lock(0, rect) == false){
                            return false;
                        }

                        u8* buffer = reinterpret_cast<u8*>( rect.bits_ );
                        if(type == Type_FullColorRLE){
                            read32RLE(buffer, is, width, height);
                        }else{
                            read32(buffer, is, width, height);
                        }
                        texture.unlock(0);
                    }
                    break;

                case 24:
                    {
                        texture = Texture::create(width, height, 1, Usage_None, Buffer_X8R8G8B8, Pool_Managed);
                        if(texture.lock(0, rect) == false){
                            return false;
                        }

                        u8* buffer = reinterpret_cast<u8*>( rect.bits_ );
                        if(type == Type_FullColorRLE){
                            read24RLE(buffer, is, width, height);
                        }else{
                            read24(buffer, is, width, height);
                        }
                        texture.unlock(0);
                    }
                    break;

                default:
                    return false;
                };
            }
#endif

            return true;
        }

    }
}

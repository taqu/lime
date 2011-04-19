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

        IOTGA::IOTGA()
        {
        }

        IOTGA::~IOTGA()
        {
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

            BufferFormat format;
#if defined(LIME_GL)
            if(transpose){
                switch(bpp)
                {
                case 32:
                    format = Buffer_A8B8G8R8;
                    texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
                    if(type == Type_FullColorRLE){
                        read32RLETranspose(texture, is, width, height);
                    }else{
                        read32(texture, is, width, height);
                    }
                    break;

                case 24:
                    format = Buffer_R8G8B8;
                    texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
                    if(type == Type_FullColorRLE){
                        read24RLE(texture, is, width, height);
                    }else{
                        read24(texture, is, width, height);
                    }
                    break;

                default:
                    return false;
                };


            }else{
                switch(bpp)
                {
                case 32:
                    format = Buffer_A8B8G8R8;
                    texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
                    if(type == Type_FullColorRLE){
                        read32RLETranspose(texture, is, width, height);
                    }else{
                        read32Transpose(texture, is, width, height);
                    }
                    break;

                case 24:
                    format = Buffer_R8G8B8;
                    texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
                    if(type == Type_FullColorRLE){
                        read24RLETranspose(texture, is, width, height);
                    }else{
                        read24Transpose(texture, is, width, height);
                    }
                    break;

                default:
                    return false;
                };
            }

#else
            if(transpose){
                switch(bpp)
                {
                case 32:
                    format = Buffer_A8R8G8B8;
                    texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
                    if(type == Type_FullColorRLE){
                        read32RLETranspose(texture, is, width, height);
                    }else{
                        read32Transpose(texture, is, width, height);
                    }
                    break;

                case 24:
                    format = Buffer_X8R8G8B8;
                    texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
                    if(type == Type_FullColorRLE){
                        read24RLETranspose(texture, is, width, height);
                    }else{
                        read24Transpose(texture, is, width, height);
                    }
                    break;

                default:
                    return false;
                };


            }else{
                switch(bpp)
                {
                case 32:
                    format = Buffer_A8R8G8B8;
                    texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
                    if(type == Type_FullColorRLE){
                        read32RLE(texture, is, width, height);
                    }else{
                        read32(texture, is, width, height);
                    }
                    break;

                case 24:
                    format = Buffer_X8R8G8B8;
                    texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
                    if(type == Type_FullColorRLE){
                        read24RLE(texture, is, width, height);
                    }else{
                        read24(texture, is, width, height);
                    }
                    break;

                default:
                    return false;
                };
            }
#endif

#if 0
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
#endif
            return true;
        }

    }
}

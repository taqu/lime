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
#include "../io/IOTGAGL.cpp"
#else
#include "../io/IOTGADX.cpp"
#endif

namespace lgraphics
{
    namespace io
    {
        bool IOTGA::read(lcore::istream& is, u8* buffer, u32& width, u32& height, DataFormat& format, bool transpose)
        {
            u8 tgaHeader[TGA_HEADER_SIZE];

            s32 startPos = is.tellg();

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

            format = Data_R8G8B8A8_UNorm;
            if(NULL == buffer){
                is.seekg(startPos, lcore::ios::beg); //ファイルポインタを元の位置に戻す
                return true;
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
                        if(type == Type_FullColorRLE){
                            read32RLE(buffer, is, width, height);
                        }else{
                            read32(buffer, is, width, height);
                        }
                    }
                    break;

                case 24:
                    {
                        if(type == Type_FullColorRLE){
                            read24RLE(buffer, is, width, height);
                        }else{
                            read24(buffer, is, width, height);
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
                        if(type == Type_FullColorRLE){
                            read32RLETranspose(buffer, is, width, height);
                        }else{
                            read32Transpose(buffer, is, width, height);
                        }
                    }
                    break;

                case 24:
                    {
                        if(type == Type_FullColorRLE){
                            read24RLETranspose(buffer, is, width, height);
                        }else{
                            read24Transpose(buffer, is, width, height);
                        }
                    }
                    break;

                default:
                    return false;
                };
            }

            return true;
        }

    }
}

/**
@file IOTGA.cpp
@author t-sakai
@date 2010/05/20 create
*/
#include "../lgraphicsAPIInclude.h"
#include <lcore/clibrary.h>

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
        //-----------------------------------------------------------------
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

            format = Data_R8G8B8A8_UNorm_SRGB;
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
namespace
{
        u8 calcRunLength(const u8* buffer, u32 index, u32 pixels)
        {
            static const u32 bpp = 4;
            static const u32 maxCount = 0x7FU;

            if(pixels<=(index+1)){
                return 0x80U | 0x01U;
            }

            const u8* next = buffer + bpp;


            u8 count = 0;
            if((buffer[0] == next[0])
                && (buffer[1] == next[1])
                && (buffer[2] == next[2])
                && (buffer[3] == next[3]))
            {
                for(u32 i=index; i<(pixels-1); ++i){

                    if((buffer[0] != next[0])
                        || (buffer[1] != next[1])
                        || (buffer[2] != next[2])
                        || (buffer[3] != next[3]))
                    {
                        break;
                    }

                    if(maxCount<=++count){
                        break;
                    }
                    next += bpp;
                }
                return 0x80U | count;

            }else{

                for(u32 i=index; i<(pixels-1); ++i){
                    if((buffer[0] == next[0])
                        && (buffer[1] == next[1])
                        && (buffer[2] == next[2])
                        && (buffer[3] == next[3]))
                    {
                        break;
                    }

                    if(maxCount<=++count){
                        break;
                    }
                    buffer += bpp;
                    next += bpp;
                }
                return count;
            }
        }

        void write32RLE(lcore::ostream& os, const u8* buffer, u32 width, u32 height)
        {
            static const u32 bpp = 4;

            u32 pixels = width*height;
            for(u32 i=0; i<pixels;){

                u8 run = calcRunLength(buffer, i, pixels);
                u8 count = run & 0x7FU;

                lcore::io::write(os, run);

                if((run & 0x80U) != 0){
                    lcore::io::write(os, buffer, 4);
                }else{
                    lcore::io::write(os, buffer, count*4);
                }
                buffer += count*4;
                i += count;
            }
        }

        void write32(lcore::ostream& os, const u8* buffer, u32 width, u32 height)
        {
            static const u32 bpp = 4;

            u32 pixels = width*height;
            u8 tmp[bpp];
            for(u32 i=0; i<pixels; ++i){
                tmp[0] = buffer[2];
                tmp[1] = buffer[1];
                tmp[2] = buffer[0];
                tmp[3] = buffer[3];
                lcore::io::write(os, tmp, bpp);
                buffer += bpp;
            }

        }
}

    //---------------------------------------------------------------------------------------------------
    bool IOTGA::read(Texture2DRef& texture,
        lcore::istream& is,
        Usage usage,
        BindFlag bindFlag,
        CPUAccessFlag access,
        ResourceMisc misc,
        u32& width, u32& height, DataFormat& format,
        bool transpose)
    {
        if(false == read(is, NULL, width, height, format, transpose)){
            return false;
        }


        u32 rowBytes = getBitsPerPixel(format) * width;
        u32 size = rowBytes * height;
        u8* buffer = LIME_NEW u8[size];

        if(false == read(is, buffer, width, height, format, transpose)){
            LIME_DELETE_ARRAY(buffer);
            return false;
        }

        lcore::io::read(is, buffer, size);

        SubResourceData initData;
        initData.pitch_ = rowBytes;
        initData.slicePitch_ = 0;
        initData.mem_ = buffer;
        SRVDesc desc;
        desc.dimension_ = lgraphics::ViewSRVDimension_Texture2D;
        desc.format_ = format;
        desc.tex2D_.mipLevels_ = 1;
        desc.tex2D_.mostDetailedMip_ = 0;

        texture = lgraphics::Texture::create2D(
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

        LIME_DELETE_ARRAY(buffer);
        return texture.valid();
    }

        //-----------------------------------------------------------------
        bool IOTGA::write(lcore::ostream& os, const u8* buffer, u32 width, u32 height)
        {
            u8 tgaHeader[TGA_HEADER_SIZE];
            lcore::memset(tgaHeader, 0, TGA_HEADER_SIZE);

            tgaHeader[Offset_WidthL] = width & 0xFFU;
            tgaHeader[Offset_WidthH] = (width>>8) & 0xFFU;

            tgaHeader[Offset_HeightL] = height & 0xFFU;
            tgaHeader[Offset_HeightH] = (height>>8) & 0xFFU;

            tgaHeader[Offset_BitPerPixel] = 32;

            tgaHeader[Offset_ImageType] = Type_FullColor;
            //tgaHeader[Offset_ImageType] = Type_FullColorRLE;

            tgaHeader[Offset_Discripter] = 8; //4bitのアルファチェネルビット深度

            lcore::io::write(os, tgaHeader, TGA_HEADER_SIZE);
            //write32RLE(os, buffer, width, height);
            write32(os, buffer, width, height);
            return true;
        }
    }
}

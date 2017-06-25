/**
@file IOTGA.cpp
@author t-sakai
@date 2010/05/20 create
*/
#include "lgraphics.h"

#include "io/IOTGA.h"
#include "Enumerations.h"
#include "TextureRef.h"

namespace lgfx
{
#if defined(LGFX_GL)
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


    //-------------------------------------------------------------------------
    void read32(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;


        //バッファにロード
        u8 tmp[bpp];
        for(u32 i=0; i<height; ++i){

            for(u32 j=0; j<width; ++j){
                lcore::io::read(src, tmp, bpp);
                buffer[0] = tmp[2];
                buffer[1] = tmp[1];
                buffer[2] = tmp[0];
                buffer[3] = tmp[3];
                buffer += bpp;
            }
        }
    }

    void read32RLE(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        //バッファにロード
        u32 pixels = width*height;
        u8 tmp[bpp];
        u8 byte;
        for(u32 i=0; i<pixels;){
            lcore::io::read(src, byte);
            //最上位ビットが０なら連続しないデータ数
            //１なら連続するデータ数
            bool b = ((byte & 0x80U) == 0)? false : true;
            u32 count = (byte & 0x7FU) + 1;
            if(b){
                //連続するデータ
                lcore::io::read(src, tmp, bpp);
                for(u32 j=0; j<count; ++j){
                    buffer[0] = tmp[2];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[0];
                    buffer[3] = tmp[3];
                    buffer += bpp;
                }
            }else{
                //連続しないデータ
                for(u32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);

                    buffer[0] = tmp[2];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[0];
                    buffer[3] = tmp[3];
                    buffer += bpp;
                }
            }
            i += count;
        }
    }


    void read24(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;

        //バッファにロード`
        u8 tmp[bpp];
        for(u32 i=0; i<height; ++i){

            for(u32 j=0; j<width; ++j){
                lcore::io::read(src, tmp, bpp);
                buffer[0] = tmp[2];
                buffer[1] = tmp[1];
                buffer[2] = tmp[0];
                buffer += bpp;
            }
        }
    }

    void read24RLE(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;

        //バッファにロードして転送
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
                    buffer[0] = tmp[2];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[0];
                    buffer += bpp;
                }
            }else{
                //連続しないデータ
                for(u32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);
                    buffer[0] = tmp[2];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[0];
                    buffer += bpp;
                }
            }
            i += count;
        }
    }


    //-------------------------------------------------------------------------
    void read32Transpose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        //バッファにロード
        u32 rowBytes = width * bpp;

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
    }

    void read32RLETranspose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        //バッファにロードして転送
        u32 rowBytes = width * bpp;

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
    }


    void read24Transpose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;

        //バッファにロード
        u32 rowBytes = width * bpp;

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
                row += bpp;
            }
            tmpBuffer -= rowBytes;
        }
    }

    void read24RLETranspose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;

        //バッファにロード
        u32 rowBytes = width * bpp;

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
    }

}

#else
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

    //-------------------------------------------------------------------------
    void read32(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        u32 pixels = width*height;

        u8 tmp[bpp];
        for(u32 i=0; i<pixels; ++i){
            lcore::io::read(src, tmp, bpp);
            buffer[0] = tmp[2];
            buffer[1] = tmp[1];
            buffer[2] = tmp[0];
            buffer[3] = tmp[3];
            buffer += bpp;
        }

        //lcore::io::read(src, buffer, pixels*bpp);
    }

    void read32RLE(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
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
                    buffer[0] = tmp[2];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[0];
                    buffer[3] = tmp[3];
                    buffer += bpp;
                }
            }else{
                //連続しないデータ
                for(u32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);
                    buffer[0] = tmp[2];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[0];
                    buffer[3] = tmp[3];
                    buffer += bpp;
                }
            }
            i += count;
        }
    }


    void read24(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;
        static const u32 dstBpp = 4;

        u32 pixels = width*height;

        u8 tmp[bpp];
        for(u32 i=0; i<pixels; ++i){
            lcore::io::read(src, tmp, bpp);
            buffer[0] = tmp[2];
            buffer[1] = tmp[1];
            buffer[2] = tmp[0];
            buffer[bpp] = 0xFFU;
            buffer += dstBpp;
        }
    }

    void read24RLE(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;
        static const u32 dstBpp = 4;

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
                    buffer[0] = tmp[2];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[0];
                    buffer[3] = 0xFFU;
                    buffer += dstBpp;
                }
            }else{
                //連続しないデータ
                for(u32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);
                    buffer[0] = tmp[2];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[0];
                    buffer[3] = 0xFFU;
                    buffer += dstBpp;
                }
            }
            i += count;
        }
    }



    //-------------------------------------------------------------------------
    void read32Transpose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        //転地する
        u32 rowBytes = width * bpp;
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
    }

    void read32RLETranspose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        //転地する
        u32 pixels = width*height;
        u32 rowBytes = width * bpp;
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
    }


    void read24Transpose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;
        static const u32 dstBpp = 4;

        //転地する
        u32 rowBytes = width * dstBpp;
        u8* tmpBuffer = buffer + rowBytes * (height-1);
        u8 tmp[bpp];
        for(u32 i=0; i<height; ++i){

            u8* row = tmpBuffer;
            for(u32 j=0; j<width; ++j){
                lcore::io::read(src, tmp, bpp);
                row[0] = tmp[2];
                row[1] = tmp[1];
                row[2] = tmp[0];
                row[bpp] = 0xFFU;
                row += dstBpp;
            }
            tmpBuffer -= rowBytes;
        }
    }

    void read24RLETranspose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;
        static const u32 dstBpp = 4;

        //転地する
        u32 rowBytes = width * dstBpp;
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
                    row[bpp] = 0xFFU;
                    row += dstBpp;

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
                    row[bpp] = 0xFFU;
                    row += dstBpp;

                    if(++x>=width){
                        x = 0;
                        tmpBuffer -= rowBytes;
                        row = tmpBuffer;
                    }
                }
            }
            i += count;
        }
    }
}
#endif

    namespace io
    {
        //-----------------------------------------------------------------
        bool IOTGA::read(
            lcore::istream& is,
            u8* buffer,
            bool sRGB,
            bool transpose,
            u32& width, u32& height, DataFormat& format)
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

            format = (sRGB)? Data_R8G8B8A8_UNorm_SRGB : Data_R8G8B8A8_UNorm;
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

        lcore::io::read(is, buffer, size);

        SubResourceData initData;
        initData.pitch_ = rowBytes;
        initData.slicePitch_ = 0;
        initData.mem_ = buffer;
        SRVDesc desc;
        desc.dimension_ = lgfx::SRVDimension_Texture2D;
        desc.format_ = format;
        desc.tex2D_.mipLevels_ = 1;
        desc.tex2D_.mostDetailedMip_ = 0;

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

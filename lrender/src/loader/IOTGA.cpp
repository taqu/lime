/**
@file IOTGA.cpp
@author t-sakai
@date 2015/09/23 create
*/
#include "loader/IOTGA.h"
#include "core/Spectrum.h"

namespace lrender
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

    //-------------------------------------------------------------------------
    void read32(Color4* buffer, lcore::istream& src, s32 width, s32 height)
    {
        static const s32 bpp = 4;

        s32 pixels = width*height;

        u8 tmp[bpp];
        for(s32 i=0; i<pixels; ++i){
            lcore::io::read(src, tmp, bpp);
            buffer->setRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
            buffer += 1;
        }

        //lcore::io::read(src, buffer, pixels*bpp);
    }

    void read32RLE(Color4* buffer, lcore::istream& src, s32 width, s32 height)
    {
        static const s32 bpp = 4;

        s32 pixels = width*height;
        u8 tmp[bpp];
        for(s32 i=0; i<pixels;){
            u8 byte;
            lcore::io::read(src, byte);
            //最上位ビットが０なら連続しないデータ数
            //１なら連続するデータ数
            bool b = ((byte & 0x80U) == 0)? false : true;
            s32 count = (byte & 0x7FU) + 1;
            if(b){
                //連続するデータ
                lcore::io::read(src, tmp, bpp);
                for(s32 j=0; j<count; ++j){
                    buffer->setRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                    buffer += 1;
                }
            }else{
                //連続しないデータ
                for(s32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);
                    buffer->setRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                    buffer += 1;
                }
            }
            i += count;
        }
    }


    void read24(Color4* buffer, lcore::istream& src, s32 width, s32 height)
    {
        static const s32 bpp = 3;

        s32 pixels = width*height;

        u8 tmp[bpp];
        for(s32 i=0; i<pixels; ++i){
            lcore::io::read(src, tmp, bpp);
            buffer->setRGBA(tmp[0], tmp[1], tmp[2], 0xFFU);
            buffer += 1;
        }
    }

    void read24RLE(Color4* buffer, lcore::istream& src, s32 width, s32 height)
    {
        static const s32 bpp = 3;

        s32 pixels = width*height;
        u8 tmp[bpp];
        for(s32 i=0; i<pixels;){
            u8 byte;
            lcore::io::read(src, byte);
            //最上位ビットが０なら連続しないデータ数
            //１なら連続するデータ数
            bool b = ((byte & 0x80U) == 0)? false : true;
            s32 count = (byte & 0x7FU) + 1;
            if(b){
                //連続するデータ
                lcore::io::read(src, tmp, bpp);
                for(s32 j=0; j<count; ++j){
                    buffer->setRGBA(tmp[0], tmp[1], tmp[2], 0xFFU);
                    buffer += 1;
                }
            }else{
                //連続しないデータ
                for(s32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);
                    buffer->setRGBA(tmp[0], tmp[1], tmp[2], 0xFFU);
                    buffer += 1;
                }
            }
            i += count;
        }
    }



    //-------------------------------------------------------------------------
    void read32Transpose(Color4* buffer, lcore::istream& src, s32 width, s32 height)
    {
        static const s32 bpp = 4;

        //転地する
        Color4* tmpBuffer = buffer + width * (height-1);
        u8 tmp[bpp];
        for(s32 i=0; i<height; ++i){

            Color4* row = tmpBuffer;
            for(s32 j=0; j<width; ++j){
                lcore::io::read(src, tmp, bpp);
                row->setRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                row += 1;
            }
            tmpBuffer -= width;
        }
    }

    void read32RLETranspose(Color4* buffer, lcore::istream& src, s32 width, s32 height)
    {
        static const s32 bpp = 4;

        //転地する
        s32 pixels = width*height;
        Color4* tmpBuffer = buffer + width * (height-1);
        Color4* row = tmpBuffer;
        s32 x = 0;
        s32 count = 0;
        u8 byte = 0;
        u8 tmp[bpp];
        for(s32 i=0; i<pixels;){

            lcore::io::read(src, byte);
            //最上位ビットが０なら連続しないデータ数
            //１なら連続するデータ数
            count = (byte & 0x7FU) + 1;
            bool b = ((byte & 0x80U) == 0)? false : true;
            if(b){
                //連続するデータ
                lcore::io::read(src, tmp, bpp);
                for(s32 j=0; j<count; ++j){
                    row->setRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                    row += 1;

                    if(++x>=width){
                        x = 0;
                        tmpBuffer -= width;
                        row = tmpBuffer;
                    }
                }
            }else{
                //連続しないデータ
                for(s32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);
                    row->setRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                    row += 1;

                    if(++x>=width){
                        x = 0;
                        tmpBuffer -= width;
                        row = tmpBuffer;
                    }
                }
            }
            i += count;
        }
    }


    void read24Transpose(Color4* buffer, lcore::istream& src, s32 width, s32 height)
    {
        static const s32 bpp = 3;

        //転地する
        Color4* tmpBuffer = buffer + width * (height-1);
        u8 tmp[bpp];
        for(s32 i=0; i<height; ++i){

            Color4* row = tmpBuffer;
            for(s32 j=0; j<width; ++j){
                lcore::io::read(src, tmp, bpp);
                row->setRGBA(tmp[0], tmp[1], tmp[2], 0xFFU);
                row += 1;
            }
            tmpBuffer -= width;
        }
    }

    void read24RLETranspose(Color4* buffer, lcore::istream& src, s32 width, s32 height)
    {
        static const s32 bpp = 3;

        //転地する
        s32 pixels = width*height;
        Color4* tmpBuffer = buffer + width * (height-1);
        Color4* row = tmpBuffer;
        s32 x = 0;
        s32 count = 0;
        u8 byte = 0;
        u8 tmp[bpp];

        for(s32 i=0; i<pixels;){
            lcore::io::read(src, byte);
            //最上位ビットが０なら連続しないデータ数
            //１なら連続するデータ数
            count = (byte & 0x7FU) + 1;
            bool b = ((byte & 0x80U) == 0)? false : true;

            if(b){
                //連続するデータ
                lcore::io::read(src, tmp, bpp);
                for(s32 j=0; j<count; ++j){
                    row->setRGBA(tmp[0], tmp[1], tmp[2], 0xFFU);
                    row += 1;

                    if(++x>=width){
                        x = 0;
                        tmpBuffer -= width;
                        row = tmpBuffer;
                    }
                }

            }else{
                //連続しないデータ
                for(s32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);
                    row->setRGBA(tmp[0], tmp[1], tmp[2], 0xFFU);
                    row += 1;

                    if(++x>=width){
                        x = 0;
                        tmpBuffer -= width;
                        row = tmpBuffer;
                    }
                }
            }
            i += count;
        }
    }
}

        //-----------------------------------------------------------------
        bool IOTGA::read(lcore::istream& is, Color4* buffer, s32& width, s32& height, bool transpose)
        {
            u8 tgaHeader[TGA_HEADER_SIZE];

            s32 startPos = is.tellg();

            lcore::io::read(is, tgaHeader, TGA_HEADER_SIZE);

            width = getU16(tgaHeader[Offset_WidthL], tgaHeader[Offset_WidthH]);
            height = getU16(tgaHeader[Offset_HeightL], tgaHeader[Offset_HeightH]);

            s32 bpp = tgaHeader[Offset_BitPerPixel];


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
                s32 length = getU16(tgaHeader[Offset_ColorMapLengL], tgaHeader[Offset_ColorMapLengH]);
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
        u8 calcRunLength(const u8* buffer, s32 index, s32 pixels)
        {
            static const s32 bpp = 4;
            static const s32 maxCount = 0x7FU;

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
                for(s32 i=index; i<(pixels-1); ++i){

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

                for(s32 i=index; i<(pixels-1); ++i){
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

        void write32RLE(lcore::ostream& os, const u8* buffer, s32 width, s32 height)
        {
            static const s32 bpp = 4;

            s32 pixels = width*height;
            for(s32 i=0; i<pixels;){

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

        void write32(lcore::ostream& os, const Color4* buffer, s32 width, s32 height)
        {
            static const s32 bpp = 4;

            s32 pixels = width*height;
            u8 tmp[bpp];
            for(s32 i=0; i<pixels; ++i){
                buffer->getRGBA(tmp[0], tmp[1], tmp[2], tmp[3]);
                lcore::io::write(os, tmp, bpp);
                buffer += 1;
            }

        }
}


    //-----------------------------------------------------------------
    bool IOTGA::write(lcore::ostream& os, const Color4* buffer, s32 width, s32 height)
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

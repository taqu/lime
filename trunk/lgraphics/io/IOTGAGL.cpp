/**
@file IOTGADX.cpp
@author t-sakai
@date 2010/05/20 create

OpenGL用
*/
namespace lgraphics
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
    void read32(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        //バッファにロード
        u32 pitch = width * bpp;
        u32 size = pitch*height;
        lcore::io::read(src, buffer, size);
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
    }


    void read24(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;

        //バッファにロード
        u32 pitch = width * bpp;
        u32 size = pitch*height;
        lcore::io::read(src, buffer, size);
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
                    buffer[0] = tmp[0];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[2];
                    buffer += bpp;
                }
            }else{
                //連続しないデータ
                for(u32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);
                    buffer[0] = tmp[0];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[2];
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

        //バッファにロードして転送
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

        //バッファにロードして転送
        u32 rowBytes = width * bpp;

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
    }

    void read24RLETranspose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;

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
    }

}
}

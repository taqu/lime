/**
@file IOTGADX.cpp
@author t-sakai
@date 2010/05/20 create

Direct3D用
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
    void read32(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
    {
        LockedRect rect;
        if(dst.lock(0, rect) == false){
            return;
        }
        u8* buffer = reinterpret_cast<u8*>( rect.bits_ );

        static const u32 bpp = 4;

        u32 pixels = width*height;

        lcore::io::read(src, buffer, pixels*bpp);

        dst.unlock(0);
    }

    void read32RLE(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
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


    void read24(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
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

    void read24RLE(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
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



    //-------------------------------------------------------------------------
    void read32Transpose(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        LockedRect rect;
        if(dst.lock(0, rect) == false){
            return;
        }

        u8* buffer = reinterpret_cast<u8*>( rect.bits_ );

        //転地する
        u32 rowBytes = width * bpp;
        u8* tmpBuffer = buffer + rowBytes * (height-1);
        u8 tmp[bpp];
        for(u32 i=0; i<height; ++i){

            u8* row = tmpBuffer;
            for(u32 j=0; j<width; ++j){
                lcore::io::read(src, tmp, bpp);
                row[0] = tmp[0];
                row[1] = tmp[1];
                row[2] = tmp[2];
                row[3] = tmp[3];
                row += bpp;
            }
            tmpBuffer -= rowBytes;
        }

        dst.unlock(0);
    }

    void read32RLETranspose(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        LockedRect rect;
        if(dst.lock(0, rect) == false){
            return;
        }
        u8* buffer = reinterpret_cast<u8*>( rect.bits_ );

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

                    row[0] = tmp[0];
                    row[1] = tmp[1];
                    row[2] = tmp[2];
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
                    row[0] = tmp[0];
                    row[1] = tmp[1];
                    row[2] = tmp[2];
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
        dst.unlock(0);
    }


    void read24Transpose(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
    {
        LockedRect rect;
        if(dst.lock(0, rect) == false){
            return;
        }
        u8* buffer = reinterpret_cast<u8*>( rect.bits_ );

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
                row[0] = tmp[0];
                row[1] = tmp[1];
                row[2] = tmp[2];
                row[bpp] = 0xFFU;
                row += dstBpp;
            }
            tmpBuffer -= rowBytes;
        }

        dst.unlock(0);
    }

    void read24RLETranspose(TextureRef& dst, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;
        static const u32 dstBpp = 4;

        LockedRect rect;
        if(dst.lock(0, rect) == false){
            return;
        }
        u8* buffer = reinterpret_cast<u8*>( rect.bits_ );

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
                    row[0] = tmp[0];
                    row[1] = tmp[1];
                    row[2] = tmp[2];
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
                    lcore::io::read(src, row, bpp);
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
        dst.unlock(0);
    }
}
}

#ifndef INC_LGRAPHICS_DX11_IOBMP_H__
#define INC_LGRAPHICS_DX11_IOBMP_H__
/**
@file IOBMP.h
@author t-sakai
@date 2010/05/13 create

*/
#include "../../lgraphics.h"
#include <lcore/liostream.h>

namespace lgfx
{
    class Texture2DRef;
    struct SRVDesc;

namespace io
{
    class IOBMP
    {
    public:
        enum Compression
        {
            Compression_RGB =0,
            Compression_RLE8,
            Compression_RLE4,
            Compression_BitFields,
            Compression_JPEG,
            Compression_PNG,
        };

        enum WriteFormat
        {
            WriteFormat_RGB,
            WriteFormat_RGBA,
        };

        static const u16 BMP_MAGIC;// = 'MB';

        typedef u32 FXPT2DOT30;
        struct CIEXYZ
        {
            FXPT2DOT30 x_;
            FXPT2DOT30 y_;
            FXPT2DOT30 z_;
        };

        struct CIEXYZTRIPLE
        {
            CIEXYZ r_;
            CIEXYZ g_;
            CIEXYZ b_;
        };

        struct BMP_HEADER
        {
            //u16 type_; /// マジック
            u32 size_; /// ファイルサイズ
            u16 reserve1_;
            u16 reserve2_;
            u32 offset_; /// ファイル先頭からデータまでのオフセット

        };

        struct BMP_INFOHEADER
        {
            u32 infoSize_;
            s32 width_;
            s32 height_;
            u16 planes_;
            u16 bitCount_;
            u32 compression_;
            u32 sizeImage_;
            s32 xPixPerMeter_;
            s32 yPixPerMeter_;
            u32 colorUsed_;
            u32 colorImportant_;
        };

        struct BMP_INFOHEADERV4
        {
            u32 infoSize_;
            s32 width_;
            s32 height_;
            u16 planes_;
            u16 bitCount_;
            u32 compression_;
            u32 sizeImage_;
            s32 xPixPerMeter_;
            s32 yPixPerMeter_;
            u32 colorUsed_;
            u32 colorImportant_;
            u32 maskR_;
            u32 maskG_;
            u32 maskB_;
            u32 maskA_;
            u32 colorSpace_;
            CIEXYZTRIPLE endpoints_;
            u32 gammaR_;
            u32 gammaG_;
            u32 gammaB_;
        };

        struct BMP_INFOHEADERV5
        {
            u32 infoSize_;
            s32 width_;
            s32 height_;
            u16 planes_;
            u16 bitCount_;
            u32 compression_;
            u32 sizeImage_;
            s32 xPixPerMeter_;
            s32 yPixPerMeter_;
            u32 colorUsed_;
            u32 colorImportant_;
            u32 maskR_;
            u32 maskG_;
            u32 maskB_;
            u32 maskA_;
            u32 colorSpace_;
            CIEXYZTRIPLE endpoints_;
            u32 gammaR_;
            u32 gammaG_;
            u32 gammaB_;
            u32 intent_;
            u32 profileData_;
            u32 profileSize_;
            u32 reserved_;
        };

        /**
        @brief bufferにロード。bufferがNULLの場合、width、height、formatを設定して返る
        */
        static bool read(
            lcore::istream& is,
            u8* buffer,
            bool sRGB,
            bool transpose,
            u32& width, u32& height, DataFormat& format);

        static bool read(
            Texture2DRef& texture,
            lcore::istream& is,
            Usage usage,
            BindFlag bindFlag,
            CPUAccessFlag access,
            ResourceMisc misc,
            bool sRGB,
            bool transpose,
            u32& width, u32& height, DataFormat& format);

        static bool write(lcore::ostream& os, const u8* buffer, u32 width, u32 height, WriteFormat format);
    };
}
}
#endif //INC_LGRAPHICS_DX11_IOBMP_H__

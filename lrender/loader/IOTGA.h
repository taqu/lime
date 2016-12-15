#ifndef INC_LRENDER_IOTGA_H__
#define INC_LRENDER_IOTGA_H__
/**
@file IOTGA.h
@author t-sakai
@date 2015/09/23 create
*/
#include <lcore/liostream.h>
#include "../lrender.h"

namespace lrender
{
    class Color4;

    class IOTGA
    {
    public:
        enum Type
        {
            Type_None = 0x00U,
            Type_IndexColor = 0x01U,
            Type_FullColor = 0x02U,
            Type_Gray = 0x03U,
            Type_IndexColorRLE = 0x09U,
            Type_FullColorRLE = 0x0AU,
            Type_GrayRLE = 0x0BU,
        };

        enum Offset
        {
            Offset_IDLeng = 0,
            Offset_ColorMapType,
            Offset_ImageType,

            Offset_ColorMapOriginL,
            Offset_ColorMapOriginH,

            Offset_ColorMapLengL,
            Offset_ColorMapLengH,

            Offset_ColorMapSize,

            Offset_OriginXL,
            Offset_OriginXH,
            Offset_OriginYL,
            Offset_OriginYH,

            Offset_WidthL,
            Offset_WidthH,
            Offset_HeightL,
            Offset_HeightH,
            Offset_BitPerPixel,
            Offset_Discripter,
        };

        static const u32 TGA_HEADER_SIZE = 18;

        /**
        @brief bufferにロード。bufferがNULLの場合、width、height、formatを設定して返る
        */
        static bool read(lcore::istream& is, Color4* buffer, s32& width, s32& height, bool transpose = false);

        static bool write(lcore::ostream& os, const Color4* buffer, s32 width, s32 height);
    };
}
#endif //INC_LRENDER_IOTGA_H__

#ifndef INC_LGRAPHICS_DX11_IOTGA_H_
#define INC_LGRAPHICS_DX11_IOTGA_H_
/**
@file IOTGA.h
@author t-sakai
@date 2010/05/20 create
@date 2018/10/06 modify to using cppimg
*/
#include "../../io/IOImage.h"

namespace lgfx
{
    class Texture2DRef;

namespace io
{
    class IOTGA
    {
    public:
        /**
        @brief bufferにロード。bufferがNULLの場合、width、height、formatを設定して返る
        */
        static bool read(
            ISStream& is,
            u8* buffer,
            s32& width,
            s32& height,
            DataFormat& format);

        static bool read(
            Texture2DRef& texture,
            ISStream& is,
            Usage usage,
            BindFlag bindFlag,
            CPUAccessFlag access,
            ResourceMisc misc,
            s32& width,
            s32& height,
            DataFormat& format);

        static bool write(
            OSStream& os,
            const u8* buffer,
            s32 width,
            s32 height,
            ColorType colorType);
    };
}
}
#endif //INC_LGRAPHICS_DX11_IOTGA_H_

#ifndef INC_LGRAPHICS_DX11_IOPNG_H_
#define INC_LGRAPHICS_DX11_IOPNG_H_
/**
@file IOPNG.h
@author t-sakai
@date 2013/07/24 create
@date 2018/10/06 modify to using cppimg
*/
#include "../../io/IOImage.h"

namespace lgfx
{
    class Texture2DRef;

namespace io
{
    class IOPNG
    {
    public:
        enum SwapRGB
        {
            Swap_RGB,
            Swap_BGR,
        };

        /**
        @brief bufferにロード。bufferがNULLの場合、width、height、format、rowBytesを設定して返る
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
    };
}
}
#endif //INC_LGRAPHICS_DX11_IOPNG_H_

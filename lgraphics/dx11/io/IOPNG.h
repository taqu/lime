#ifndef INC_LGRAPHICS_DX11_IOPNG_H__
#define INC_LGRAPHICS_DX11_IOPNG_H__
/**
@file IOPNG.h
@author t-sakai
@date 2013/07/24 create
*/
#include "../../lgraphics.h"
#include <lcore/liostream.h>
#include "../../Enumerations.h"

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

        static bool checkSignature(lcore::istream& is);

        /**
        @brief bufferにロード。bufferがNULLの場合、width、height、format、rowBytesを設定して返る
        */
        static bool read(
            lcore::istream& is,
            u8* buffer,
            bool sRGB,
            SwapRGB swap,
            u32& width, u32& height, lsize_t& rowBytes, DataFormat& format);

        static bool read(
            Texture2DRef& texture,
            lcore::istream& is,
            Usage usage,
            BindFlag bindFlag,
            CPUAccessFlag access,
            ResourceMisc misc,
            bool sRGB,
            SwapRGB swap,
            u32& width, u32& height, lsize_t& rowBytes, DataFormat& format);
    };
}
}
#endif //INC_LGRAPHICS_DX11_IOPNG_H__

#ifndef INC_LRENDER_IOPNG_H__
#define INC_LRENDER_IOPNG_H__
/**
@file IOPNG.h
@author t-sakai
@date 2015/09/23 create
*/
#include <lcore/liostream.h>
#include "../lrender.h"

namespace lrender
{
    class Color4;

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
        static bool read(lcore::istream& is, Color4* buffer, s32& width, s32& height, s32& rowBytes, SwapRGB swap);
    };
}
#endif //INC_LRENDER_IOPNG_H__

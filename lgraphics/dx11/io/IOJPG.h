#ifndef INC_LGRAPHICS_DX11_IOJPG_H_
#define INC_LGRAPHICS_DX11_IOJPG_H_
/**
@file IOJPG.h
@author t-sakai
@date 2016/11/23 create
@date 2018/10/06 modify to using cppimg
*/
#include "io/IOImage.h"

namespace lgfx
{
    class Texture2DRef;

    namespace io
    {
        class IOJPG
        {
        public:
            static bool read(
                ISStream& is,
                u8* buffer,
                bool transpose,
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
                bool transpose,
                s32& width,
                s32& height,
                DataFormat& format);
        };
    }
}
#endif //INC_LGRAPHICS_DX11_IOJPG_H_

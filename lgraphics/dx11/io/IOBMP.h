#ifndef INC_LGRAPHICS_DX11_IOBMP_H_
#define INC_LGRAPHICS_DX11_IOBMP_H_
/**
@file IOBMP.h
@author t-sakai
@date 2010/05/13 create
@date 2018/10/06 modify to using cppimg
*/
#include "../../io/IOImage.h"

namespace lgfx
{
    class Texture2DRef;
    struct SRVDesc;

namespace io
{
    class IOBMP
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

        static bool write(
            OSStream& os,
            const u8* buffer,
            s32 width,
            s32 height,
            ColorType colorType);
    };
}
}
#endif //INC_LGRAPHICS_DX11_IOBMP_H_

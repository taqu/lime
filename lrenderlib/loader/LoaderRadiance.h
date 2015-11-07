#ifndef INC_LRENDER_LOADERRADIANCE_H__
#define INC_LRENDER_LOADERRADIANCE_H__
/**
@file LoaderRadiance.h
@author t-sakai
@date 2013/05/22 create
*/
#include "../lrender.h"

namespace lcore
{
    class istream;
}

namespace lrender
{
    class Image;

    class LoaderRadiance
    {
    public:
        enum Code
        {
            Code_ReadError = -1,
            Code_Success = 0,
        };

        struct RGBEHeader
        {
            s32 valid_;
            s8 id_[16];
            f32 gamma_;
            f32 exposure_;
        };

        static s32 readHeader(lcore::istream& in, f32& gamma, f32& exposure, s32& width, s32& height);

        static s32 readRLE(lcore::istream& in, Image& image);
    };
}
#endif //INC_LRENDER_LOADERRADIANCE_H__

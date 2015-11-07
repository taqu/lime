#ifndef INC_IMAGEPROCESSING_H__
#define INC_IMAGEPROCESSING_H__
/**
@file ImageProcessing.h
@author t-sakai
@date 2015/10/07 create
*/
#include "../lrender.h"
#include "core/Spectrum.h"

namespace lrender
{
    class Image;

    class ImageProcessing
    {
    public:
        static void dilate(Image& dst, const Image& src);
    };
}
#endif //INC_IMAGEPROCESSING_H__

#ifndef INC_LGRAPHICS_IOJPEG_H__
#define INC_LGRAPHICS_IOJPEG_H__
/**
@file IOJPEG.h
@author t-sakai
@date 2011/01/08 create
*/
#include "../lgraphicscore.h"
#include <lcore/liostream.h>

namespace lgraphics
{
    class TextureRef;

namespace io
{
    class IOJPEG
    {
    public:
        IOJPEG();
        ~IOJPEG();

        static bool read(const Char* filepath, TextureRef& texture);
    };
}
}
#endif //INC_LGRAPHICS_IOJPEG_H__

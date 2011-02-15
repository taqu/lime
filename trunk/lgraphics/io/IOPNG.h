#ifndef INC_LGRAPHICS_IOPNG_H__
#define INC_LGRAPHICS_IOPNG_H__
/**
@file IOPNG.h
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
    class IOPNG
    {
    public:
        IOPNG();
        ~IOPNG();

        static bool read(lcore::istream& is, TextureRef& texture);
    };
}
}
#endif //INC_LGRAPHICS_IOPNG_H__

/**
@file IOTexture.cpp
@author t-sakai
@date 2010/05/06 create

*/
#include "IOTexture.h"
#include "../api/TextureRef.h"
#include "IODDS.h"

namespace lgraphics
{
namespace io
{
    IOTexture::IOTexture()
    {
    }

    IOTexture::~IOTexture()
    {
    }

    bool IOTexture::read(lcore::istream& is, TextureRef& texture)
    {
        is >> texture.getName();

        return IODDS::read(is, texture);
    }

    bool IOTexture::write(lcore::ostream& os, TextureRef& texture)
    {
        os << texture.getName();
        return IODDS::write(os, texture);
    }
}
}

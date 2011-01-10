/**
@file IOJPEG.cpp
@author t-sakai
@date 2011/01/08
*/
#include "../lgraphicsAPIInclude.h"

#include "IOJPEG.h"
#include "../api/Enumerations.h"
#include "../api/TextureRef.h"

namespace lgraphics
{
namespace io
{

    IOJPEG::IOJPEG()
    {
    }

    IOJPEG::~IOJPEG()
    {
    }

    bool IOJPEG::read(lcore::istream& is, TextureRef& texture)
    {
        return true;
    }

}
}

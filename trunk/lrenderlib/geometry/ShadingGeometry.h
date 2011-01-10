#ifndef INC_LRENDER_SHADINGGEOMETRY_H__
#define INC_LRENDER_SHADINGGEOMETRY_H__
/**
@file ShadingGeometry.h
@author t-sakai
@date 2010/01/22 create

*/
#include "../lrendercore.h"

namespace lrender
{
    class ShadingGeometry
    {
    public:
        Vector3 position_;
        Vector3 normal_;
        Vector4 color_;
    };
}

#endif //INC_LRENDER_SHADINGGEOMETRY_H__

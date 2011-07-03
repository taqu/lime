#ifndef INC_LRENDER_MATERIAL_H__
#define INC_LRENDER_MATERIAL_H__
/**
@file Material.h
@author t-sakai
@date 2009/12/23 create
*/
#include "../lrendercore.h"

namespace lrender
{
    class Material
    {
    public:
        Material();
        ~Material();

        Vector3 color_;
    };
}

#endif //INC_LRENDER_MATERIAL_H__

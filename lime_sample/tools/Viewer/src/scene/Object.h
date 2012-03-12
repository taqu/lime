#ifndef INC_LSCENE_OBJECT_H__
#define INC_LSCENE_OBJECT_H__
/**
@file Object.h
@author t-sakai
@date 2011/02/16 create
*/
#include <lframework/scene/lscene.h>
#include "../render/Drawable.h"
#include <lcore/Buffer.h>

namespace lrender
{
    class Batch;
}

namespace lscene
{
    class Geometry;
    class Material;

    class Object : public lrender::Drawable
    {
    public:
        Object();

        Object(
            u32 numGeometries,
            u32 numMaterials);

        ~Object();

    private:
        Object(const Object&);
        Object& operator=(const Object&);
    };
}

#endif //INC_LSCENE_OBJECT_H__

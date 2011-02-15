#ifndef INC_LSCENE_OBJECT_H__
#define INC_LSCENE_OBJECT_H__
/**
@file Object.h
@author t-sakai
@date 2011/02/16 create
*/
#include "lscene.h"
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

        void initializeShader();

        u32 getNumGeometries() const{ return numGeometries_;}
        Geometry& getGeometry(u32 index);

        u32 getNumMaterials() const{ return numMaterials_;}
        Material& getMaterial(s32 index);

        void addDraw();
        void removeDraw();

        void swap(Object& rhs);
    private:
        Object(const Object&);
        Object& operator=(const Object&);

        u32 numGeometries_;
        Geometry *geometries_;

        u32 numMaterials_;
        Material *materials_;

        lrender::Batch *batches_;

        lcore::Buffer resourceBuffer_;
    };
}

#endif //INC_LSCENE_OBJECT_H__

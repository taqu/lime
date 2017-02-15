#ifndef INC_LFRAMEWORK_PRIMITIVEGEOMETRY_H__
#define INC_LFRAMEWORK_PRIMITIVEGEOMETRY_H__
/**
@file PrimitiveGeometry.h
@author t-sakai
@date 2017/02/12 create
*/
#include "../lframework.h"
#include <lcore/intrusive_ptr.h>
#include "Resource.h"

namespace lmath
{
    class Sphere;
}

namespace lfw
{
    class Geometry;
    class Model;
    class ResourceModel;

    class PrimitiveGeometry
    {
    public:
        struct Vertex
        {
            f32 position_[3];
            u16 normal_[4];
            u16 texcoord_[2];
        };

        static ResourceModel* createPlane(f32 scale);
    private:
        PrimitiveGeometry(const PrimitiveGeometry&);
        PrimitiveGeometry& operator=(const PrimitiveGeometry&);

        static bool create(Model& model, f32 scale);
        static void createGeometry(
            Model& model,
            u32 numVertices,
            Vertex* vertices,
            u32 numIndices,
            u16* indices,
            const lmath::Sphere& sphere);
    };
}
#endif //INC_LFRAMEWORK_PRIMITIVEGEOMETRY_H__

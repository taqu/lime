#ifndef INC_LRENDER_SHAPE_H__
#define INC_LRENDER_SHAPE_H__
/**
@file Shape.h
@author t-sakai
@date 2009/12/23 create
*/
#include <lcore/smart_ptr/intrusive_ptr.h>
#include "../lrendercore.h"

namespace lmath
{
    class Ray;
}

namespace lrender
{
    class Element;
    struct HitRecord;
    class ShadingGeometry;

    class Shape
    {
    public:
        virtual ~Shape()
        {
        }

        virtual u32 calcNumElements() const =0;
        virtual bool getShapes(Shape** shapes, u32 bufferCount) =0;

        virtual void calcBBox(Vector3& bmin, Vector3& bmax) const =0;
        virtual void calcMedian(Vector3& median) const =0;

        virtual bool hit(HitRecord& hitRecord, const lmath::Ray& ray) const =0;

        virtual void calcShadingGeometry(ShadingGeometry& shadingGeom, const HitRecord& hitRecord) const = 0;
    protected:
        Shape(){}
    };
}
#endif //INC_LRENDER_SHAPE_H__

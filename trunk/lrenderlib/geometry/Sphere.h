#ifndef INC_LRENDER_SPHERE_H__
#define INC_LRENDER_SPHERE_H__
/**
@file Sphere.h
@author t-sakai
@date 2010/01/12 create

*/
#include "../lrendercore.h"
#include <lmath/geometry/Sphere.h>
#include "Shape.h"

namespace lrender
{
    class Sphere : public Shape
    {
    public:
        Sphere()
        {
        }

        Sphere(const Vector3& position, f32 radius)
            :impl_(position, radius)
        {
        }

        virtual ~Sphere()
        {
        }

        virtual u32 calcNumElements() const
        {
            return 1;
        }

        virtual bool getElements(Element* elements, u32 bufferCount);

        virtual void calcBBox(u32 index, Vector3& bmin, Vector3& bmax) const;
        virtual void calcMedian(u32 index, Vector3& median) const;

        virtual bool hit(HitRecord& hitRecord, u32 index, const lmath::Ray& ray) const;
        virtual void calcShadingGeometry(ShadingGeometry& shadingGeom, const HitRecord& hitRecord) const;
    private:
        lmath::Sphere impl_;
    };
}
#endif //INC_LRENDER_SPHERE_H__

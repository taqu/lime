#ifndef INC_LRENDER_SHAPE_H__
#define INC_LRENDER_SHAPE_H__
/**
@file Shape.h
@author t-sakai
@date 2015/09/11 create
*/

#include "../lrender.h"
#include "../core/Triangle.h"
#include "../core/AABB.h"
#include "../accelerator/Accelerator.h"
#include "../bsdf/BSDF.h"

namespace lrender
{
    class Ray;
    class RayDifferential;
    class Intersection;

    class Shape : public ReferenceCounted
    {
    public:
        typedef lcore::intrusive_ptr<Shape> pointer;
        typedef lcore::vector_arena<Shape::pointer> ShapeVector;

        enum Component
        {
            Component_None = 0,
            Component_Normal = (0x01<<0),
            Component_Texcoord = (0x01<<1),
        };

        virtual ~Shape()
        {}

        inline s32 getID() const;
        inline void setID(s32 id);

        inline void addComponent(Component component);
        inline bool hasComponent(Component component) const;

        inline const BSDF::pointer& getBSDF() const;
        inline void setBSDF(BSDF::pointer& brdf);

        virtual s32 getPrimitiveCount() const =0;
        virtual void getBBox(AABB& bbox) const =0;

        //virtual bool intersect(const Ray& ray, f32& t, Intersection& intersection) =0;
        //virtual bool intersectP(const Ray& ray) =0;

        virtual bool intersect(f32& t, f32& b1, f32& b2, s32 primitive, const Ray& ray) const=0;
        virtual void getTriangles(VectorShapePrimitive& triangles) const=0;
        virtual void getTexcoord(Vector2 uvs[3], s32 primitive) const =0;
        virtual void getPrimitive(PrimitiveSample& sample, s32 primitive) const=0;

        virtual Vector3 getCentroid(s32 primitive) const=0;
        virtual AABB getBBox(s32 primitive) const=0;
        virtual void getPrimitiveSOA(Vector4 vx[3], Vector4 vy[3], Vector4 vz[3], s32 index, s32 primitive) const=0;

        virtual void getIntersection(Intersection& intersection, const RayDifferential& ray, f32 b1, f32 b2, s32 primitive) const=0;

        virtual f32 getSurfaceArea(s32 primitive) const
        {
            return 0.0f;
        }
    protected:
        Shape(const Shape&);
        Shape& operator=(const Shape&);

        Shape()
            :components_(0)
            ,id_(-1)
        {}

        s32 id_;
        s32 components_;
        BSDF::pointer bsdf_;
    };

    inline s32 Shape::getID() const
    {
        return id_;
    }

    inline void Shape::setID(s32 id)
    {
        id_ = id;
    }

    inline void Shape::addComponent(Component component)
    {
        components_ |= component;
    }

    inline bool Shape::hasComponent(Component component) const
    {
        return 0 != (components_ & component);
    }

    inline const BSDF::pointer& Shape::getBSDF() const
    {
        return bsdf_;
    }

    inline void Shape::setBSDF(BSDF::pointer& bsdf)
    {
        bsdf_ = bsdf;
    }
}
#endif //INC_LRENDER_SHAPE_H__

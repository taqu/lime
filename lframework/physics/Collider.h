#ifndef INC_LFRAMEWORK_COLLIDER_H__
#define INC_LFRAMEWORK_COLLIDER_H__
/**
@file Collider.h
@author t-sakai
@date 2016/12/19 create
*/
#include "lcollide.h"
#include <lmath/geometry/Sphere.h>
#include <lmath/geometry/Ray.h>

namespace lfw
{
    //-------------------------------------------------------
    //---
    //--- ColliderSphere
    //---
    //-------------------------------------------------------
    class ColliderSphere : public ColliderBase3D
    {
    public:
        inline ColliderSphere();
        inline ColliderSphere(CollideCallback callback, void* data, u16 id, u16 next, u16 group);

        virtual bool test(const lmath::Ray& ray, f32& t) const;
        virtual void getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax);

        lmath::Sphere bsphere_;
    };

    inline ColliderSphere::ColliderSphere()
        :ColliderBase3D(NULL, NULL, Invalid, Invalid, 0, CollisionType_Sphere)
    {}

    inline ColliderSphere::ColliderSphere(CollideCallback callback, void* data, u16 id, u16 next, u16 group)
        :ColliderBase3D(callback, data, id, next, group, CollisionType_Sphere)
    {}

    //-------------------------------------------------------
    //---
    //--- ColliderRay
    //---
    //-------------------------------------------------------
    class ColliderRay : public ColliderBase3D
    {
    public:
        inline ColliderRay();
        inline ColliderRay(CollideCallback callback, void* data, u16 id, u16 next, u16 group);

        virtual bool test(const lmath::Ray& ray, f32& t) const;
        virtual void getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax);

        lmath::Ray ray_;
    };

    inline ColliderRay::ColliderRay()
        :ColliderBase3D(NULL, NULL, Invalid, Invalid, 0, CollisionType_Ray)
    {}

    inline ColliderRay::ColliderRay(CollideCallback callback, void* data, u16 id, u16 next, u16 group)
        :ColliderBase3D(callback, data, id, next, group, CollisionType_Ray)
    {}

    //-------------------------------------------------------
    //---
    //--- ColliderAABB
    //---
    //-------------------------------------------------------
    class ColliderAABB : public ColliderBase3D
    {
    public:
        inline ColliderAABB();
        inline ColliderAABB(CollideCallback callback, void* data, u16 id, u16 next, u16 group);

        virtual bool test(const lmath::Ray& ray, f32& t) const;
        virtual void getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax);

        lmath::Vector4 bmin_;
        lmath::Vector4 bmax_;
    };

    inline ColliderAABB::ColliderAABB()
        :ColliderBase3D(NULL, NULL, Invalid, Invalid, 0, CollisionType_AABB)
    {}

    inline ColliderAABB::ColliderAABB(CollideCallback callback, void* data, u16 id, u16 next, u16 group)
        :ColliderBase3D(callback, data, id, next, group, CollisionType_AABB)
    {}

    //-------------------------------------------------------
    //---
    //--- ColliderCapsule
    //---
    //-------------------------------------------------------
    class ColliderCapsule : public ColliderBase3D
    {
    public:
        inline ColliderCapsule();
        inline ColliderCapsule(CollideCallback callback, void* data, u16 id, u16 next, u16 group);

        virtual bool test(const lmath::Ray& ray, f32& t) const;
        virtual void getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax);

        lmath::Vector3 p0_;
        lmath::Vector3 p1_;
        f32 radius_;
    };

    inline ColliderCapsule::ColliderCapsule()
        :ColliderBase3D(NULL, NULL, Invalid, Invalid, 0, CollisionType_Capsule)
    {}

    inline ColliderCapsule::ColliderCapsule(CollideCallback callback, void* data, u16 id, u16 next, u16 group)
        :ColliderBase3D(callback, data, id, next, group, CollisionType_Capsule)
    {}
}
#endif //INC_LFRAMEWORK_COLLIDER_H__

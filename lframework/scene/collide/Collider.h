#ifndef INC_LCOLLIDE_COLLIDER_H__
#define INC_LCOLLIDE_COLLIDER_H__
/**
@file Collider.h
@author t-sakai
@date 2015/02/18 create
*/
#include "lcollide.h"
#include <lmath/geometry/Sphere.h>
#include <lmath/geometry/Ray.h>

namespace lscene
{
    class NodeBase;
}

namespace lcollide
{
    //-------------------------------------------------------
    //---
    //--- Collider
    //---
    //-------------------------------------------------------
    class Collider : public lcollide::ColliderBase3
    {
    public:
        lscene::NodeBase* getNode()
        {
            return reinterpret_cast<lscene::NodeBase*>(data_);
        }
    protected:
        inline Collider();
        inline Collider(lscene::NodeBase* node, u16 group, u16 type);
    };

    inline Collider::Collider()
    {}

    inline Collider::Collider(lscene::NodeBase* node, u16 group, u16 type)
        :ColliderBase3(node, group, type)
    {}

    //-------------------------------------------------------
    //---
    //--- ColliderSphere
    //---
    //-------------------------------------------------------
    class ColliderSphere : public Collider
    {
    public:
        inline ColliderSphere();
        inline ColliderSphere(lscene::NodeBase* node, u16 group);

        virtual bool test(const lmath::Ray& ray, f32& t) const;
        virtual void getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax);

        lmath::Sphere bsphere_;
    };

    inline ColliderSphere::ColliderSphere()
    {}

    inline ColliderSphere::ColliderSphere(lscene::NodeBase* node, u16 group)
        :Collider(node, group, CollisionType_Sphere)
    {}

    //-------------------------------------------------------
    //---
    //--- ColliderRay
    //---
    //-------------------------------------------------------
    class ColliderRay : public Collider
    {
    public:
        inline ColliderRay();
        inline ColliderRay(lscene::NodeBase* node, u16 group);

        virtual bool test(const lmath::Ray& ray, f32& t) const;
        virtual void getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax);

        lmath::Ray ray_;
    };

    inline ColliderRay::ColliderRay()
    {}

    inline ColliderRay::ColliderRay(lscene::NodeBase* node, u16 group)
        :Collider(node, group, CollisionType_Ray)
    {}

    //-------------------------------------------------------
    //---
    //--- ColliderAABB
    //---
    //-------------------------------------------------------
    class ColliderAABB : public Collider
    {
    public:
        inline ColliderAABB();
        inline ColliderAABB(lscene::NodeBase* node, u16 group);

        virtual bool test(const lmath::Ray& ray, f32& t) const;
        virtual void getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax);

        lmath::Vector4 bmin_;
        lmath::Vector4 bmax_;
    };

    inline ColliderAABB::ColliderAABB()
    {}

    inline ColliderAABB::ColliderAABB(lscene::NodeBase* node, u16 group)
        :Collider(node, group, CollisionType_AABB)
    {}
}
#endif //INC_LCOLLIDE_COLLIDER_H__

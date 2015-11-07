/**
@file AABB.cpp
@author t-sakai
@date 2015/09/14 create
*/
#include "AABB.h"
#include <lmath/geometry/Sphere.h>

namespace lrender
{
    void AABB::extend(const Vector3& point)
    {
        bmin_.x_ = lcore::minimum(bmin_.x_, point.x_);
        bmin_.y_ = lcore::minimum(bmin_.y_, point.y_);
        bmin_.z_ = lcore::minimum(bmin_.z_, point.z_);

        bmax_.x_ = lcore::maximum(bmax_.x_, point.x_);
        bmax_.y_ = lcore::maximum(bmax_.y_, point.y_);
        bmax_.z_ = lcore::maximum(bmax_.z_, point.z_);
    }

    void AABB::extend(const AABB& bbox)
    {
        bmin_.x_ = lcore::minimum(bmin_.x_, bbox.bmin_.x_);
        bmin_.y_ = lcore::minimum(bmin_.y_, bbox.bmin_.y_);
        bmin_.z_ = lcore::minimum(bmin_.z_, bbox.bmin_.z_);

        bmax_.x_ = lcore::maximum(bmax_.x_, bbox.bmax_.x_);
        bmax_.y_ = lcore::maximum(bmax_.y_, bbox.bmax_.y_);
        bmax_.z_ = lcore::maximum(bmax_.z_, bbox.bmax_.z_);
    }

    s32 AABB::maxExtentAxis() const
    {
        Vector3 ext = extent();
        s32 axis = (ext.x_<ext.y_)? 1 : 0;
        axis = (ext.z_<ext[axis])? axis : 2;
        return axis;
    }

    f32 AABB::getMaxExtent() const
    {
        s32 axis = maxExtentAxis();

        if(bmax_[axis]<=bmin_[axis]){
            return 0.0f;
        }

        return (bmax_[axis] - bmin_[axis]);
    }

    f32 AABB::halfArea() const
    {
        f32 dx = bmax_.x_ - bmin_.x_;
        f32 dy = bmax_.y_ - bmin_.y_;
        f32 dz = bmax_.z_ - bmin_.z_;
        return (dx*dy + dy*dz + dz*dx);
    }

    void AABB::getBSphere(lmath::Sphere& bsphere) const
    {
        s32 axis = maxExtentAxis();

        if(bmax_[axis]<=bmin_[axis]){
            bsphere.zero();
            return;
        }

        f32 radius = 0.5f * (bmax_[axis] - bmin_[axis]);

        Vector3 center;
        center.add(bmin_, bmax_);
        center *= 0.5f;

        bsphere.setPosition(center);
        bsphere.setRadius(radius);
    }

    f32 AABB::getBSphereRadius() const
    {
        s32 axis = maxExtentAxis();

        if(bmax_[axis]<=bmin_[axis]){
            return 0.0f;
        }

        f32 radius = 0.5f * (bmax_[axis] - bmin_[axis]);
        return radius;
    }


    void AABB2D::extend(const Vector2& point)
    {
        bmin_.x_ = lcore::minimum(bmin_.x_, point.x_);
        bmin_.y_ = lcore::minimum(bmin_.y_, point.y_);

        bmax_.x_ = lcore::maximum(bmax_.x_, point.x_);
        bmax_.y_ = lcore::maximum(bmax_.y_, point.y_);
    }

    void AABB2D::extend(const AABB2D& bbox)
    {
        bmin_.x_ = lcore::minimum(bmin_.x_, bbox.bmin_.x_);
        bmin_.y_ = lcore::minimum(bmin_.y_, bbox.bmin_.y_);

        bmax_.x_ = lcore::maximum(bmax_.x_, bbox.bmax_.x_);
        bmax_.y_ = lcore::maximum(bmax_.y_, bbox.bmax_.y_);
    }

    s32 AABB2D::maxExtentAxis() const
    {
        Vector2 ext = extent();
        s32 axis = (ext.x_<ext.y_)? 1 : 0;
        return axis;
    }

    f32 AABB2D::getMaxExtent() const
    {
        s32 axis = maxExtentAxis();

        if(bmax_[axis]<=bmin_[axis]){
            return 0.0f;
        }

        return (bmax_[axis] - bmin_[axis]);
    }

    f32 AABB2D::halfArea() const
    {
        f32 dx = bmax_.x_ - bmin_.x_;
        f32 dy = bmax_.y_ - bmin_.y_;
        return (dx*dy);
    }

    bool AABB2D::testPoint(const Vector2& point) const
    {
        if(point.x_<bmin_.x_){
            return false;
        }
        if(point.y_<bmin_.y_){
            return false;
        }
        if(bmax_.x_<point.x_){
            return false;
        }
        if(bmax_.y_<point.y_){
            return false;
        }
        return true;
    }
}

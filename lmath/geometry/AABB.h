#ifndef INC_LMATH_AABB_H__
#define INC_LMATH_AABB_H__
/**
@file AABB.h
@author t-sakai
@date 2015/12/24 create
*/
#include "../Vector2.h"
#include "../Vector3.h"

namespace lmath
{
    class Sphere;
}

namespace lmath
{
    //--------------------------------------------------------------
    //---
    //--- AABB
    //---
    //--------------------------------------------------------------
    class AABB
    {
    public:
        AABB()
        {}

        AABB(const Vector3& bmin, const Vector3& bmax)
            :bmin_(bmin)
            ,bmax_(bmax)
        {}

        void zero()
        {
            bmin_ = lmath::Vector3::zero();
            bmax_ = lmath::Vector3::zero();
        }

        void setInvalid()
        {
            bmin_.x_ = bmin_.y_ = bmin_.z_ = lcore::numeric_limits<f32>::maximum();
            bmax_.x_ = bmax_.y_ = bmax_.z_ = lcore::numeric_limits<f32>::lowest();
        }

        Vector3 center() const
        {
            return {(bmin_.x_ + bmax_.x_)*0.5f,
                (bmin_.y_ + bmax_.y_)*0.5f,
                (bmin_.z_ + bmax_.z_)*0.5f};
        }

        Vector3 extent() const
        {
            return {bmax_.x_ - bmin_.x_, bmax_.y_ - bmin_.y_, bmax_.z_ - bmin_.z_};
        }

        Vector3 diagonal() const
        {
            return {bmax_.x_-bmin_.x_,
                bmax_.y_-bmin_.y_,
                bmax_.z_-bmin_.z_};
        }

        void expand(f32 delta);

        void extend(const Vector3& point);
        void extend(const AABB& bbox);

        s32 maxExtentAxis() const;
        f32 getMaxExtent() const;

        f32 halfArea() const;

        void getBSphere(lmath::Sphere& bsphere) const;
        void setBSphere(const lmath::Sphere& bsphere);
        f32 getBSphereRadius() const;

        bool contains(const Vector3& point) const;

        bool contains(const AABB& rhs) const;
        bool overlaps(const AABB& rhs) const;

        Vector3 bmin_;
        Vector3 bmax_;
    };

    //--------------------------------------------------------------
    //---
    //--- AABB2D
    //---
    //--------------------------------------------------------------
    class AABB2D
    {
    public:
        AABB2D()
        {}

        AABB2D(const Vector2& bmin, const Vector2& bmax)
            :bmin_(bmin)
            ,bmax_(bmax)
        {}

        void zero()
        {
            bmin_ = lmath::Vector2::zero();
            bmax_ = lmath::Vector2::zero();
        }

        void setInvalid()
        {
            bmin_.x_ = bmin_.y_ = lcore::numeric_limits<f32>::maximum();
            bmax_.x_ = bmax_.y_ = lcore::numeric_limits<f32>::lowest();
        }

        Vector2 extent() const
        {
            return {bmax_.x_ - bmin_.x_, bmax_.y_ - bmin_.y_};
        }

        Vector2 diagonal() const
        {
            return {bmax_.x_-bmin_.x_, bmax_.y_-bmin_.y_};
        }

        void extend(const Vector2& point);
        void extend(const AABB2D& bbox);

        s32 maxExtentAxis() const;
        f32 getMaxExtent() const;

        f32 halfArea() const;

        bool contains(const Vector2& point) const;

        bool testPoint(const Vector2& point) const;

        Vector2 bmin_;
        Vector2 bmax_;
    };
}
#endif //INC_LMATH_AABB_H__

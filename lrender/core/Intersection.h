#ifndef INC_LRENDER_INTERSECTION_H__
#define INC_LRENDER_INTERSECTION_H__
/**
@file Intersection.h
@author t-sakai
@date 2015/09/12 create
*/
#include "../lrender.h"
#include "Ray.h"

namespace lrender
{
    class Ray;
    class RayDifferential;
    class Shape;
    
    class Intersection
    {
    public:
        static const s32 PrimitiveMask = 0x7FFFFFFF;

        Intersection()
            :dudx_(0.0f)
            ,dudy_(0.0f)
            ,dvdx_(0.0f)
            ,dvdy_(0.0f)
            ,uv_(0.0f, 0.0f)
            ,primitive_(0)
            ,b0_(0.0f)
            ,b1_(0.0f)
            ,b2_(0.0f)
            ,shape_(NULL)
        {}

        void create(const Shape* shape, s32 primitive, const PrimitiveSample& primitiveSample, f32 b0, f32 b1, f32 b2);
        void create(const Vector3& position, const Vector3& normal, const Vector2& uv);

        inline bool isIntersect() const;
        inline s32 getPrimitive() const;
        inline void setPrimitive(s32 primitive);
        inline bool hasDifferentials() const;
        inline void setHasDifferentials(bool has);
        inline void clearDifferentials();

        void computeDifferentials(const RayDifferential& rayDifferential);

        Vector3 worldToLocal(const Vector3& v) const;
        Vector3 localToWorld(const Vector3& v) const;
        Ray nextRay(const Vector3& direction, f32 tmax) const;
        Ray nextCosineWeightedHemisphere(f32 u0, f32 u1, f32 tmax) const;
        Ray nextHemisphere(f32 u0, f32 u1, f32 tmax) const;

        Vector3 point_;
        Vector3 shadingNormal_;
        Vector3 geometricNormal_;
        Vector3 binormal0_;
        Vector3 binormal1_;
        Vector3 dpdu_;
        Vector3 dpdv_;
        Vector3 dndu_;
        Vector3 dndv_;

        Vector3 dpdx_;
        Vector3 dpdy_;
        f32 dudx_, dudy_;
        f32 dvdx_, dvdy_;

        Vector2 uv_;
        f32 b0_;
        f32 b1_;
        f32 b2_;
        const Shape* shape_;
    private:
        s32 primitive_;
    };

    inline bool Intersection::isIntersect() const
    {
        return NULL != shape_;
    }

    inline s32 Intersection::getPrimitive() const
    {
        return primitive_&PrimitiveMask;
    }

    inline void Intersection::setPrimitive(s32 primitive)
    {
        primitive_ = (primitive_&~PrimitiveMask) | (primitive&PrimitiveMask);
    }

    inline bool Intersection::hasDifferentials() const
    {
        return 0 != (primitive_ & ~(PrimitiveMask));
    }

    inline void Intersection::setHasDifferentials(bool has)
    {
        if(has){
            primitive_ |= ~PrimitiveMask;
        } else{
            primitive_ &= PrimitiveMask;
        }
    }

    inline void Intersection::clearDifferentials()
    {
        setHasDifferentials(false);
        dpdx_ = lmath::Vector3::zero();
        dpdy_ = lmath::Vector3::zero();
        dudx_ = dudy_ = 0.0f;
        dvdx_ = dvdy_ = 0.0f;
    }
}
#endif //INC_LRENDER_INTERSECTION_H__

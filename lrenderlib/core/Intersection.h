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
            ,shape_(NULL)
        {}

        //binormal0_ = normalize(dpdu_);
        //binormal1_.cross(shadingNormal_, binormal0_);

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
        primitive_ |= (primitive&PrimitiveMask);
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
        dpdx_.zero();
        dpdy_.zero();
        dudx_ = dudy_ = 0.0f;
        dvdx_ = dvdy_ = 0.0f;
    }
}
#endif //INC_LRENDER_INTERSECTION_H__

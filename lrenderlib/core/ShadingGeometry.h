#ifndef INC_LRENDER_SHADINGGEOMETRY_H__
#define INC_LRENDER_SHADINGGEOMETRY_H__
/**
@file ShadingGeometry.h
@author t-sakai
@date 2015/10/06 create
*/
#include "../lrender.h"
#include "Ray.h"

namespace lrender
{
    class ShadingGeometry
    {
    public:
        ShadingGeometry();
        ShadingGeometry(const PrimitiveSample& primitiveSample, const MapperSample& mapperSample);
        ShadingGeometry(const PrimitiveSample& primitiveSample, f32 b0, f32 b1, f32 b2);
        ~ShadingGeometry();

        Vector3 worldToLocal(const Vector3& v) const;
        Vector3 localToWorld(const Vector3& v) const;
        Ray nextRay(const Vector3& direction, f32 tmax) const;
        Ray nextCosineWeightedHemisphere(f32 u0, f32 u1, f32 tmax) const;
        Ray nextHemisphere(f32 u0, f32 u1, f32 tmax) const;

        Vector3 point_;
        Vector3 shadingNormal_;
        //Vector3 geometricNormal_;
        Vector3 binormal0_;
        Vector3 binormal1_;
        Vector3 dpdu_;
        Vector3 dpdv_;
        //Vector3 dndu_;
        //Vector3 dndv_;

        Vector2 uv_;
    };
}
#endif //INC_LRENDER_SHADINGGEOMETRY_H__

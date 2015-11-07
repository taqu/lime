/**
@file ShadingGeometry.cpp
@author t-sakai
@date 2015/10/06 create
*/
#include "ShadingGeometry.h"

namespace lrender
{
    ShadingGeometry::ShadingGeometry()
    {
    }

    ShadingGeometry::ShadingGeometry(const PrimitiveSample& primitiveSample, const MapperSample& mapperSample)
    {
        ShadingGeometry(primitiveSample, mapperSample.b0_, mapperSample.b1_, mapperSample.b2_);
    }

    ShadingGeometry::ShadingGeometry(const PrimitiveSample& primitiveSample, f32 b0, f32 b1, f32 b2)
    {
        point_ = weightAverage(
            b0, b1, b2,
            primitiveSample.positions_[0], primitiveSample.positions_[1], primitiveSample.positions_[2]);

        shadingNormal_ = weightAverage(
            b0, b1, b2,
            primitiveSample.normals_[0], primitiveSample.normals_[1], primitiveSample.normals_[2]);

        uv_ = weightAverage(
            b0, b1, b2,
            primitiveSample.uvs_[0], primitiveSample.uvs_[1], primitiveSample.uvs_[2]);

        Vector2 duv02; duv02.sub(primitiveSample.uvs_[0], primitiveSample.uvs_[2]);
        Vector2 duv12; duv12.sub(primitiveSample.uvs_[1], primitiveSample.uvs_[2]);

        Vector3 dp02, dp12;
        dp02.sub(primitiveSample.positions_[0], primitiveSample.positions_[2]);
        dp12.sub(primitiveSample.positions_[1], primitiveSample.positions_[2]);
        f32 determinant = duv02[0]*duv12[1] - duv02[1]*duv12[0];

        if(lmath::isZero(determinant)){
            lrender::orthonormalBasis(dpdu_, dpdv_, shadingNormal_);

        }else{
            f32 invDet = 1.0f/determinant;
            dpdu_.mul(duv12[1], dp02);
            dpdu_.muladd(-duv02[1], dp12, dpdu_);
            dpdu_ *= invDet;

            dpdv_.mul(-duv12[0], dp02);
            dpdv_.muladd(duv02[0], dp12, dpdv_);
            dpdv_ *= invDet;
        }

        binormal0_ = normalizeChecked(dpdu_);
        binormal1_.cross(binormal0_, shadingNormal_);

        f32 l2 = binormal1_.lengthSqr();
        if(DOT_EPSILON<l2){
            binormal1_.normalize(l2);
            binormal0_.cross(binormal1_, shadingNormal_);
        }else{
            lrender::orthonormalBasis(binormal0_, binormal1_, shadingNormal_);
        }
    }

    ShadingGeometry::~ShadingGeometry()
    {
    }

    Vector3 ShadingGeometry::worldToLocal(const Vector3& v) const
    {
        return Vector3(v.dot(binormal0_), v.dot(binormal1_), v.dot(shadingNormal_));
    }

    Vector3 ShadingGeometry::localToWorld(const Vector3& v) const
    {
        return Vector3(
            binormal0_.x_*v.x_ + binormal1_.x_*v.y_ + shadingNormal_.x_*v.z_,
            binormal0_.y_*v.x_ + binormal1_.y_*v.y_ + shadingNormal_.y_*v.z_,
            binormal0_.z_*v.x_ + binormal1_.z_*v.y_ + shadingNormal_.z_*v.z_);
    }

    Ray ShadingGeometry::nextRay(const Vector3& direction, f32 tmax) const
    {
        Vector3 origin;
        origin.muladd(RAY_EPSILON, direction, point_);
        return Ray(origin, direction, 0.0f, tmax, RAY_EPSILON);
    }

    Ray ShadingGeometry::nextCosineWeightedHemisphere(f32 u0, f32 u1, f32 tmax) const
    {
        Vector3 d;
        lmath::cosineWeightedRandomOnHemisphere(d.x_, d.y_, d.z_, u0, u1);
        d = localToWorld(d);
        d.normalize();
        return nextRay(d, tmax);
    }

    Ray ShadingGeometry::nextHemisphere(f32 u0, f32 u1, f32 tmax) const
    {
        Vector3 d;
        lmath::cosineWeightedRandomOnHemisphere(d.x_, d.y_, d.z_, u0, u1);
        d = localToWorld(d);
        d.normalize();
        return nextRay(d, tmax);
    }
}

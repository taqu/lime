/**
@file Intersection.cpp
@author t-sakai
@date 2015/09/27 create
*/
#include "core/Intersection.h"
#include "core/Ray.h"

namespace lrender
{
    void Intersection::create(const Shape* shape, s32 primitive, const PrimitiveSample& primitiveSample, f32 b0, f32 b1, f32 b2)
    {
        shape_ = shape;
        primitive_ = primitive;

        b0_ = b0;
        b1_ = b1;
        b2_ = b2;
        point_ = weightAverage(
            b0, b1, b2,
            primitiveSample.positions_[0], primitiveSample.positions_[1], primitiveSample.positions_[2]);

        shadingNormal_ = weightAverage(
            b0, b1, b2,
            primitiveSample.normals_[0], primitiveSample.normals_[1], primitiveSample.normals_[2]);

        uv_ = weightAverage(
            b0, b1, b2,
            primitiveSample.uvs_[0], primitiveSample.uvs_[1], primitiveSample.uvs_[2]);

        Vector2 duv02 = sub(primitiveSample.uvs_[0], primitiveSample.uvs_[2]);
        Vector2 duv12 = sub(primitiveSample.uvs_[1], primitiveSample.uvs_[2]);

        Vector3 dp02 = sub(primitiveSample.positions_[0], primitiveSample.positions_[2]);
        Vector3 dp12 = sub(primitiveSample.positions_[1], primitiveSample.positions_[2]);
        f32 determinant = duv02[0]*duv12[1] - duv02[1]*duv12[0];

        if(lmath::isZero(determinant)){
            lrender::orthonormalBasis(dpdu_, dpdv_, shadingNormal_);

        }else{
            f32 invDet = 1.0f/determinant;
            dpdu_ = (duv12[1]*dp02) + (-duv02[1]*dp12);
            dpdu_ *= invDet;

            dpdv_ = (-duv12[0]*dp02) + (duv02[0]*dp12);
            dpdv_ *= invDet;
        }

        binormal0_ = normalizeChecked(dpdu_);
        binormal1_ = cross(binormal0_, shadingNormal_);

        f32 l2 = binormal1_.lengthSqr();
        if(DOT_EPSILON<l2){
            binormal1_ = normalize(binormal1_, l2);
            binormal0_ = cross(binormal1_, shadingNormal_);
        }else{
            lrender::orthonormalBasis(binormal0_, binormal1_, shadingNormal_);
        }
    }

    void Intersection::create(const Vector3& position, const Vector3& normal, const Vector2& uv)
    {
        shape_ = NULL;
        primitive_ = 0;

        point_ = position;
        shadingNormal_ = normal;
        uv_ = uv;
        lrender::orthonormalBasis(dpdu_, dpdv_, shadingNormal_);

        binormal0_ = normalizeChecked(dpdu_);
        binormal1_ = cross(binormal0_, shadingNormal_);

        f32 l2 = binormal1_.lengthSqr();
        if(DOT_EPSILON<l2){
            binormal1_ = normalize(binormal1_, l2);
            binormal0_ = cross(binormal1_, shadingNormal_);
        }else{
            lrender::orthonormalBasis(binormal0_, binormal1_, shadingNormal_);
        }
    }

    void Intersection::computeDifferentials(const RayDifferential& ray)
    {
        if(!ray.hasDifferentials_){
            setHasDifferentials(false);
            dudx_ = dvdx_ = 0.0f;
            dudy_ = dvdy_ = 0.0f;
            return;
        }

        f32 d = dot(geometricNormal_, point_);
        f32 ndrx = dot(geometricNormal_, ray.rxDirection_);
        f32 ndry = dot(geometricNormal_, ray.ryDirection_);
        if(lmath::isZero(ndrx) || lmath::isZero(ndry)){
            clearDifferentials();
            return;
        }

        f32 tx = -(dot(geometricNormal_, ray.rxOrigin_) - d)/ndrx;
        f32 ty = -(dot(geometricNormal_, ray.ryOrigin_) - d)/ndry;

        Vector3 px = muladd(tx, ray.rxDirection_, ray.rxOrigin_);
        Vector3 py = muladd(ty, ray.ryDirection_, ray.ryOrigin_);

        dpdx_ = px - point_;
        dpdy_ = py - point_;

        f32 absX = lcore::absolute(geometricNormal_.x_);
        f32 absY = lcore::absolute(geometricNormal_.y_);
        f32 absZ = lcore::absolute(geometricNormal_.z_);

        s32 dim[2];
        if(absY<absX && absZ<absX){
            dim[0] = 1;
            dim[1] = 2;
        }else if(absZ<absY){
            dim[0] = 0;
            dim[1] = 2;
        }else{
            dim[0] = 0;
            dim[1] = 1;
        }

        f32 A[2][2] = {{dpdu_[dim[0]], dpdv_[dim[0]]}, {dpdu_[dim[1]], dpdv_[dim[1]]}};
        f32 Bx[2] = {px[dim[0]] - point_[dim[0]], px[dim[1]] - point_[dim[1]]};
        f32 By[2] = {py[dim[0]] - point_[dim[0]], py[dim[1]] - point_[dim[1]]};

        if(!lmath::solveLinearSystem2x2(A, By, dudx_, dvdx_)){
            dudx_ = dvdx_ = 0.0f;
        }
        if(!lmath::solveLinearSystem2x2(A, Bx, dudy_, dvdy_)){
            dudy_ = dvdy_ = 0.0f;
        }
    }

    Vector3 Intersection::worldToLocal(const Vector3& v) const
    {
        return Vector3(dot(v, binormal0_), dot(v, binormal1_), dot(v, shadingNormal_));
    }

    Vector3 Intersection::localToWorld(const Vector3& v) const
    {
        return Vector3(
            binormal0_.x_*v.x_ + binormal1_.x_*v.y_ + shadingNormal_.x_*v.z_,
            binormal0_.y_*v.x_ + binormal1_.y_*v.y_ + shadingNormal_.y_*v.z_,
            binormal0_.z_*v.x_ + binormal1_.z_*v.y_ + shadingNormal_.z_*v.z_);
    }

    Ray Intersection::nextRay(const Vector3& direction, f32 tmax) const
    {
        Vector3 origin = muladd(RAY_EPSILON, geometricNormal_, point_);
        return Ray(origin, direction, 0.0f, tmax, RAY_EPSILON);
    }

    Ray Intersection::nextCosineWeightedHemisphere(f32 u0, f32 u1, f32 tmax) const
    {
        Vector3 d;
        lmath::cosineWeightedRandomOnHemisphere(d.x_, d.y_, d.z_, u0, u1);
        d = normalize(localToWorld(d));
        return nextRay(d, tmax);
    }

    Ray Intersection::nextHemisphere(f32 u0, f32 u1, f32 tmax) const
    {
        Vector3 d;
        lmath::cosineWeightedRandomOnHemisphere(d.x_, d.y_, d.z_, u0, u1);
        d = normalize(localToWorld(d));
        return nextRay(d, tmax);
    }
}

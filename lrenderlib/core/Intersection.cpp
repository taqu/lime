/**
@file Intersection.cpp
@author t-sakai
@date 2015/09/27 create
*/
#include "Intersection.h"
#include "core/Ray.h"

namespace lrender
{
    void Intersection::computeDifferentials(const RayDifferential& ray)
    {
        if(!ray.hasDifferentials_){
            setHasDifferentials(false);
            dudx_ = dvdx_ = 0.0f;
            dudy_ = dvdy_ = 0.0f;
            return;
        }

        f32 d = geometricNormal_.dot(point_);
        f32 ndrx = geometricNormal_.dot(ray.rxDirection_);
        f32 ndry = geometricNormal_.dot(ray.ryDirection_);
        if(lmath::isZero(ndrx) || lmath::isZero(ndry)){
            clearDifferentials();
            return;
        }

        f32 tx = -(geometricNormal_.dot(ray.rxOrigin_) - d)/ndrx;
        f32 ty = -(geometricNormal_.dot(ray.ryOrigin_) - d)/ndry;

        Vector3 px, py;
        px.muladd(tx, ray.rxDirection_, ray.rxOrigin_);
        py.muladd(ty, ray.ryDirection_, ray.ryOrigin_);

        dpdx_.sub(px, point_);
        dpdy_.sub(py, point_);

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
        return Vector3(v.dot(binormal0_), v.dot(binormal1_), v.dot(shadingNormal_));
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
        Vector3 origin;
        origin.muladd(RAY_EPSILON, direction, point_);
        return Ray(origin, direction, 0.0f, tmax, RAY_EPSILON);
    }
}

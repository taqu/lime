/**
@file PrimitiveTest.cpp
@author t-sakai
@date 2009/12/20
*/
#include "PrimitiveTest.h"
#include "Plane.h"
#include "Sphere.h"

namespace lmath
{
    //---------------------------------------------------------------------------------
    // 点から平面上の最近傍点を計算
    void closestPointPointVsPlane(lmath::Vector3& result, const lmath::Vector3& point, const Plane& plane)
    {
        //平面は正規化されている
        LASSERT(isEqual(plane.normal_.lengthSqr(), 1.0f));

        f32 t = plane.normal_.dot(point) - plane.d_;
        result = plane.normal_;
        result *= -t;
        result += point;
    }

    //---------------------------------------------------------------------------------
    // 点から平面への距離を計算
    f32 distancePointVsPlane(const lmath::Vector3& point, const Plane& plane)
    {
        //平面は正規化されている
        LASSERT(lmath::isEqual(plane.normal_.lengthSqr(), 1.0f));

        f32 t = plane.normal_.dot(point) - plane.d_;
        return t;
    }

    //---------------------------------------------------------------------------------
    // 点から線分への最近傍点を計算
    f32 closestPointPointVsSegment(
        f32& t,
        lmath::Vector3& result,
        const lmath::Vector3& point,
        const lmath::Vector3& l1,
        const lmath::Vector3& l2)
    {
        Vector3 v0 = l1;
        v0 -= l2;

        Vector3 v1 = point;
        v1 -= l1;

        t = v1.dot(v0);
        if(t<=0.0f){
            t = 0.0f;
            result = l1;
        }else{
            f32 denom = v0.lengthSqr();
            if(t>=denom){
                t = 1.0f;
                result = l2;
            }else{
                t /= denom;
                result = v0;
                result *= t;
                result += l1;
            }
        }
        return t;
    }

    //---------------------------------------------------------------------------------
    // 点から線分への距離を計算
    f32 distancePointVsSegment(
        const lmath::Vector3& point,
        const lmath::Vector3& l1,
        const lmath::Vector3& l2)
    {
        Vector3 v0 = l2;
        v0 -= l1;

        Vector3 v1 = point;
        v1 -= l1;

        Vector3 v2 = point;
        v2 -= l2;

        f32 t = v0.dot(v1);
        f32 l;
        if(t<=0.0f){
            l = v1.lengthSqr();
        }else{

            f32 u = v0.lengthSqr();
            if(u>=t){
                l = v2.lengthSqr();
            }else{
                l = v1.lengthSqr() - t*t/u;
            }
        }
        return lmath::sqrt(l);
    }

    //---------------------------------------------------------------------------------
    // 球と平面が交差するか
    bool testSphereVsPlane(f32 &t, const Sphere& sphere, const Plane& plane)
    {
        t = distancePointVsPlane(sphere.position_, plane);
        return t<=sphere.radius_;
    }
}

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
        f32 t = plane.dot(point);
        result.set(plane.v_.x_, plane.v_.y_, plane.v_.z_);
        result *= -t;
        result += point;
    }

    //---------------------------------------------------------------------------------
    // 点から平面への距離を計算
    f32 distancePointVsPlane(const lmath::Vector3& point, const Plane& plane)
    {
        f32 t = plane.dot(point);
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
    f32 sqDistancePointAABB(
        const lmath::Vector4& point,
        const lmath::Vector4& bmin,
        const lmath::Vector4& bmax)
    {
        f32 distance = 0.0f;
        for(s32 i=0; i<3; ++i){
            if(point[i]<bmin[i]){
                f32 t = bmin[i] - point[i];
                distance += t*t;
            }
            if(bmax[i]<point[i]){
                f32 t = point[i] - bmax[i];
                distance += t*t;
            }
        }
        return distance;
    }

    //---------------------------------------------------------------------------------
    void closestPointPointVSAABB(
        lmath::Vector4& result,
        const lmath::Vector4& point,
        const lmath::Vector4& bmin,
        const lmath::Vector4& bmax)
    {
        result.w_ = 0.0f;
        for(s32 i=0; i<3; ++i){
            f32 v = point[i];
            if(v<bmin[i]){
                v = bmin[i];
            }
            if(bmax[i]<v){
                v = bmax[i];
            }
            result[i] = v;
        }
    }

    //---------------------------------------------------------------------------------
    // 球と平面が交差するか
    bool testSphereVsPlane(f32 &t, const Sphere& sphere, const Plane& plane)
    {
        t = distancePointVsPlane(Vector3(sphere.s_.x_, sphere.s_.y_, sphere.s_.z_), plane);
        return (t<=sphere.s_.w_);
    }

    //---------------------------------------------------------------------------------
    bool testSphereVsSphere(const Sphere& sphere0, const Sphere& sphere1)
    {
        f32 distance = sphere0.distance(sphere1);
        f32 radius = sphere0.s_.w_ + sphere1.s_.w_ + F32_EPSILON;

        return (distance <= radius);
    }

    //---------------------------------------------------------------------------------
    bool testSphereVsSphere(f32& distance, const Sphere& sphere0, const Sphere& sphere1)
    {
        distance = sphere0.distance(sphere1);
        f32 radius = sphere0.s_.w_ + sphere1.s_.w_ + F32_EPSILON;

        return (distance <= radius);
    }

    //---------------------------------------------------------------------------------
    bool testAABBVsAABB(const lmath::Vector4& bmin0, const lmath::Vector4& bmax0, const lmath::Vector4& bmin1, const lmath::Vector4& bmax1)
    {
        if(bmax0.x_<bmin1.x_ || bmin0.x_>bmax1.x_){
            return false;
        }

        if(bmax0.y_<bmin1.y_ || bmin0.y_>bmax1.y_){
            return false;
        }

        if(bmax0.z_<bmin1.z_ || bmin0.z_>bmax1.z_){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------------------------
    bool testSphereVsAABB(const Sphere& sphere, const lmath::Vector4& bmin, const lmath::Vector4& bmax)
    {
        f32 distance = sqDistancePointAABB(sphere.s_, bmin, bmax);
        return distance <= (sphere.getRadius()*sphere.getRadius());
    }

    //---------------------------------------------------------------------------------
    bool testSphereVsAABB(lmath::Vector4& close, const Sphere& sphere, const lmath::Vector4& bmin, const lmath::Vector4& bmax)
    {
        closestPointPointVSAABB(close, sphere.s_, bmin, bmax);

        lmath::Vector4 d;
        d.sub(close, sphere.s_);
        d.w_ = 0.0f;
        return d.lengthSqr() <= (sphere.getRadius()*sphere.getRadius());
    }

    //---------------------------------------------------------------------------------
    void clipTriangle(
        s32& numTriangles,
        Triangle* triangles,
        const Plane& plane)
    {

        Triangle& triangle = triangles[numTriangles-1];
        f32 d[3];
        s32 indices[3];

        s32 inside = 0;
        s32 outside = 3;
        for(s32 i=0; i<3; ++i){
            f32 pd = plane.dot(triangle.v_[i]);
            if(pd<0.0f){
                indices[inside] = i;
                d[inside] = pd;
                ++inside;
            }else{
                --outside;
                indices[outside] = i;
                d[outside] = pd;
            }
        }

        switch(inside)
        {
        case 0:
            --numTriangles;
            break;

        case 1:
            {
                Vector4& p0 = triangle.v_[indices[0]];
                Vector4& p1 = triangle.v_[indices[2]];
                Vector4& p2 = triangle.v_[indices[1]];

                f32 t;
                t = d[0] / (d[0] - d[2]);
                Vector4 new0;
                new0.setLerp(p0, p1, t);

                t = d[0] / (d[0] - d[1]);
                Vector4 new1;
                new1.setLerp(p0, p2, t);

                triangle.v_[0] = p0;
                triangle.v_[1] = new0;
                triangle.v_[2] = new1;
            }
            break;

        case 2:
            {
                if(isEqual(d[2], 0.0f)){
                    return;
                }
                Vector4& p0 = triangle.v_[indices[2]];
                Vector4& p1 = triangle.v_[indices[0]];
                Vector4& p2 = triangle.v_[indices[1]];

                f32 t;
                t = d[2] / (d[2] - d[0]);
                Vector4 new0;
                new0.setLerp(p0, p1, t);

                t = d[2] / (d[2] - d[1]);
                Vector4 new1;
                new1.setLerp(p0, p2, t);

                triangle.v_[0] = p1;
                triangle.v_[1] = new0;
                triangle.v_[2] = p2;

                triangles[numTriangles].v_[0] = p2;
                triangles[numTriangles].v_[1] = new0;
                triangles[numTriangles].v_[2] = new1;
                ++numTriangles;
            }
            break;
        };

    }
}

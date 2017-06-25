/**
@file PrimitiveTest.cpp
@author t-sakai
@date 2009/12/20
*/
#include "geometry/PrimitiveTest.h"
#include "geometry/Plane.h"
#include "geometry/Sphere.h"
#include "Vector2.h"

namespace lmath
{
    //---------------------------------------------------------------------------------
    // 点から平面上の最近傍点を計算
    void closestPointPointPlane(lmath::Vector3& result, const lmath::Vector3& point, const Plane& plane)
    {
        f32 t = plane.dot(point);
        result.set(plane.nx_, plane.ny_, plane.nz_);
        result *= -t;
        result += point;
    }

    //---------------------------------------------------------------------------------
    // 点から平面への距離を計算
    f32 distancePointPlane(f32 x, f32 y, f32 z, const Plane& plane)
    {
        return plane.dot(x, y, z);
    }
    f32 distancePointPlane(const lmath::Vector3& point, const Plane& plane)
    {
        return plane.dot(point);
    }

    //---------------------------------------------------------------------------------
    // 点から線分への最近傍点を計算
    f32 closestPointPointSegment(
        lmath::Vector3& result,
        const lmath::Vector3& point,
        const lmath::Vector3& l0,
        const lmath::Vector3& l1)
    {
        Vector3 v0 = l1-l0;
        Vector3 v1 = point - l0;

        f32 t = dot(v1, v0);
        if(t<=LMATH_F32_EPSILON){
            t = 0.0f;
            result = l0;
        }else{
            f32 denom = v0.lengthSqr();
            if(denom<=t){
                t = 1.0f;
                result = l1;
            }else{
                t /= denom;
                result = v0;
                result *= t;
                result += l0;
            }
        }
        return t;
    }

    //---------------------------------------------------------------------------------
    // 点から線分への距離を計算
    f32 distancePointSegmentSqr(
        const lmath::Vector3& point,
        const lmath::Vector3& l0,
        const lmath::Vector3& l1)
    {
        Vector3 v0 = l1 - l0;
        Vector3 v1 = point - l0;

        f32 t = dot(v0, v1);
        if(t<=LMATH_F32_EPSILON){
            return v1.lengthSqr();
        }else{
            f32 u = v0.lengthSqr();
            if(u<=t){
                return distanceSqr(point, l1);
            }else{
                return v1.lengthSqr() - t*t/u;
            }
        }
    }

    //---------------------------------------------------------------------------------
    // 点から直線への最近傍点を計算
    f32 closestPointPointLine(
        lmath::Vector3& result,
        const lmath::Vector3& point,
        const lmath::Vector3& l0,
        const lmath::Vector3& l1)
    {
        Vector3 v0 = l1 - l0;
        Vector3 v1 = point - l0;

        f32 t;
        f32 denom = v0.lengthSqr();
        if(denom<=LMATH_F32_EPSILON){
            t = 0.0f;
            result = l0;
        } else{
            t = dot(v1, v0);
            t /= denom;
            result = v0;
            result *= t;
            result += l0;
        }
        return t;
    }

    //---------------------------------------------------------------------------------
    // 点から直線への距離を計算
    f32 distancePointLineSqr(
        const lmath::Vector3& point,
        const lmath::Vector3& l0,
        const lmath::Vector3& l1)
    {
        Vector3 v0 = l1 - l0;
        Vector3 v1 = point - l0;

        f32 denom = v0.lengthSqr();
        if(denom<=LMATH_F32_EPSILON){
            return v1.lengthSqr();
        }else{
            f32 t = dot(v1, v0);
            return v1.lengthSqr() - t*t/denom;
        }
    }

    //---------------------------------------------------------------------------------
    // 線分と線分の最近傍点を計算
    f32 closestPointSegmentSegmentSqr(
        f32& s,
        f32& t,
        Vector3& c0,
        Vector3& c1,
        const Vector3& p0,
        const Vector3& q0,
        const Vector3& p1,
        const Vector3& q1)
    {
        Vector3 d0 = q0-p0;
        Vector3 d1 = q1-p1;
        Vector3 r = p0-p1;
        f32 a = dot(d0, d0);
        f32 e = dot(d1, d1);
        f32 f = dot(d1, r);
        if(a<=LMATH_F32_EPSILON && e<=LMATH_F32_EPSILON){
            s = t = 0.0f;
            c0 = p0;
            c1 = p1;
            return dot(r, r);
        }
        if(a<=LMATH_F32_EPSILON){
            s = 0.0f;
            t = lcore::clamp01(f/e);
        }else if(e<=LMATH_F32_EPSILON){
            t = 0.0f;
            s = lcore::clamp01(-dot(d0,r)/a);
        }else{
            f32 b = dot(d0,d1);
            f32 c = dot(d0,r);
            f32 denom = a*e - b*b;
            if(LMATH_F32_EPSILON<denom){
                s = lcore::clamp01((b*f-c*e)/denom);
            }else{
                s = 0.0f;
            }
            f32 tnorm = b*s + f;
            if(tnorm<0.0f){
                t = 0.0f;
                s = lcore::clamp01(-c/a);
            }else if(e<tnorm){
                t = 1.0f;
                s = lcore::clamp01((b-c)/a);
            }else{
                t = tnorm/e;
            }
        }

        c0 = p0 + d0 * s;
        c1 = p1 + d1 * t;
        return distanceSqr(c0, c1);
    }

    //---------------------------------------------------------------------------------
    f32 distancePointAABBSqr(
        const f32* point,
        const f32* bmin,
        const f32* bmax)
    {
        f32 distance = 0.0f;
        for(s32 i=0; i<3; ++i){
            if(point[i]<bmin[i]){
                f32 t = bmin[i] - point[i];
                distance += t*t;

            } else if(bmax[i]<point[i]){
                f32 t = point[i] - bmax[i];
                distance += t*t;
            }
        }
        return distance;
    }

    //---------------------------------------------------------------------------------
    void closestPointPointAABB(
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

            }else if(bmax[i]<v){
                v = bmax[i];
            }
            result[i] = v;
        }
    }

    //---------------------------------------------------------------------------------
    void closestPointPointAABB(
        lmath::Vector3& result,
        const lmath::Vector3& point,
        const lmath::Vector3& bmin,
        const lmath::Vector3& bmax)
    {
        for(s32 i=0; i<3; ++i){
            f32 v = point[i];
            if(v<bmin[i]){
                v = bmin[i];

            }else if(bmax[i]<v){
                v = bmax[i];
            }
            result[i] = v;
        }
    }

    //---------------------------------------------------------------------------------
    // 球と平面が交差するか
    bool testSpherePlane(f32 &t, const Sphere& sphere, const Plane& plane)
    {
        t = distancePointPlane(sphere.x_, sphere.y_, sphere.z_, plane);
        return (lcore::absolute(t)<=sphere.r_);
    }

    //---------------------------------------------------------------------------------
    bool testSphereSphere(const Sphere& sphere0, const Sphere& sphere1)
    {
        f32 distance = sphere0.distance(sphere1);
        f32 radius = sphere0.r_ + sphere1.r_ + LMATH_F32_EPSILON;

        return (distance <= radius);
    }

    //---------------------------------------------------------------------------------
    bool testSphereSphere(f32& distance, const Sphere& sphere0, const Sphere& sphere1)
    {
        distance = sphere0.distance(sphere1);
        f32 radius = sphere0.r_ + sphere1.r_ + LMATH_F32_EPSILON;

        return (distance <= radius);
    }

    //---------------------------------------------------------------------------------
    //AABBの交差判定
    bool testAABBAABB(const lmath::Vector4& bmin0, const lmath::Vector4& bmax0, const lmath::Vector4& bmin1, const lmath::Vector4& bmax1)
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

    s32 testAABBAABB(const lm128 bbox0[2][3], const lm128 bbox1[2][3])
    {
        u32 mask = 0xFFFFFFFFU;
        f32 fmask = *((f32*)&mask);

        lm128 t = _mm_set1_ps(fmask);
        for(s32 i=0; i<3; ++i){
            t = _mm_and_ps(t, _mm_cmple_ps(bbox0[0][i], bbox1[1][i]));
            t = _mm_and_ps(t, _mm_cmple_ps(bbox1[0][i], bbox0[1][i]));
        }
        return _mm_movemask_ps(t);
    }

    //---------------------------------------------------------------------------------
    bool testSphereAABB(const Sphere& sphere, const lmath::Vector4& bmin, const lmath::Vector4& bmax)
    {
        f32 distance = distancePointAABBSqr(sphere, bmin, bmax);
        return distance <= (sphere.radius()*sphere.radius());
    }

    //---------------------------------------------------------------------------------
    bool testSphereAABB(lmath::Vector4& close, const Sphere& sphere, const lmath::Vector4& bmin, const lmath::Vector4& bmax)
    {
        closestPointPointAABB(close, sphere, bmin, bmax);

        Vector4 d = Vector4::construct(close - sphere);
        d.w_ = 0.0f;
        return d.lengthSqr() <= (sphere.radius()*sphere.radius());
    }

    //---------------------------------------------------------------------------------
    bool testSphereCapsule(const Sphere& sphere, const Vector3& p0, const Vector3& q0, f32 r0)
    {
        f32 distanceSqr = distancePointSegmentSqr(sphere.position(), p0, q0);
        f32 radius = sphere.radius() + r0;
        return distanceSqr <= (radius*radius);
    }

    //---------------------------------------------------------------------------------
    bool testCapsuleCapsule(
        const Vector3& p0,
        const Vector3& q0,
        f32 r0,
        const Vector3& p1,
        const Vector3& q1,
        f32 r1)
    {
        f32 s,t;
        Vector3 c0,c1;
        f32 distanceSqr = closestPointSegmentSegmentSqr(
            s,t,
            c0, c1,
            p0, q0,
            p1, q1);
        f32 radius = r0 + r1;
        return distanceSqr <= (radius*radius);
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
                Vector4 new0 = Vector4::construct(lerp(p0, p1, t));

                t = d[0] / (d[0] - d[1]);
                Vector4 new1 = Vector4::construct(lerp(p0, p2, t));

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
                Vector4 new0 = Vector4::construct(lerp(p0, p1, t));

                t = d[2] / (d[2] - d[1]);
                Vector4 new1 = Vector4::construct(lerp(p0, p2, t));

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

    bool testPointInPolygon(const lmath::Vector2& point, const lmath::Vector2* points, s32 n)
    {
        LASSERT(2<n);
        s32 i0=n-1,i1=0;
        bool yflag0 = (point.y_ <= points[i0].y_);

        bool flag = false;
        for(; i1<n; i0=i1,++i1){
            bool yflag1 = (point.y_ <= points[i1].y_);
            if(yflag0 != yflag1){
                if(yflag0 != yflag1) {
                    if(((point.x_-points[i0].x_)*(points[i1].y_ - points[i0].y_) <= (points[i1].x_ - points[i0].x_) * (point.y_ - points[i0].y_)) == yflag1) {
                        flag = !flag;
                    }
                }
            }
            yflag0 = yflag1;
        }
        return flag;
    }

    bool testPointInTriangle(
        f32& b0, f32& b1, f32& b2,
        const Vector2& p,
        const Vector2& p0, const Vector2& p1, const Vector2& p2)
    {
        f32 t00 = p0.x_-p2.x_;
        f32 t01 = p1.x_-p2.x_;
        f32 t10 = p0.y_-p2.y_;
        f32 t11 = p1.y_-p2.y_;
        f32 determinant = t00*t11 - t10*t01;
        if(lmath::isZero(determinant)){
            b0 = 1.0f;
            b1 = 0.0f;
            b2 = 0.0f;
            return false;
        }

        f32 invDet = 1.0f/determinant;
        b0 = ((p1.y_-p2.y_)*(p.x_-p2.x_) + (p2.x_-p1.x_)*(p.y_-p2.y_))*invDet;
        b1 = ((p2.y_-p0.y_)*(p.x_-p2.x_) + (p0.x_-p2.x_)*(p.y_-p2.y_))*invDet;
        b2 = 1.0f-b0-b1;
        return (0.0f<=b0) && (0.0f<=b1) && (0.0f<=b2);
    }

    void barycentricCoordinate(
        f32& b0, f32& b1, f32& b2,
        const Vector2& p,
        const Vector2& p0,
        const Vector2& p1,
        const Vector2& p2)
    {
        f32 t00 = p0.x_-p2.x_;
        f32 t01 = p1.x_-p2.x_;
        f32 t10 = p0.y_-p2.y_;
        f32 t11 = p1.y_-p2.y_;
        f32 determinant = t00*t11 - t10*t01;
        if(lmath::isZero(determinant)){
            b0 = 1.0f;
            b1 = 0.0f;
            b2 = 0.0f;
            return;
        }

        f32 invDet = 1.0f/determinant;
        b0 = ((p1.y_-p2.y_)*(p.x_-p2.x_) + (p2.x_-p1.x_)*(p.y_-p2.y_))*invDet;
        b1 = ((p2.y_-p0.y_)*(p.x_-p2.x_) + (p0.x_-p2.x_)*(p.y_-p2.y_))*invDet;
        b2 = 1.0f-b0-b1;
    }
}

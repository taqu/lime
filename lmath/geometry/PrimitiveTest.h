#ifndef INC_LMATH_PRIMITIVETEST_H__
#define INC_LMATH_PRIMITIVETEST_H__
/**
@file PrimitiveTest.h
@author t-sakai
@date 2009/12/20 create
*/
#include "../lmath.h"
#include "../Vector3.h"
#include "../Vector4.h"

namespace lmath
{
    class Vector2;
    class Plane;
    class Sphere;

    //---------------------------------------------------------------------------------
    /**
    @brief 点から平面上の最近傍点を計算
    @param result ... 出力
    @param point ... 入力点
    @param plane ... 入力平面
    @notice 入力平面は正規化されている
    */
    void closestPointPointPlane(lmath::Vector3& result, const lmath::Vector3& point, const Plane& plane);

    //---------------------------------------------------------------------------------
    /**
    @brief 点から平面への距離を計算
    @return 距離
    @param point ... 入力点
    @param plane ... 入力平面
    @notice 入力平面は正規化されている
    */
    f32 distancePointPlane(const lmath::Vector3& point, const Plane& plane);

    //---------------------------------------------------------------------------------
    /**
    @brief 点から線分への最近傍点を計算
    @return result = t*(l2-l1)となるt
    @param result ... 出力
    @param point ... 
    @param l1 ... 
    @param l2 ... 
    */
    f32 closestPointPointSegment(
        lmath::Vector3& result,
        const lmath::Vector3& point,
        const lmath::Vector3& l1,
        const lmath::Vector3& l2);

    //---------------------------------------------------------------------------------
    /**
    @brief 点から線分への距離を計算
    @return 点から線分への距離
    @param point ... 
    @param l1 ... 
    @param l2 ... 
    */
    f32 distancePointSegment(
        const lmath::Vector3& point,
        const lmath::Vector3& l1,
        const lmath::Vector3& l2);

    //---------------------------------------------------------------------------------
    f32 sqDistancePointAABB(
        const f32* point,
        const f32* bmin,
        const f32* bmax);

    inline f32 sqDistancePointAABB(const lmath::Vector4& point, const lmath::Vector4& bmin, const lmath::Vector4& bmax)
    {
        return sqDistancePointAABB(&point.x_, &bmin.x_, &bmax.x_);
    }

    inline f32 sqDistancePointAABB(const lmath::Vector3& point, const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        return sqDistancePointAABB(&point.x_, &bmin.x_, &bmax.x_);
    }

    inline f32 sqDistancePointAABB(const lmath::Vector3& point, const lmath::Vector4& bmin, const lmath::Vector4& bmax)
    {
        return sqDistancePointAABB(&point.x_, &bmin.x_, &bmax.x_);
    }

    inline f32 sqDistancePointAABB(const lmath::Vector4& point, const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        return sqDistancePointAABB(&point.x_, &bmin.x_, &bmax.x_);
    }

    //---------------------------------------------------------------------------------
    void closestPointPointAABB(
        lmath::Vector4& result,
        const lmath::Vector4& point,
        const lmath::Vector4& bmin,
        const lmath::Vector4& bmax);

    void closestPointPointAABB(
        lmath::Vector3& result,
        const lmath::Vector3& point,
        const lmath::Vector3& bmin,
        const lmath::Vector3& bmax);

    //---------------------------------------------------------------------------------
    /**
    @return 球と平面が交差するか
    @param sphere ...
    @param plane ... 
    @notice 接する場合も含む
    */
    bool testSpherePlane(f32 &t, const Sphere& sphere, const Plane& plane);

    //---------------------------------------------------------------------------------
    bool testSphereSphere(const Sphere& sphere0, const Sphere& sphere1);

    //---------------------------------------------------------------------------------
    bool testSphereSphere(f32& distance, const Sphere& sphere0, const Sphere& sphere1);

    //---------------------------------------------------------------------------------
    //AABBの交差判定
    bool testAABBAABB(const lmath::Vector4& bmin0, const lmath::Vector4& bmax0, const lmath::Vector4& bmin1, const lmath::Vector4& bmax1);
    s32 testAABBAABB(const lm128 bbox0[2][3], const lm128 bbox1[2][3]);

    //---------------------------------------------------------------------------------
    bool testSphereAABB(const Sphere& sphere, const lmath::Vector4& bmin, const lmath::Vector4& bmax);

    //---------------------------------------------------------------------------------
    bool testSphereAABB(lmath::Vector4& close, const Sphere& sphere, const lmath::Vector4& bmin, const lmath::Vector4& bmax);

    //---------------------------------------------------------------------------------
    struct Triangle
    {
        Vector4 v_[3];
    };

    /**
    */
    void clipTriangle(
        s32& numTriangles,
        Triangle* triangles,
        const Plane& plane);


    //---------------------------------------------------------------------------------
    bool testPointInPolygon(const lmath::Vector2& point, const lmath::Vector2* points, s32 n);

    bool testPointInTriangle(
        f32& b0, f32& b1, f32& b2,
        const Vector2& p,
        const Vector2& p0, const Vector2& p1, const Vector2& p2);

    void barycentricCoordinate(
        f32& b0, f32& b1, f32& b2,
        const Vector2& p,
        const Vector2& p0,
        const Vector2& p1,
        const Vector2& p2);
}

#endif //INC_LMATH_PRIMITIVETEST_H__

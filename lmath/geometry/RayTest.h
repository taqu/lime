#ifndef INC_LMATH_RAYTEST_H__
#define INC_LMATH_RAYTEST_H__
/**
@file RayTest.h
@author t-sakai
@date 2009/12/21 create
*/
#include "../lmath.h"

namespace lmath
{
    class Ray;
    class Plane;
    class Sphere;
    class Vector3;

    //-----------------------------------------------------------
    lm128 dot4vec(
        const lm128& vx0,
        const lm128& vy0,
        const lm128& vz0,
        const lm128& vx1,
        const lm128& vy1,
        const lm128& vz1);

    void cross4vec(
        lm128& vx,
        lm128& vy,
        lm128& vz,
        const lm128& vx0,
        const lm128& vy0,
        const lm128& vz0,
        const lm128& vx1,
        const lm128& vy1,
        const lm128& vz1);


    //-----------------------------------------------------------
    /**
    @brief 線分と平面の交差判定
    @return 交差するか
    @param t ... 交差点の陰関数パラメータ。交差点 = t*ray.direction_ + ray.origin_
    @param ray ...
    @param plane ...
    */
    bool testRayPlane(f32& t, const Ray& ray, const Plane& plane);

    //-----------------------------------------------------------
    /**
    @brief 線分と球の交差判定
    @return 交差するか
    @param t ... 交差点の陰関数パラメータ。交差点 = t*ray.direction_ + ray.origin_
    @param ray ...
    @param sphere ...

    線分の始点が球の内側の場合 t=0
    */
    bool testRaySphere(f32& t, const Ray& ray, const Sphere& sphere);

    //-----------------------------------------------------------
    /**
    @brief 線分と球の交差判定
    @return 交差するか
    @param tmin ... 交差点の陰関数パラメータ。交差点 = tmin*ray.direction_ + ray.origin_
    @param tmax ... 交差点の陰関数パラメータ。交差点 = tmax*ray.direction_ + ray.origin_
    @param ray ...
    @param sphere ...
    */
    bool testRaySphere(f32& tmin, f32& tmax, const Ray& ray, const Sphere& sphere);

    //-----------------------------------------------------------
    /**
    @brief 線分と球表面の交差判定
    @return 交差するか
    @param t ... 交差点の陰関数パラメータ。交差点 = t*ray.direction_ + ray.origin_
    @param ray ...
    @param sphere ...
    */
    bool testRaySphereSurface(f32& t, const Ray& ray, const Sphere& sphere);

    //-----------------------------------------------------------
    /**
    @brief 線分と三角形の交差判定
    @return 交差するか
    @param t ... 交差点の陰関数パラメータ。交差点 = t*ray.direction_ + ray.origin_
    @param ray ...
    @param v0 ...
    @param v1 ...
    @param v2 ...
    */
    bool testRayTriangleFront(f32& t, f32& v, f32& w, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2);
    bool testRayTriangleBack(f32& t, f32& v, f32& w, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2);
    Result testRayTriangleBoth(f32& t, f32& v, f32& w, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    /**
    @brief 線分と三角形の交差判定
    @return 交差するか
    @param u ... 重心座標
    @param v ... 重心座標
    @param w ... 重心座標
    @param startPoint ... 線分の始点
    @param endPoint ... 線分の終点
    @param v0 ...
    @param v1 ...
    @param v2 ...
    */
    bool testRayTriangleFront(f32& u, f32& v, f32& w, const Vector3& startPoint, const Vector3& endPoint, const Vector3& v0, const Vector3& v1, const Vector3& v2);
    bool testRayTriangleBack(f32& u, f32& v, f32& w, const Vector3& startPoint, const Vector3& endPoint, const Vector3& v0, const Vector3& v1, const Vector3& v2);
    Result testRayTriangleBoth(f32& u, f32& v, f32& w, const Vector3& startPoint, const Vector3& endPoint, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    /**
    @return 下位4ビットが結果フラグ
    */
    s32 testRayTriangleFront(
        lm128& t,
        lm128& v,
        lm128& w,
        const lm128 origin[3],
        const lm128 direction[3],
        const lm128 vx[3],
        const lm128 vy[3],
        const lm128 vz[3]);

    /**
    @return 下位4ビットが結果フラグ
    */
    s32 testRayTriangleBack(
        lm128& t,
        lm128& v,
        lm128& w,
        const lm128 origin[3],
        const lm128 direction[3],
        const lm128 vx[3],
        const lm128 vy[3],
        const lm128 vz[3]);

    /**
    @return Little Endianで8bitずつResultを返す
    */
    s32 testRayTriangleBoth(
        lm128& t,
        lm128& v,
        lm128& w,
        const lm128 origin[3],
        const lm128 direction[3],
        const lm128 vx[3],
        const lm128 vy[3],
        const lm128 vz[3]);

    //-----------------------------------------------------------
    bool testRayRectangle(f32& t, const Ray& ray, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3);


    //-----------------------------------------------------------
    /**
    @brief 線分とAABBの交差判定
    @return 交差するか
    @param tmin ...
    @param tmax ...
    @param ray ...
    @param bmin ...
    @param bmax ...

    交差点の陰関数パラメータを最大最小返す
    */
    bool testRayAABB(f32& tmin, f32& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax);

    bool testRayAABB(const Ray& ray, const Vector3& bmin, const Vector3& bmax);

    bool testRayAABB(f32& tmin, f32& tmax, const Ray& ray, const Vector4& bmin, const Vector4& bmax);

    bool testRayAABB(const Ray& ray, const Vector4& bmin, const Vector4& bmax);

    s32 testRayAABB(
        lm128 tmin,
        lm128 tmax,
        lm128 origin[3],
        lm128 invDir[3],
        const s32 sign[3],
        const lm128 bbox[2][3]);

    //-----------------------------------------------------------
    // 線分とスラブ（平行な平面の組の間）の交差判定
    bool testRaySlab(f32& tmin, f32& tmax, const Ray& ray, f32 slabMin, f32 slabMax, s32 axis);
}

#endif //INC_LMATH_RAYTEST_H__

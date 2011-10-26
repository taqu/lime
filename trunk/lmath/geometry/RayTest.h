#ifndef INC_LMATH_RAYTEST_H__
#define INC_LMATH_RAYTEST_H__
/**
@file RayTest.h
@author t-sakai
@date 2009/12/21 create
*/
#include "../lmathcore.h"

namespace lmath
{
    class Ray;
    class Plane;
    class Sphere;
    class Vector3;


    //---------------------------------------------------------------------------------
    //---
    //--- SIMD使わないバージョン
    //---
    //---------------------------------------------------------------------------------
    bool testRayPlane_NoSIMD(f32& t, const Ray& ray, const Plane& plane);
    bool testRayPlane_NoSIMD(f64& t, const Ray& ray, const Plane& plane);

    //-----------------------------------------------------------
    bool testRaySphere_NoSIMD(f32& t, const Ray& ray, const Sphere& sphere);
    bool testRaySphere_NoSIMD(f64& t, const Ray& ray, const Sphere& sphere);


    //-----------------------------------------------------------
    bool testRayTriangle_NoSIMD(f32& t, f32& u, f32& v, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    bool testRayTriangle_NoSIMD(f64& t, f64& u, f64& v, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    //-----------------------------------------------------------
    bool testRayRectangle_NoSIMD(f32& t, const Ray& ray, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3);

    //-----------------------------------------------------------
    bool testRayAABB_NoSIMD(f32& tmin, f32& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax);

    bool testRayAABB_NoSIMD(f64& tmin, f64& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax);

    bool testRayAABB_NoSIMD(const Ray& ray, const Vector3& bmin, const Vector3& bmax);

    //-----------------------------------------------------------
    bool testRaySlab_NoSIMD(f32& tmin, f32& tmax, const Ray& ray, f32 slabMin, f32 slabMax, s32 axis);




    //---------------------------------------------------------------------------------
    //---
    //--- SSEバージョン
    //---
    //---------------------------------------------------------------------------------
    bool testRayPlane_SSE(f32& t, const Ray& ray, const Plane& plane);

    bool testRayPlane_SSE(f64& t, const Ray& ray, const Plane& plane);

    //-----------------------------------------------------------
    bool testRaySphere_SSE(f32& t, const Ray& ray, const Sphere& sphere);
    bool testRaySphere_SSE(f64& t, const Ray& ray, const Sphere& sphere);


    //-----------------------------------------------------------
    bool testRayTriangle_SSE(f32& t, f32& u, f32& v, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    bool testRayTriangle_SSE(f64& t, f64& u, f64& v, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2);


    //-----------------------------------------------------------
    bool testRayRectangle_SSE(f32& t, const Ray& ray, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3);


    //-----------------------------------------------------------
    bool testRayAABB_SSE(f32& tmin, f32& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax);

    bool testRayAABB_SSE(f64& tmin, f64& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax);

    bool testRayAABB_SSE(const Ray& ray, const Vector3& bmin, const Vector3& bmax);

    //-----------------------------------------------------------
    bool testRaySlab_SSE(f32& tmin, f32& tmax, const Ray& ray, f32 slabMin, f32 slabMax, s32 axis);



    //---------------------------------------------------------------------------------
    //---
    //--- インターフェイス
    //---
    //---------------------------------------------------------------------------------
    /**
    @brief 線分と平面の交差判定
    @return 交差するか
    @param t ... 交差点の陰関数パラメータ。交差点 = t*ray.direction_ + ray.origin_
    @param ray ... 
    @param plane ... 
    */
    inline bool testRayPlane(f32& t, const Ray& ray, const Plane& plane)
    {
#if defined(LMATH_USE_SSE2)
        return testRayPlane_NoSIMD(t, ray, plane);
#else
        return testRayPlane_NoSIMD(t, ray, plane);
#endif
    }

    inline bool testRayPlane(f64& t, const Ray& ray, const Plane& plane)
    {
#if defined(LMATH_USE_SSE2)
        return testRayPlane_NoSIMD(t, ray, plane);
#else
        return testRayPlane_NoSIMD(t, ray, plane);
#endif
    }

    //-----------------------------------------------------------
    /**
    @brief 線分と球の交差判定
    @return 交差するか
    @param t ... 交差点の陰関数パラメータ。交差点 = t*ray.direction_ + ray.origin_
    @param ray ... 
    @param sphere ... 
    */
    inline bool testRaySphere(f32& t, const Ray& ray, const Sphere& sphere)
    {
#if defined(LMATH_USE_SSE2)
        return testRaySphere_NoSIMD(t, ray, sphere);
#else
        return testRaySphere_NoSIMD(t, ray, sphere);
#endif
    }

    inline bool testRaySphere(f64& t, const Ray& ray, const Sphere& sphere)
    {
#if defined(LMATH_USE_SSE2)
        return testRaySphere_NoSIMD(t, ray, sphere);
#else
        return testRaySphere_NoSIMD(t, ray, sphere);
#endif
    }

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
    inline bool testRayTriangle(f32& t, f32& u, f32& v, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
#if defined(LMATH_USE_SSE2)
        return testRayTriangle_NoSIMD(t, u, v, ray, v0, v1, v2);
#else
        return testRayTriangle_NoSIMD(t, u, v, ray, v0, v1, v2);
#endif
    }

    inline bool testRayTriangle(f64& t, f64& u, f64& v, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
#if defined(LMATH_USE_SSE2)
        return testRayTriangle_NoSIMD(t, u, v, ray, v0, v1, v2);
#else
        return testRayTriangle_NoSIMD(t, u, v, ray, v0, v1, v2);
#endif
    }

    inline bool testRayRectangle(f32& t, const Ray& ray, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3)
    {
#if defined(LMATH_USE_SSE2)
        return testRayRectangle_NoSIMD(t, ray, p0, p1, p2, p3);
#else
        return testRayRectangle_NoSIMD(t, ray, p0, p1, p2, p3);
#endif
    }


    //-----------------------------------------------------------
    /**
    @brief 線分とAABBの交差判定
    @return 交差するか
    @param tmin ... 
    @param tmax ...
    @param ray ... 
    @param bmin ... 
    @param bmax ... 

    交差点の陰関数パラメータを最大最小返す。線線の開始点がAABB内部にあれば最小は0になる。
    */
    inline bool testRayAABB(f32& tmin, f32& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax)
    {
#if defined(LMATH_USE_SSE2)
        return testRayAABB_NoSIMD(tmin, tmax, ray, bmin, bmax);
#else
        return testRayAABB_NoSIMD(tmin, tmax, ray, bmin, bmax);
#endif
    }

    inline bool testRayAABB(f64& tmin, f64& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax)
    {
#if defined(LMATH_USE_SSE2)
        return testRayAABB_NoSIMD(tmin, tmax, ray, bmin, bmax);
#else
        return testRayAABB_NoSIMD(tmin, tmax, ray, bmin, bmax);
#endif
    }

    inline bool testRayAABB(const Ray& ray, const Vector3& bmin, const Vector3& bmax)
    {
#if defined(LMATH_USE_SSE2)
        return testRayAABB_NoSIMD(ray, bmin, bmax);
#else
        return testRayAABB_NoSIMD(ray, bmin, bmax);
#endif
    }

    //-----------------------------------------------------------
    // 線分とスラブ（平行な平面の組の間）の交差判定
    inline bool testRaySlab(f32& tmin, f32& tmax, const Ray& ray, f32 slabMin, f32 slabMax, s32 axis)
    {
#if defined(LMATH_USE_SSE2)
        return testRaySlab_NoSIMD(tmin, tmax, ray, slabMin, slabMax, axis);
#else
        return testRaySlab_NoSIMD(tmin, tmax, ray, slabMin, slabMax, axis);
#endif
    }
}

#endif //INC_LMATH_RAYTEST_H__

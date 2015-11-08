#ifndef INC_LRENDER_GEOMETRY_H__
#define INC_LRENDER_GEOMETRY_H__
/**
@file Geometry.h
@author t-sakai
@date 2015/10/07 create
*/
#include "../lrender.h"

namespace lrender
{
    class Ray;

    __m128 dot4vec(
        const __m128& vx0,
        const __m128& vy0,
        const __m128& vz0,
        const __m128& vx1,
        const __m128& vy1,
        const __m128& vz1);

    void cross4vec(
        __m128& vx,
        __m128& vy,
        __m128& vz,
        const __m128& vx0,
        const __m128& vy0,
        const __m128& vz0,
        const __m128& vx1,
        const __m128& vy1,
        const __m128& vz1);

    //-----------------------------------------------------------
    /**
    @brief üªÆOp`Ìð·»è
    @return ð··é©
    @param t ... ð·_ÌAÖp[^Bð·_ = t*ray.direction_ + ray.origin_
    @param ray ... 
    @param v0 ... 
    @param v1 ... 
    @param v2 ... 
    */
    bool testRayTriangle(f32& t, f32& v, f32& w, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    s32 testRayTriangle(
        __m128& t,
        __m128& u,
        __m128& v,
        const __m128 origin[3],
        const __m128 direction[3],
        const __m128 vx[3],
        const __m128 vy[3],
        const __m128 vz[3]);

    //-----------------------------------------------------------
    /**
    @brief üªÆAABBÌð·»è
    @return ð··é©
    @param tmin ... 
    @param tmax ...
    @param ray ... 
    @param bmin ... 
    @param bmax ... 

    ð·_ÌAÖp[^ðÅåÅ¬Ô·
    */
    bool testRayAABB(f32& tmin, f32& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax);

    bool testRayAABB(const Ray& ray, const Vector3& bmin, const Vector3& bmax);

    bool testRayAABB(f32& tmin, f32& tmax, const Ray& ray, const Vector4& bmin, const Vector4& bmax);

    //-----------------------------------------------------------
    // üªÆXui½sÈ½ÊÌgÌÔjÌð·»è
    bool testRaySlab(f32& tmin, f32& tmax, const Ray& ray, f32 slabMin, f32 slabMax, s32 axis);


    //-----------------------------------------------------------
    // üªÆAABBÌð·»è
    s32 testRayAABB(
        __m128 tmin,
        __m128 tmax,
        __m128 origin[3],
        __m128 invDir[3],
        const s32 sign[3],
        const __m128 bbox[2][3]);

    //AABBÌð·»è
    s32 testAABB(const __m128 bbox0[2][3], const __m128 bbox1[2][3]);

    //
    s32 testSphereAABB(const __m128 position[3], const __m128 radius, const __m128 bbox[2][3]);


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
#endif //INC_LRENDER_GEOMETRY_H__

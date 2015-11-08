/**
@file Geometry.cpp
@author t-sakai
@date 2015/10/07 create
*/
#include "Geometry.h"
#include "Ray.h"

namespace lrender
{
    __m128 dot4vec(
        const __m128& vx0,
        const __m128& vy0,
        const __m128& vz0,
        const __m128& vx1,
        const __m128& vy1,
        const __m128& vz1)
    {
        __m128 tx = _mm_mul_ps(vx0, vx1);
        __m128 ty = _mm_mul_ps(vy0, vy1);
        __m128 tz = _mm_mul_ps(vz0, vz1);

        return _mm_add_ps(_mm_add_ps(tx, ty), tz);
    }

    void cross4vec(
        __m128& vx,
        __m128& vy,
        __m128& vz,
        const __m128& vx0,
        const __m128& vy0,
        const __m128& vz0,
        const __m128& vx1,
        const __m128& vy1,
        const __m128& vz1)
    {
        __m128 tx = _mm_sub_ps(_mm_mul_ps(vy0, vz1), _mm_mul_ps(vz0, vy1));
        __m128 ty = _mm_sub_ps(_mm_mul_ps(vz0, vx1), _mm_mul_ps(vx0, vz1));
        __m128 tz = _mm_sub_ps(_mm_mul_ps(vx0, vy1), _mm_mul_ps(vy0, vx1));
        vx = tx;
        vy = ty;
        vz = tz;
    }

    //-----------------------------------------------------------
    // 線分と三角形の交差判定
    bool testRayTriangle(f32& t, f32& v, f32& w, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        Vector3 d0;
        d0.sub(v1, v0);

        Vector3 d1;
        d1.sub(v2, v0);

        Vector3 c;
        c.cross(ray.direction_, d1);

        f32 discr = c.dot(d0);

        Vector3 qvec;
        if(F32_EPSILON<discr){
            //表面
            Vector3 tvec;
            tvec.sub(ray.origin_, v0);
            v = tvec.dot(c);
            if(v<0.0f || discr<v){
                return false;
            }

            qvec.cross(tvec, d0);
            w = qvec.dot(ray.direction_);
            if(w<0.0f || discr<(v+w)){
                return false;
            }

        //}else if(discr < -F32_EPSILON){
        //    //裏面
        //    Vector3 tvec;
        //    tvec.sub(ray.origin_, v0);
        //    u = tvec.dot(c);
        //    if(u>0.0f || discr<u){
        //        return false;
        //    }

        //    qvec.cross(tvec, d0);
        //    v = qvec.dot(ray.direction_);
        //    if(0.0f<v || (u+v)<discr){
        //        return false;
        //    }

        }else{
            return false;
        }

        f32 invDiscr = 1.0f/discr;

        t = d1.dot(qvec);
        t *= invDiscr;
        v *= invDiscr;
        w *= invDiscr;
        return true;
    }

    //-----------------------------------------------------------
    // 線分と三角形の交差判定
    s32 testRayTriangle(
        __m128& t,
        __m128& u,
        __m128& v,
        const __m128 origin[3],
        const __m128 direction[3],
        const __m128 vx[3],
        const __m128 vy[3],
        const __m128 vz[3])
    {
        __m128 dx10 = _mm_sub_ps(vx[1], vx[0]);
        __m128 dy10 = _mm_sub_ps(vy[1], vy[0]);
        __m128 dz10 = _mm_sub_ps(vz[1], vz[0]);

        __m128 dx20 = _mm_sub_ps(vx[2], vx[0]);
        __m128 dy20 = _mm_sub_ps(vy[2], vy[0]);
        __m128 dz20 = _mm_sub_ps(vz[2], vz[0]);

        __m128 cx, cy, cz;
        cross4vec(
            cx, cy, cz,
            direction[0], direction[1], direction[2],
            dx20, dy20, dz20);

        __m128 epsilon = _mm_set1_ps(F32_EPSILON);
        __m128 zero = _mm_setzero_ps();

        __m128 discr = dot4vec(cx, cy, cz, dx10, dy10, dz10);
        __m128 disc0 = _mm_cmplt_ps(epsilon, discr);

        //表面判定
        __m128 tvecx = _mm_sub_ps(origin[0], vx[0]);
        __m128 tvecy = _mm_sub_ps(origin[1], vy[0]);
        __m128 tvecz = _mm_sub_ps(origin[2], vz[0]);
        u = dot4vec(cx, cy, cz, tvecx, tvecy, tvecz);
        __m128 disc1 = _mm_and_ps(_mm_cmple_ps(zero, u), _mm_cmple_ps(u, discr));

        __m128 qvecx, qvecy, qvecz;
        cross4vec(qvecx, qvecy, qvecz, tvecx, tvecy, tvecz, dx10, dy10, dz10);
        v = dot4vec(direction[0], direction[1], direction[2], qvecx, qvecy, qvecz);
        __m128 disc2 = _mm_and_ps(_mm_cmple_ps(zero, v), _mm_cmple_ps(_mm_add_ps(u, v), discr));

        __m128 invDiscr = _mm_div_ps(_mm_set1_ps(1.0f), discr);

        t = dot4vec(dx20, dy20, dz20, qvecx, qvecy, qvecz);
        t = _mm_mul_ps(t, invDiscr);
        u = _mm_mul_ps(u, invDiscr);
        v = _mm_mul_ps(v, invDiscr);
        return _mm_movemask_ps(_mm_and_ps(disc0, _mm_and_ps(disc1, disc2)));
    }

    //-----------------------------------------------------------
    // 線分とAABBの交差判定
    bool testRayAABB(f32& tmin, f32& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax)
    {
        tmin = ray.tmin_;
        tmax = ray.tmax_;

        for(s32 i=0; i<3; ++i){
            if(lcore::absolute(ray.direction_[i])<F32_EPSILON){
                //線分とスラブが平行で、原点がスラブの中にない
                if(ray.origin_[i]<bmin[i] || bmax[i]<ray.origin_[i]){
                    return false;
                }

            }else{
                f32 invD = ray.invDirection_[i];
                f32 t1 = (bmin[i] - ray.origin_[i]) * invD;
                f32 t2 = (bmax[i] - ray.origin_[i]) * invD;

                if(t1>t2){
                    if(t2>tmin) tmin = t2;
                    if(t1<tmax) tmax = t1;
                }else{
                    if(t1>tmin) tmin = t1;
                    if(t2<tmax) tmax = t2;
                }

                if(tmin > tmax){
                    return false;
                }
                if(tmax < 0.0f){
                    return false;
                }
            }
        }
        return true;
    }


    //-----------------------------------------------------------
    bool testRayAABB(const Ray& ray, const Vector3& bmin, const Vector3& bmax)
    {
        //AABB半分
        Vector3 e = bmax;
        e -= bmin;


        //線分の始点から終点へのベクトル
        Vector3 d;
        d.mul(ray.tmax_, ray.direction_);

        //線分の中点
        Vector3 m;
        m.mul(ray.tmin_, ray.direction_);
        m += ray.origin_;
        m -= bmin;
        m -= bmax;

        //
        f32 adx = lcore::absolute(d.x_);
        if(lcore::absolute(m.x_) > e.x_ + adx) return false;

        f32 ady = lcore::absolute(d.y_);
        if(lcore::absolute(m.y_) > e.y_ + ady) return false;

        f32 adz = lcore::absolute(d.z_);
        if(lcore::absolute(m.z_) > e.z_ + adz) return false;

        adx += F32_EPSILON;
        ady += F32_EPSILON;
        adz += F32_EPSILON;

        if(lcore::absolute(m.y_ * d.z_ - m.z_ * d.y_) > (e.y_ * adz + e.z_ * ady)) return false;
        if(lcore::absolute(m.z_ * d.x_ - m.x_ * d.z_) > (e.x_ * adz + e.z_ * adx)) return false;
        if(lcore::absolute(m.x_ * d.y_ - m.y_ * d.x_) > (e.x_ * ady + e.y_ * adx)) return false;

        return true;
    }

    bool testRayAABB(f32& tmin, f32& tmax, const Ray& ray, const Vector4& bmin, const Vector4& bmax)
    {
        Vector3 bmin3(bmin);
        Vector3 bmax3(bmax);
        return testRayAABB(tmin, tmax, ray, bmin3, bmax3);
    }

    bool testRayAABB(const Ray& ray, const Vector4& bmin, const Vector4& bmax)
    {
        Vector3 bmin3(bmin);
        Vector3 bmax3(bmax);
        return testRayAABB(ray, bmin3, bmax3);
    }

    //-----------------------------------------------------------
    // 線分とスラブの交差判定
    bool testRaySlab(f32& tmin, f32& tmax, const Ray& ray, f32 slabMin, f32 slabMax, s32 axis)
    {
        f32 d = ray.invDirection_[axis];
        tmin = (slabMin - ray.origin_[axis]) * d;
        tmax = (slabMax - ray.origin_[axis]) * d;

        if(tmin > tmax){
            lcore::swap(tmin, tmax);
        }

        if(tmin < 0.0f){
            if(tmax < 0.0f){
                return false;
            }
            tmin = 0.0f;
        }else{
            if(ray.tmax_ < tmin){
                return false;
            }
        }
        return true;
    }

    //-----------------------------------------------------------
    // 線分とAABBの交差判定
    s32 testRayAABB(
        __m128 tmin,
        __m128 tmax,
        __m128 origin[3],
        __m128 invDir[3],
        const s32 sign[3],
        const __m128 bbox[2][3])
    {
        for(s32 i=0; i<3; ++i){
            tmin = _mm_max_ps(
                tmin,
                _mm_mul_ps(_mm_sub_ps(bbox[sign[i]][i], origin[i]), invDir[i]));

            tmax = _mm_min_ps(
                tmax,
                _mm_mul_ps(_mm_sub_ps(bbox[1-sign[i]][i], origin[i]), invDir[i]));
        }

        return _mm_movemask_ps(_mm_cmpge_ps(tmax, tmin));
    }

    //AABBの交差判定
    s32 testAABB(const __m128 bbox0[2][3], const __m128 bbox1[2][3])
    {
        u32 mask = 0xFFFFFFFFU;
        f32 fmask = *((f32*)&mask);

        __m128 t = _mm_set1_ps(fmask);
        for(s32 i=0; i<3; ++i){
            t = _mm_and_ps(t, _mm_cmple_ps(bbox0[0][i], bbox1[1][i]));
            t = _mm_and_ps(t, _mm_cmple_ps(bbox1[0][i], bbox0[1][i]));
        }
        return _mm_movemask_ps(t);
    }

    //
    s32 testSphereAABB(const __m128 position[3], const __m128 radius, const __m128 bbox[2][3])
    {
        u32 mask = 0xFFFFFFFFU;
        f32 fmask = *((f32*)&mask);

        __m128 tbbox[2][3];
        for(s32 i=0; i<3; ++i){
            tbbox[0][i] = _mm_sub_ps(bbox[0][i], radius);
            tbbox[1][i] = _mm_add_ps(bbox[1][i], radius);
        }
        __m128 t = _mm_set1_ps(fmask);
        for(s32 i=0; i<3; ++i){
            t = _mm_and_ps(t, _mm_cmple_ps(position[i], tbbox[1][i]));
            t = _mm_and_ps(t, _mm_cmple_ps(tbbox[0][i], position[i]));
        }
        return _mm_movemask_ps(t);
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

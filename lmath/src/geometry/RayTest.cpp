/**
@file RayTest.cpp
@author t-sakai
@date 2009/12/21
*/
#include "lmath.h"
#include "Vector3.h"
#include "geometry/Plane.h"
#include "geometry/Sphere.h"
#include "geometry/Ray.h"
#include "geometry/RayTest.h"

namespace lmath
{
    //-----------------------------------------------------------
    lm128 dot4vec(
        const lm128& vx0,
        const lm128& vy0,
        const lm128& vz0,
        const lm128& vx1,
        const lm128& vy1,
        const lm128& vz1)
    {
        lm128 tx = _mm_mul_ps(vx0, vx1);
        lm128 ty = _mm_mul_ps(vy0, vy1);
        lm128 tz = _mm_mul_ps(vz0, vz1);

        return _mm_add_ps(_mm_add_ps(tx, ty), tz);
    }

    void cross4vec(
        lm128& vx,
        lm128& vy,
        lm128& vz,
        const lm128& vx0,
        const lm128& vy0,
        const lm128& vz0,
        const lm128& vx1,
        const lm128& vy1,
        const lm128& vz1)
    {
        lm128 tx = _mm_sub_ps(_mm_mul_ps(vy0, vz1), _mm_mul_ps(vz0, vy1));
        lm128 ty = _mm_sub_ps(_mm_mul_ps(vz0, vx1), _mm_mul_ps(vx0, vz1));
        lm128 tz = _mm_sub_ps(_mm_mul_ps(vx0, vy1), _mm_mul_ps(vy0, vx1));
        vx = tx;
        vy = ty;
        vz = tz;
    }

    //-----------------------------------------------------------
    // 線分と平面の交差判定
    bool testRayPlane(f32& t, const Ray& ray, const Plane& plane)
    {
        // 0割り、0/0が起きた場合は、条件分岐で交差していないと判定される(IEEE754準拠
        lmath::Vector3 n = plane.normal();

        t = plane.d() - dot(n, ray.origin_);
        t /= dot(n, ray.direction_);

        // INFとの比較は正常に行われる。NaNと数値の判定は常に偽(IEEE754準拠
        return (0.0f<=t && t<=ray.t_);
    }


    //-----------------------------------------------------------
    // 線分と球の交差判定
    bool testRaySphere(f32& t, const Ray& ray, const Sphere& sphere)
    {
        Vector3 m = sub(ray.origin_, sphere.position());

        f32 b = dot(m, ray.direction_);
        f32 c = dot(m, m) - sphere.radius() * sphere.radius();

        // 線分の起点が球の外で、向きが球の方向と逆
        if(0.0f<c){
            if(0.0f<b){// 線分の起点が球の外で、向きが球の方向と逆
                return false;
            }
        }else{
            t = 0.0f;
            return true;
        }

        f32 discr = b*b - c; //判別式
        if(discr < 0.0f){
            return false;
        }

        discr = lmath::sqrt(discr);
        b = -b;
        t = b-discr;
        f32 tmax = b + discr;
        return (tmax<=ray.t_);
    }

    //-----------------------------------------------------------
    // 線分と球の交差判定
    bool testRaySphere(f32& tmin, f32& tmax, const Ray& ray, const Sphere& sphere)
    {
        Vector3 m = sub(ray.origin_, sphere.position());

        f32 b = dot(m, ray.direction_);
        f32 c = dot(m, m) - sphere.radius() * sphere.radius();

        // 線分の起点が球の外で、向きが球の方向と逆
        if(0.0f<c && 0.0f<b){
            return false;
        }

        f32 discr = b*b - c; //判別式
        if(discr < 0.0f){
            return false;
        }

        discr = lmath::sqrt(discr);
        b = -b;

        tmin = b - discr;
        tmax = b + discr;
        return (c<=0.0f)? true : (tmax<=ray.t_);
    }


    //-----------------------------------------------------------
    // 線分と球表面の交差判定
    bool testRaySphereSurface(f32& t, const Ray& ray, const Sphere& sphere)
    {
        Vector3 m = sub(ray.origin_, sphere.position());

        f32 b = dot(m, ray.direction_);
        f32 c = dot(m, m) - sphere.radius() * sphere.radius();

        // 線分の起点が球の外で、向きが球の方向と逆
        if(0.0f<c){
            if(0.0f<b){// 線分の起点が球の外で、向きが球の方向と逆
                return false;
            }
        }

        f32 discr = b*b - c; //判別式
        if(discr < 0.0f){
            return false;
        }

        discr = lmath::sqrt(discr);
        b = -b;
        t = b+discr;
        return (t<=ray.t_);
    }

    //-----------------------------------------------------------
#define LMATH_TEST_RAY_TRIAGNLE_HELPER_FRONT(ret_fail) \
    Vector3 tvec = ray.origin_-v0;\
    v = dot(tvec, c);\
    if(v<0.0f || discr<v){\
        return ret_fail;\
    }\
    qvec = cross(tvec, d0);\
    w = dot(qvec, ray.direction_);\
    if(w<0.0f || discr<(v+w)){\
        return ret_fail;\
    }\

#define LMATH_TEST_RAY_TRIAGNLE_HELPER_BACK(ret_fail) \
    Vector3 tvec = ray.origin_-v0;\
    v = dot(tvec, c);\
    if(0.0f<v || v<discr){\
        return ret_fail;\
    }\
    qvec = cross(tvec, d0);\
    w = dot(qvec, ray.direction_);\
    if(0.0f<w || (v+w)<discr){\
        return ret_fail;\
    }\

    // 線分と三角形の交差判定
    bool testRayTriangleFront(f32& t, f32& v, f32& w, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        Vector3 d0 = sub(v1, v0);
        Vector3 d1 = sub(v2, v0);

        Vector3 c = cross(ray.direction_, d1);

        f32 discr = dot(c, d0);

        Vector3 qvec;
        if(LMATH_F32_EPSILON < discr){
            //表
            LMATH_TEST_RAY_TRIAGNLE_HELPER_FRONT(false)

        }else {
            return false;
        }

        f32 invDiscr = 1.0f/discr;

        t = dot(d1, qvec);
        t *= invDiscr;
        v *= invDiscr;
        w *= invDiscr;
        return true;
    }

    bool testRayTriangleBack(f32& t, f32& v, f32& w, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        Vector3 d0 = v1-v0;
        Vector3 d1 = v2-v0;
        Vector3 c = cross(ray.direction_, d1);

        f32 discr = dot(c, d0);
        Vector3 qvec;
        if(discr < -LMATH_F32_EPSILON){
            //裏面
            LMATH_TEST_RAY_TRIAGNLE_HELPER_BACK(false)

        }else{
            return false;
        }

        f32 invDiscr = 1.0f/discr;

        t = dot(d1, qvec);
        t *= invDiscr;
        v *= invDiscr;
        w *= invDiscr;
        return true;
    }

    // 線分と三角形の交差判定
    Result testRayTriangleBoth(f32& t, f32& v, f32& w, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        Vector3 d0 = v1-v0;
        Vector3 d1 = v2-v0;
        Vector3 c = cross(ray.direction_, d1);

        Result result = Result_Fail;
        f32 discr = dot(c, d0);
        Vector3 qvec;
        if(LMATH_F32_EPSILON<discr){
            //表面
            result = Result_Front;
            LMATH_TEST_RAY_TRIAGNLE_HELPER_FRONT(Result_Fail)

        }else if(discr < -LMATH_F32_EPSILON){
            //裏面
            result = Result_Back;
            LMATH_TEST_RAY_TRIAGNLE_HELPER_BACK(Result_Fail)

        }else{
            return Result_Fail;
        }

        f32 invDiscr = 1.0f/discr;

        t = dot(d1, qvec);
        t *= invDiscr;
        v *= invDiscr;
        w *= invDiscr;
        return (Result)(Result_Success | result);
    }


    //-----------------------------------------------------------
    // 線分と三角形の交差判定
    bool testRayTriangleFront(f32& u, f32& v, f32& w, const Vector3& startPoint, const Vector3& endPoint, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        Vector3 d = sub(endPoint, startPoint);
        Vector3 d0 = sub(v0, startPoint);
        Vector3 d1 = sub(v1, startPoint);
        Vector3 d2 = sub(v2, startPoint);

        Vector3 m = cross(d, d2);
        u = dot(d1, m);
        if(u<0.0f){
            return false;
        }

        v = -dot(d0, m);
        if(v<0.0f){
            return false;
        }

        m = cross(d, d1);
        w = dot(d0, m);
        if(w<0.0f){
            return false;
        }

        f32 denom = 1.0f/(u+v+w);
        u *= denom;
        v *= denom;
        w = 1.0f - u - v;
        return true;
    }

    bool testRayTriangleBack(f32& u, f32& v, f32& w, const Vector3& startPoint, const Vector3& endPoint, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        Vector3 d = sub(endPoint, startPoint);
        Vector3 d0 = sub(v0, startPoint);
        Vector3 d1 = sub(v1, startPoint);
        Vector3 d2 = sub(v2, startPoint);

        Vector3 m = cross(d, d2);
        u = dot(d1, m);
        if(0.0f<u){
            return false;
        }

        v = -dot(d0, m);
        if(0.0f<v){
            return false;
        }

        m = cross(d, d1);
        w = dot(d0, m);
        if(0.0f<w){
            return false;
        }

        f32 denom = 1.0f/(u+v+w);
        u *= denom;
        v *= denom;
        w = 1.0f - u - v;
        return true;
    }

    Result testRayTriangleBoth(f32& u, f32& v, f32& w, const Vector3& startPoint, const Vector3& endPoint, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        Vector3 d = sub(endPoint, startPoint);
        Vector3 d0 = sub(v0, startPoint);
        Vector3 d1 = sub(v1, startPoint);
        Vector3 d2 = sub(v2, startPoint);

        Vector3 m = cross(d, d2);
        u = dot(d1, m);
        v = -dot(d0, m);
        if(!lcore::isSameSign(u,v)){
            return Result_Fail;
        }

        m = cross(d, d1);
        w = dot(d0, m);
        if(!lcore::isSameSign(u,w)){
            return Result_Fail;
        }

        Result result = (0.0f<=u)? Result_Front : Result_Back;
        f32 denom = 1.0f/(u+v+w);
        u *= denom;
        v *= denom;
        w = 1.0f - u - v;
        return result;
    }

    s32 testRayTriangleFront(
        lm128& t,
        lm128& v,
        lm128& w,
        const lm128 origin[3],
        const lm128 direction[3],
        const lm128 vx[3],
        const lm128 vy[3],
        const lm128 vz[3])
    {
        lm128 dx10 = _mm_sub_ps(vx[1], vx[0]);
        lm128 dy10 = _mm_sub_ps(vy[1], vy[0]);
        lm128 dz10 = _mm_sub_ps(vz[1], vz[0]);

        lm128 dx20 = _mm_sub_ps(vx[2], vx[0]);
        lm128 dy20 = _mm_sub_ps(vy[2], vy[0]);
        lm128 dz20 = _mm_sub_ps(vz[2], vz[0]);

        lm128 cx, cy, cz;
        cross4vec(
            cx, cy, cz,
            direction[0], direction[1], direction[2],
            dx20, dy20, dz20);

        lm128 zero = _mm_setzero_ps();

        lm128 discr = dot4vec(cx, cy, cz, dx10, dy10, dz10);
        //表面判定
        lm128 disc0 = _mm_cmplt_ps(_mm_set1_ps(LMATH_F32_EPSILON), discr);

        lm128 tvecx = _mm_sub_ps(origin[0], vx[0]);
        lm128 tvecy = _mm_sub_ps(origin[1], vy[0]);
        lm128 tvecz = _mm_sub_ps(origin[2], vz[0]);
        v = dot4vec(cx, cy, cz, tvecx, tvecy, tvecz);
        lm128 disc1 = _mm_and_ps(_mm_cmple_ps(zero, v), _mm_cmple_ps(v, discr));

        lm128 qvecx, qvecy, qvecz;
        cross4vec(qvecx, qvecy, qvecz, tvecx, tvecy, tvecz, dx10, dy10, dz10);
        w = dot4vec(direction[0], direction[1], direction[2], qvecx, qvecy, qvecz);
        lm128 disc2 = _mm_and_ps(_mm_cmple_ps(zero, w), _mm_cmple_ps(_mm_add_ps(v, w), discr));

        lm128 invDiscr = _mm_div_ps(_mm_set1_ps(1.0f), discr);

        t = dot4vec(dx20, dy20, dz20, qvecx, qvecy, qvecz);
        t = _mm_mul_ps(t, invDiscr);
        v = _mm_mul_ps(v, invDiscr);
        w = _mm_mul_ps(w, invDiscr);
        return _mm_movemask_ps(_mm_and_ps(disc0, _mm_and_ps(disc1, disc2)));
    }

    s32 testRayTriangleBack(
        lm128& t,
        lm128& v,
        lm128& w,
        const lm128 origin[3],
        const lm128 direction[3],
        const lm128 vx[3],
        const lm128 vy[3],
        const lm128 vz[3])
    {
        lm128 dx10 = _mm_sub_ps(vx[1], vx[0]);
        lm128 dy10 = _mm_sub_ps(vy[1], vy[0]);
        lm128 dz10 = _mm_sub_ps(vz[1], vz[0]);

        lm128 dx20 = _mm_sub_ps(vx[2], vx[0]);
        lm128 dy20 = _mm_sub_ps(vy[2], vy[0]);
        lm128 dz20 = _mm_sub_ps(vz[2], vz[0]);

        lm128 cx, cy, cz;
        cross4vec(
            cx, cy, cz,
            direction[0], direction[1], direction[2],
            dx20, dy20, dz20);

        lm128 zero = _mm_setzero_ps();

        lm128 discr = dot4vec(cx, cy, cz, dx10, dy10, dz10);
        //裏面判定
        lm128 disc0 = _mm_cmplt_ps(discr, _mm_set1_ps(-LMATH_F32_EPSILON));

        lm128 tvecx = _mm_sub_ps(origin[0], vx[0]);
        lm128 tvecy = _mm_sub_ps(origin[1], vy[0]);
        lm128 tvecz = _mm_sub_ps(origin[2], vz[0]);
        v = dot4vec(cx, cy, cz, tvecx, tvecy, tvecz);
        lm128 disc1 = _mm_and_ps(_mm_cmple_ps(v, zero), _mm_cmple_ps(discr, v));

        lm128 qvecx, qvecy, qvecz;
        cross4vec(qvecx, qvecy, qvecz, tvecx, tvecy, tvecz, dx10, dy10, dz10);
        w = dot4vec(direction[0], direction[1], direction[2], qvecx, qvecy, qvecz);
        lm128 disc2 = _mm_and_ps(_mm_cmple_ps(w, zero), _mm_cmple_ps(discr, _mm_add_ps(v, w)));

        lm128 invDiscr = _mm_div_ps(_mm_set1_ps(1.0f), discr);

        t = dot4vec(dx20, dy20, dz20, qvecx, qvecy, qvecz);
        t = _mm_mul_ps(t, invDiscr);
        v = _mm_mul_ps(v, invDiscr);
        w = _mm_mul_ps(w, invDiscr);
        return _mm_movemask_ps(_mm_and_ps(disc0, _mm_and_ps(disc1, disc2)));
    }

    s32 testRayTriangleBoth(
        lm128& t,
        lm128& v,
        lm128& w,
        const lm128 origin[3],
        const lm128 direction[3],
        const lm128 vx[3],
        const lm128 vy[3],
        const lm128 vz[3])
    {
        lm128 dx10 = _mm_sub_ps(vx[1], vx[0]);
        lm128 dy10 = _mm_sub_ps(vy[1], vy[0]);
        lm128 dz10 = _mm_sub_ps(vz[1], vz[0]);

        lm128 dx20 = _mm_sub_ps(vx[2], vx[0]);
        lm128 dy20 = _mm_sub_ps(vy[2], vy[0]);
        lm128 dz20 = _mm_sub_ps(vz[2], vz[0]);

        lm128 cx, cy, cz;
        cross4vec(
            cx, cy, cz,
            direction[0], direction[1], direction[2],
            dx20, dy20, dz20);

        lm128 zero = _mm_setzero_ps();

        lm128 discr = dot4vec(cx, cy, cz, dx10, dy10, dz10);

        //表面判定
        lm128 f_disc0 = _mm_cmplt_ps(_mm_set1_ps(LMATH_F32_EPSILON), discr);

        lm128 tvecx = _mm_sub_ps(origin[0], vx[0]);
        lm128 tvecy = _mm_sub_ps(origin[1], vy[0]);
        lm128 tvecz = _mm_sub_ps(origin[2], vz[0]);
        v = dot4vec(cx, cy, cz, tvecx, tvecy, tvecz);
        lm128 f_disc1 = _mm_and_ps(_mm_cmple_ps(zero, v), _mm_cmple_ps(v, discr));

        lm128 qvecx, qvecy, qvecz;
        cross4vec(qvecx, qvecy, qvecz, tvecx, tvecy, tvecz, dx10, dy10, dz10);
        w = dot4vec(direction[0], direction[1], direction[2], qvecx, qvecy, qvecz);
        lm128 f_disc2 = _mm_and_ps(_mm_cmple_ps(zero, w), _mm_cmple_ps(_mm_add_ps(v, w), discr));
        lm128 f_mask = _mm_and_ps(f_disc0, _mm_and_ps(f_disc1, f_disc2));

        //裏面判定
        lm128 b_disc0 = _mm_cmplt_ps(discr, _mm_set1_ps(-LMATH_F32_EPSILON));
        lm128 b_disc1 = _mm_and_ps(_mm_cmple_ps(v, zero), _mm_cmple_ps(discr, v));
        lm128 b_disc2 = _mm_and_ps(_mm_cmple_ps(w, zero), _mm_cmple_ps(discr, _mm_add_ps(v, w)));
        lm128 b_mask = _mm_and_ps(b_disc0, _mm_and_ps(b_disc1, b_disc2));

        lm128i three = _mm_set1_epi32(3);
        lm128i five = _mm_set1_epi32(5);
        lm128i vret = _mm_or_si128(_mm_and_si128(_mm_castps_si128(f_mask), three), _mm_and_si128(_mm_castps_si128(b_mask), five));

        lm128 invDiscr = _mm_div_ps(_mm_set1_ps(1.0f), discr);

        t = dot4vec(dx20, dy20, dz20, qvecx, qvecy, qvecz);
        t = _mm_mul_ps(t, invDiscr);
        v = _mm_mul_ps(v, invDiscr);
        w = _mm_mul_ps(w, invDiscr);

        LALIGN16 s32 side[4];
        _mm_store_si128((lm128i*)side, vret);
        return (side[0]<<0) | (side[1]<<8) | (side[2]<<16) | (side[3]<<24);
    }

    //-----------------------------------------------------------
    bool testRayRectangle(f32& t, const Ray& ray, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3)
    {
        Vector3 d0 = p0;
        d0 -= ray.origin_;

        Vector3 d1 = p1;
        d1 -= ray.origin_;

        Vector3 d2 = p2;
        d2 -= ray.origin_;

        Vector3 m = cross(d2, ray.direction_);
        f32 v = dot(d0, m);

        //TODO:高速化
        f32 tu,tv;
        if(v>=0.0f){

            return testRayTriangleFront(t, tu, tv, ray, p0, p1, p2);
        }else{
            return testRayTriangleFront(t, tu, tv, ray, p3, p0, p2);
        }
    }

    //-----------------------------------------------------------
    // 線分とAABBの交差判定
    bool testRayAABB(f32& tmin, f32& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax)
    {
        tmin = 0.0f;
        tmax = ray.t_;

        for(s32 i=0; i<3; ++i){
            if(lcore::absolute(ray.direction_[i])<LMATH_F32_EPSILON){
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
        Vector3 d = mul(ray.t_, ray.direction_);

        //線分の中点
        Vector3 m = add(d, ray.origin_);
        m -= bmin;
        m -= bmax;

        //
        f32 adx = lcore::absolute(d.x_);
        if(lcore::absolute(m.x_) > e.x_ + adx) return false;

        f32 ady = lcore::absolute(d.y_);
        if(lcore::absolute(m.y_) > e.y_ + ady) return false;

        f32 adz = lcore::absolute(d.z_);
        if(lcore::absolute(m.z_) > e.z_ + adz) return false;

        adx += LMATH_F32_EPSILON;
        ady += LMATH_F32_EPSILON;
        adz += LMATH_F32_EPSILON;

        if(lcore::absolute(m.y_ * d.z_ - m.z_ * d.y_) > (e.y_ * adz + e.z_ * ady)) return false;
        if(lcore::absolute(m.z_ * d.x_ - m.x_ * d.z_) > (e.x_ * adz + e.z_ * adx)) return false;
        if(lcore::absolute(m.x_ * d.y_ - m.y_ * d.x_) > (e.x_ * ady + e.y_ * adx)) return false;

        return true;
    }

    bool testRayAABB(f32& tmin, f32& tmax, const Ray& ray, const Vector4& bmin, const Vector4& bmax)
    {
        Vector3 bmin3 = Vector3::construct(bmin);
        Vector3 bmax3 = Vector3::construct(bmax);
        return testRayAABB(tmin, tmax, ray, bmin3, bmax3);
    }

    bool testRayAABB(const Ray& ray, const Vector4& bmin, const Vector4& bmax)
    {
        Vector3 bmin3 = Vector3::construct(bmin);
        Vector3 bmax3 = Vector3::construct(bmax);
        return testRayAABB(ray, bmin3, bmax3);
    }

    s32 testRayAABB(
        lm128 tmin,
        lm128 tmax,
        lm128 origin[3],
        lm128 invDir[3],
        const s32 sign[3],
        const lm128 bbox[2][3])
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

    //-----------------------------------------------------------
    // 線分とスラブの交差判定
    bool testRaySlab(f32& tmin, f32& tmax, const Ray& ray, f32 slabMin, f32 slabMax, s32 axis)
    {
#if 0
        if(isEqual(ray.direction_[axis], 0.0f)){
            //線分とスラブが平行。線分原点がスラブ内になければ失敗
            if(ray.origin_[axis]<slabMin || slabMax<ray.origin_[axis]){
                return false;
            }
        }else{
            f32 d = 1.0f/ray.direction_[axis];
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
                if(ray.t_ < tmin){
                    return false;
                }
            }
        }
#else
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
            if(ray.t_ < tmin){
                return false;
            }
        }
#endif
        return true;
    }
}

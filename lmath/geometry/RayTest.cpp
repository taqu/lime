/**
@file RayTest.cpp
@author t-sakai
@date 2009/12/21
*/
#include "../lmathcore.h"
#include "../Vector3.h"
#include "Plane.h"
#include "Sphere.h"
#include "Ray.h"
#include "RayTest.h"

namespace lmath
{
    // 線分と平面の交差判定
    bool testRayPlane(f32& t, const Ray& ray, const Plane& plane)
    {
        // 0割り、0/0が起きた場合は、条件分岐で交差していないと判定される(IEEE754準拠
        lmath::Vector3 n;
        plane.getNormal(n);

        f32 t0 = plane.getD() - n.dot(ray.origin_);
        t0 /= n.dot(ray.direction_);

        // INFとの比較は正常に行われる。NaNと数値の判定は常に偽(IEEE754準拠
        if(t0>=0.0 && t0<=ray.t_){
            t = t0;
            return true;
        }
        return false;
    }


    //-----------------------------------------------------------
    // 線分と球の交差判定
    bool testRaySphere(f32& t, const Ray& ray, const Sphere& sphere)
    {
        Vector3 m;
        sphere.getPosition(m);
        m.sub(ray.origin_, m);


        f32 b = m.dot(ray.direction_);
        f32 c = m.dot(m) - sphere.getRadius() * sphere.getRadius();

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
        Vector3 m;
        sphere.getPosition(m);
        m.sub(ray.origin_, m);


        f32 b = m.dot(ray.direction_);
        f32 c = m.dot(m) - sphere.getRadius() * sphere.getRadius();

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
    // 線分と三角形の交差判定
    bool testRayTriangle(f32& t, f32& u, f32& v, const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        Vector3 d0 = v1;
        d0 -= v0;

        Vector3 d1 = v2;
        d1 -= v0;

        Vector3 c;
        c.cross(ray.direction_, d1);

        f32 discr = c.dot(d0);

        Vector3 qvec;
        if(discr > F32_EPSILON){
            Vector3 tvec = ray.origin_;
            tvec -= v0;
            u = tvec.dot(c);
            if(u<0.0f || u>discr){
                return false;
            }

            qvec.cross(tvec, d0);
            v = qvec.dot(ray.direction_);
            if(v<0.0f || u+v>discr){
                return false;
            }

        }else if(discr < -F32_EPSILON){
            Vector3 tvec = ray.origin_;
            tvec -= v0;
            u = tvec.dot(c);
            if(u>0.0f || u<discr){
                return false;
            }

            qvec.cross(tvec, d0);
            v = qvec.dot(ray.direction_);
            if(v>0.0f || u+v<discr){
                return false;
            }

        }else{
            return false;
        }

        f32 invDiscr = 1.0f/discr;

        t = d1.dot(qvec);
        t *= invDiscr;
        u *= invDiscr;
        v *= invDiscr;
        return true;
    }


    bool testRayRectangle(f32& t, const Ray& ray, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3)
    {
        Vector3 d0 = p0;
        d0 -= ray.origin_;

        Vector3 d1 = p1;
        d1 -= ray.origin_;

        Vector3 d2 = p2;
        d2 -= ray.origin_;

        Vector3 m;
        m.cross(d2, ray.direction_);
        f32 v = d0.dot(m);

        //TODO:高速化
        f32 tu,tv;
        if(v>=0.0f){
            
            return testRayTriangle(t, tu, tv, ray, p0, p1, p2);
        }else{
            return testRayTriangle(t, tu, tv, ray, p3, p0, p2);
        }
    }

    //-----------------------------------------------------------
    // 線分とAABBの交差判定
    bool testRayAABB(f32& tmin, f32& tmax, const Ray& ray, const Vector3& bmin, const Vector3& bmax)
    {
        tmin = 0.0f;
        tmax = ray.t_;

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
        Vector3 d = ray.direction_;
        d *= ray.t_;

        //線分の中点
        Vector3 m = d;
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

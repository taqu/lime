/**
@file Sphere.cpp
@author t-sakai
@date 2011/11/04 create
*/
#include "geometry/Sphere.h"

namespace lmath
{
    namespace
    {
        class MiniSphere
        {
        public:
            MiniSphere(u32 numPoints, const Vector3** points)
                :numPoints_(numPoints)
                ,points_(points)
            {}

            void recurse(u32 index, u32 numPoints, u32 numSos);

            u32 numPoints_;
            const Vector3** points_;

            Sphere sphere_;
        };

        /**
        @param
        @param
        @param numSos ... 0-4のサポート点。球の境界を決める4つの点
        */
        void MiniSphere::recurse(u32 index, u32 numPoints, u32 numSos)
        {
            switch(numSos)
            {
            case 0:
                sphere_ = Sphere(0.0f, 0.0f, 0.0f, 0.0f);
                break;
            case 1:
                sphere_ = Sphere(*points_[index-1], 0.0f);
                break;
            case 2:
                sphere_ = Sphere::circumscribed(*points_[index-1], *points_[index-2]);
                break;
            case 3:
                sphere_ = Sphere::circumscribed(*points_[index-1], *points_[index-2], *points_[index-3]);
                break;
            case 4:
                sphere_ = Sphere::circumscribed(*points_[index-1], *points_[index-2], *points_[index-3], *points_[index-4]);
                return;
            };

            for(u32 i = 0; i < numPoints; ++i){
                if (sphere_.signedDistanceSqr(*points_[index + i]) > 0.0f){
                    for(u32 j = i; j > 0; --j){
                        const Vector3* t = points_[index + j];
                        points_[index + j] = points_[index + j - 1];
                        points_[index + j - 1] = t;
                    }

                    recurse(index + 1, i, numSos + 1);
                }
            }
        }

        // 3x3部分行列の行列式
        f32 determinant33(const Vector3& r0, const Vector3& r1, const Vector3& r2)
        {
            return r0.x_ * (r1.y_ * r2.z_ - r1.z_ * r2.y_)
                + r0.y_ * (r1.z_ * r2.x_ - r1.x_ * r2.z_)
                + r0.z_ * (r1.x_ * r2.y_ - r1.y_ * r2.x_);
        }
    }

    void Sphere::translate(const lmath::Vector3& position)
    {
        x_ += position.x_;
        y_ += position.y_;
        z_ += position.z_;
    }

    void Sphere::translate(const lmath::Vector4& position)
    {
        x_ += position.x_;
        y_ += position.y_;
        z_ += position.z_;
    }

    // 点に外接する球を計算
    Sphere Sphere::circumscribed(const Vector3& p0, const Vector3& p1)
    {
        Vector3 d = sub(p1, p0);
        d *= 0.5f;

        f32 radius = d.length();

        Vector3 center = lmath::add(p0, d);
        return Sphere(center, radius);
    }

    // 点に外接する球を計算
    Sphere Sphere::circumscribed(const Vector3& p0, const Vector3& p1, const Vector3& p2)
    {
        Vector3 d0 = sub(p2, p1);
        Vector3 d1 = sub(p0, p2);
        Vector3 d2 = sub(p1, p0);

        f32 n0 = d0.length();
        f32 n1 = d1.length();
        f32 n2 = d2.length();

        Vector3 center;
        f32 r;
        if (n0 < LMATH_F32_EPSILON || n1 < LMATH_F32_EPSILON || n2 < LMATH_F32_EPSILON)
        {
            center = lmath::add(p0, p1);
            center += p2;
            center *= (1.0f/3.0f);

            if (n0 < LMATH_F32_EPSILON)
            {
                r = 0.5f * n1;
            }
            else if (n1 < LMATH_F32_EPSILON)
            {
                r = 0.5f * n2;
            }
            else
            {
                r = 0.5f * n0;
            }
            return Sphere(center, r);
        }

        f32 f0 = n0 * n0;
        f32 f1 = n1 * n1;
        f32 f2 = n2 * n2;

        Vector3 barycentric;//重心座標
        barycentric.x_ = f0 * (-f0 + f1 + f2);
        barycentric.y_ = f1 * (f0 - f1 + f2);
        barycentric.z_ = f2 * (f0 + f1 - f2);

        f32 ratio = 1.0f / (barycentric.x_ + barycentric.y_ + barycentric.z_);

        Vector3 t0 = p0; t0 *= (barycentric.x_ * ratio);
        Vector3 t1 = p1; t1 *= (barycentric.y_ * ratio);
        Vector3 t2 = p2; t2 *= (barycentric.z_ * ratio);

        center = lmath::add(t0, t1);
        center += t2;

        float denom = (n0 + n1 + n2) * (-n0 + n1 + n2) * (n0 - n1 + n2) * (n0 + n1 - n2);
        denom = lmath::sqrt(denom);
        r = n0 * n1 * n2 / denom;

        return Sphere(center, r);
    }

    // 点に外接する球を計算
    Sphere Sphere::circumscribed(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3)
    {
        Vector3 a = sub(p1, p0);
        Vector3 b = sub(p2, p0);
        Vector3 c = sub(p3, p0);

        Vector3 center;
        f32 r;

        f32 det = determinant33(a, b, c);
        if(det<LMATH_F32_EPSILON){
            center = lmath::add(p0, p1);
            center += p2;
            center += p3;
            center *= 0.25f;

            r = lmath::distance(p0, center);
            return Sphere(center, r);
        }

        f32 coff = 0.5f / det;

        f32 a2 = dot(a, a);
        f32 b2 = dot(b, b);
        f32 c2 = dot(c, c);

        Vector3 cross0 = cross(b, c);
        Vector3 cross1 = cross(c, a);
        Vector3 cross2 = cross(a, b);

        cross0 *= a2;
        cross1 *= b2;
        cross2 *= c2;


        center = lmath::add(cross0, cross1);
        center += cross2;
        center *= coff;

        center += p0;

        r = lmath::distance(p0, center);
        return Sphere(center, r);
    }


    Sphere Sphere::calcMiniSphere(const Vector3* points, u32 numPoints)
    {
        const Vector3** ppPoints = LNEW const Vector3*[numPoints];

        for(u32 i=0; i<numPoints; ++i){
            ppPoints[i] = &points[i];
        }

        MiniSphere miniSphere(numPoints, ppPoints);
        miniSphere.recurse(0, numPoints, 0);

        LDELETE_ARRAY(ppPoints);
        return miniSphere.sphere_;
    }

    void Sphere::combine(const Sphere& s0, const Sphere& s1)
    {
        Vector4 dv = Vector4::construct(sub((const Vector4&)s0, (const Vector4&)s1));

        f32 distance = lmath::sqrt(dv.x_*dv.x_ + dv.y_*dv.y_ + dv.z_*dv.z_);

        f32 d = (s0.r_<s1.r_)? -dv.w_ : dv.w_;
        if(distance<=(d+LMATH_F32_EPSILON)){
            (Vector4&)*this = (s0.r_<s1.r_)? s1 : s0;

        }else{

            f32 r = (s0.r_ + s1.r_ + distance) * 0.5f;

            d = (s0.r_<s1.r_)? s1.r_ : s0.r_;
            d = r - d;

            (Vector4&)*this = Vector4::construct(lerp(s0, s1, d/distance));
            r_ = r;
        }
    }

    f32 Sphere::signedDistanceSqr(const Vector3& p) const
    {
        Vector3 d = {p.x_-x_, p.y_-y_, p.z_-z_};

        return dot(d, d) - r_*r_;
    }

    f32 Sphere::distance(const Sphere& rhs) const
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = _mm_loadu_ps(&x_);
        lm128 r1 = _mm_loadu_ps(&rhs.x_);

        r0 = _mm_sub_ps(r0, r1);
        r0 = _mm_mul_ps(r0, r0);

        r1 = r0;
        r0 = _mm_add_ps(r0, _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(0, 0, 0, 1)));
        r0 = _mm_add_ps(r0, _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(0, 0, 0, 2)));
        r0 = _mm_sqrt_ss(r0);

        f32 ret;
        _mm_store_ss(&ret, r0);
        return ret;
#else
        lmath::Vector4 r;
        r.sub(s_, rhs.s_);
        r.w_ = 0.0f;
        return r.length();
#endif
    }
}

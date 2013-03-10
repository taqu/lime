/**
@file Sphere.cpp
@author t-sakai
@date 2011/11/04 create
*/
#include "Sphere.h"

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
        @param numSos ... 0-4�̃T�|�[�g�_�B���̋��E�����߂�4�̓_
        */
        void MiniSphere::recurse(u32 index, u32 numPoints, u32 numSos)
        {
            switch(numSos)
            {
            case 0:
                sphere_ = Sphere(0.0f, 0.0f, 0.0f, SphereRadiusEpsilon);
                break;
            case 1:
                sphere_ = Sphere(*points_[index-1], SphereRadiusEpsilon);
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

        // 3x3�����s��̍s��
        f32 determinant33(const Vector3& r0, const Vector3& r1, const Vector3& r2)
        {
            return r0.x_ * (r1.y_ * r2.z_ - r1.z_ * r2.y_)
                + r0.y_ * (r1.z_ * r2.x_ - r1.x_ * r2.z_)
                + r0.z_ * (r1.x_ * r2.y_ - r1.y_ * r2.x_);
        }
    }

    // �_�ɊO�ڂ��鋅���v�Z
    Sphere Sphere::circumscribed(const Vector3& p0, const Vector3& p1)
    {
        Vector3 d;
        d.sub(p1, p0);
        d *= 0.5f;

        f32 radius = d.length() + SphereRadiusEpsilon;

        Vector3 center;
        center.add(p0, d);

        return Sphere(center, radius);
    }

    // �_�ɊO�ڂ��鋅���v�Z
    Sphere Sphere::circumscribed(const Vector3& p0, const Vector3& p1, const Vector3& p2)
    {
        Vector3 d0; d0.sub(p2, p1);
        Vector3 d1; d1.sub(p0, p2);
        Vector3 d2; d2.sub(p1, p0);

        f32 n0 = d0.length();
        f32 n1 = d1.length();
        f32 n2 = d2.length();

        Vector3 center;
        f32 r;
        if (n0 < F32_EPSILON || n1 < F32_EPSILON || n2 < F32_EPSILON)
        {
            center.add(p0, p1);
            center += p2;
            center *= (1.0f/3.0f);

            if (n0 < F32_EPSILON)
            {
                r = 0.5f * n1;
            }
            else if (n1 < F32_EPSILON)
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

        Vector3 barycentric;//�d�S���W
        barycentric.x_ = f0 * (-f0 + f1 + f2);
        barycentric.y_ = f1 * (f0 - f1 + f2);
        barycentric.z_ = f2 * (f0 + f1 - f2);

        f32 ratio = 1.0f / (barycentric.x_ + barycentric.y_ + barycentric.z_);

        Vector3 t0 = p0; t0 *= (barycentric.x_ * ratio);
        Vector3 t1 = p1; t1 *= (barycentric.y_ * ratio);
        Vector3 t2 = p2; t2 *= (barycentric.z_ * ratio);

        center.add(t0, t1);
        center += t2;

        float denom = (n0 + n1 + n2) * (-n0 + n1 + n2) * (n0 - n1 + n2) * (n0 + n1 - n2);
        denom = lmath::sqrt(denom);
        r = n0 * n1 * n2 / denom + SphereRadiusEpsilon;

        return Sphere(center, r);
    }

    // �_�ɊO�ڂ��鋅���v�Z
    Sphere Sphere::circumscribed(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3)
    {
        Vector3 a; a.sub(p1, p0);
        Vector3 b; b.sub(p2, p0);
        Vector3 c; c.sub(p3, p0);

        Vector3 center;
        f32 r;

        f32 det = determinant33(a, b, c);
        if(det<F32_EPSILON){
            center.add(p0, p1);
            center += p2;
            center += p3;
            center *= 0.25f;

            r = p0.distance(center) + SphereRadiusEpsilon;
            return Sphere(center, r);
        }

        f32 coff = 0.5f / det;

        f32 a2 = a.dot(a);
        f32 b2 = b.dot(b);
        f32 c2 = c.dot(c);

        Vector3 cross0; cross0.cross(b, c);
        Vector3 cross1; cross1.cross(c, a);
        Vector3 cross2; cross2.cross(a, b);

        cross0 *= a2;
        cross1 *= b2;
        cross2 *= c2;


        center.add(cross0, cross1);
        center += cross2;
        center *= coff;

        center += p0;

        r = p0.distance(center) + SphereRadiusEpsilon;
        return Sphere(center, r);
    }


    Sphere Sphere::calcMiniSphere(const Vector3* points, u32 numPoints)
    {
        const Vector3** ppPoints = LIME_NEW const Vector3*[numPoints];

        for(u32 i=0; i<numPoints; ++i){
            ppPoints[i] = &points[i];
        }

        MiniSphere miniSphere(numPoints, ppPoints);
        miniSphere.recurse(0, numPoints, 0);

        LIME_DELETE_ARRAY(ppPoints);
        return miniSphere.sphere_;
    }

    void Sphere::combine(const Sphere& s0, const Sphere& s1)
    {
        f32 distance = s0.s_.distance3(s1.s_);

        f32 d = (s0.s_.w_<s1.s_.w_)? s1.s_.w_ - s0.s_.w_ : s0.s_.w_ - s1.s_.w_;
        if(distance<=d){
            s_ = (s0.s_.w_<s1.s_.w_)? s1.s_ : s0.s_;

        }else{

            f32 r = s0.s_.w_ + s1.s_.w_;
            r += distance;
            r *= 0.5f;

            d = (s0.s_.w_<s1.s_.w_)? s1.s_.w_ : s0.s_.w_;
            d = r - d;

            s_.setLerp(s0.s_, s1.s_, d/distance);
            s_.w_ = r + SphereRadiusEpsilon;
        }
    }

    void Sphere::add(const Sphere& s1)
    {
        f32 distance = s_.distance3(s1.s_);

        f32 d = (s_.w_<s1.s_.w_)? s1.s_.w_ - s_.w_ : s_.w_ - s1.s_.w_;
        if(distance<=d){
            s_ = (s_.w_<s1.s_.w_)? s1.s_ : s_;

        }else{

            f32 r = s_.w_ + s1.s_.w_;
            r += distance;
            r *= 0.5f;

            d = (s_.w_<s1.s_.w_)? s1.s_.w_ : s_.w_;
            d = r - d;

            s_.setLerp(s_, s1.s_, d/distance);
            s_.w_ = r + SphereRadiusEpsilon;
        }
    }

    f32 Sphere::signedDistanceSqr(const Vector3& p) const
    {
        Vector3 d(p.x_-s_.x_, p.y_-s_.y_, p.z_-s_.z_);

        return d.dot(d) - s_.w_ * s_.w_;
    }

    f32 Sphere::distance(const Sphere& rhs) const
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = _mm_loadu_ps(&s_.x_);
        lm128 r1 = _mm_loadu_ps(&rhs.s_.x_);

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
/**
@file Frustum.cpp
@author t-sakai
@date 2011/11/03 create

*/
#include "Frustum.h"

#include <lmath/lmath.h>
#include <lmath/lmathutil.h>
#include <lmath/geometry/Sphere.h>
#include <lmath/geometry/PrimitiveTest.h>

#include "Camera.h"
using namespace lmath;

namespace lscene
{
#if 1
    //---------------------------------------------------
    // ビュー空間の視錐台計算
    void Frustum::calcInView(const Camera& camera, f32 znear, f32 zfar)
    {
        const lmath::Matrix44& proj = camera.getProjMatrix();

#if defined(LMATH_USE_SSE)
        //lm128 r0 = _mm_loadu_ps(&proj.m_[0][0]);
        //lm128 r1 = _mm_loadu_ps(&proj.m_[1][0]);
        //lm128 r2 = _mm_loadu_ps(&proj.m_[2][0]);
        //lm128 r3 = _mm_loadu_ps(&proj.m_[3][0]);

        //lm128 t0 = _mm_add_ps(r3, r0);
        //_mm_storeu_ps(&planes_[0].v_.x_, t0);

        //lm128 t1 = _mm_sub_ps(r3, r0);
        //_mm_storeu_ps(&planes_[1].v_.x_, t1);

        //lm128 t2 = _mm_add_ps(r3, r1);
        //_mm_storeu_ps(&planes_[2].v_.x_, t2);

        //lm128 t3 = _mm_sub_ps(r3, r1);
        //_mm_storeu_ps(&planes_[3].v_.x_, t3);

        //_mm_storeu_ps(&planes_[4].v_.x_, r2);

        //lm128 t5 = _mm_sub_ps(r2, r3);
        //_mm_storeu_ps(&planes_[5].v_.x_, t5);

        lm128 r0 = _mm_loadu_ps(&proj.m_[0][0]);
        lm128 r1 = _mm_loadu_ps(&proj.m_[1][0]);
        //lm128 r2 = _mm_loadu_ps(&proj.m_[2][0]);
        lm128 r3 = _mm_loadu_ps(&proj.m_[3][0]);

        f32 f;
        *((u32*)&f) = 0x80000000U;
        lm128 mask = _mm_load1_ps(&f);

        lm128 t0 = _mm_xor_ps(r0, mask);
        t0 = _mm_sub_ps(t0, r3);
        _mm_storeu_ps(&planes_[0].v_.x_, t0);

        lm128 t1 = _mm_sub_ps(r0, r3);
        _mm_storeu_ps(&planes_[1].v_.x_, t1);

        lm128 t2 = _mm_xor_ps(r1, mask);
        t2 = _mm_sub_ps(t2, r3);
        _mm_storeu_ps(&planes_[2].v_.x_, t2);

        lm128 t3 = _mm_sub_ps(r1, r3);
        _mm_storeu_ps(&planes_[3].v_.x_, t3);

        //lm128 t4 = _mm_xor_ps(r2, _mm_set1_ps(-0.0f));
        //_mm_storeu_ps(&planes_[4].v_.x_, t4);

        //lm128 t5 = _mm_sub_ps(r2, r3);
        //_mm_storeu_ps(&planes_[5].v_.x_, t5);
#else
        //planes_[0].v_.x_ = proj.m_[3][0] + proj.m_[0][0];
        //planes_[0].v_.y_ = proj.m_[3][1] + proj.m_[0][1];
        //planes_[0].v_.z_ = proj.m_[3][2] + proj.m_[0][2];
        //planes_[0].v_.w_ = proj.m_[3][3] + proj.m_[0][3];

        //planes_[1].v_.x_ = proj.m_[3][0] - proj.m_[0][0];
        //planes_[1].v_.y_ = proj.m_[3][1] - proj.m_[0][1];
        //planes_[1].v_.z_ = proj.m_[3][2] - proj.m_[0][2];
        //planes_[1].v_.w_ = proj.m_[3][3] - proj.m_[0][3];

        //planes_[2].v_.x_ = proj.m_[3][0] + proj.m_[1][0];
        //planes_[2].v_.y_ = proj.m_[3][1] + proj.m_[1][1];
        //planes_[2].v_.z_ = proj.m_[3][2] + proj.m_[1][2];
        //planes_[2].v_.w_ = proj.m_[3][3] + proj.m_[1][3];

        //planes_[3].v_.x_ = proj.m_[3][0] - proj.m_[1][0];
        //planes_[3].v_.y_ = proj.m_[3][1] - proj.m_[1][1];
        //planes_[3].v_.z_ = proj.m_[3][2] - proj.m_[1][2];
        //planes_[3].v_.w_ = proj.m_[3][3] - proj.m_[1][3];

        //planes_[4].v_.x_ = proj.m_[2][0];
        //planes_[4].v_.y_ = proj.m_[2][1];
        //planes_[4].v_.z_ = proj.m_[2][2];
        //planes_[4].v_.w_ = proj.m_[2][3];

        //planes_[5].v_.x_ = -proj.m_[2][0] + proj.m_[3][0];
        //planes_[5].v_.y_ = -proj.m_[2][1] + proj.m_[3][1];
        //planes_[5].v_.z_ = -proj.m_[2][2] + proj.m_[3][2];
        //planes_[5].v_.w_ = -proj.m_[2][3] + proj.m_[3][3];

        planes_[0].v_.x_ = -proj.m_[0][0] - proj.m_[3][0];
        planes_[0].v_.y_ = -proj.m_[0][1] - proj.m_[3][1];
        planes_[0].v_.z_ = -proj.m_[0][2] - proj.m_[3][2];
        planes_[0].v_.w_ = -proj.m_[0][3] - proj.m_[3][3];

        planes_[1].v_.x_ = proj.m_[0][0] - proj.m_[3][0];
        planes_[1].v_.y_ = proj.m_[0][1] - proj.m_[3][1];
        planes_[1].v_.z_ = proj.m_[0][2] - proj.m_[3][2];
        planes_[1].v_.w_ = proj.m_[0][3] - proj.m_[3][3];

        planes_[2].v_.x_ = -proj.m_[1][0] - proj.m_[3][0];
        planes_[2].v_.y_ = -proj.m_[1][1] - proj.m_[3][1];
        planes_[2].v_.z_ = -proj.m_[1][2] - proj.m_[3][2];
        planes_[2].v_.w_ = -proj.m_[1][3] - proj.m_[3][3];

        planes_[3].v_.x_ = proj.m_[1][0] - proj.m_[3][0];
        planes_[3].v_.y_ = proj.m_[1][1] - proj.m_[3][1];
        planes_[3].v_.z_ = proj.m_[1][2] - proj.m_[3][2];
        planes_[3].v_.w_ = proj.m_[1][3] - proj.m_[3][3];

        //planes_[4].v_.x_ = -proj.m_[2][0];
        //planes_[4].v_.y_ = -proj.m_[2][1];
        //planes_[4].v_.z_ = -proj.m_[2][2];
        //planes_[4].v_.w_ = -proj.m_[2][3];

        //planes_[5].v_.x_ = proj.m_[2][0] - proj.m_[3][0];
        //planes_[5].v_.y_ = proj.m_[2][1] - proj.m_[3][1];
        //planes_[5].v_.z_ = proj.m_[2][2] - proj.m_[3][2];
        //planes_[5].v_.w_ = proj.m_[2][3] - proj.m_[3][3];
#endif

        planes_[4].v_.x_ = 0.0f;
        planes_[4].v_.y_ = 0.0f;
        planes_[4].v_.z_ = -1.0f;
        planes_[4].v_.w_ = znear;

        planes_[5].v_.x_ = 0.0f;
        planes_[5].v_.y_ = 0.0f;
        planes_[5].v_.z_ = 1.0f;
        planes_[5].v_.w_ = -zfar;

        for(s32 i=0; i<4; ++i){
            planes_[i].normalize();
        }
    }
#endif

    //---------------------------------------------------
    void Frustum::calcInWorld(const Camera& camera)
    {
        const lmath::Matrix44& proj = camera.getViewProjMatrix();

#if defined(LMATH_USE_SSE)
        //lm128 r0 = _mm_loadu_ps(&proj.m_[0][0]);
        //lm128 r1 = _mm_loadu_ps(&proj.m_[1][0]);
        //lm128 r2 = _mm_loadu_ps(&proj.m_[2][0]);
        //lm128 r3 = _mm_loadu_ps(&proj.m_[3][0]);

        //lm128 t0 = _mm_add_ps(r3, r0);
        //_mm_storeu_ps(&planes_[0].v_.x_, t0);

        //lm128 t1 = _mm_sub_ps(r3, r0);
        //_mm_storeu_ps(&planes_[1].v_.x_, t1);

        //lm128 t2 = _mm_add_ps(r3, r1);
        //_mm_storeu_ps(&planes_[2].v_.x_, t2);

        //lm128 t3 = _mm_sub_ps(r3, r1);
        //_mm_storeu_ps(&planes_[3].v_.x_, t3);

        //_mm_storeu_ps(&planes_[4].v_.x_, r2);

        //lm128 t5 = _mm_sub_ps(r2, r3);
        //_mm_storeu_ps(&planes_[5].v_.x_, t5);

        lm128 r0 = _mm_loadu_ps(&proj.m_[0][0]);
        lm128 r1 = _mm_loadu_ps(&proj.m_[1][0]);
        lm128 r2 = _mm_loadu_ps(&proj.m_[2][0]);
        lm128 r3 = _mm_loadu_ps(&proj.m_[3][0]);

        f32 f;
        *((u32*)&f) = 0x80000000U;
        lm128 mask = _mm_load1_ps(&f);

        lm128 t0 = _mm_xor_ps(r0, mask);
        t0 = _mm_sub_ps(t0, r3);
        _mm_storeu_ps(&planes_[0].v_.x_, t0);

        lm128 t1 = _mm_sub_ps(r0, r3);
        _mm_storeu_ps(&planes_[1].v_.x_, t1);

        lm128 t2 = _mm_xor_ps(r1, mask);
        t2 = _mm_sub_ps(t2, r3);
        _mm_storeu_ps(&planes_[2].v_.x_, t2);

        lm128 t3 = _mm_sub_ps(r1, r3);
        _mm_storeu_ps(&planes_[3].v_.x_, t3);

        lm128 t4 = _mm_xor_ps(r2, mask);
        _mm_storeu_ps(&planes_[4].v_.x_, t4);

        lm128 t5 = _mm_sub_ps(r2, r3);
        _mm_storeu_ps(&planes_[5].v_.x_, t5);
#else
        //planes_[0].v_.x_ = proj.m_[3][0] + proj.m_[0][0];
        //planes_[0].v_.y_ = proj.m_[3][1] + proj.m_[0][1];
        //planes_[0].v_.z_ = proj.m_[3][2] + proj.m_[0][2];
        //planes_[0].v_.w_ = proj.m_[3][3] + proj.m_[0][3];

        //planes_[1].v_.x_ = proj.m_[3][0] - proj.m_[0][0];
        //planes_[1].v_.y_ = proj.m_[3][1] - proj.m_[0][1];
        //planes_[1].v_.z_ = proj.m_[3][2] - proj.m_[0][2];
        //planes_[1].v_.w_ = proj.m_[3][3] - proj.m_[0][3];

        //planes_[2].v_.x_ = proj.m_[3][0] + proj.m_[1][0];
        //planes_[2].v_.y_ = proj.m_[3][1] + proj.m_[1][1];
        //planes_[2].v_.z_ = proj.m_[3][2] + proj.m_[1][2];
        //planes_[2].v_.w_ = proj.m_[3][3] + proj.m_[1][3];

        //planes_[3].v_.x_ = proj.m_[3][0] - proj.m_[1][0];
        //planes_[3].v_.y_ = proj.m_[3][1] - proj.m_[1][1];
        //planes_[3].v_.z_ = proj.m_[3][2] - proj.m_[1][2];
        //planes_[3].v_.w_ = proj.m_[3][3] - proj.m_[1][3];

        //planes_[4].v_.x_ = proj.m_[2][0];
        //planes_[4].v_.y_ = proj.m_[2][1];
        //planes_[4].v_.z_ = proj.m_[2][2];
        //planes_[4].v_.w_ = proj.m_[2][3];

        //planes_[5].v_.x_ = -proj.m_[2][0] + proj.m_[3][0];
        //planes_[5].v_.y_ = -proj.m_[2][1] + proj.m_[3][1];
        //planes_[5].v_.z_ = -proj.m_[2][2] + proj.m_[3][2];
        //planes_[5].v_.w_ = -proj.m_[2][3] + proj.m_[3][3];

        planes_[0].v_.x_ = -proj.m_[0][0] - proj.m_[3][0];
        planes_[0].v_.y_ = -proj.m_[0][1] - proj.m_[3][1];
        planes_[0].v_.z_ = -proj.m_[0][2] - proj.m_[3][2];
        planes_[0].v_.w_ = -proj.m_[0][3] - proj.m_[3][3];

        planes_[1].v_.x_ = proj.m_[0][0] - proj.m_[3][0];
        planes_[1].v_.y_ = proj.m_[0][1] - proj.m_[3][1];
        planes_[1].v_.z_ = proj.m_[0][2] - proj.m_[3][2];
        planes_[1].v_.w_ = proj.m_[0][3] - proj.m_[3][3];

        planes_[2].v_.x_ = -proj.m_[1][0] - proj.m_[3][0];
        planes_[2].v_.y_ = -proj.m_[1][1] - proj.m_[3][1];
        planes_[2].v_.z_ = -proj.m_[1][2] - proj.m_[3][2];
        planes_[2].v_.w_ = -proj.m_[1][3] - proj.m_[3][3];

        planes_[3].v_.x_ = proj.m_[1][0] - proj.m_[3][0];
        planes_[3].v_.y_ = proj.m_[1][1] - proj.m_[3][1];
        planes_[3].v_.z_ = proj.m_[1][2] - proj.m_[3][2];
        planes_[3].v_.w_ = proj.m_[1][3] - proj.m_[3][3];

        planes_[4].v_.x_ = -proj.m_[2][0];
        planes_[4].v_.y_ = -proj.m_[2][1];
        planes_[4].v_.z_ = -proj.m_[2][2];
        planes_[4].v_.w_ = -proj.m_[2][3];

        planes_[5].v_.x_ = proj.m_[2][0] - proj.m_[3][0];
        planes_[5].v_.y_ = proj.m_[2][1] - proj.m_[3][1];
        planes_[5].v_.z_ = proj.m_[2][2] - proj.m_[3][2];
        planes_[5].v_.w_ = proj.m_[2][3] - proj.m_[3][3];
#endif

        for(s32 i=0; i<6; ++i){
            planes_[i].normalize();
        }
    }

    //---------------------------------------------------
    void Frustum::calcFromProjection(const lmath::Matrix44& proj, f32 znear, f32 zfar)
    {
#if defined(LMATH_USE_SSE)
        //lm128 r0 = _mm_loadu_ps(&proj.m_[0][0]);
        //lm128 r1 = _mm_loadu_ps(&proj.m_[1][0]);
        //lm128 r2 = _mm_loadu_ps(&proj.m_[2][0]);
        //lm128 r3 = _mm_loadu_ps(&proj.m_[3][0]);

        //lm128 t0 = _mm_add_ps(r3, r0);
        //_mm_storeu_ps(&planes_[0].v_.x_, t0);

        //lm128 t1 = _mm_sub_ps(r3, r0);
        //_mm_storeu_ps(&planes_[1].v_.x_, t1);

        //lm128 t2 = _mm_add_ps(r3, r1);
        //_mm_storeu_ps(&planes_[2].v_.x_, t2);

        //lm128 t3 = _mm_sub_ps(r3, r1);
        //_mm_storeu_ps(&planes_[3].v_.x_, t3);

        //_mm_storeu_ps(&planes_[4].v_.x_, r2);

        //lm128 t5 = _mm_sub_ps(r2, r3);
        //_mm_storeu_ps(&planes_[5].v_.x_, t5);

        lm128 r0 = _mm_loadu_ps(&proj.m_[0][0]);
        lm128 r1 = _mm_loadu_ps(&proj.m_[1][0]);
        //lm128 r2 = _mm_loadu_ps(&proj.m_[2][0]);
        lm128 r3 = _mm_loadu_ps(&proj.m_[3][0]);

        f32 f;
        *((u32*)&f) = 0x80000000U;
        lm128 mask = _mm_load1_ps(&f);

        lm128 t0 = _mm_xor_ps(r0, mask);
        t0 = _mm_sub_ps(t0, r3);
        _mm_storeu_ps(&planes_[0].v_.x_, t0);

        lm128 t1 = _mm_sub_ps(r0, r3);
        _mm_storeu_ps(&planes_[1].v_.x_, t1);

        lm128 t2 = _mm_xor_ps(r1, mask);
        t2 = _mm_sub_ps(t2, r3);
        _mm_storeu_ps(&planes_[2].v_.x_, t2);

        lm128 t3 = _mm_sub_ps(r1, r3);
        _mm_storeu_ps(&planes_[3].v_.x_, t3);

        //lm128 t4 = _mm_xor_ps(r2, _mm_set1_ps(-0.0f));
        //_mm_storeu_ps(&planes_[4].v_.x_, t4);

        //lm128 t5 = _mm_sub_ps(r2, r3);
        //_mm_storeu_ps(&planes_[5].v_.x_, t5);
#else
        //planes_[0].v_.x_ = proj.m_[3][0] + proj.m_[0][0];
        //planes_[0].v_.y_ = proj.m_[3][1] + proj.m_[0][1];
        //planes_[0].v_.z_ = proj.m_[3][2] + proj.m_[0][2];
        //planes_[0].v_.w_ = proj.m_[3][3] + proj.m_[0][3];

        //planes_[1].v_.x_ = proj.m_[3][0] - proj.m_[0][0];
        //planes_[1].v_.y_ = proj.m_[3][1] - proj.m_[0][1];
        //planes_[1].v_.z_ = proj.m_[3][2] - proj.m_[0][2];
        //planes_[1].v_.w_ = proj.m_[3][3] - proj.m_[0][3];

        //planes_[2].v_.x_ = proj.m_[3][0] + proj.m_[1][0];
        //planes_[2].v_.y_ = proj.m_[3][1] + proj.m_[1][1];
        //planes_[2].v_.z_ = proj.m_[3][2] + proj.m_[1][2];
        //planes_[2].v_.w_ = proj.m_[3][3] + proj.m_[1][3];

        //planes_[3].v_.x_ = proj.m_[3][0] - proj.m_[1][0];
        //planes_[3].v_.y_ = proj.m_[3][1] - proj.m_[1][1];
        //planes_[3].v_.z_ = proj.m_[3][2] - proj.m_[1][2];
        //planes_[3].v_.w_ = proj.m_[3][3] - proj.m_[1][3];

        //planes_[4].v_.x_ = proj.m_[2][0];
        //planes_[4].v_.y_ = proj.m_[2][1];
        //planes_[4].v_.z_ = proj.m_[2][2];
        //planes_[4].v_.w_ = proj.m_[2][3];

        //planes_[5].v_.x_ = -proj.m_[2][0] + proj.m_[3][0];
        //planes_[5].v_.y_ = -proj.m_[2][1] + proj.m_[3][1];
        //planes_[5].v_.z_ = -proj.m_[2][2] + proj.m_[3][2];
        //planes_[5].v_.w_ = -proj.m_[2][3] + proj.m_[3][3];

        planes_[0].v_.x_ = -proj.m_[0][0] - proj.m_[3][0];
        planes_[0].v_.y_ = -proj.m_[0][1] - proj.m_[3][1];
        planes_[0].v_.z_ = -proj.m_[0][2] - proj.m_[3][2];
        planes_[0].v_.w_ = -proj.m_[0][3] - proj.m_[3][3];

        planes_[1].v_.x_ = proj.m_[0][0] - proj.m_[3][0];
        planes_[1].v_.y_ = proj.m_[0][1] - proj.m_[3][1];
        planes_[1].v_.z_ = proj.m_[0][2] - proj.m_[3][2];
        planes_[1].v_.w_ = proj.m_[0][3] - proj.m_[3][3];

        planes_[2].v_.x_ = -proj.m_[1][0] - proj.m_[3][0];
        planes_[2].v_.y_ = -proj.m_[1][1] - proj.m_[3][1];
        planes_[2].v_.z_ = -proj.m_[1][2] - proj.m_[3][2];
        planes_[2].v_.w_ = -proj.m_[1][3] - proj.m_[3][3];

        planes_[3].v_.x_ = proj.m_[1][0] - proj.m_[3][0];
        planes_[3].v_.y_ = proj.m_[1][1] - proj.m_[3][1];
        planes_[3].v_.z_ = proj.m_[1][2] - proj.m_[3][2];
        planes_[3].v_.w_ = proj.m_[1][3] - proj.m_[3][3];

        //planes_[4].v_.x_ = -proj.m_[2][0];
        //planes_[4].v_.y_ = -proj.m_[2][1];
        //planes_[4].v_.z_ = -proj.m_[2][2];
        //planes_[4].v_.w_ = -proj.m_[2][3];

        //planes_[5].v_.x_ = proj.m_[2][0] - proj.m_[3][0];
        //planes_[5].v_.y_ = proj.m_[2][1] - proj.m_[3][1];
        //planes_[5].v_.z_ = proj.m_[2][2] - proj.m_[3][2];
        //planes_[5].v_.w_ = proj.m_[2][3] - proj.m_[3][3];
#endif

        planes_[4].v_.x_ = 0.0f;
        planes_[4].v_.y_ = 0.0f;
        planes_[4].v_.z_ = -1.0f;
        planes_[4].v_.w_ = znear;

        planes_[5].v_.x_ = 0.0f;
        planes_[5].v_.y_ = 0.0f;
        planes_[5].v_.z_ = 1.0f;
        planes_[5].v_.w_ = -zfar;

        for(s32 i=0; i<4; ++i){
            planes_[i].normalize();
        }
    }

    //---------------------------------------------------
    // 球が視錐台内にあるか
    bool Frustum::contains(f32 x, f32 y, f32 z, f32 radius) const
    {
        lmath::Vector4 p(x, y, z, 1.0f);
        f32 distance;
        for(s32 i=0; i<6; ++i){
            distance = planes_[i].dot(p);
            if(radius < distance){
                return false;
            }
        }
        return true;
    }

    //---------------------------------------------------
    // AABBが視錐台内にあるか
    bool Frustum::contains(const lmath::Vector3& bmin, const lmath::Vector3& bmax) const
    {
        lmath::Vector4 p;
        p.one();
        for(s32 i=0; i<6; ++i){
            p.x_ = (0<=planes_[i].v_.x_)? bmin.x_ : bmax.x_;
            p.y_ = (0<=planes_[i].v_.y_)? bmin.y_ : bmax.y_;
            p.z_ = (0<=planes_[i].v_.z_)? bmin.z_ : bmax.z_;
            if(0.0f<planes_[i].dot(p)){
                return false;
            }
        }
        return true;
    }

    //---------------------------------------------------
    // AABBが視錐台内にあるか
    bool Frustum::contains(const lmath::Vector4& bmin, const lmath::Vector4& bmax) const
    {
        lmath::Vector4 p;
        p.one();

        for(s32 i=0; i<6; ++i){
            p.x_ = (0<=planes_[i].v_.x_)? bmin.x_ : bmax.x_;
            p.y_ = (0<=planes_[i].v_.y_)? bmin.y_ : bmax.y_;
            p.z_ = (0<=planes_[i].v_.z_)? bmin.z_ : bmax.z_;

            if(0.0f<planes_[i].dot(p)){
                return false;
            }
        }
        return true;
    }

    //---------------------------------------------------
    // 視錐台の８頂点取得
    void Frustum::getPoints(lmath::Vector4* points) const
    {
        f32 slopeX = planes_[0].v_.z_ / planes_[0].v_.x_;
        f32 slopeY = planes_[2].v_.z_ / planes_[2].v_.y_;

        f32 halfWidth = planes_[4].v_.w_ * slopeX;
        f32 halfHeight = planes_[4].v_.w_ * slopeY;

        points[0].set( halfWidth,   halfHeight, planes_[4].v_.w_, 1.0f); //右上
        points[1].set(-halfWidth,   halfHeight, planes_[4].v_.w_, 1.0f); //左上
        points[2].set(-halfWidth,  -halfHeight, planes_[4].v_.w_, 1.0f); //左下
        points[3].set( halfWidth,  -halfHeight, planes_[4].v_.w_, 1.0f); //右下


        f32 far = -planes_[5].v_.w_;
        halfWidth = far * slopeX;
        halfHeight = far * slopeY;

        points[4].set( halfWidth,   halfHeight, far, 1.0f); //右上
        points[5].set(-halfWidth,   halfHeight, far, 1.0f); //左上
        points[6].set(-halfWidth,  -halfHeight, far, 1.0f); //左下
        points[7].set( halfWidth,  -halfHeight, far, 1.0f); //右下
    }

    //---------------------------------------------------
    // 視錐台の８頂点取得
    void Frustum::getPoints(lmath::Vector4* points, f32 znear, f32 zfar) const
    {
        f32 slopeX = planes_[0].v_.z_ / planes_[0].v_.x_;
        f32 slopeY = planes_[2].v_.z_ / planes_[2].v_.y_;

        f32 halfWidth = znear * slopeX;
        f32 halfHeight = znear * slopeY;

        points[0].set( halfWidth,   halfHeight, znear, 1.0f); //右上
        points[1].set(-halfWidth,   halfHeight, znear, 1.0f); //左上
        points[2].set(-halfWidth,  -halfHeight, znear, 1.0f); //左下
        points[3].set( halfWidth,  -halfHeight, znear, 1.0f); //右下

        halfWidth = zfar * slopeX;
        halfHeight = zfar * slopeY;

        points[4].set( halfWidth,   halfHeight, zfar, 1.0f); //右上
        points[5].set(-halfWidth,   halfHeight, zfar, 1.0f); //左上
        points[6].set(-halfWidth,  -halfHeight, zfar, 1.0f); //左下
        points[7].set( halfWidth,  -halfHeight, zfar, 1.0f); //右下
    }

    void Frustum::calcFitNearFar(f32& znear, f32& zfar, const lmath::Vector4& aabbMin, const lmath::Vector4& aabbMax, const Camera& camera)
    {
        static const s32 AABBTriIndices[] = 
        {
            0,1,2,  1,2,3,
            4,5,6,  5,6,7,
            0,2,4,  2,4,6,
            1,3,5,  3,5,7,
            0,1,4,  1,4,5,
            2,3,6,  3,6,7 
        };

        Frustum frustum;
        frustum.calcFromProjection(camera.getProjMatrix(), camera.getZNear(), camera.getZFar());

        znear = FLT_MAX;
        zfar = -FLT_MIN;

        lmath::Vector4 points[8];
        lmath::calcAABBPoints(points, aabbMin, aabbMax);

        for(s32 i=0; i<8; ++i){
            points[i].mul(camera.getViewMatrix(), points[i]);
        }

        //視錐台の各側面について
        lmath::Triangle triangles[16];
        s32 numTriangles = 0;
        for(s32 i=0; i<12; ++i){
            s32 index = 3*i;
            triangles[0].v_[0] = points[ AABBTriIndices[index] ];
            triangles[0].v_[1] = points[ AABBTriIndices[index + 1] ];
            triangles[0].v_[2] = points[ AABBTriIndices[index + 2] ];
            numTriangles = 1;

            for(s32 j=0; j<4; ++j){
                s32 count = numTriangles;
                for(s32 k=0; k<count; ++k){
                    lmath::clipTriangle(numTriangles, triangles, frustum.planes_[j]);
                } //for(s32 k=0; k<count; ++k){

            } //for(s32 j=0; j<4; ++j){

            for(s32 j=0; j<numTriangles; ++j){
                for(s32 k=0; k<3; ++k){
                    if(zfar < triangles[j].v_[k].z_){
                        zfar = triangles[j].v_[k].z_;
                    }
                    if(triangles[j].v_[k].z_ < znear){
                        znear = triangles[j].v_[k].z_;
                    }
                }
            }
        }
    }

}

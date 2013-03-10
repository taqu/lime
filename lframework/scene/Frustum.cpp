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
    //---------------------------------------------------
    // ビュー空間の視錐台計算
    void Frustum::calc(const Camera& camera, f32 zfar)
    {
        const lmath::Matrix44& proj = camera.getProjMatrix();

#if defined(LMATH_USE_SSE)
        //lm128 r0 = _mm_loadu_ps(&proj.m_[0][0]);
        //lm128 r1 = _mm_loadu_ps(&proj.m_[1][0]);
        //lm128 r3 = _mm_loadu_ps(&proj.m_[3][0]);

        //lm128 t0 = _mm_add_ps(r3, r0);
        //_mm_storeu_ps(&planes_[0].v_.x_, t0);

        //lm128 t1 = _mm_sub_ps(r3, r0);
        //_mm_storeu_ps(&planes_[1].v_.x_, t1);

        //lm128 t2 = _mm_add_ps(r3, r1);
        //_mm_storeu_ps(&planes_[2].v_.x_, t2);

        //lm128 t3 = _mm_sub_ps(r3, r1);
        //_mm_storeu_ps(&planes_[3].v_.x_, t3);

        lm128 r0 = _mm_loadu_ps(&proj.m_[0][0]);
        lm128 r1 = _mm_loadu_ps(&proj.m_[1][0]);
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
#endif
        planes_[4].v_.x_ = 0.0f;
        planes_[4].v_.y_ = 0.0f;
        planes_[4].v_.z_ = -1.0f;
        planes_[4].v_.w_ = camera.getZNear();

        planes_[5].v_.x_ = 0.0f;
        planes_[5].v_.y_ = 0.0f;
        planes_[5].v_.z_ = 1.0f;
        planes_[5].v_.w_ = -zfar;

        for(s32 i=0; i<4; ++i){
            planes_[i].normalize();
        }
    }


    //---------------------------------------------------
    // ビュー空間の球が視錐台内にあるか
    bool Frustum::contains(f32 x, f32 y, f32 z, f32 radius)
    {
        lmath::Vector4 p(x, y, z, 1.0f);
        float distance;
        for(s32 i=0; i<6; ++i){
            distance = planes_[i].dot(p);
            if(radius < distance){
                return false;
            }

        }
        return true;
    }

    //---------------------------------------------------
    // 視錐台の８頂点取得
    void Frustum::getPoints(lmath::Vector4* points)
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
    void Frustum::getPoints(lmath::Vector4* points, f32 znear)
    {
        f32 slopeX = planes_[0].v_.z_ / planes_[0].v_.x_;
        f32 slopeY = planes_[2].v_.z_ / planes_[2].v_.y_;

        f32 halfWidth = znear * slopeX;
        f32 halfHeight = znear * slopeY;

        points[0].set( halfWidth,   halfHeight, znear, 1.0f); //右上
        points[1].set(-halfWidth,   halfHeight, znear, 1.0f); //左上
        points[2].set(-halfWidth,  -halfHeight, znear, 1.0f); //左下
        points[3].set( halfWidth,  -halfHeight, znear, 1.0f); //右下


        f32 far = -planes_[5].v_.w_;
        halfWidth = far * slopeX;
        halfHeight = far * slopeY;

        points[4].set( halfWidth,   halfHeight, far, 1.0f); //右上
        points[5].set(-halfWidth,   halfHeight, far, 1.0f); //左上
        points[6].set(-halfWidth,  -halfHeight, far, 1.0f); //左下
        points[7].set( halfWidth,  -halfHeight, far, 1.0f); //右下
    }

    f32 Frustum::calcFitZFar(const lmath::Vector4& aabbMin, const lmath::Vector4& aabbMax, const Camera& camera, f32 minz, f32 maxz)
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

        //const lmath::Matrix44& proj = camera.getProjMatrix();

        Frustum frustum;
        frustum.calc(camera, camera.getZFar());


        f32 zfar = minz;

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
                    if(zfar<triangles[j].v_[k].z_){
                        zfar = triangles[j].v_[k].z_;
                    }
                }
            }
        }
        return (maxz < zfar)? maxz : zfar;
    }
}

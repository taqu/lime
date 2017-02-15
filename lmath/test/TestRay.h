#include <gtest/gtest.h>
#include "lmath.h"
#include "geometry/Ray.h"
#include "geometry/RayTest.h"

namespace lmath
{
    class TestRay : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };


    TEST_F(TestRay, RayTriangleSingle00)
    {
        {
            Ray ray(Vector3::construct(-1.0f, -1.0f, -1.0f), normalize(Vector3::construct(1.0f, 1.0f, 1.0f)), 100.0f);

            Vector3 points[3] =
            {
                {-1.0f, -1.0f, 0.0f}, {1.0f, 2.0f, 0.0f}, {2.0f, -1.0f, 0.0f},
            };
            bool bresult;
            f32 t,u,v,w;
            bresult = testRayTriangleFront(t,v,w, ray, points[0], points[1], points[2]);
            u = 1.0f-v-w;
            Vector3 p = points[0]*u + points[1]*v + points[2]*w;
            EXPECT_LT(v, u);
            EXPECT_LT(w, v);
            EXPECT_TRUE(bresult);
            EXPECT_TRUE(isZero(p.x_)); EXPECT_TRUE(isZero(p.y_)); EXPECT_TRUE(isZero(p.z_));

            bresult = testRayTriangleFront(t,v,w, ray, points[0], points[2], points[1]);
            EXPECT_FALSE(bresult);
            bresult = testRayTriangleBack(t,v,w, ray, points[0], points[1], points[2]);
            EXPECT_FALSE(bresult);
            bresult = testRayTriangleBack(t,v,w, ray, points[0], points[2], points[1]);
            EXPECT_TRUE(bresult);

            Result sresult;
            sresult = testRayTriangleBoth(t,v,w, ray, points[0], points[1], points[2]);
            EXPECT_EQ(sresult, Result_Front);
            sresult = testRayTriangleBoth(t,v,w, ray, points[0], points[2], points[1]);
            EXPECT_EQ(sresult, Result_Back);

            ray = Ray(Vector3::construct(-10.0f, -1.0f, -1.0f), normalize(Vector3::construct(-10.0f, 1.0f, 1.0f)), 100.0f);
            bresult = testRayTriangleFront(t,v,w, ray, points[0], points[1], points[2]);
            EXPECT_FALSE(bresult);
            bresult = testRayTriangleFront(t,v,w, ray, points[0], points[1], points[2]);
            EXPECT_FALSE(bresult);
            sresult = testRayTriangleBoth(t,v,w, ray, points[0], points[1], points[2]);
            EXPECT_EQ(sresult, Result_Fail);
        }

        {
            Vector3 sp = Vector3::construct(-1.0f, -1.0f, -1.0f);
            Vector3 ep = Vector3::construct(1.0f, 1.0f, 1.0f);

            Vector3 points[3] =
            {
                {-1.0f, -1.0f, 0.0f}, {1.0f, 2.0f, 0.0f}, {2.0f, -1.0f, 0.0f},
            };
            bool bresult;
            f32 t,u,v,w;
            bresult = testRayTriangleFront(t,v,w, sp, ep, points[0], points[1], points[2]);
            u = 1.0f-v-w;
            Vector3 p = points[0]*u + points[1]*v + points[2]*w;
            EXPECT_LT(v, u);
            EXPECT_LT(w, v);
            EXPECT_TRUE(bresult);
            EXPECT_TRUE(isZero(p.x_)); EXPECT_TRUE(isZero(p.y_)); EXPECT_TRUE(isZero(p.z_));

            bresult = testRayTriangleFront(t,v,w, sp, ep, points[0], points[2], points[1]);
            EXPECT_FALSE(bresult);
            bresult = testRayTriangleBack(t,v,w, sp, ep, points[0], points[1], points[2]);
            EXPECT_FALSE(bresult);
            bresult = testRayTriangleBack(t,v,w, sp, ep, points[0], points[2], points[1]);
            EXPECT_TRUE(bresult);

            Result sresult;
            sresult = testRayTriangleBoth(t,v,w, sp, ep, points[0], points[1], points[2]);
            EXPECT_EQ(sresult, Result_Front);
            sresult = testRayTriangleBoth(t,v,w, sp, ep, points[0], points[2], points[1]);
            EXPECT_EQ(sresult, Result_Back);


            sp = Vector3::construct(-10.0f, -1.0f, -1.0f);
            ep = Vector3::construct(-10.0f, 1.0f, 1.0f);
            bresult = testRayTriangleFront(t,v,w, sp, ep, points[0], points[1], points[2]);
            EXPECT_FALSE(bresult);
            bresult = testRayTriangleFront(t,v,w, sp, ep, points[0], points[1], points[2]);
            EXPECT_FALSE(bresult);
            sresult = testRayTriangleBoth(t,v,w, sp, ep, points[0], points[1], points[2]);
            EXPECT_EQ(sresult, Result_Fail);
        }

    }

    TEST_F(TestRay, RayTriangleSingle01)
    {
        {
            Ray rays[3] =
            {
                Ray(Vector3::construct(-1.0f, -1.0f, -1.0f), normalize(Vector3::construct(0.0f, 0.0f, 1.0f)), 100.0f),
                Ray(Vector3::construct( 1.0f,  2.0f, -1.0f), normalize(Vector3::construct(0.0f, 0.0f, 1.0f)), 100.0f),
                Ray(Vector3::construct( 2.0f, -1.0f, -1.0f), normalize(Vector3::construct(0.0f, 0.0f, 1.0f)), 100.0f),
            };

            Vector3 points[3] =
            {
                {-1.0f, -1.0f, 0.0f}, {1.0f, 2.0f, 0.0f}, {2.0f, -1.0f, 0.0f},
            };
            bool bresult;
            f32 t,v,w;

            for(s32 i=0; i<3; ++i){
                const Ray& ray = rays[i];
                bresult = testRayTriangleFront(t, v, w, ray, points[0], points[1], points[2]);
                EXPECT_TRUE(bresult);
                bresult = testRayTriangleFront(t, v, w, ray, points[0], points[2], points[1]);
                EXPECT_FALSE(bresult);

                bresult = testRayTriangleBack(t, v, w, ray, points[0], points[1], points[2]);
                EXPECT_FALSE(bresult);
                bresult = testRayTriangleBack(t, v, w, ray, points[0], points[2], points[1]);
                EXPECT_TRUE(bresult);

                Result sresult;
                sresult = testRayTriangleBoth(t, v, w, ray, points[0], points[1], points[2]);
                EXPECT_EQ(sresult, Result_Front);
                sresult = testRayTriangleBoth(t, v, w, ray, points[0], points[2], points[1]);
                EXPECT_EQ(sresult, Result_Back);
            }

        }
    }

    TEST_F(TestRay, RayTriangleSIMD00)
    {
        {
            lm128 origin[3];
            lm128 direction[3];
            lm128 invDir[3];

            lm128 vx[3];
            lm128 vy[3];
            lm128 vz[3];

            lm128 vt,vv,vw;

            Ray ray(Vector3::construct(-1.0f, -1.0f, -1.0f), normalize(Vector3::construct(1.0f, 1.0f, 1.0f)), 100.0f);

            f32 px[3][4] =
            {
                {-1.0f,-1.0f,-1.0f,-1.0f},
                { 2.0f, 1.0f, 2.0f, 1.0f},
                { 1.0f, 2.0f, 1.0f, 2.0f},
            };
            f32 py[3][4] =
            {
                {-1.0f,-1.0f,-1.0f,-1.0f},
                {-1.0f, 2.0f,-1.0f, 2.0f},
                { 2.0f,-1.0f, 2.0f,-1.0f},
            };
            f32 pz[3][4] =
            {
                { -1.0f, 0.0f, 0.5f, 1.0f},
                { -1.0f, 0.0f, 0.5f, 1.0f},
                { -1.0f, 0.0f, 0.5f, 1.0f},
            };

            origin[0] = _mm_load1_ps(&ray.origin_.x_);
            origin[1] = _mm_load1_ps(&ray.origin_.y_);
            origin[2] = _mm_load1_ps(&ray.origin_.z_);

            direction[0] = _mm_load1_ps(&ray.direction_.x_);
            direction[1] = _mm_load1_ps(&ray.direction_.y_);
            direction[2] = _mm_load1_ps(&ray.direction_.z_);

            invDir[0] = _mm_set1_ps(1.0f/ray.direction_.x_);
            invDir[1] = _mm_set1_ps(1.0f/ray.direction_.y_);
            invDir[2] = _mm_set1_ps(1.0f/ray.direction_.z_);

            for(s32 i=0; i<3; ++i){
                vx[i] = _mm_loadu_ps(px[i]);
                vy[i] = _mm_loadu_ps(py[i]);
                vz[i] = _mm_loadu_ps(pz[i]);
            }

            s32 sresult;

            sresult = testRayTriangleFront(vt,vv,vw,origin,direction,vx,vy,vz);
            EXPECT_EQ(sresult,10);

            sresult = testRayTriangleBack(vt,vv,vw,origin,direction,vx,vy,vz);
            EXPECT_EQ(sresult,5);

            sresult = testRayTriangleBoth(vt,vv,vw,origin,direction,vx,vy,vz);
            EXPECT_EQ(sresult,0x03050305);
        }
    }
}

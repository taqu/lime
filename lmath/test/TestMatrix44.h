#include <gtest/gtest.h>
#include <lcore/Random.h>
#include "lmath.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix44.h"

namespace lmath
{
    class TestMatrix44 : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

        lmath::Vector4 getEyePosition(const lmath::Matrix44& view) const
        {
            lmath::Vector4 eye;
            eye.x_ = -(view.m_[0][0] * view.m_[0][3] + view.m_[1][0] * view.m_[1][3] + view.m_[2][0] * view.m_[2][3]);
            eye.y_ = -(view.m_[0][1] * view.m_[0][3] + view.m_[1][1] * view.m_[1][3] + view.m_[2][1] * view.m_[2][3]);
            eye.z_ = -(view.m_[0][2] * view.m_[0][3] + view.m_[1][2] * view.m_[1][3] + view.m_[2][2] * view.m_[2][3]);
            eye.w_ = 0.0f;
            return eye;
        }
    };


    TEST_F(TestMatrix44, TestLookAt)
    {
        s32 resolution = 360;
        lmath::Matrix44 view, invview0, invview1;
        lmath::Vector4 eye = Vector4::construct(1.0f, 1.0f, -1.0f, 0.0f);

        for(s32 i=0; i<(resolution*2); ++i){
            Quaternion rotation;
            rotation.setRotateY(static_cast<f32>(i)/resolution*360.0f*RAD_TO_DEG);
            lookAt(view, invview0, eye, rotation);

            lmath::Vector4 p = getEyePosition(view);
            EXPECT_FLOAT_EQ(p.x_, eye.x_);
            EXPECT_FLOAT_EQ(p.y_, eye.y_);
            EXPECT_FLOAT_EQ(p.z_, eye.z_);
            EXPECT_FLOAT_EQ(p.w_, eye.w_);

            view.getInvert(invview1);
            for(s32 j=0; j<4; ++j){
                for(s32 k=0; k<4; ++k){
                    EXPECT_NEAR(invview0(j, k), invview1(j, k), 1.0e-6f);
                }
            }

            Matrix44 identity;
            identity.mul(view, invview0);
            for(s32 j=0; j<4; ++j){
                for(s32 k=0; k<4; ++k){
                    if(j==k){
                        EXPECT_NEAR(identity(j,k), 1.0f, 1.0e-5f);
                    }else{
                        EXPECT_NEAR(identity(j,k), 0.0f, 1.0e-5f);
                    }
                }
            }
        }

        lookAt(view, invview0, eye, Vector4::construct(0.0f), Vector4::Up);
        lmath::Vector4 p = getEyePosition(view);
        EXPECT_FLOAT_EQ(p.x_, eye.x_);
        EXPECT_FLOAT_EQ(p.y_, eye.y_);
        EXPECT_FLOAT_EQ(p.z_, eye.z_);
        EXPECT_FLOAT_EQ(p.w_, eye.w_);

        lcore::RandXorshift64Star32 random(lcore::getDefaultSeed64());
        lmath::Vector4 position = Vector4::construct(1.0f, 1.0f, 5.0f, 1.0f);
        for(s32 i=1; i<90; ++i){
            position.x_ = random.frand()*2.0f - 1.0f;
            lmath::Vector4 at = position;
            at.x_ = random.frand()*2.0f - 1.0f;
            at.w_ = 0.0f;
            //lookAt(view, invview0, lmath::Vector4(0.1f, 0.1f, -1.0f, 0.0f), at, lmath::Vector4::Up);
            lookAt(view, invview0, at);
            view.getInvert(invview1);

            for(s32 j=0; j<4; ++j){
                for(s32 k=0; k<4; ++k){
                    EXPECT_NEAR(invview0(j, k), invview1(j, k), 1.0e-6f);
                }
            }

            Matrix44 identity;
            identity.mul(invview0, view);
            for(s32 j=0; j<4; ++j){
                for(s32 k=0; k<4; ++k){
                    if(j==k){
                        EXPECT_NEAR(identity(j,k), 1.0f, 1.0e-6f);
                    }else{
                        EXPECT_NEAR(identity(j,k), 0.0f, 1.0e-6f);
                    }
                }
            }

            lmath::Vector4 viewposition = Vector4::construct(mul(view, position));
            lmath::Vector4 worldposition = Vector4::construct(mul(invview0, viewposition));
            EXPECT_FLOAT_EQ(position.x_, worldposition.x_);
            EXPECT_FLOAT_EQ(position.y_, worldposition.y_);
            EXPECT_FLOAT_EQ(position.z_, worldposition.z_);
        }
    }

    TEST_F(TestMatrix44, TestPerspective)
    {
        lmath::Matrix44 proj, invproj0, invproj1;

        for(s32 i=1; i<90; ++i){
            f32 fovy = static_cast<f32>(i) * DEG_TO_RAD;
            perspectiveFovReverseZ(proj, invproj0, fovy, 800.0f/600.0f, 0.1f, 100.0f);
            proj.getInvert(invproj1);

            for(s32 j=0; j<4; ++j){
                for(s32 k=0; k<4; ++k){
                    EXPECT_FLOAT_EQ(invproj0(j, k), invproj1(j, k));
                }
            }
        }
    }

    TEST_F(TestMatrix44, TestInvViewProjection)
    {
        lcore::RandXorshift64Star32 random;
        random.srand(lcore::getDefaultSeed64());

        lmath::Matrix44 proj, invproj;
        lmath::Matrix44 view, invview;
        lmath::Matrix44 vp, invvp;

        for(s32 i=1; i<90; ++i){
            f32 fovy = static_cast<f32>(i) * DEG_TO_RAD;
            perspectiveFovReverseZ(proj, invproj, fovy, 800.0f/600.0f, 0.1f, 1000.0f);

            for(s32 j=-10; j<10; ++j){
                lookAt(view, invview, Vector4::construct(random.frand()*2.0f-1.0f, random.frand()*2.0f-1.0f, -0.1f), Vector4::construct(0.0f), Vector4::Up);
                vp.mul(proj, view);
                vp.getInvert(invvp);

                for(s32 k=0; k<100; ++k){
                    Vector4 p0 = Vector4::construct(random.frand()*10.0f-5.0f, random.frand()*10.0f-5.0f, k*0.01f, 1.0f);
                    Vector4 sp = Vector4::construct(mul(vp, p0));
                    sp /= sp.w_;
                    Vector4 p1 = Vector4::construct(mul(invvp, sp));
                    p1 /= p1.w_;
                    EXPECT_NEAR(p0.x_, p1.x_, 1.0e-5f);
                    EXPECT_NEAR(p0.y_, p1.y_, 1.0e-5f);
                    EXPECT_NEAR(p0.z_, p1.z_, 1.0e-5f);
                }
            }
        }//for(s32 i
    }

    TEST_F(TestMatrix44, TestRestoration)
    {
        lcore::RandXorshift64Star32 random;

        lmath::Matrix44 proj, invproj;
        lmath::Matrix44 viewproj, invviewproj;
        lmath::Vector4 position = Vector4::construct(1.0f, 1.0f, 5.0f, 1.0f);

        for(s32 i=1; i<90; ++i){
            position.x_ = random.frand()*2.0f - 1.0f;
            f32 fovy = static_cast<f32>(i) * DEG_TO_RAD;
            perspectiveFovReverseZ(proj, invproj, fovy, 800.0f/600.0f, 0.1f, 100.0f);
            lmath::Vector4 screenposition = Vector4::construct(mul(proj, position));
            screenposition /= screenposition.w_;
            if(screenposition.x_<-1.0f || 1.0f<screenposition.x_){
                continue;
            }
            if(screenposition.y_<-1.0f || 1.0f<screenposition.y_){
                continue;
            }
            lmath::Vector4 viewposition = Vector4::construct(mul(invproj, screenposition));
            viewposition /= viewposition.w_;
            EXPECT_FLOAT_EQ(position.x_, viewposition.x_);
            EXPECT_FLOAT_EQ(position.y_, viewposition.y_);
            EXPECT_FLOAT_EQ(position.z_, viewposition.z_);
        }

        for(s32 i=1; i<90; ++i){
            position.x_ = random.frand()*2.0f - 1.0f;
            lmath::Matrix44 view, invview;
            lmath::Vector4 at = position;
            at.x_ = random.frand()*2.0f - 1.0f;
            at.w_ = 0.0f;
            lookAt(view, invview, Vector4::construct(0.1f, 0.1f, -1.0f, 0.0f), at, lmath::Vector4::Up);
            lmath::Vector4 viewposition = Vector4::construct(mul(view, position));
            lmath::Vector4 worldposition = Vector4::construct(mul(invview, viewposition));
            EXPECT_FLOAT_EQ(position.x_, worldposition.x_);
            EXPECT_FLOAT_EQ(position.y_, worldposition.y_);
            EXPECT_FLOAT_EQ(position.z_, worldposition.z_);
        }

        for(s32 i=1; i<90; ++i){
            position.x_ = random.frand()*2.0f - 1.0f;
            f32 fovy = static_cast<f32>(i) * DEG_TO_RAD;
            perspectiveFovReverseZ(proj, invproj, fovy, 800.0f/600.0f, 0.1f, 100.0f);
            lmath::Matrix44 view, invview;
            lmath::Vector4 at = position;
            at.x_ = random.frand()*2.0f - 1.0f;
            at.w_ = 0.0f;
            lookAt(view, invview, lmath::Vector4::construct(0.1f, 0.1f, -1.0f, 0.0f), at, lmath::Vector4::Up);
            viewproj.mul(proj, view);
            invviewproj.mul(invview, invproj);
            lmath::Vector4 screenposition = Vector4::construct(mul(viewproj, position));
            screenposition /= screenposition.w_;
            if(screenposition.x_<-1.0f || 1.0f<screenposition.x_){
                continue;
            }
            if(screenposition.y_<-1.0f || 1.0f<screenposition.y_){
                continue;
            }
            lm128 viewpos = mul(invproj, screenposition);
            viewpos = mul(invview, Vector4::construct(viewpos));
            lmath::Vector4 viewposition = Vector4::construct(viewpos);
            viewpos = viewpos / viewposition.w_;
            viewposition = Vector4::construct(viewpos);
            EXPECT_FLOAT_EQ(position.x_, viewposition.x_);
            EXPECT_FLOAT_EQ(position.y_, viewposition.y_);
            EXPECT_FLOAT_EQ(position.z_, viewposition.z_);
        }
    }
}

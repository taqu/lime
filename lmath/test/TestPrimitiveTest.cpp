#include <catch_wrap.hpp>
#include "lmath.h"
#include "geometry/PrimitiveTest.h"
#include <lcore/Random.h>

namespace lmath
{
    TEST_CASE("TestPrimitiveTest::TestPointSegment")
    {
        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());

        Vector3 p, l1,l0;
        f32 t,l,d;
        Vector3 result;

        p = Vector3::construct(0.5f, 0.5f, 0.5f);
        l0 = Vector3::construct(-0.5f, -0.5f, 0.0f);
        l1 = Vector3::construct(0.0f, -0.5f, -0.5f);
        t = closestPointPointSegment(result, p, l0, l1);
        l = distancePointSegment(p, l0, l1);
        d = distance(p, result);
        EXPECT_FLOAT_EQ(t, 0.5f);
        EXPECT_FLOAT_EQ(l, d);
        p = t*(l1-l0) + l0;
        EXPECT_FLOAT_EQ(result.x_, p.x_);
        EXPECT_FLOAT_EQ(result.y_, p.y_);
        EXPECT_FLOAT_EQ(result.z_, p.z_);

        p = Vector3::construct(-1.0f, 0.5f, 0.0f);
        l0 = Vector3::construct(-0.5f, -0.5f, 0.0f);
        l1 = Vector3::construct(0.0f, -0.5f, -0.5f);
        t = closestPointPointSegment(result, p, l0, l1);
        l = distancePointSegment(p, l0, l1);
        d = distance(p, result);
        EXPECT_FLOAT_EQ(t, 0.0f);
        EXPECT_FLOAT_EQ(l, d);
        p = t*(l1-l0) + l0;
        EXPECT_FLOAT_EQ(result.x_, p.x_);
        EXPECT_FLOAT_EQ(result.y_, p.y_);
        EXPECT_FLOAT_EQ(result.z_, p.z_);

        p = Vector3::construct(0.0f, 0.5f, -1.0f);
        l0 = Vector3::construct(-0.5f, -0.5f, 0.0f);
        l1 = Vector3::construct(0.0f, -0.5f, -0.5f);
        t = closestPointPointSegment(result, p, l0, l1);
        l = distancePointSegment(p, l0, l1);
        d = distance(p, result);
        EXPECT_FLOAT_EQ(t, 1.0f);
        EXPECT_FLOAT_EQ(l, d);
        p = t*(l1-l0) + l0;
        EXPECT_FLOAT_EQ(result.x_, p.x_);
        EXPECT_FLOAT_EQ(result.y_, p.y_);
        EXPECT_FLOAT_EQ(result.z_, p.z_);

        static const s32 NumSamples = 1024;
        for(s32 i=0; i<NumSamples; ++i){
            f32 x = 2.0f*random.frand()-1.0f;
            f32 z = 2.0f*random.frand()-1.0f;

            p = Vector3::construct(x, 0.5f, z);
            t = closestPointPointSegment(result, p, l0, l1);
            l = distancePointSegment(p, l0, l1);
            d = distance(p, result);
            EXPECT_LE(t,1.0f);
            EXPECT_GE(t,0.0f);
            EXPECT_FLOAT_EQ(l, d);
            p = t*(l1-l0) + l0;
            EXPECT_FLOAT_EQ(result.x_, p.x_);
            EXPECT_FLOAT_EQ(result.y_, p.y_);
            EXPECT_FLOAT_EQ(result.z_, p.z_);
        }
    }

    TEST_CASE("TestPrimitiveTest::TestPointLine")
    {
        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());

        Vector3 p, l1,l0;
        f32 t,l,d;
        Vector3 result;

        p = Vector3::construct(0.5f, 0.5f, 0.5f);
        l0 = Vector3::construct(-0.5f, -0.5f, 0.0f);
        l1 = Vector3::construct(0.0f, -0.5f, -0.5f);
        t = closestPointPointLine(result, p, l0, l1);
        l = distancePointLine(p, l0, l1);
        d = distance(p, result);
        EXPECT_FLOAT_EQ(t, 0.5f);
        EXPECT_FLOAT_EQ(l, d);
        EXPECT_FLOAT_EQ(dot(l1-l0,result-p), 0.0f);
        p = t*(l1-l0) + l0;
        EXPECT_FLOAT_EQ(result.x_, p.x_);
        EXPECT_FLOAT_EQ(result.y_, p.y_);
        EXPECT_FLOAT_EQ(result.z_, p.z_);

        p = Vector3::construct(-1.0f, 0.5f, 0.0f);
        l0 = Vector3::construct(-0.5f, -0.5f, 0.0f);
        l1 = Vector3::construct(0.0f, -0.5f, -0.5f);
        t = closestPointPointLine(result, p, l0, l1);
        l = distancePointLine(p, l0, l1);
        d = distance(p, result);
        EXPECT_FLOAT_EQ(t, -0.5f);
        EXPECT_FLOAT_EQ(l, d);
        EXPECT_FLOAT_EQ(dot(l1-l0,result-p), 0.0f);
        p = t*(l1-l0) + l0;
        EXPECT_FLOAT_EQ(result.x_, p.x_);
        EXPECT_FLOAT_EQ(result.y_, p.y_);
        EXPECT_FLOAT_EQ(result.z_, p.z_);

        p = Vector3::construct(0.0f, 0.5f, -1.0f);
        l0 = Vector3::construct(-0.5f, -0.5f, 0.0f);
        l1 = Vector3::construct(0.0f, -0.5f, -0.5f);
        t = closestPointPointLine(result, p, l0, l1);
        l = distancePointLine(p, l0, l1);
        d = distance(p, result);
        EXPECT_FLOAT_EQ(t, 1.5f);
        EXPECT_FLOAT_EQ(dot(l1-l0,result-p), 0.0f);
        EXPECT_FLOAT_EQ(l, d);
        p = t*(l1-l0) + l0;
        EXPECT_FLOAT_EQ(result.x_, p.x_);
        EXPECT_FLOAT_EQ(result.y_, p.y_);
        EXPECT_FLOAT_EQ(result.z_, p.z_);

        static const s32 NumSamples = 1024;
        for(s32 i=0; i<NumSamples; ++i){
            f32 x = 2.0f*random.frand()-1.0f;
            f32 z = 2.0f*random.frand()-1.0f;

            p = Vector3::construct(x, 0.5f, z);
            t = closestPointPointLine(result, p, l0, l1);
            l = distancePointLine(p, l0, l1);
            d = distance(p, result);
            EXPECT_LE(t,3.0f);
            EXPECT_GE(t,-3.0f);
            EXPECT_FLOAT_EQ(l, d);
            EXPECT_FLOAT_EQ(dot(l1-l0,result-p), 0.0f);
            p = t*(l1-l0) + l0;
            EXPECT_FLOAT_EQ(result.x_, p.x_);
            EXPECT_FLOAT_EQ(result.y_, p.y_);
            EXPECT_FLOAT_EQ(result.z_, p.z_);
        }
    }
}

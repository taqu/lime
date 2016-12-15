#include <gtest/gtest.h>
#include <lcore/Random.h>
#include "lmath.h"
#include "Vector4.h"
#include "Quaternion.h"

namespace lmath
{
    class TestQuaternion : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

    };

    TEST_F(TestQuaternion, TestDirection)
    {
        lcore::RandXorshift random;
        random.srand(lcore::getDefaultSeed());
        Quaternion quat;
        lmath::Vector4 direction;
        for(s32 i=-100; i<100; ++i){
            direction.set(1.0f, static_cast<f32>(i)/100.0f, random.frand(), 0.0f);
            direction = normalize(direction);
            quat.lookAt(direction);
            lmath::Vector4 d;
            quat.getDireciton(d);
            EXPECT_NEAR(d.x_, direction.x_, 1.0e-5f);
            EXPECT_NEAR(d.y_, direction.y_, 1.0e-5f);
            EXPECT_NEAR(d.z_, direction.z_, 1.0e-5f);
            EXPECT_NEAR(d.w_, direction.w_, 1.0e-5f);
        }
    }
}

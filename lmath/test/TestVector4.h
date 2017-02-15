#include <gtest/gtest.h>
#include "lmath.h"
#include "Vector4.h"
#include "Matrix44.h"

namespace lmath
{
    class TestVector4 : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };


    TEST_F(TestVector4, Test_mulPoint)
    {
        Vector4 v = Vector4::construct(4.0f, 3.0f, 2.0f, 1.0f);
        Matrix44 m;
        m.identity();

        Vector4 result = mulPoint(m, v);
        EXPECT_FLOAT_EQ(result.x_, 4.0f);
        EXPECT_FLOAT_EQ(result.y_, 3.0f);
        EXPECT_FLOAT_EQ(result.z_, 2.0f);
        EXPECT_FLOAT_EQ(result.w_, 1.0f);
    }

    TEST_F(TestVector4, Test_manhattanDistance3)
    {
        Vector4 v0 = Vector4::construct(4.0f, 3.0f, 2.0f, 1.0f);
        Vector4 v1 = Vector4::construct(3.0f, 2.0f, 1.0f, 0.0f);

        f32 distance = manhattanDistance3(v0, v1);

        EXPECT_FLOAT_EQ(distance, 3.0f);
    }
}

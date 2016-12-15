#include <gtest/gtest.h>
#include "lcore.h"

namespace lcore
{
    class TestCore : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

    };

    TEST_F(TestCore, Clamp01)
    {
        f32 result;
        result = lcore::clamp01(0.1f);
        EXPECT_FLOAT_EQ(result, 0.1f);

        result = lcore::clamp01(-1.0e+5f);
        EXPECT_FLOAT_EQ(result, 0.0f);

        result = lcore::clamp01(1.0e+5f);
        EXPECT_FLOAT_EQ(result, 1.0f);
    }

    TEST_F(TestCore, SameSign)
    {
        EXPECT_TRUE(lcore::isSameSign(1.0f, 1.0f));
        EXPECT_TRUE(lcore::isSameSign(-1.0f, -1.0f));
        EXPECT_FALSE(lcore::isSameSign(1.0f, -1.0f));
        EXPECT_FALSE(lcore::isSameSign(-1.0f, 1.0f));

        EXPECT_TRUE(lcore::isSameSign(1.0, 1.0));
        EXPECT_TRUE(lcore::isSameSign(-1.0, -1.0));
        EXPECT_FALSE(lcore::isSameSign(1.0, -1.0));
        EXPECT_FALSE(lcore::isSameSign(-1.0, 1.0));
    }
}

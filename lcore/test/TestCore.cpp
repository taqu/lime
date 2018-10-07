#include <catch_wrap.hpp>
#include "lcore.h"

namespace lcore
{
    TEST_CASE("TestCore::Clamp01")
    {
        f32 result;
        result = clamp01(0.1f);
        EXPECT_FLOAT_EQ(result, 1.0f);

        result = clamp01(-1.0e+5f);
        EXPECT_FLOAT_EQ(result, 0.0f);

        result = clamp01(1.0e+5f);
        EXPECT_FLOAT_EQ(result, 1.0f);
    }

    TEST_CASE("TestCore::SameSign")
    {
        CHECK(isSameSign(1.0f, 1.0f));
        CHECK(isSameSign(-1.0f, -1.0f));
        CHECK_FALSE(isSameSign(1.0f, -1.0f));
        CHECK_FALSE(isSameSign(-1.0f, 1.0f));

        CHECK(isSameSign(1.0, 1.0));
        CHECK(isSameSign(-1.0, -1.0));
        CHECK_FALSE(isSameSign(1.0, -1.0));
        CHECK_FALSE(isSameSign(-1.0, 1.0));
    }
}

#include <catch_wrap.hpp>
#include "lmath.h"
#include "Vector4.h"

namespace lmath
{
    TEST_CASE("TestMath::TestSphericalCartesian")
    {
        Vector4 v;
        lmath::sphericalToCartesian(
            v,
            DEG_TO_RAD*45.0f,
            DEG_TO_RAD*45.0f,
            Vector4::Right,
            Vector4::Up,
            Vector4::Forward);

        EXPECT_FLOAT_EQ(v.x_, 0.5f);
        EXPECT_FLOAT_EQ(v.y_, 0.5f);
        EXPECT_FLOAT_EQ(v.z_, 0.707106769f);
        EXPECT_FLOAT_EQ(v.w_, 0.0f);
    }
}

#include <catch_wrap.hpp>
#include "lmath.h"
#include "Vector3.h"

namespace lmath
{
    TEST_CASE("TestVector3::muladd")
    {
        const f32 a = 3.0f;
        Vector3 v0 = Vector3::construct(3.0f, 2.0f, 1.0f);
        Vector3 v1 = Vector3::construct(3.0f, 2.0f, 1.0f);
        Vector3 v2 = Vector3::construct(2.0f, 2.0f, 2.0f);

        Vector3 a0 = muladd(v2, v0, v1);
        Vector3 aa0 = Vector3::construct(9.0f, 6.0f, 3.0f);
        EXPECT_TRUE(a0 == aa0); 

        Vector3 a1 = muladd(a, v0, v1);
        Vector3 aa1 = Vector3::construct(12.0f, 8.0f, 4.0f);
        EXPECT_TRUE(a1 == aa1);
    }
}

#include <catch_wrap.hpp>
#include "Any.h"

namespace lcore
{
    TEST_CASE("TestAny::Construct")
    {
        {
            Any any;
            EXPECT_EQ(any.getType(), Any::Type_None);
        }

        {
            Any any(0);
            EXPECT_EQ(any.getType(), Any::Type_Int);
            EXPECT_EQ((s32)any, 0);
        }

        {
            Any any(0.0f);
            EXPECT_EQ(any.getType(), Any::Type_Float);
            EXPECT_FLOAT_EQ((f32)any, 0.0f);
        }

        {
            Any any("test");
            EXPECT_EQ(any.getType(), Any::Type_String);
            EXPECT_STR_EQ((const Char*)any, "test");
            EXPECT_EQ(4, any.length());
        }
    }

    TEST_CASE("TestAny::Substitute")
    {
        Any any;
        EXPECT_EQ(any.getType(), Any::Type_None);

        {
            any = 0;
            EXPECT_EQ(any.getType(), Any::Type_Int);
            EXPECT_EQ((s32)any, 0);
        }

        {
            any = 0.0f;
            EXPECT_EQ(any.getType(), Any::Type_Float);
            EXPECT_FLOAT_EQ((f32)any, 0.0f);
        }

        {
            any = "test";
            EXPECT_EQ(any.getType(), Any::Type_String);
            EXPECT_STR_EQ((const Char*)any, "test");
            EXPECT_EQ(4, any.length());
        }

        {
            Any anyInt(0);
            any = anyInt;
            EXPECT_EQ(any.getType(), Any::Type_Int);
            EXPECT_EQ((s32)any, 0);
        }

        {
            Any anyFloat(0.0f);
            any = anyFloat;
            EXPECT_EQ(any.getType(), Any::Type_Float);
            EXPECT_FLOAT_EQ((f32)any, 0.0f);
        }

        {
            Any anyString("test");
            any = anyString;
            EXPECT_EQ(any.getType(), Any::Type_String);
            EXPECT_STR_EQ((const Char*)any, "test");
            EXPECT_EQ(4, any.length());
        }
    }
}

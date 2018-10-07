#include <catch_wrap.hpp>
#include "lstring.h"

namespace lcore
{
    TEST_CASE("TestString::Construct00")
    {
        String empty;
        EXPECT_EQ(String::ExpandSize, empty.capacity());
        EXPECT_EQ(0, empty.length());
        EXPECT_STR_EQ("", empty.c_str());
    }

    TEST_CASE("TestString::Construct01")
    {
        String str(32);
        EXPECT_EQ(32, str.capacity());
        EXPECT_EQ(0, str.length());
        EXPECT_STR_EQ("", str.c_str());
    }

    TEST_CASE("TestString::Construct02")
    {
        String str("test123");
        EXPECT_EQ(8, str.capacity());
        EXPECT_EQ(7, str.length());
        EXPECT_STR_EQ("test123", str.c_str());
    }


    TEST_CASE("TestString::Construct03")
    {
        const Char* testStr = "abcdefgh";
        int length = strlen_s32(testStr);
        String str(testStr);
        EXPECT_EQ(static_cast<s32>((length+1+7)&~0x07U), str.capacity());
        EXPECT_EQ(length, str.length());
        EXPECT_STR_EQ(testStr, str.c_str());
    }

    TEST_CASE("estString::Construct04")
    {
        const Char* testStr = "abcdefgh";
        String str(testStr);
        String str2(str);
        EXPECT_EQ(str.capacity(), str2.capacity());
        EXPECT_EQ(str.length(), str2.length());
        EXPECT_STR_EQ(str.c_str(), str2.c_str());
    }

    TEST_CASE("TestString::Substitute")
    {
        const Char* testStr = "abcdefgh";
        String str;
        String str2;
        str = testStr;
        str2 = str;
        EXPECT_EQ(str.capacity(), str2.capacity());
        EXPECT_EQ(str.length(), str2.length());
        EXPECT_STR_EQ(str.c_str(), str2.c_str());
    }

    TEST_CASE("TestString::Compare")
    {
        String str0("abcdefgh");
        String str1("abc");

        EXPECT_EQ(false, str0==str1);
        EXPECT_EQ(true, str0!=str1);
        EXPECT_GT(lcore::compare(str0, str1), 0);
    }

    TEST_CASE("TestString::Assign")
    {
        String str0("abcdefghij");
        str0.assign(7, "abcdefgh", toupper);

        EXPECT_EQ(7, str0.length());
        EXPECT_EQ(16, str0.capacity());
        EXPECT_STR_EQ("ABCDEFG", str0.c_str());
    }

    TEST_CASE("TestString::Append")
    {
        String str0("abcdefghij");
        str0.append(3, "abc");

        EXPECT_EQ(13, str0.length());
        EXPECT_EQ(16, str0.capacity());
        EXPECT_STR_EQ("abcdefghijabc", str0.c_str());
    }

    TEST_CASE("TestString::Print")
    {
        String str0("abcdefghij");
        str0.append(3, "abc");
        String str1("abc");
        str1.printf("%s%s%s%s", "abc", "def", "ghij", "abc");

        EXPECT_STR_EQ(str0.c_str(), str1.c_str());
    }

    TEST_CASE("TestString::Fill")
    {
        String str0("aaaaaaaaaa");
        String str1;
        str1.fill(10, 'a');

        EXPECT_STR_EQ(str0.c_str(), str1.c_str());
    }

    TEST_CASE("TestString::Mix")
    {
        String str0("abcdefghij");
        str0.append(3, "abc");
        String str1("abc");
        str1.printf("%s%s%s", "abc", "def", "ghijabc");
        EXPECT_STR_EQ(str0.c_str(), str1.c_str());

        str0.assign(3, "abc");
        str1.assign(3, "abc");
        EXPECT_STR_EQ(str0.c_str(), str1.c_str());

        str0.clear(24);
        str1.clear();
        EXPECT_STR_EQ(str0.c_str(), str1.c_str());

        str0.append(4, "abcd");
        str0.pop_back();
        str1.printf("a%s", "bc");
        EXPECT_STR_EQ(str0.c_str(), str1.c_str());
    }
}

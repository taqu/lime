#include <gtest/gtest.h>
#include "lstring.h"

namespace lcore
{
    class TestString : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

    };

    TEST_F(TestString, Construct00)
    {
        String empty;
        EXPECT_EQ(String::ExpandSize, empty.capacity());
        EXPECT_EQ(0, empty.length());
        EXPECT_STREQ("", empty.c_str());
    }

    TEST_F(TestString, Construct01)
    {
        String str(32);
        EXPECT_EQ(32, str.capacity());
        EXPECT_EQ(0, str.length());
        EXPECT_STREQ("", str.c_str());
    }

    TEST_F(TestString, Construct02)
    {
        String str("test123");
        EXPECT_EQ(8, str.capacity());
        EXPECT_EQ(7, str.length());
        EXPECT_STREQ("test123", str.c_str());
    }


    TEST_F(TestString, Construct03)
    {
        const Char* testStr = "abcdefgh";
        int length = strlen_s32(testStr);
        String str(testStr);
        EXPECT_EQ((length+1+7)&~0x07U, str.capacity());
        EXPECT_EQ(length, str.length());
        EXPECT_STREQ(testStr, str.c_str());
    }

    TEST_F(TestString, Construct04)
    {
        const Char* testStr = "abcdefgh";
        String str(testStr);
        String str2(str);
        EXPECT_EQ(str.capacity(), str2.capacity());
        EXPECT_EQ(str.length(), str2.length());
        EXPECT_STREQ(str.c_str(), str2.c_str());
    }

    TEST_F(TestString, Substitute)
    {
        const Char* testStr = "abcdefgh";
        String str;
        String str2;
        str = testStr;
        str2 = str;
        EXPECT_EQ(str.capacity(), str2.capacity());
        EXPECT_EQ(str.length(), str2.length());
        EXPECT_STREQ(str.c_str(), str2.c_str());
    }

    TEST_F(TestString, Compare)
    {
        String str0("abcdefgh");
        String str1("abc");

        EXPECT_EQ(false, str0==str1);
        EXPECT_EQ(true, str0!=str1);
        EXPECT_GT(lcore::compare(str0, str1), 0);
    }

    TEST_F(TestString, Assign)
    {
        String str0("abcdefghij");
        str0.assign(7, "abcdefgh", toupper);

        EXPECT_EQ(7, str0.length());
        EXPECT_EQ(16, str0.capacity());
        EXPECT_STREQ("ABCDEFG", str0.c_str());
    }

    TEST_F(TestString, Append)
    {
        String str0("abcdefghij");
        str0.append(3, "abc");

        EXPECT_EQ(13, str0.length());
        EXPECT_EQ(16, str0.capacity());
        EXPECT_STREQ("abcdefghijabc", str0.c_str());
    }

    TEST_F(TestString, Print)
    {
        String str0("abcdefghij");
        str0.append(3, "abc");
        String str1("abc");
        str1.printf("%s%s%sabc", "abc", "def", "ghij");

        EXPECT_STREQ(str0.c_str(), str1.c_str());
    }

    TEST_F(TestString, Fill)
    {
        String str0("aaaaaaaaaa");
        String str1;
        str1.fill(10, 'a');

        EXPECT_STREQ(str0.c_str(), str1.c_str());
    }

    TEST_F(TestString, Mix)
    {
        String str0("abcdefghij");
        str0.append(3, "abc");
        String str1("abc");
        str1.printf("%s%s%s", "abc", "def", "ghijabc");
        EXPECT_STREQ(str0.c_str(), str1.c_str());

        str0.assign(3, "abc");
        str1.assign(3, "abc");
        EXPECT_STREQ(str0.c_str(), str1.c_str());

        str0.clear(24);
        str1.clear();
        EXPECT_STREQ(str0.c_str(), str1.c_str());

        str0.append(4, "abcd");
        str0.pop_back();
        str1.printf("a%s", "bc");
        EXPECT_STREQ(str0.c_str(), str1.c_str());
    }
}

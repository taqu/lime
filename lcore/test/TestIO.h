#include <gtest/gtest.h>
#include "io/lio.h"
#include "LString.h"

namespace lcore
{
    class TestIO : public ::testing::Test
    {
    protected:

        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F(TestIO, Path)
    {
        EXPECT_TRUE(Path::exists("./TestIO.h"));
        EXPECT_FALSE(Path::exists("./NONE"));
        EXPECT_TRUE(Path::exists(lcore::String(".")));
        EXPECT_FALSE(Path::exists(lcore::String("./NONEDIRECTORY")));

        EXPECT_TRUE(Path::isFile("./TestIO.h"));
        EXPECT_FALSE(Path::isFile("."));
        EXPECT_FALSE(Path::isFile("./NONE"));
        EXPECT_TRUE(Path::isDirectory("."));
        EXPECT_FALSE(Path::isDirectory("./TestIO.h"));
        EXPECT_FALSE(Path::isDirectory("./NONE"));
        EXPECT_TRUE(Path::isNormalDirectory("data"));
        EXPECT_TRUE(Path::isSpecialDirectory("./."));

#if _WIN32 || _WIN64
        EXPECT_TRUE(Path::isRoot("D://"));
        EXPECT_FALSE(Path::isRoot("/"));
        EXPECT_TRUE(Path::isAbsolute("D://"));
        EXPECT_FALSE(Path::isAbsolute("/"));
#else
        EXPECT_FALSE(Path::isRoot("D://"));
        EXPECT_TRUE(Path::isRoot("/"));
        EXPECT_FALSE(Path::isAbsolute("D://"));
        EXPECT_TRUE(Path::isAbsolute("/"));
#endif
        lcore::String filename;
        Path::getFilename(filename, lcore::strlen_s32("/path/to/test.txt"), "/path/to/test.txt");
        EXPECT_STREQ(filename.c_str(), "test.txt");

        lcore::String current, path, directoryName;
        Path::getCurrentDirectory(current);
        Path::getDirectoryname(directoryName, current.length(), current.c_str());
        EXPECT_STREQ(directoryName.c_str(), "test");
        Path::setCurrentDirectory("..");
        Path::getCurrentDirectory(path);
        Path::getDirectoryname(directoryName, path.length(), path.c_str());
        EXPECT_STREQ(directoryName.c_str(), "lcore");

        Path::setCurrentDirectory(current.c_str());
    }
}

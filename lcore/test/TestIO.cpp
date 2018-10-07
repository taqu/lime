#include <catch_wrap.hpp>
#include "File.h"
#include "LString.h"

namespace lcore
{
    TEST_CASE("TestIO::Path")
    {
        String current, path, filename, directoryname;
        Path::getCurrentDirectory(current);
        CHECK(Path::exists("./TestIO.cpp"));
        CHECK_FALSE(Path::exists("./NONE"));
        CHECK(Path::exists(String(".")));
        CHECK_FALSE(Path::exists(String("./NONEDIRECTORY")));

        CHECK(Path::isFile("./TestIO.cpp"));
        CHECK_FALSE(Path::isFile("."));
        CHECK_FALSE(Path::isFile("./NONE"));
        CHECK(Path::isDirectory("."));
        CHECK_FALSE(Path::isDirectory("./TestIO.cpp"));
        CHECK_FALSE(Path::isDirectory("./NONE"));
        CHECK(Path::isNormalDirectory("data"));
        CHECK(Path::isSpecialDirectory("./."));

#if _WIN32 || _WIN64
        CHECK(Path::isRoot("D://"));
        CHECK_FALSE(Path::isRoot("/"));
        CHECK(Path::isAbsolute("D://"));
        CHECK_FALSE(Path::isAbsolute("/"));
#else
        CHECK_FALSE(Path::isRoot("D://"));
        CHECK(Path::isRoot("/"));
        CHECK_FALSE(Path::isAbsolute("D://"));
        CHECK(Path::isAbsolute("/"));
#endif
        Path::getFilename(filename, lcore::strlen_s32("/path/to/test.txt"), "/path/to/test.txt");
        EXPECT_STR_EQ(filename.c_str(), "test.txt");

        Path::getCurrentDirectory(current);
        Path::getDirectoryname(directoryname, current.length(), current.c_str());
        EXPECT_STR_EQ(directoryname.c_str(), "test");
        Path::setCurrentDirectory("..");
        Path::getCurrentDirectory(path);
        Path::getDirectoryname(directoryname, path.length(), path.c_str());
        EXPECT_STR_EQ(directoryname.c_str(), "lcore");

        Path::setCurrentDirectory(current.c_str());
    }
}

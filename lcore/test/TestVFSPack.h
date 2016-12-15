#include <gtest/gtest.h>

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#endif
#include "io/VFSPack.h"

namespace lcore
{
    class TestVFSPack : public ::testing::Test
    {
    protected:

        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F(TestVFSPack, write)
    {
        if(!Path::exists("data.lpak")){
            bool result;
            result = lcore::writeVFSPack("data.lpak", "data");
            EXPECT_TRUE(result);
        }
    }

    TEST_F(TestVFSPack, read)
    {
        bool result;
        VFSPack vfsPack;
        result = lcore::readVFSPack(vfsPack, "data.lpak", true);
        EXPECT_TRUE(result);
        CloseHandle(vfsPack.handle_);
        LDELETE_ARRAY(vfsPack.entries_);
        LDELETE_ARRAY(vfsPack.stringBuffer_);
    }

}

#include <catch_wrap.hpp>

#include "VFSPack.h"
#include "File.h"

namespace lcore
{
    TEST_CASE("TestVFSPack::write")
    {
        if(!Path::exists("data.lpak")){
            bool result;
            result = lcore::writeVFSPack("data.lpak", "data");
            CHECK(result);
        }
    }

    TEST_CASE("TestVFSPack::read")
    {
        bool result;
        VFSPack vfsPack;
        result = lcore::readVFSPack(vfsPack, "data.lpak", true);
        CHECK(result);
        CloseHandle(vfsPack.handle_);
        LDELETE_ARRAY(vfsPack.entries_);
        LDELETE_ARRAY(vfsPack.stringBuffer_);
    }
}

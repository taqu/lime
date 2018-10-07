#include <catch_wrap.hpp>
#include "Configuration.h"

namespace lcore
{
    TEST_CASE("TestConfiguration::Load")
    {
        Configuration configuration;
        EXPECT_EQ(configuration.load("Unknown.conf"), false);
        EXPECT_EQ(configuration.load("Config.conf"), true);

        EXPECT_EQ(configuration.get("ikey0", -1), 0);
        EXPECT_EQ(configuration.get("ikey1", -1), 10);
        EXPECT_EQ(configuration.get("ikey2", -1), 200);
        EXPECT_FLOAT_EQ(configuration.get("fkey0", -1.0f), 0.0f);
        EXPECT_FLOAT_EQ(configuration.get("fkey1", -1.0f), 0.1f);
        EXPECT_FLOAT_EQ(configuration.get("fkey2", -1.0f), 0.002f);
        Configuration::StringPair strPair;
        strPair = configuration.get("skey0", "");
        EXPECT_STR_EQ(strPair.str_, "value0");
    }
}

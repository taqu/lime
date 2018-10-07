#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include <lcore/lcore.h>

int main(int argc, char** argv)
{
    lcore::initializeSystem();
    int result = Catch::Session().run(argc, argv);
    lcore::terminateSystem();
    return result;
}

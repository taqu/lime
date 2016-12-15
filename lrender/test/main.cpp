#include <gtest/gtest.h>
#include <lcore/lcore.h>
#include "TestKDTree.h"

int main(int argc, char** argv)
{
    lcore::initializeSystem();
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    lcore::terminateSystem();
    return result;
}

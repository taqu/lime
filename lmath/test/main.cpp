#include <gtest/gtest.h>
#include <lcore/lcore.h>
#include "TestMath.h"
#include "TestVector4.h"
#include "TestMatrix44.h"
#include "TestQuaternion.h"
#include "TestRay.h"

int main(int argc, char** argv)
{
    lcore::initializeSystem();
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    lcore::terminateSystem();
    return result;
}

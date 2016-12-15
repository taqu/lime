#include <gtest/gtest.h>
#include <lcore/lcore.h>
#include "ecs/lecs.h"
#include "TestHandleBaseAllocator.h"
#include "TestTree.h"
#include "TestEntity.h"
#include "TestComponent.h"
#include "TestTreeComponent.h"
#include "TestComponentBehavior.h"
#include "TestResources.h"

int main(int argc, char** argv)
{
    lcore::initializeSystem();
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    lcore::terminateSystem();
    return result;
}

#include <gtest/gtest.h>
#include "TestCore.h"
#include "TestString.h"
#include "TestArray.h"
#include "TestHandleTable.h"
#include "TestHashMap.h"
#include "TestPool.h"
#include "TestQueue.h"
#include "TestBitArray.h"
#include "TestLOUDS.h"
#include "TestAny.h"
#include "TestConfiguration.h"
#include "TestIO.h"
#include "TestVFSPack.h"
#include "TestFileSystem.h"
#include "TestSort.h"

int main(int argc, char** argv)
{
    lcore::initializeSystem();
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    lcore::terminateSystem();
    return result;
}

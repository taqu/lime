#include <catch_wrap.hpp>

#include "Thread.h"
#include "CPU.h"

namespace lcore
{
    static void threadProc(u32 /*threadId*/, void* /*data*/)
    {
    }

    TEST_CASE("TestThreadAffinity::ProcessAffinity")
    {
        LHANDLE mainThread = getCurrentThread();
        ThreadAffinity threadAffinity;
        threadAffinity.initialize();
        s32 cpuIDs[ThreadAffinity::MaxCores];

        //メインスレッドを物理コアに割り当て
        cpuIDs[0] = threadAffinity.setAffinity(mainThread, true);

        ThreadRaw threads[8];
        for(s32 i=0; i<threadAffinity.getCPUInformation().getNumCores()-1; ++i){
            bool result = threads[i].create(threadProc, NULL, true);
            CHECK(result);
            cpuIDs[i+1] = threadAffinity.setAffinity(threads[i], false);
        }
        for(s32 i=0; i<threadAffinity.getCPUInformation().getNumCores()-1; ++i){
            threads[i].start();
        }

        lcore::sleep(500);

        for(s32 i=0; i<=threadAffinity.getCPUInformation().getNumCores(); ++i){
            for(s32 j=i+1; j<=threadAffinity.getCPUInformation().getNumCores(); ++j){
                EXPECT_NE(cpuIDs[i], cpuIDs[j]);
            }
        }
    }
}

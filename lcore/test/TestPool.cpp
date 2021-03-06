#include <catch_wrap.hpp>
#include "Random.h"
#include "Pool.h"

namespace lcore
{
    TEST_CASE("TestPool::Pool")
    {
        static const s32 NumSamples = 50;
        lcore::PagePool<s32> pagePool(33);
        s32* elements[NumSamples];
        for(s32 i=0; i<NumSamples; ++i){
            elements[i] = pagePool.construct();
        }
        for(s32 i=0; i<NumSamples; ++i){
            for(s32 j=i+1; j<NumSamples; ++j){
                EXPECT_NE(elements[i], elements[j]);
            }
        }
        for(s32 i=0; i<NumSamples; ++i){
            pagePool.destruct(elements[i]);
        }
    }
}

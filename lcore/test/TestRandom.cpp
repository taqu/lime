#include <catch_wrap.hpp>
#include "lcore.h"
#include "Random.h"

namespace lcore
{
    TEST_CASE("TestRandom::TestRange")
    {
        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());
        for(s32 i=0; i<1024*1024; ++i){
            s32 sr;
            u32 ur;

            sr = range_ropen(random, -10, 10);
            EXPECT_LE(-10,sr);
            EXPECT_LT(sr,10);
            ur = range_ropen(random, 10U, 20U);
            EXPECT_LE(10U, ur);
            EXPECT_LT(ur, 20U);

            sr = range_rclose(random, -10, 10);
            EXPECT_LE(-10,sr);
            EXPECT_LE(sr,10);
            ur = range_rclose(random, 10U, 20U);
            EXPECT_LE(10U, ur);
            EXPECT_LE(ur, 20U);

        }
    }

    TEST_CASE("TestRandom::SpeedRange")
    {
        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());
        s32 numSamples = 1024*1024;
        s32* sr = LNEW s32[numSamples];

        s32 minVal = random.rand()%1024;
        s32 maxVal = random.rand()%100 + minVal;

        lcore::ClockType clockDiv = lcore::getPerformanceCounter();
        for(s32 i=0; i<numSamples; ++i){
            sr[i] = range_rclose(random, minVal, maxVal);
        }
        lcore::f64 timeDiv = lcore::calcTime64(clockDiv, lcore::getPerformanceCounter());
        for(s32 i=0; i<numSamples; ++i){
            EXPECT_LE(minVal,sr[i]);
            EXPECT_LE(sr[i],maxVal);
        }

        LOG_INFO("Div:" <<  timeDiv);
        LDELETE_ARRAY(sr);
    }
}

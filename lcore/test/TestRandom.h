#include <gtest/gtest.h>

#include "lcore.h"
#include "Random.h"

namespace lcore
{
    class TestRandom : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F(TestRandom, TestRange)
    {
        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());
        for(s32 i=0; i<1024*1024; ++i){
            s32 sr;
            u32 ur;

            sr = lcore::random::range_ropen(random, -10, 10);
            EXPECT_LE(-10,sr);
            EXPECT_LT(sr,10);
            ur = lcore::random::range_ropen(random, 10U, 20U);
            EXPECT_LE(10U, ur);
            EXPECT_LT(ur, 20U);

            sr = lcore::random::range_rclose(random, -10, 10);
            EXPECT_LE(-10,sr);
            EXPECT_LE(sr,10);
            ur = lcore::random::range_rclose(random, 10U, 20U);
            EXPECT_LE(10U, ur);
            EXPECT_LE(ur, 20U);

        }
    }

    TEST_F(TestRandom, SpeedRange)
    {
        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());
        s32 numSamples = 1024*1024;
        s32* sr = LNEW s32[numSamples];

        s32 minVal = random.rand()%1024;
        s32 maxVal = random.rand()%100 + minVal;

        lcore::ClockType clockDiv = lcore::getPerformanceCounter();
        for(s32 i=0; i<numSamples; ++i){
            sr[i] = lcore::random::range_rclose(random, minVal, maxVal);
        }
        lcore::f64 timeDiv = lcore::calcTime64(clockDiv, lcore::getPerformanceCounter());
        for(s32 i=0; i<numSamples; ++i){
            EXPECT_LE(minVal,sr[i]);
            EXPECT_LE(sr[i],maxVal);
        }

        std::cerr << "Div  : " <<  timeDiv << std::endl;
        LDELETE_ARRAY(sr);
    }
}

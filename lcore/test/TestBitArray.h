#include <gtest/gtest.h>
#include "Random.h"
#include "BitArray.h"

namespace lcore
{
    class TestBitArray : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };


    TEST_F(TestBitArray, Test00)
    {
        static const s32 NumSamples = 1111;
        RandWELL random(getDefaultSeed());
        BitArray bitArray(129);
        bool* result = LNEW bool[NumSamples];
        for(s32 i=0; i<NumSamples; ++i){
            result[i] = 0==(random.rand()&0x01U);
        }

        for(s32 i=0; i<NumSamples; ++i){
            bitArray.add(i, result[i]);
        }
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(result[i], bitArray.check(i));
        }

        for(s32 i=0; i<NumSamples; ++i){
            bitArray.reset(i);
        }
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(false, bitArray.check(i));
        }

        for(s32 i=0; i<NumSamples; ++i){
            result[i] = 0==(random.rand()&0x01U);
            if(result[i]){
                bitArray.set(i);
            }
        }

        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(result[i], bitArray.check(i));
        }
        LDELETE_ARRAY(result);
    }
}

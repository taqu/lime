#include <gtest/gtest.h>
#include "Random.h"
#include "HandleTable.h"

namespace lcore
{
    class TestHandleTable : public ::testing::Test
    {
    protected:
        typedef HandleTable<u32, 24> HandleTableType;
        typedef HandleTableType::handle_type Handle;

        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

    };

    TEST_F(TestHandleTable, HandleTable)
    {
        static const s32 NumSamples = 100;
        static const s32 HalfSamples = NumSamples>>1;

        lcore::RandXorshift random;
        HandleTableType handleTable;
        s32 order[NumSamples];
        Handle handles[NumSamples];

        for(s32 i=0; i<NumSamples; ++i){
            order[i] = i;
        }
        lcore::random::shuffle(random, order, order+NumSamples);

        for(s32 i=0; i<NumSamples; ++i){
            handles[i] = handleTable.create();
        }
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_TRUE(handleTable.valid(handles[i]));
        }
        for(s32 i=0; i<HalfSamples; ++i){
            s32 idx = order[i];
            handleTable.destroy(handles[idx]);
        }
        for(s32 i=0; i<HalfSamples; ++i){
            s32 idx = order[i];
            EXPECT_FALSE(handleTable.valid(handles[idx]));
        }
        for(s32 i=HalfSamples; i<NumSamples; ++i){
            s32 idx = order[i];
            EXPECT_TRUE(handleTable.valid(handles[idx]));
        }
        for(s32 i=HalfSamples; i<NumSamples; ++i){
            s32 idx = order[i];
            handleTable.destroy(handles[idx]);
            EXPECT_FALSE(handleTable.valid(handles[idx]));
        }
    }
}

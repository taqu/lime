#include <catch_wrap.hpp>
#include "Random.h"
#include "HandleTable.h"

namespace lcore
{
    typedef HandleTable<u32, 24> HandleTableType;

    TEST_CASE("TestHandleTable::HandleTable")
    {
        static const s32 NumSamples = 100;
        static const s32 HalfSamples = NumSamples>>1;

        lcore::RandXorshift random;
        HandleTableType handleTable;
        s32 order[NumSamples];
        HandleTableType::handle_type handles[NumSamples];

        for(s32 i=0; i<NumSamples; ++i){
            order[i] = i;
        }
        shuffle(random, order, order+NumSamples);

        for(s32 i=0; i<NumSamples; ++i){
            handles[i] = handleTable.create();
        }
        for(s32 i=0; i<NumSamples; ++i){
            CHECK(handleTable.valid(handles[i]));
        }
        for(s32 i=0; i<HalfSamples; ++i){
            s32 idx = order[i];
            handleTable.destroy(handles[idx]);
        }
        for(s32 i=0; i<HalfSamples; ++i){
            s32 idx = order[i];
            CHECK_FALSE(handleTable.valid(handles[idx]));
        }
        for(s32 i=HalfSamples; i<NumSamples; ++i){
            s32 idx = order[i];
            CHECK(handleTable.valid(handles[idx]));
        }
        for(s32 i=HalfSamples; i<NumSamples; ++i){
            s32 idx = order[i];
            handleTable.destroy(handles[idx]);
            CHECK_FALSE(handleTable.valid(handles[idx]));
        }
    }
}

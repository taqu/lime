#include <catch_wrap.hpp>
#include "lcore.h"
#include "Random.h"

namespace lcore
{
    TEST_CASE("TestColor")
    {
        static const s32 Size = 2048;
        f32* src = LNEW f32[Size*4];
        f32* dst = LNEW f32[Size*4];

        RandXorshift random;
        random.srand(getDefaultSeed());

        f32 rgba[4];
        ClockType start;
        f32 time;

        for(s32 i = 0; i<(Size<<2); i+=4){
            s32 index = i;
            src[index+0] = random.frand();
            src[index+1] = random.frand();
            src[index+2] = random.frand();
            src[index+3] = random.frand();
        }

        start = lcore::getPerformanceCounter();
        for(s32 i = 0; i<(Size<<2); i+=4){
            toLinear_NOSIMD(rgba, src+i);
            toSRGB_NOSIMD(dst+i, rgba);
        }
        time = lcore::calcTime(start, lcore::getPerformanceCounter());
        LOG_INFO("time: " << time);
        for(s32 i = 0; i<(Size<<2); ++i){
            EXPECT_FLOAT_EQ(src[i], dst[i]);
        }

        start = lcore::getPerformanceCounter();
        for(s32 i = 0; i<(Size<<2); i += 4){
            toLinear_SIMD(rgba, src+i);
            toSRGB_SIMD(dst+i, rgba);
        }
        time = lcore::calcTime(start, lcore::getPerformanceCounter());
        LOG_INFO("time: " << time);
        for(s32 i = 0; i<(Size<<2); ++i){
            EXPECT_FLOAT_EQ(src[i], dst[i]);
        }

        LDELETE_ARRAY(dst);
        LDELETE_ARRAY(src);
    }
}

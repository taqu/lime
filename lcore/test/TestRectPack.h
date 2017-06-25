#include <gtest/gtest.h>
#include "Random.h"
#include "RectPack.h"

namespace lcore
{
    class TestRectPack : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
        static bool collide(const RectPackRect& rect0, const RectPackRect& rect1)
        {
            u16 l0 = rect0.X();
            u16 r0 = rect0.X() + rect0.W();
            u16 t0 = rect0.Y() + rect0.H();
            u16 b0 = rect0.Y();
            u16 l1 = rect1.X();
            u16 r1 = rect1.X() + rect1.W();
            u16 t1 = rect1.Y() + rect1.H();
            u16 b1 = rect1.Y();

            if(r0<=l1){
                return false;
            }
            if(r1<=l0){
                return false;
            }
            if(b0<=t1){
                return false;
            }
            if(b1<=t0){
                return false;
            }
            return true;
        }
    };

    TEST_F(TestRectPack, Pack00)
    {
        RandXorshift128Plus32 random(lcore::getDefaultSeed64());

        static const s32 NumSamples = 1024;
        static const s32 Size = 1024*8;
        RectPackRect rects[NumSamples];
        RectPack::Node nodes[Size*2];
        RectPack::Context context;
        for(s32 i=0; i<NumSamples; ++i){
            u16 w = static_cast<u16>(lcore::random::range_rclose(random, 1, 8));
            EXPECT_TRUE(1<=w && w<=8);
            u16 h = static_cast<u16>(lcore::random::range_rclose(random, 1, 8));
            EXPECT_TRUE(1<=h && h<=8);
            rects[i].ID() = i;
            rects[i].W() = w;
            rects[i].H() = h;
        }

        RectPack::initialize(context, Size, Size, Size*2, nodes);
        bool result = RectPack::pack(context, NumSamples, rects);
        EXPECT_TRUE(result);
        EXPECT_TRUE(context.free_<=(Size*2));
        for(s32 i=0; i<NumSamples; ++i){
            for(s32 j=0; j<i; ++j){
                EXPECT_TRUE(rects[i].ID() != rects[j].ID());
                EXPECT_FALSE(collide(rects[i], rects[j]));
            }
        }
    }
}

#include <catch_wrap.hpp>
#include "Array.h"
#include <jemalloc/jemalloc.h>
#include "Random.h"

namespace lcore
{
namespace
{
    class TestClass
    {
    public:
        TestClass()
        {}

        explicit TestClass(s32 value)
            :value_(value)
        {}

        friend static bool operator==(const TestClass& lhs, const TestClass& rhs)
        {
            return lhs.value_ == rhs.value_;
        }

        friend static bool operator==(s32 lhs, const TestClass& rhs)
        {
            return lhs == rhs.value_;
        }

        friend static bool operator==(const TestClass& lhs, s32 rhs)
        {
            return lhs.value_ == rhs;
        }

    private:
        s32 value_;
    };

    struct AllocStats
    {
        size_t allocated_;
        size_t active_;
        size_t metadata_;
        size_t resident_;
        size_t mapped_;
    };

    AllocStats getAllocStats()
    {
        // Update the statistics cached by mallctl.
        uint64_t epoch = 1;
        size_t sz = sizeof(epoch);
        je_mallctl("epoch", &epoch, &sz, &epoch, sz);

        AllocStats stats = {0};
        size_t allocated, active, metadata, resident, mapped;
        sz = sizeof(size_t);
        if(je_mallctl("stats.allocated", &allocated, &sz, NULL, 0) == 0
            && je_mallctl("stats.active", &active, &sz, NULL, 0) == 0
            && je_mallctl("stats.metadata", &metadata, &sz, NULL, 0) == 0
            && je_mallctl("stats.resident", &resident, &sz, NULL, 0) == 0
            && je_mallctl("stats.mapped", &mapped, &sz, NULL, 0) == 0)
        {
            stats.allocated_ = allocated;
            stats.active_ = active;
            stats.metadata_ = metadata;
            stats.resident_ = resident;
            stats.mapped_ = mapped;
        }
        return stats;
    }
}


    TEST_CASE("TestArray::TestNormalArray")
    {
        lcore::RandXorshift64Star32 random(lcore::getDefaultSeed());

        const s32 NumSamples = 127;
        s32 expects[NumSamples];
        lcore::Array<TestClass> array;

        EXPECT_EQ(array.capacity(), 0);

        //Push back
        for(s32 i=0; i<NumSamples; ++i){
            expects[i] = static_cast<s32>(random.rand());
            array.push_back(TestClass(expects[i]));
        }

        EXPECT_EQ(array.size(), NumSamples);
        EXPECT_EQ(array.capacity(), 128);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i], expects[i]);
        }

        //Expand
        array.resize(257);
        EXPECT_EQ(array.size(), 257);
        EXPECT_EQ(array.capacity(), 272);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i], expects[i]);
        }

        //Reserve
        array.reserve(1024);
        EXPECT_EQ(array.size(), 257);
        EXPECT_EQ(array.capacity(), 1024);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i], expects[i]);
        }

        //Shrink
        array.resize(63);
        for(s32 i=0; i<array.size(); ++i){
            EXPECT_EQ(array[i], expects[i]);
        }
    }

    TEST_CASE("TestArray::TestPointerArray")
    {
        lcore::RandXorshift random(lcore::getDefaultSeed());

        const s32 NumSamples = 127;
        s32 expects[NumSamples];
        lcore::Array<TestClass*> array;

        EXPECT_EQ(array.capacity(), 0);

        //Push back
        for(s32 i=0; i<NumSamples; ++i){
            expects[i] = static_cast<s32>(random.rand());
            array.push_back(LNEW TestClass(expects[i]));
        }

        EXPECT_EQ(array.size(), NumSamples);
        EXPECT_EQ(array.capacity(), 128);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(*array[i], expects[i]);
        }

        //Expand
        array.resize(257);
        EXPECT_EQ(array.size(), 257);
        EXPECT_EQ(array.capacity(), 272);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(*array[i], expects[i]);
        }
        for(s32 i = NumSamples; i<array.size(); ++i){
            array[i] = NULL;
        }

        //Reserve
        array.reserve(1024);
        EXPECT_EQ(array.size(), 257);
        EXPECT_EQ(array.capacity(), 1024);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(*array[i], expects[i]);
        }

        //Shrink
        for(s32 i=63; i<array.size(); ++i){
            LDELETE(array[i]);
        }
        array.resize(63);
        for(s32 i=0; i<array.size(); ++i){
            EXPECT_EQ(*array[i], expects[i]);
        }

        for(s32 i=0; i<array.size(); ++i){
            LDELETE(array[i]);
        }
    }

    TEST_CASE("TestArray::TestArrayPOD")
    {
        lcore::RandXorshift random(lcore::getDefaultSeed());

        const s32 NumSamples = 127;
        s32 expects[NumSamples];
        Array<s32> array;

        EXPECT_EQ(array.capacity(), 0);

        //Push back
        for(s32 i=0; i<NumSamples; ++i){
            expects[i] = static_cast<s32>(random.rand());
            array.push_back(expects[i]);
        }

        EXPECT_EQ(array.size(), NumSamples);
        EXPECT_EQ(array.capacity(), 128);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i], expects[i]);
        }

        //Expand
        array.resize(257);
        EXPECT_EQ(array.size(), 257);
        EXPECT_EQ(array.capacity(), 272);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i], expects[i]);
        }

        //Reserve
        array.reserve(1024);
        EXPECT_EQ(array.size(), 257);
        EXPECT_EQ(array.capacity(), 1024);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i], expects[i]);
        }

        //Shrink
        array.resize(63);
        for(s32 i=0; i<array.size(); ++i){
            EXPECT_EQ(array[i], expects[i]);
        }

        //Remove at
        array.removeAt(0);
        for(s32 i=0; i<array.size(); ++i){
            EXPECT_EQ(array[i], expects[i+1]);
        }
    }
}

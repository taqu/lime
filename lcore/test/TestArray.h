#include <gtest/gtest.h>
#include "Array.h"
#include <jemalloc/jemalloc.h>
#include "Random.h"

namespace lcore
{
    class TestArray : public ::testing::Test
    {
    protected:
        class TestClass
        {
        public:
            TestClass()
                :ptr_(NULL)
            {}

            explicit TestClass(int x)
            {
                ptr_ = LNEW int(x);
            }

            TestClass(const TestClass& rhs)
                :ptr_(NULL)
            {
                if(NULL != rhs.ptr_){
                    ptr_ = LNEW int(*rhs.ptr_);
                }
            }

            ~TestClass()
            {
                LDELETE(ptr_);
            }

            void set(int x)
            {
                LDELETE(ptr_);
                ptr_ = LNEW int(x);
            }

            const int& get() const
            {
                return *ptr_;
            }

            TestClass& operator=(const TestClass& rhs)
            {
                LDELETE(ptr_);
                if(NULL != rhs.ptr_){
                    ptr_ = LNEW int(*rhs.ptr_);
                }
                return *this;
            }
            int* ptr_;
        };

        struct AllocStats
        {
            lsize_t allocated_;
            lsize_t active_;
            lsize_t metadata_;
            lsize_t resident_;
            lsize_t mapped_;
        };

        AllocStats getAllocStats()
        {
            // Update the statistics cached by mallctl.
            uint64_t epoch = 1;
            lsize_t sz = sizeof(epoch);
            je_mallctl("epoch", &epoch, &sz, &epoch, sz);

            AllocStats stats = {0};
            lsize_t allocated, active, metadata, resident, mapped;
            sz = sizeof(size_t);
            if (je_mallctl("stats.allocated", &allocated, &sz, NULL, 0) == 0
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

        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F(TestArray, TestNormalArray)
    {
        lcore::RandXorshift64Star32 random(lcore::getDefaultSeed());

        const s32 NumSamples = 127;
        s32 expects[NumSamples];
        lcore::Array<TestClass> array;

        EXPECT_EQ(array.getNewCapacity(0), 16);
        EXPECT_EQ(array.getInitCapacity(17), 32);
        EXPECT_EQ(array.capacity(), 0);

        //Push back
        for(s32 i=0; i<NumSamples; ++i){
            expects[i] = static_cast<s32>(random.rand());
            array.push_back(TestClass(expects[i]));
        }

        EXPECT_EQ(array.size(), NumSamples);
        EXPECT_EQ(array.capacity(), 128);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i].get(), expects[i]);
        }

        //Expand
        array.resize(257);
        EXPECT_EQ(array.size(), 257);
        EXPECT_EQ(array.capacity(), 272);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i].get(), expects[i]);
        }

        //Reserve
        array.reserve(1024);
        EXPECT_EQ(array.size(), 257);
        EXPECT_EQ(array.capacity(), 1024);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i].get(), expects[i]);
        }

        //Shrink
        array.resize(63);
        for(s32 i=0; i<array.size(); ++i){
            EXPECT_EQ(array[i].get(), expects[i]);
        }
    }

    TEST_F(TestArray, TestPointerArray)
    {
        lcore::RandXorshift random(lcore::getDefaultSeed());

        const s32 NumSamples = 127;
        s32 expects[NumSamples];
        lcore::Array<TestClass*> array;

        EXPECT_EQ(array.getNewCapacity(0), 16);
        EXPECT_EQ(array.getInitCapacity(17), 32);
        EXPECT_EQ(array.capacity(), 0);

        //Push back
        for(s32 i=0; i<NumSamples; ++i){
            expects[i] = static_cast<s32>(random.rand());
            array.push_back(LNEW TestClass(expects[i]));
        }

        EXPECT_EQ(array.size(), NumSamples);
        EXPECT_EQ(array.capacity(), 128);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i]->get(), expects[i]);
        }

        //Expand
        array.resize(257);
        EXPECT_EQ(array.size(), 257);
        EXPECT_EQ(array.capacity(), 272);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i]->get(), expects[i]);
        }

        //Reserve
        array.reserve(1024);
        EXPECT_EQ(array.size(), 257);
        EXPECT_EQ(array.capacity(), 1024);
        for(s32 i=0; i<NumSamples; ++i){
            EXPECT_EQ(array[i]->get(), expects[i]);
        }

        //Shrink
        for(s32 i=63; i<array.size(); ++i){
            LDELETE(array[i]);
        }
        array.resize(63);
        for(s32 i=0; i<array.size(); ++i){
            EXPECT_EQ(array[i]->get(), expects[i]);
        }

        for(s32 i=0; i<array.size(); ++i){
            LDELETE(array[i]);
        }
    }

    TEST_F(TestArray, TestArrayPOD)
    {
        lcore::RandXorshift random(lcore::getDefaultSeed());

        const s32 NumSamples = 127;
        s32 expects[NumSamples];
        lcore::ArrayPOD<s32> array;

        EXPECT_EQ(array.getNewCapacity(0), 16);
        EXPECT_EQ(array.getInitCapacity(17), 32);
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

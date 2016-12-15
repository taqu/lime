#include <gtest/gtest.h>
#include <string>
#include "Random.h"
#include "HashMap.h"

namespace lcore
{
    class TestHashMapCollection : public ::testing::Test
    {
    protected:
        static const s32 NumAlphaNumber = 62;

        inline char getAlphaNumber(u32 index)
        {
            index %= NumAlphaNumber;
            return AlphaNumber[index];
        }

        static const int NumSamples = 1024;
        static const int Length = 5;

        virtual void SetUp()
        {
            AlphaNumber = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            lcore::RandXorshift rand(lcore::getDefaultSeed());
            for(s32 i=0; i<NumSamples; ++i){
                strings_[i] = LNEW char[Length+1];
                for(s32 j=0; j<Length; ++j){
                    strings_[i][j] = getAlphaNumber(rand.rand());
                }
                strings_[i][Length] = CharNull;
                order_[i] = i;
            }
            lcore::random::shuffle(rand, order_, order_+NumSamples);
        }

        virtual void TearDown()
        {
            for(s32 i=0; i<NumSamples; ++i){
                LDELETE_ARRAY(strings_[i]);
            }
        }

        std::string AlphaNumber;
        char* strings_[NumSamples];
        s32 order_[NumSamples];
    };


    TEST_F(TestHashMapCollection, TestHopscotch)
    {
        const s32 HalfNumSamples = NumSamples>>1;
        typedef lcore::HopscotchHashMap<StringWrapper, Char*> HashMapType;
        HashMapType hashmap;
        for(s32 i=0; i<NumSamples; ++i){
            hashmap.insert(StringWrapper(Length, strings_[i]), strings_[i]);
        }
        for(s32 i=0; i<NumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[i]));
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_STREQ(strings_[i], hashmap.getValue(pos));
        }

        for(s32 i=0; i<HalfNumSamples; ++i){
            hashmap.erase(StringWrapper(Length, strings_[i]));
        }

        for(s32 i=0; i<HalfNumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[i]));
            EXPECT_EQ(false, hashmap.valid(pos));
        }
        for(s32 i=HalfNumSamples; i<NumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[i]));
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_STREQ(strings_[i], hashmap.getValue(pos));
        }
        hashmap.clear();

        //-----------------------------------------
        for(s32 i=0; i<NumSamples; ++i){
            s32 index = order_[i];
            hashmap.insert(StringWrapper(Length, strings_[index]), strings_[index]);
        }
        for(s32 i=0; i<NumSamples; ++i){
            s32 index = order_[i];
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[index]));
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_STREQ(strings_[index], hashmap.getValue(pos));
        }

        for(s32 i=0; i<HalfNumSamples; ++i){
            s32 index = order_[i];
            hashmap.erase(StringWrapper(Length, strings_[index]));
        }
        for(s32 i=0; i<HalfNumSamples; ++i){
            s32 index = order_[i];
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[index]));
            EXPECT_EQ(false, hashmap.valid(pos));
        }
        for(s32 i=HalfNumSamples; i<NumSamples; ++i){
            s32 index = order_[i];
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[index]));
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_STREQ(strings_[index], hashmap.getValue(pos));
        }
    }

    TEST_F(TestHashMapCollection, TestHashMap)
    {
        const s32 HalfNumSamples = NumSamples>>1;
        typedef lcore::HashMap<StringWrapper, Char*> HashMapType;
        HashMapType hashmap;
        for(s32 i=0; i<NumSamples; ++i){
            hashmap.insert(StringWrapper(Length, strings_[i]), strings_[i]);
        }
        for(s32 i=0; i<NumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[i]));
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_STREQ(strings_[i], hashmap.getValue(pos));
        }

        for(s32 i=0; i<HalfNumSamples; ++i){
            hashmap.erase(StringWrapper(Length, strings_[i]));
        }

        for(s32 i=0; i<HalfNumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[i]));
            EXPECT_EQ(false, hashmap.valid(pos));
        }
        for(s32 i=HalfNumSamples; i<NumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[i]));
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_STREQ(strings_[i], hashmap.getValue(pos));
        }
        hashmap.clear();

        //-----------------------------------------
        for(s32 i=0; i<NumSamples; ++i){
            s32 index = order_[i];
            hashmap.insert(StringWrapper(Length, strings_[index]), strings_[index]);
        }
        for(s32 i=0; i<NumSamples; ++i){
            s32 index = order_[i];
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[index]));
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_STREQ(strings_[index], hashmap.getValue(pos));
        }

        for(s32 i=0; i<HalfNumSamples; ++i){
            s32 index = order_[i];
            hashmap.erase(StringWrapper(Length, strings_[index]));
        }
        for(s32 i=0; i<HalfNumSamples; ++i){
            s32 index = order_[i];
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[index]));
            EXPECT_EQ(false, hashmap.valid(pos));
        }
        for(s32 i=HalfNumSamples; i<NumSamples; ++i){
            s32 index = order_[i];
            HashMapType::size_type pos = hashmap.find(StringWrapper(Length, strings_[index]));
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_STREQ(strings_[index], hashmap.getValue(pos));
        }
    }
}

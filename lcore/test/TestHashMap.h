#include <gtest/gtest.h>
#include <string>
#include "Random.h"
#include "HashMap.h"

namespace lcore
{
    namespace hash_detail
    {
        template<>
        inline u32 calcHash<std::string>(const std::string& x)
        {
            return lcore::xxHash32(reinterpret_cast<const lcore::u8*>(x.c_str()), static_cast<lcore::s32>(x.size()));
        }
    }

    class TestHashMapCollection : public ::testing::Test
    {
    protected:
        static const int NumSamples = 16364;
#ifdef _DEBUG
        static const int SpeedSamples = NumSamples;
#else
        static const int SpeedSamples = 1000*1000;
#endif
        static const int MinKeyLength = 2;
        static const int MaxKeyLength = 16;
        static const int MinValueLength = 8;
        static const int MaxValueLength = 32;

        void createRandomString(std::string& str, u32 length, lcore::RandXorshift128Plus32& random)
        {
            static const char* ASCII = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            static const s32 ASCIILength = strlen_s32(ASCII);
            str.reserve(length+1);
            for(u32 i=0; i<length; ++i){
                int c = random.rand()%ASCIILength;
                str.push_back(ASCII[c]);
            }
        }

        virtual void SetUp()
        {
            keys_ = LNEW std::string[SpeedSamples];
            values_ = LNEW std::string[SpeedSamples];

            Char buffer[32];
            u32 seed = lcore::getDefaultSeed();
            std::cerr << seed << std::endl;
            lcore::RandXorshift128Plus32 rand(seed);
            for(s32 i=0; i<SpeedSamples; ++i){
                u32 keyLength = rand.rand()%(MaxKeyLength-MinKeyLength)+MinKeyLength;
                createRandomString(keys_[i], keyLength, rand);
                sprintf_s(buffer, 32, "%d", i);
                keys_[i].append(buffer);

                u32 valueLength = rand.rand()%(MaxValueLength-MinValueLength)+MinValueLength;
                createRandomString(values_[i], valueLength, rand);
            }
        }

        virtual void TearDown()
        {
            LDELETE_ARRAY(values_);
            LDELETE_ARRAY(keys_);
        }

        template<class T>
        void test();

        template<class T>
        void speedtest(const Char* name);

        std::string AlphaNumber;
        std::string* keys_;
        std::string* values_;
    };

    template<class T>
    void TestHashMapCollection::test()
    {
        const s32 HalfNumSamples = NumSamples>>1;
        typedef T HashMapType;
        HashMapType hashmap;
        for(s32 i=0; i<NumSamples; ++i){
            hashmap.insert(keys_[i], values_[i]);
        }
        for(s32 i=0; i<NumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(keys_[i]);
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_EQ(values_[i], hashmap.getValue(pos));
        }

        for(s32 i=0; i<HalfNumSamples; ++i){
            hashmap.erase(keys_[i]);
        }

        for(s32 i=0; i<HalfNumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(keys_[i]);
            EXPECT_EQ(false, hashmap.valid(pos));
        }
        for(s32 i=HalfNumSamples; i<NumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(keys_[i]);
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_EQ(values_[i], hashmap.getValue(pos));
        }
        for(s32 i=0; i<NumSamples; ++i){
            hashmap.insert(keys_[i], values_[i]);
        }
        for(s32 i=0; i<NumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(keys_[i]);
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_EQ(values_[i], hashmap.getValue(pos));
        }

        hashmap.clear();

        //-----------------------------------------
        for(s32 i=0; i<NumSamples; ++i){
            hashmap.insert(keys_[i], values_[i]);
        }
        for(s32 i=0; i<NumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(keys_[i]);
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_EQ(values_[i], hashmap.getValue(pos));
        }

        for(s32 i=0; i<HalfNumSamples; ++i){
            hashmap.erase(keys_[i]);
        }

        for(s32 i=0; i<HalfNumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(keys_[i]);
            EXPECT_EQ(false, hashmap.valid(pos));
        }
        for(s32 i=HalfNumSamples; i<NumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(keys_[i]);
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_EQ(values_[i], hashmap.getValue(pos));
        }
        for(s32 i=0; i<NumSamples; ++i){
            hashmap.insert(keys_[i], values_[i]);
        }
        for(s32 i=0; i<NumSamples; ++i){
            HashMapType::size_type pos = hashmap.find(keys_[i]);
            EXPECT_EQ(true, hashmap.valid(pos));
            EXPECT_EQ(values_[i], hashmap.getValue(pos));
        }
    }

    template<class T>
    void TestHashMapCollection::speedtest(const Char* name)
    {
        static const s32 HalfNumSamples = SpeedSamples>>1;

        typedef T HashMapType;

        HashMapType::size_type* results = LNEW HashMapType::size_type[SpeedSamples];
        HashMapType hashmap;

        //Insert
        //--------------------------
        lcore::ClockType clockInsert = lcore::getPerformanceCounter();
        for(s32 i = 0; i<SpeedSamples; ++i){
            hashmap.insert(keys_[i], values_[i]);
        }
        lcore::f64 timeInsert = lcore::calcTime64(clockInsert, lcore::getPerformanceCounter());

        //Find
        //--------------------------
        lcore::ClockType clockFind = lcore::getPerformanceCounter();
        for(s32 i = 0; i<SpeedSamples; ++i){
            results[i] = hashmap.find(keys_[i]);
        }
        lcore::f64 timeFind = lcore::calcTime64(clockFind, lcore::getPerformanceCounter());

        for(s32 i = 0; i<SpeedSamples; ++i){
            EXPECT_EQ(true, hashmap.valid(results[i]));
        }

        //Erase
        //--------------------------
        lcore::ClockType clockErase = lcore::getPerformanceCounter();
        for(s32 i = 0; i<HalfNumSamples; ++i){
            hashmap.erase(keys_[i]);
        }
        lcore::f64 timeErase = lcore::calcTime64(clockErase, lcore::getPerformanceCounter());

        //Find2
        //--------------------------
        lcore::ClockType clockFind2 = lcore::getPerformanceCounter();
        for(s32 i = 0; i<HalfNumSamples; ++i){
            results[i] = hashmap.find(keys_[i]);
        }
        for(s32 i = HalfNumSamples; i<SpeedSamples; ++i){
            results[i] = hashmap.find(keys_[i]);
        }
        lcore::f64 timeFind2 = lcore::calcTime64(clockFind2, lcore::getPerformanceCounter());

        for(s32 i = 0; i<HalfNumSamples; ++i){
            EXPECT_EQ(false, hashmap.valid(results[i]));
        }
        for(s32 i = HalfNumSamples; i<SpeedSamples; ++i){
            EXPECT_EQ(true, hashmap.valid(results[i]));
        }

        LDELETE_ARRAY(results);
        std::cerr << name << std::endl;
        std::cerr << "    insert: " << timeInsert << std::endl;
        std::cerr << "    find: " << timeFind << std::endl;
        std::cerr << "    erase: " << timeErase << std::endl;
        std::cerr << "    find2: " << timeFind2 << std::endl;
        std::cerr << "    capacity: " << hashmap.capacity() << std::endl;
    }

    TEST_F(TestHashMapCollection, TestHopscotch)
    {
        test<lcore::HopscotchHashMap<std::string, std::string>>();
    }

    TEST_F(TestHashMapCollection, TestHashMap)
    {
        test<lcore::HashMap<std::string, std::string>>();
    }

    TEST_F(TestHashMapCollection, TestRHHashMap)
    {
        test<lcore::RHHashMap<std::string, std::string>>();
    }

    TEST_F(TestHashMapCollection, Speed)
    {
        speedtest<lcore::HashMap<std::string, std::string>>("HashMap");
        speedtest<lcore::HopscotchHashMap<std::string, std::string>>("Hopscotch");
        speedtest<lcore::RHHashMap<std::string, std::string>>("RobinHood");
    }
}

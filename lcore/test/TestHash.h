#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <iostream>

#include "lcore.h"
#include "MurmurHash.h"
#include "xxHash.h"
#include "Random.h"

namespace lcore
{
    class TestHash : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F(TestHash, TestXXHashSpecialized)
    {
        static const s32 NumSamples = 10*1000;
        {
            lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());
            for(s32 i=0; i<NumSamples; ++i){
                u32 x = random.rand();
                u32 hash0 = xxHash32(reinterpret_cast<const u8*>(&x), sizeof(u32));
                u32 hash1 = xxHash32(x);
                EXPECT_EQ(hash0, hash1);
            }
        }

        {
            lcore::RandXorshift128Plus random(lcore::getDefaultSeed64());
            for(s32 i=0; i<NumSamples; ++i){
                u64 x = random.rand();
                u32 hash0 = xxHash32(reinterpret_cast<const u8*>(&x), sizeof(u64));
                u32 hash1 = xxHash32(x);
                EXPECT_EQ(hash0, hash1);
            }
        }

        {
            lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());
            for(s32 i=0; i<NumSamples; ++i){
                u32 x = random.rand();
                u64 hash0 = xxHash64(reinterpret_cast<const u8*>(&x), sizeof(u32));
                u64 hash1 = xxHash64(x);
                EXPECT_EQ(hash0, hash1);
            }
        }

        {
            lcore::RandXorshift128Plus random(lcore::getDefaultSeed64());
            for(s32 i=0; i<NumSamples; ++i){
                u64 x = random.rand();
                u64 hash0 = xxHash64(reinterpret_cast<const u8*>(&x), sizeof(u64));
                u64 hash1 = xxHash64(x);
                EXPECT_EQ(hash0, hash1);
            }
        }
    }

    TEST_F(TestHash, CollisionNumbers32)
    {
#ifdef _DEBUG
        static const s32 NumSamples = 1000;
#else
        static const s32 NumSamples = 10*1000;
#endif
        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());
        std::vector<u32> numbers;
        numbers.reserve(NumSamples);
        std::vector<u32> hashMurmur;
        hashMurmur.reserve(NumSamples);
        std::vector<u32> hashXX;
        hashXX.reserve(NumSamples);
        std::vector<u32> hashFnv;
        hashFnv.reserve(NumSamples);
        //std::vector<u32> hashBernstein;
        //hashBernstein.reserve(NumSamples);

        for(s32 i=0; i<NumSamples; ++i){
            u32 x = random.rand();
            bool found = false;
            for(s32 j=0; j<numbers.size(); ++j){
                if(x == numbers[j]){
                    found = true;
                    break;
                }
            }
            if(!found){
                numbers.push_back(x);
            }
        }
        std::cerr << "num of samples: " << numbers.size() << std::endl;

        s32 colMurmur = 0;
        s32 colXX = 0;
        s32 colFnv = 0;
        //s32 colBern = 0;
        for(s32 i=0; i<numbers.size(); ++i){
            const s32 length = sizeof(u32);
            const u8* data = reinterpret_cast<const u8*>(&numbers[i]);
            hashMurmur.push_back(MurmurHash32(data, length));
            hashXX.push_back(xxHash32(numbers[i]));
            hashFnv.push_back(hash_FNV1(data, length));

            for(s32 j=0; j<i; ++j){
                if(hashMurmur[i] == hashMurmur[j]){
                    ++colMurmur;
                }
                if(hashXX[i] == hashXX[j]){
                    ++colXX;
                }
                if(hashFnv[i] == hashFnv[j]){
                    ++colFnv;
                }
                //if(hashBernstein[i] == hashBernstein[j]){
                //    ++colBern;
                //}
            }
        }
        std::cerr << "Murmur3  : " <<  colMurmur << std::endl;
        std::cerr << "xxHash   : " <<  colXX << std::endl;
        std::cerr << "FNV1     : " <<  colFnv << std::endl;
        //std::cerr << "Bernstein: " <<  colBern << std::endl;
    }

    TEST_F(TestHash, CollisionNumbers64)
    {
#ifdef _DEBUG
        static const s32 NumSamples = 1000;
#else
        static const s32 NumSamples = 10*1000;
#endif
        lcore::RandXorshift128Plus random(lcore::getDefaultSeed64());
        std::vector<u64> numbers;
        numbers.reserve(NumSamples);
        std::vector<u64> hashXX;
        hashXX.reserve(NumSamples);
        std::vector<u64> hashFnv;
        hashFnv.reserve(NumSamples);
        //std::vector<u64> hashBernstein;
        //hashBernstein.reserve(NumSamples);

        for(s32 i=0; i<NumSamples; ++i){
            u64 x = random.rand();
            bool found = false;
            for(s32 j=0; j<numbers.size(); ++j){
                if(x == numbers[j]){
                    found = true;
                    break;
                }
            }
            if(!found){
                numbers.push_back(x);
            }
        }
        std::cerr << "num of samples: " << numbers.size() << std::endl;

        s32 colXX = 0;
        s32 colFnv = 0;
        //s32 colBern = 0;
        for(s32 i=0; i<numbers.size(); ++i){
            const s32 length = sizeof(u64);
            const u8* data = reinterpret_cast<const u8*>(&numbers[i]);
            hashXX.push_back(xxHash64(numbers[i]));
            hashFnv.push_back(hash_FNV1(data, length));
            //hashBernstein.push_back(hash_Bernstein(data, length));

            for(s32 j=0; j<i; ++j){
                if(hashXX[i] == hashXX[j]){
                    ++colXX;
                }
                if(hashFnv[i] == hashFnv[j]){
                    ++colFnv;
                }
                //if(hashBernstein[i] == hashBernstein[j]){
                //    ++colBern;
                //}
            }
        }
        std::cerr << "xxHash   : " <<  colXX << std::endl;
        std::cerr << "FNV1     : " <<  colFnv << std::endl;
    }

    TEST_F(TestHash, CollisionString)
    {
#ifdef _DEBUG
        static const s32 NumSamples = 500;
#else
        static const s32 NumSamples = 5*1000;
#endif
        static const s32 Length = 16;
        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());
        std::vector<std::string> strings;
        strings.reserve(NumSamples);
        std::vector<u32> hashMurmur;
        hashMurmur.reserve(NumSamples);
        std::vector<u32> hashXX;
        hashXX.reserve(NumSamples);
        std::vector<u32> hashFnv;
        hashFnv.reserve(NumSamples);
        //std::vector<u32> hashBernstein;
        //hashBernstein.reserve(NumSamples);

        std::string tmp;
        tmp.reserve(Length);
        for(s32 i=0; i<NumSamples; ++i){
            tmp.clear();
            s32 length = (s32)random.rand()&(Length-1);
            for(s32 l=0; l<length; ++l){
                s32 x = (s32)random.rand()&63 + 48;
                tmp.append(1, (Char)x);
            }
            bool found = false;
            for(s32 j=0; j<strings.size(); ++j){
                if(tmp == strings[j]){
                    found = true;
                    break;
                }
            }
            if(!found){
                strings.push_back(tmp);
            }
        }
        std::cerr << "num of samples: " << strings.size() << std::endl;

        s32 colMurmur = 0;
        s32 colXX = 0;
        s32 colFnv = 0;
        //s32 colBern = 0;
        for(s32 i=0; i<strings.size(); ++i){
            const s32 length = (s32)strings[i].length();
            const u8* data = reinterpret_cast<const u8*>(strings[i].c_str());
            hashMurmur.push_back(MurmurHash32(data, length));
            hashXX.push_back(xxHash32(data, length));
            hashFnv.push_back(hash_FNV1(data, length));
            //hashBernstein.push_back(hash_Bernstein(data, length));

            for(s32 j=0; j<i; ++j){
                if(hashMurmur[i] == hashMurmur[j]){
                    ++colMurmur;
                }
                if(hashXX[i] == hashXX[j]){
                    ++colXX;
                }
                if(hashFnv[i] == hashFnv[j]){
                    ++colFnv;
                }
                //if(hashBernstein[i] == hashBernstein[j]){
                //    ++colBern;
                //}
            }
        }
        std::cerr << "Murmur3  : " <<  colMurmur << std::endl;
        std::cerr << "xxHash   : " <<  colXX << std::endl;
        std::cerr << "FNV1     : " <<  colFnv << std::endl;
        //std::cerr << "Bernstein: " <<  colBern << std::endl;
    }

    TEST_F(TestHash, SpeedNumber)
    {
        static const s32 NumSamples = 1000*1000;
        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());
        std::vector<u32> numbers;
        numbers.reserve(NumSamples);
        std::vector<u32> hashMurmur;
        hashMurmur.reserve(NumSamples);
        std::vector<u32> hashXX;
        hashXX.reserve(NumSamples);
        std::vector<u32> hashFnv;
        hashFnv.reserve(NumSamples);
        std::vector<u32> hashBernstein;
        hashBernstein.reserve(NumSamples);

        for(s32 i=0; i<NumSamples; ++i){
            numbers.push_back(random.rand());
        }

        lcore::ClockType clockMurmur = lcore::getPerformanceCounter();
        for(s32 i=0; i<numbers.size(); ++i){
            const s32 length = sizeof(u32);
            const u8* data = reinterpret_cast<const u8*>(&numbers[i]);
            hashMurmur.push_back(MurmurHash32(data, length));
        }
        lcore::f64 timeMurmur = lcore::calcTime64(clockMurmur, lcore::getPerformanceCounter());

        lcore::ClockType clockXX = lcore::getPerformanceCounter();
        for(s32 i=0; i<numbers.size(); ++i){
            hashXX.push_back(xxHash32(numbers[i]));
        }
        lcore::f64 timeXX = lcore::calcTime64(clockXX, lcore::getPerformanceCounter());

        lcore::ClockType clockFNV = lcore::getPerformanceCounter();
        for(s32 i=0; i<numbers.size(); ++i){
            const s32 length = sizeof(u32);
            const u8* data = reinterpret_cast<const u8*>(&numbers[i]);
            hashFnv.push_back(hash_FNV1(data, length));
        }
        lcore::f64 timeFNV = lcore::calcTime64(clockFNV, lcore::getPerformanceCounter());

        //lcore::ClockType clockBern = lcore::getPerformanceCounter();
        //for(s32 i=0; i<strings.size(); ++i){
        //    const s32 length = strings[i].length();
        //    const u8* data = reinterpret_cast<const u8*>(strings[i].c_str());
        //    hashBernstein.push_back(hash_Bernstein(data, length));
        //}
        //lcore::f64 timeBern = lcore::calcTime64(clockBern, lcore::getPerformanceCounter());

        std::cerr << "Murmur3  : " <<  timeMurmur << std::endl;
        std::cerr << "xxHash   : " <<  timeXX << std::endl;
        std::cerr << "FNV1     : " <<  timeFNV << std::endl;
        //std::cerr << "Bernstein: " <<  timeBern << std::endl;
    }

    TEST_F(TestHash, SpeedString)
    {
        static const s32 NumSamples = 100*1000;

        static const s32 Length = 16;
        lcore::RandXorshift128Plus32 random(lcore::getDefaultSeed64());
        std::vector<std::string> strings;
        strings.reserve(NumSamples);
        std::vector<u32> hashMurmur;
        hashMurmur.reserve(NumSamples);
        std::vector<u32> hashXX;
        hashXX.reserve(NumSamples);
        std::vector<u32> hashFnv;
        hashFnv.reserve(NumSamples);
        std::vector<u32> hashBernstein;
        hashBernstein.reserve(NumSamples);

        std::string tmp;
        tmp.reserve(Length);
        for(s32 i=0; i<NumSamples; ++i){
            tmp.clear();
            s32 length = (s32)Length;
            for(s32 l=0; l<length; ++l){
                s32 x = (s32)random.rand()&63 + 48;
                tmp.append(1, (Char)x);
            }
            strings.push_back(tmp);
        }

        lcore::ClockType clockMurmur = lcore::getPerformanceCounter();
        for(s32 i=0; i<strings.size(); ++i){
            const s32 length = (s32)strings[i].length();
            const u8* data = reinterpret_cast<const u8*>(strings[i].c_str());
            hashMurmur.push_back(MurmurHash32(data, length));
        }
        lcore::f64 timeMurmur = lcore::calcTime64(clockMurmur, lcore::getPerformanceCounter());

        lcore::ClockType clockXX = lcore::getPerformanceCounter();
        for(s32 i=0; i<strings.size(); ++i){
            const s32 length = (s32)strings[i].length();
            const u8* data = reinterpret_cast<const u8*>(strings[i].c_str());
            hashXX.push_back(xxHash32(data, length));
        }
        lcore::f64 timeXX = lcore::calcTime64(clockXX, lcore::getPerformanceCounter());

        lcore::ClockType clockFNV = lcore::getPerformanceCounter();
        for(s32 i=0; i<strings.size(); ++i){
            const s32 length = (s32)strings[i].length();
            const u8* data = reinterpret_cast<const u8*>(strings[i].c_str());
            hashFnv.push_back(hash_FNV1(data, length));
        }
        lcore::f64 timeFNV = lcore::calcTime64(clockFNV, lcore::getPerformanceCounter());

        //lcore::ClockType clockBern = lcore::getPerformanceCounter();
        //for(s32 i=0; i<strings.size(); ++i){
        //    const s32 length = strings[i].length();
        //    const u8* data = reinterpret_cast<const u8*>(strings[i].c_str());
        //    hashBernstein.push_back(hash_Bernstein(data, length));
        //}
        //lcore::f64 timeBern = lcore::calcTime64(clockBern, lcore::getPerformanceCounter());

        std::cerr << "Murmur3  : " <<  timeMurmur << std::endl;
        std::cerr << "xxHash   : " <<  timeXX << std::endl;
        std::cerr << "FNV1     : " <<  timeFNV << std::endl;
        //std::cerr << "Bernstein: " <<  timeBern << std::endl;
    }
}

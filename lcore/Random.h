#ifndef INC_RANDOM_H__
#define INC_RANDOM_H__
/**
@file Random.h
@author t-sakai
@date 2011/08/20 create
*/
#include "lcore.h"

namespace lcore
{

    u32 getStaticSeed();
    u32 getDefaultSeed();

    //---------------------------------------------
    //---
    //--- RandomXorshift
    //---
    //---------------------------------------------
    class RandomXorshift
    {
    public:
        RandomXorshift();
        explicit RandomXorshift(u32 seed);
        ~RandomXorshift();

        /**
        @brief 擬似乱数生成器初期化
        @param seed
        */
        void srand(u32 seed);

        /**
        @brief 1 - 0xFFFFFFFFUの乱数生成
        */
        u32 rand();

        /**
        @brief 0 - 1の乱数生成
        */
        f32 frand();

        /**
        @brief 0.0 - 0.999999881の乱数生成
        */
        f32 frand2();

        /**
        @brief 0 - 1の乱数生成
        */
        f64 drand();

        void swap(RandomXorshift& rhs);
    private:
        u32 rand(u32 v, u32 i);

        u32 x_;
        u32 y_;
        u32 z_;
        u32 w_;
    };

    //---------------------------------------------
    //---
    //--- RandomXorshift128Plus
    //---
    //---------------------------------------------
    class RandomXorshift128Plus
    {
    public:
        RandomXorshift128Plus();
        explicit RandomXorshift128Plus(u64 seed);
        ~RandomXorshift128Plus();

        /**
        @brief 擬似乱数生成器初期化
        @param seed
        */
        void srand(u64 seed);

        /**
        @brief 64bitの乱数生成
        */
        u64 rand();

        void swap(RandomXorshift128Plus& rhs);
    private:
        u64 rand(u64 v, u64 i);

        u64 s0_;
        u64 s1_;
    };

    //---------------------------------------------
    //---
    //--- RandomWELL
    //---
    //---------------------------------------------
    class RandomWELL
    {
    public:
        RandomWELL();
        explicit RandomWELL(u32 seed);
        ~RandomWELL();

        /**
        @brief 擬似乱数生成器初期化
        @param seed
        */
        void srand(u32 seed);

        /**
        @brief 0 - 0xFFFFFFFFUの乱数生成
        */
        u32 rand();

        /**
        @brief 0 - 1の乱数生成
        */
        f32 frand();

        /**
        @brief 0.0 - 0.999999881の乱数生成
        */
        f32 frand2();

        /**
        @brief 0 - 1の乱数生成
        */
        f64 drand();

        void swap(RandomWELL& rhs);
    private:
        static const u32 N = 16;

        u32 state_[N];
        u32 index_;
    };

namespace random
{
    /**
    @brief [vmin, vmax)
    */
    template<class T, class U>
    U range_ropen(T& random, U vmin, U vmax)
    {
        return static_cast<U>(random.frand2()*(vmax-vmin)) + vmin;
    }

    /**
    @brief [vmin, vmax)
    */
    template<class T>
    f32 range_ropen(T& random, f32 vmin, f32 vmax)
    {
        return (vmax-vmin)*random.frand2() + vmin;
    }

    /**
    @brief [vmin, vmax]
    */
    template<class T, class U>
    U range(T& random, U vmin, U vmax)
    {
        LASSERT(vmin<=vmax);
        u32 i = random.rand() % (vmax - vmin + 1);
        return vmin + static_cast<U>(i);
    }

    /**
    @brief [vmin, vmax]
    */
    template<class T>
    f32 range(T& random, f32 vmin, f32 vmax)
    {
        LASSERT(vmin<=vmax);
        return (vmax-vmin)*random.frand() + vmin;
    }

    /**
    @brief [0, v)
    */
    template<class T, class U>
    U range(T& random, U v)
    {
        u32 i = random.rand() % v;
        return static_cast<U>(i);
    }

    template<class T, class U>
    void shuffle(T& random, U* start, U* end)
    {
        for(U* i=end-1; start<i; --i){
            u32 offset = range(random, static_cast<u32>(i-start));
            lcore::swap(*i, *(start+offset));
        }
	}
}
}

#endif //INC_RANDOM_H__

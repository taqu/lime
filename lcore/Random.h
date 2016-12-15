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
    u64 getStaticSeed64();
    u64 getDefaultSeed64();

    //---------------------------------------------
    //---
    //--- RandXorshift
    //---
    //---------------------------------------------
    class RandXorshift
    {
    public:
        RandXorshift();
        explicit RandXorshift(u32 seed);
        ~RandXorshift();

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

        void swap(RandXorshift& rhs);
    private:
        u32 x_;
        u32 y_;
        u32 z_;
        u32 w_;
    };

    //---------------------------------------------
    //---
    //--- RandXorshift64Star
    //---
    //---------------------------------------------
    class RandXorshift64Star
    {
    public:
        RandXorshift64Star();
        explicit RandXorshift64Star(u64 seed);
        ~RandXorshift64Star();

        /**
        @brief 擬似乱数生成器初期化
        @param seed
        */
        void srand(u64 seed);

        /**
        @brief 64bitの乱数生成
        */
        u64 rand();

        void swap(RandXorshift64Star& rhs);
    private:
        u64 s_;
    };

    //---------------------------------------------
    //---
    //--- RandXorshift64Star32
    //---
    //---------------------------------------------
    class RandXorshift64Star32
    {
    public:
        RandXorshift64Star32();
        explicit RandXorshift64Star32(u64 seed);
        ~RandXorshift64Star32();

        /**
        @brief 擬似乱数生成器初期化
        @param seed
        */
        void srand(u64 seed);

        /**
        @brief 32bitの乱数生成
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

        void swap(RandXorshift64Star32& rhs);
    private:
        u64 s_;
        u64 r_;
        s32 flag_;
    };

    //---------------------------------------------
    //---
    //--- RandXorshift128Plus
    //---
    //---------------------------------------------
    class RandXorshift128Plus
    {
    public:
        RandXorshift128Plus();
        explicit RandXorshift128Plus(u64 seed);
        ~RandXorshift128Plus();

        /**
        @brief 擬似乱数生成器初期化
        @param seed
        */
        void srand(u64 seed);

        /**
        @brief 64bitの乱数生成
        */
        u64 rand();

        void swap(RandXorshift128Plus& rhs);
    private:
        u64 s0_;
        u64 s1_;
    };

    //---------------------------------------------
    //---
    //--- RandXorshift128Plus32
    //---
    //---------------------------------------------
    class RandXorshift128Plus32
    {
    public:
        RandXorshift128Plus32();
        explicit RandXorshift128Plus32(u64 seed);
        ~RandXorshift128Plus32();

        /**
        @brief 擬似乱数生成器初期化
        @param seed
        */
        void srand(u64 seed);

        /**
        @brief 32bitの乱数生成
        */
        u32 rand();

        void swap(RandXorshift128Plus32& rhs);
    private:
        u64 s0_;
        u64 s1_;
        u64 r_;
        s32 flag_;
    };

    //---------------------------------------------
    //---
    //--- RandWELL
    //---
    //---------------------------------------------
    class RandWELL
    {
    public:
        RandWELL();
        explicit RandWELL(u32 seed);
        ~RandWELL();

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

        void swap(RandWELL& rhs);
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
    U range_rclose(T& random, U vmin, U vmax)
    {
        LASSERT(vmin<=vmax);
        u32 i = random.rand() % (vmax - vmin + 1);
        return vmin + static_cast<U>(i);
    }

    /**
    @brief [vmin, vmax]
    */
    template<class T>
    f32 range_rclose(T& random, f32 vmin, f32 vmax)
    {
        LASSERT(vmin<=vmax);
        return (vmax-vmin)*random.frand() + vmin;
    }


    /**
    @brief [0, v)
    */
    template<class T, class U>
    U range_ropen(T& random, U v)
    {
        u32 i = random.rand() % v;
        return static_cast<U>(i);
    }

    template<class T, class U>
    void shuffle(T& random, U* start, U* end)
    {
        U* s = start-1;
        for(U* i=end-1; start<i; --i){
            u32 offset = range_ropen(random, (u32)(i-s));
            lcore::swap(*i, *(start+offset));
        }
	}

    template<class T, class U>
    void shuffle(T& random, s32 num, U* v)
    {
        s32 n = num+1;
        for(s32 i=num-1; 0<i; --i){
            u32 offset = range_ropen(random, (u32)(n-i));
            lcore::swap(v[i], v[offset]);
        }
	}
}
    void cryptRandom(u32 size, void* buffer);
}

#endif //INC_RANDOM_H__

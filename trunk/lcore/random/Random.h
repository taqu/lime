#ifndef INC_RANDOM_H__
#define INC_RANDOM_H__
/**
@file Random.h
@author t-sakai
@date 2011/08/20 create
*/
#include "../lcore.h"

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
        ~RandomXorshift();

        /**
        @brief ‹[——”¶¬Ší‰Šú‰»
        @param seed
        */
        void srand(u32 seed);

        /**
        @brief 0 - 0xFFFFFFFFU‚Ì—”¶¬
        */
        u32 rand();

        /**
        @brief 0 - 1‚Ì—”¶¬
        */
        f32 frand();

        /**
        @brief 0 - 1‚Ì—”¶¬
        */
        f64 drand();

    private:
        u32 rand(u32 v, u32 i);

        u32 x_;
        u32 y_;
        u32 z_;
        u32 w_;
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
        ~RandomWELL();

        /**
        @brief ‹[——”¶¬Ší‰Šú‰»
        @param seed
        */
        void srand(u32 seed);

        /**
        @brief 0 - 0xFFFFFFFFU‚Ì—”¶¬
        */
        u32 rand();

        /**
        @brief 0 - 1‚Ì—”¶¬
        */
        f32 frand();

        /**
        @brief 0 - 1‚Ì—”¶¬
        */
        f64 drand();

    private:
        static const u32 N = 16;

        u32 state_[N];
        u32 index_;
    };
}

#endif //INC_RANDOM_H__

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
/**
@file MurmurHash.cpp
@author t-sakai
@date 2016/02/29
*/
#include "MurmurHash.h"

namespace lcore
{
    //----------------------------------------------------
    //---
    //--- MurmurHash
    //---
    //----------------------------------------------------
    namespace
    {
        static const u32 c1_32 = 0xcc9e2d51;
        static const u32 c2_32 = 0x1b873593;

        static inline u32 rotl32(u32 x, int r)
        {
            return (x<<r) | (x>>(32-r));
        }

        static inline u32 read32LE(const u32* ptr, int i)
        {
            return ptr[i];
        }

        static inline u32 fmix32(u32 h)
        {
            h ^= h >> 16;
            h *= 0x85ebca6bU;
            h ^= h >> 13;
            h *= 0xc2b2ae35U;
            h ^= h >> 16;
            return h;
        }
    }

    void MurmurHash32Init(MurmurHashContext32& context, u32 seed)
    {
        memset(&context, 0, sizeof(MurmurHashContext32));
        context.h1_ = seed;
    }

    void MurmurHash32Update4(MurmurHashContext32& context, const u8* data, u32 offset, u32 length)
    {
        data += offset;
        s32 nblocks = length >> 2;

        const u32* blocks = (const u32*)(data+(nblocks<<2));
        for(s32 i = -nblocks; i < 0; ++i){
            u32 k1 = read32LE(blocks, i);
            k1 *= c1_32;
            k1 = rotl32(k1, 15);
            k1 *= c2_32;

            context.h1_ ^= k1;
            context.h1_ = rotl32(context.h1_, 13);
            context.h1_ = context.h1_ * 5 + 0xe6546b64U;
        }
        context.totalLength_ += (u32)length;
    }

    void MurmurHash32Tail(MurmurHashContext32& context, const u8* data, u32 offset, u32 length)
    {
        data += offset;
        s32 nblocks = length >> 2;

        const u32* blocks = (const u32*)(data+(nblocks<<2));
        for(s32 i = -nblocks; i < 0; ++i){
            u32 k1 = read32LE(blocks, i);
            k1 *= c1_32;
            k1 = rotl32(k1, 15);
            k1 *= c2_32;

            context.h1_ ^= k1;
            context.h1_ = rotl32(context.h1_, 13);
            context.h1_ = context.h1_ * 5 + 0xe6546b64U;
        }

        const u8* tail = (data + (nblocks << 2));

        u32 l1 = 0;
        switch(length & 3)
        {
        case 3:
            l1 ^= tail[2] << 16;
        case 2:
            l1 ^= tail[1] << 8;
        case 1:
            l1 ^= tail[0];
            l1 *= c1_32;
            l1 = rotl32(l1, 15);
            l1 *= c2_32;
            context.h1_ ^= l1;
            break;
        }

        context.totalLength_ += (u32)length;
    }

    u32 MurmurHash32Finalize(MurmurHashContext32& context)
    {
        context.h1_ ^= context.totalLength_;
        return fmix32(context.h1_);
    }
}

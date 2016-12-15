/*
   xxHash - Extremely Fast Hash algorithm
   Header File
   Copyright (C) 2012-2016, Yann Collet.

   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
   - xxHash source repository : https://github.com/Cyan4973/xxHash
*/
/**
@file xxHash.cpp
@author t-sakai
@date 2016/02/28 create
*/
#include "xxHash.h"

namespace lcore
{
//----------------------------------------------------
//---
//--- xxHash
//---
//----------------------------------------------------
    namespace
    {
        static const u32 PRIME32_1 = 2654435761U;
        static const u32 PRIME32_2 = 2246822519U;
        static const u32 PRIME32_3 = 3266489917U;
        static const u32 PRIME32_4 = 668265263U;
        static const u32 PRIME32_5 = 374761393U;

        static const u64 PRIME64_1 = 11400714785074694791UL;
        static const u64 PRIME64_2 = 14029467366897019727UL;
        static const u64 PRIME64_3 = 1609587929392839161UL;
        static const u64 PRIME64_4 = 9650029242287828579UL;
        static const u64 PRIME64_5 = 2870177450012600261UL;


        static inline u32 rotl32(u32 x, int r)
        {
            return (x<<r) | (x>>(32-r));
        }

        static inline u64 rotl64(u64 x, int r)
        {
            return (x << r) | (x >> (64 - r));
        }

        static inline u32 read32LE(const void* ptr)
        {
            return *((u32*)ptr);
        }

        static inline u64 read64LE(const void* ptr)
        {
            return *((u64*)ptr);
        }
    }

    void xxHash32Init(xxHashContext32& context, u32 seed)
    {
        memset(&context, 0, sizeof(xxHashContext32));
        context.seed_ = seed;
        context.v1_ = seed + PRIME32_1 + PRIME32_2;
        context.v2_ = seed + PRIME32_2;
        context.v3_ = seed + 0;
        context.v4_ = seed - PRIME32_1;
    }

    void xxHash32Update(xxHashContext32& context, const u8* input, u32 length)
    {
        context.totalLength_ += length;
        if((context.memSize_ + length) < 16) {
            memcpy((u8*)(context.mem_) + context.memSize_, input, length);
            context.memSize_ += length;
            return;
        }

        const u8* ptr = input;
        const u8* end = input + length;
        if(0 != context.memSize_) {
            memcpy((u8*)(context.mem_) + context.memSize_, input, 16 - context.memSize_);

            const u32* mem = context.mem_;
            context.v1_ += read32LE(mem) * PRIME32_2;
            context.v1_ = rotl32(context.v1_, 13);
            context.v1_ *= PRIME32_1;
            ++mem;
            context.v2_ += read32LE(mem) * PRIME32_2;
            context.v2_ = rotl32(context.v2_, 13);
            context.v2_ *= PRIME32_1;
            ++mem;
            context.v3_ += read32LE(mem) * PRIME32_2;
            context.v3_ = rotl32(context.v3_, 13);
            context.v3_ *= PRIME32_1;
            ++mem;
            context.v4_ += read32LE(mem) * PRIME32_2;
            context.v4_ = rotl32(context.v4_, 13);
            context.v4_ *= PRIME32_1;

            ptr += 16 - context.memSize_;
            context.memSize_ = 0;
        }
        if((ptr + 16) <= end) {
            const u8* limit = end - 16;
            u32 v1 = context.v1_;
            u32 v2 = context.v2_;
            u32 v3 = context.v3_;
            u32 v4 = context.v4_;
            do {
                v1 += read32LE(ptr) * PRIME32_2;
                v1 = rotl32(v1, 13);
                v1 *= PRIME32_1;
                ptr += 4;
                v2 += read32LE(ptr) * PRIME32_2;
                v2 = rotl32(v2, 13);
                v2 *= PRIME32_1;
                ptr += 4;
                v3 += read32LE(ptr) * PRIME32_2;
                v3 = rotl32(v3, 13);
                v3 *= PRIME32_1;
                ptr += 4;
                v4 += read32LE(ptr) * PRIME32_2;
                v4 = rotl32(v4, 13);
                v4 *= PRIME32_1;
                ptr += 4;
            } while(ptr <= limit);

            context.v1_ = v1;
            context.v2_ = v2;
            context.v3_ = v3;
            context.v4_ = v4;
        }
        if(ptr < end) {
            context.memSize_ = (u32)(end - ptr);
            memcpy(context.mem_, ptr, context.memSize_);
        }
    }

    u32 xxHash32Finalize(xxHashContext32& context)
    {
        u32 h32;
        if(16 <= context.totalLength_) {
            h32 = rotl32(context.v1_, 1) + rotl32(context.v2_, 7) + rotl32(context.v3_, 12) + rotl32(context.v4_, 18);
        } else {
            h32 = context.seed_ + PRIME32_5;
        }
        h32 += (u32)context.totalLength_;

        const u8* ptr = (const u8*)context.mem_;
        const u8* end = ptr + context.memSize_;

        while((ptr + 4) <= end) {
            h32 += read32LE(ptr) * PRIME32_3;
            h32 = rotl32(h32, 17) * PRIME32_4;
            ptr += 4;
        }
        while(ptr < end) {
            h32 += PRIME32_5*ptr[0];
            h32 = rotl32(h32, 11) * PRIME32_1;
            ++ptr;
        }

        h32 ^= h32 >> 15;
        h32 *= PRIME32_2;
        h32 ^= h32 >> 13;
        h32 *= PRIME32_3;
        h32 ^= h32 >> 16;
        return h32;
    }


    void xxHash64Init(xxHashContext64& context, u64 seed)
    {
        memset(&context, 0, sizeof(xxHashContext64));
        context.seed_ = seed;
        context.v1_ = seed + PRIME64_1 + PRIME64_2;
        context.v2_ = seed + PRIME64_2;
        context.v3_ = seed + 0;
        context.v4_ = seed - PRIME64_1;
    }

    void xxHash64Update(xxHashContext64& context, const u8* input, u32 length)
    {
        context.totalLength_ += length;
        if((context.memSize_ + length) < 32) {
            memcpy((u8*)context.mem_ + context.memSize_, input, length);
            context.memSize_ += length;
            return;
        }

        const u8* ptr = input;
        const u8* end = input + length;
        if(0 != context.memSize_) {
            memcpy((u8*)context.mem_ + context.memSize_, input, 32 - context.memSize_);
            const u64* mem = context.mem_;
            context.v1_ += read64LE(mem) * PRIME64_2;
            context.v1_ = rotl64(context.v1_, 31);
            context.v1_ *= PRIME64_1;
            ++mem;
            context.v2_ += read64LE(mem) * PRIME64_2;
            context.v2_ = rotl64(context.v2_, 31);
            context.v2_ *= PRIME64_1;
            ++mem;
            context.v3_ += read64LE(mem) * PRIME64_2;
            context.v3_ = rotl64(context.v3_, 31);
            context.v3_ *= PRIME64_1;
            ++mem;
            context.v4_ += read64LE(mem) * PRIME64_2;
            context.v4_ = rotl64(context.v4_, 31);
            context.v4_ *= PRIME64_1;

            ptr += 32 - context.memSize_;
            context.memSize_ = 0;
        }
        if((ptr + 32) <= end) {
            const u8* limit = end - 32;
            u64 v1 = context.v1_;
            u64 v2 = context.v2_;
            u64 v3 = context.v3_;
            u64 v4 = context.v4_;
            do {
                v1 += read64LE(ptr) * PRIME64_2;
                v1 = rotl64(v1, 31);
                v1 *= PRIME64_1;
                ptr += 8;
                v2 += read64LE(ptr) * PRIME64_2;
                v2 = rotl64(v2, 31);
                v2 *= PRIME64_1;
                ptr += 8;
                v3 += read64LE(ptr) * PRIME64_2;
                v3 = rotl64(v3, 31);
                v3 *= PRIME64_1;
                ptr += 8;
                v4 += read64LE(ptr) * PRIME64_2;
                v4 = rotl64(v4, 31);
                v4 *= PRIME64_1;
                ptr += 8;
            } while(ptr <= limit);

            context.v1_ = v1;
            context.v2_ = v2;
            context.v3_ = v3;
            context.v4_ = v4;
        }
        if(ptr < end) {
            context.memSize_ = end - ptr;
            memcpy(context.mem_, ptr, context.memSize_);
        }
    }

    u64 xxHash64Finalize(xxHashContext64& context)
    {
        u64 h64;
        if(32 <= context.totalLength_) {
            u64 v1 = context.v1_;
            u64 v2 = context.v2_;
            u64 v3 = context.v3_;
            u64 v4 = context.v4_;

            h64 = rotl64(v1, 1) + rotl64(v2, 7) + rotl64(v3, 12) + rotl64(v4, 18);

            v1 *= PRIME64_2;
            v1 = rotl64(v1, 31);
            v1 *= PRIME64_1;
            h64 ^= v1;
            h64 = h64*PRIME64_1 + PRIME64_4;

            v2 *= PRIME64_2;
            v2 = rotl64(v2, 31);
            v2 *= PRIME64_1;
            h64 ^= v2;
            h64 = h64*PRIME64_1 + PRIME64_4;

            v3 *= PRIME64_2;
            v3 = rotl64(v3, 31);
            v3 *= PRIME64_1;
            h64 ^= v3;
            h64 = h64*PRIME64_1 + PRIME64_4;

            v4 *= PRIME64_2;
            v4 = rotl64(v4, 31);
            v4 *= PRIME64_1;
            h64 ^= v4;
            h64 = h64*PRIME64_1 + PRIME64_4;
        } else {
            h64 = context.seed_ + PRIME64_5;
        }

        h64 += context.totalLength_;

        const u8* ptr = (const u8*)context.mem_;
        const u8* end = ptr + context.memSize_;
        while((ptr + 8) <= end) {
            u64 k1 = read64LE(ptr);
            k1 *= PRIME64_2;
            k1 = rotl64(k1, 31);
            k1 *= PRIME64_1;
            h64 ^= k1;
            h64 = rotl64(h64, 27) * PRIME64_1 + PRIME64_4;
            ptr += 8;
        }
        if((ptr + 4) <= end) {
            h64 ^= PRIME64_1 * read32LE(ptr);
            h64 = rotl64(h64, 23) * PRIME64_2 + PRIME64_3;
            ptr += 4;
        }
        while(ptr < end) {
            h64 ^= PRIME64_5 * ptr[0];
            h64 = rotl64(h64, 11) * PRIME64_1;
            ++ptr;
        }

        h64 ^= h64 >> 33;
        h64 *= PRIME64_2;
        h64 ^= h64 >> 29;
        h64 *= PRIME64_3;
        h64 ^= h64 >> 32;
        return h64;
    }
}


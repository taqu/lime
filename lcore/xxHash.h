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
#ifndef INC_LCORE_XXHASH_H__
#define INC_LCORE_XXHASH_H__
/**
@file xxHash.h
@author t-sakai
@date 2016/02/28 create
*/
#include "lcore.h"

namespace lcore
{
    //----------------------------------------------------
    //---
    //--- xxHash
    //---
    //----------------------------------------------------
    struct xxHashContext32
    {
        u64 totalLength_;
        u32 seed_;
        u32 v1_;
        u32 v2_;
        u32 v3_;
        u32 v4_;
        u32 mem_[4];
        u32 memSize_;
    };

    struct xxHashContext64
    {
        u64 totalLength_;
        u64 seed_;
        u64 v1_;
        u64 v2_;
        u64 v3_;
        u64 v4_;
        u64 mem_[4];
        u64 memSize_;
    };

    static const u32 xxHash32_DefaultSeed = 88675123U;
    static const u64 xxHash64_DefaultSeed = 2685821657736338717ULL;

    //--- xxHash32
    //--------------------------------------------------------------------------
    void xxHash32Init(xxHashContext32& context, u32 seed=xxHash32_DefaultSeed);
    void xxHash32Update(xxHashContext32& context, const u8* input, u32 length);
    u32 xxHash32Finalize(xxHashContext32& context);
    u32 xxHash32(const u8* data, s32 length, u32 seed=xxHash32_DefaultSeed);
    /**
    @brief 4バイト用
    */
    u32 xxHash32_4(const u8* data, u32 seed);
    /**
    @brief 8バイト用
    */
    u32 xxHash32_8(const u8* data, u32 seed);

    inline u32 xxHash32(s32 x, u32 seed=xxHash32_DefaultSeed)
    {
        return xxHash32_4(reinterpret_cast<const u8*>(&x), seed);
    }
    inline u32 xxHash32(u32 x, u32 seed=xxHash32_DefaultSeed)
    {
        return xxHash32_4(reinterpret_cast<const u8*>(&x), seed);
    }

    inline u32 xxHash32(s64 x, u32 seed=xxHash32_DefaultSeed)
    {
        return xxHash32_8(reinterpret_cast<const u8*>(&x), seed);
    }
    inline u32 xxHash32(u64 x, u32 seed=xxHash32_DefaultSeed)
    {
        return xxHash32_8(reinterpret_cast<const u8*>(&x), seed);
    }

    //--- xxHash64
    //--------------------------------------------------------------------------
    void xxHash64Init(xxHashContext64& context, u64 seed=xxHash64_DefaultSeed);
    void xxHash64Update(xxHashContext64& context, const u8* input, u32 length);
    u64 xxHash64Finalize(xxHashContext64& context);
    u64 xxHash64(const u8* data, s32 length, u64 seed=xxHash64_DefaultSeed);
    /**
    @brief 4バイト用
    */
    u64 xxHash64_4(const u8* data, u64 seed);
    /**
    @brief 8バイト用
    */
    u64 xxHash64_8(const u8* data, u64 seed);

    inline u64 xxHash64(s32 x, u64 seed=xxHash64_DefaultSeed)
    {
        return xxHash64_4(reinterpret_cast<const u8*>(&x), seed);
    }
    inline u64 xxHash64(u32 x, u64 seed=xxHash64_DefaultSeed)
    {
        return xxHash64_4(reinterpret_cast<const u8*>(&x), seed);
    }

    inline u64 xxHash64(s64 x, u64 seed=xxHash64_DefaultSeed)
    {
        return xxHash64_8(reinterpret_cast<const u8*>(&x), seed);
    }
    inline u64 xxHash64(u64 x, u64 seed=xxHash64_DefaultSeed)
    {
        return xxHash64_8(reinterpret_cast<const u8*>(&x), seed);
    }
}
#endif //INC_LCORE_XXHASH_H__

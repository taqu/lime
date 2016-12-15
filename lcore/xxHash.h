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

    void xxHash32Init(xxHashContext32& context, u32 seed);
    void xxHash32Update(xxHashContext32& context, const u8* input, u32 length);
    u32 xxHash32Finalize(xxHashContext32& context);

    void xxHash64Init(xxHashContext64& context, u64 seed);
    void xxHash64Update(xxHashContext64& context, const u8* input, u32 length);
    u64 xxHash64Finalize(xxHashContext64& context);
}
#endif //INC_LCORE_XXHASH_H__

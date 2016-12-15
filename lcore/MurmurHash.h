//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
#ifndef INC_LCORE_MURMURHASH_H__
#define INC_LCORE_MURMURHASH_H__
/**
@file MurmurHash.h
@author t-sakai
@date 2016/02/29
*/
#include "lcore.h"

namespace lcore
{
    //----------------------------------------------------
    //---
    //--- MurmurHash
    //---
    //----------------------------------------------------

    struct MurmurHashContext32
    {
        u32 totalLength_;
        u32 h1_;
        u32 memLength_;
        u8 mem_[4];
    };

    void MurmurHash32Init(MurmurHashContext32& context, u32 seed);
    void MurmurHash32Update4(MurmurHashContext32& context, const u8* data, u32 offset, u32 length);
    void MurmurHash32Tail(MurmurHashContext32& context, const u8* data, u32 offset, u32 length);
    u32 MurmurHash32Finalize(MurmurHashContext32& context);
}
#endif //INC_LCORE_MURMURHASH_H__

#ifndef INC_PACK_PACK_H__
#define INC_PACK_PACK_H__
/**
@file Pack.h
@author t-sakai
@date 2012/03/20 create

*/
#include <lcore/lcore.h>

namespace pack
{

    using lcore::s8;
    using lcore::s16;
    using lcore::s32;

    using lcore::u8;
    using lcore::u16;
    using lcore::u32;

    using lcore::f32;
    using lcore::f64;

    using lcore::Char;

    static const u32 MaxFileNameLength = 31;
    static const u32 MaxFileNameBufferSize = MaxFileNameLength + 1;
    static const u32 MaxPathLen = 256;

    /// リトルエンディアンか
    bool isLittleEndian();

    /// バイト反転
    template<class T>
    void byteReverse(T& x)
    {
        u8* l = (u8*)&x;
        u8* h = l + sizeof(T);
        u8 t;

        while(--h>l){
            t = *l;
            *l = *h;
            --l;
            h = t;
        }
    }

    enum Endian
    {
        Endian_Little = 0,
    };

    struct PackHeader
    {
        u8 endian_;
        u8 reserved_;
        u16 numFiles_;
    };

    struct FileEntry
    {
        u32 size_;
        s32 offset_;
        Char name_[MaxFileNameBufferSize];
    };
}
#endif //INC_PACK_PACK_H__

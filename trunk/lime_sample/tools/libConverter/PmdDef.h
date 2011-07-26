#ifndef INC_PMD_PMDDEF_H__
#define INC_PMD_PMDDEF_H__
/**
@file PmdDef.h
@author t-sakai
@date 2011/07/17 create
*/
#include "converter.h"

namespace pmd
{
    using lcore::Char;
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;

    using lcore::u8;
    using lcore::u16;
    using lcore::u32;

    using lcore::f32;

    typedef lcore::Char CHAR;

    typedef u8 BYTE;
    typedef u16 WORD;
    typedef u32 DWORD;
    typedef f32 FLOAT;

    static const u32 NameSize = 20;
    static const u32 FileNameSize = 20;
    static const u32 CommentSize = 256;
    static const u32 LabelNameSize = 50;
    static const u32 ToonTexturePathSize = 100;
    static const u32 FilePathSize = 256;
    static const u32 NumToonTextures = 11;
}
#endif //INC_PMD_PMDDEF_H__

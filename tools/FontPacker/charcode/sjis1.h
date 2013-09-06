#ifndef INC_CHARCODE_SJIS1_H__
#define INC_CHARCODE_SJIS1_H__
/**
@file sjis1.h
@author t-sakai
@date 2013/08/29 create
*/
#include "charcode.h"

namespace charcode
{
    struct Entry
    {
        u16 code_;
        const Char* str_;
    };

    static const u32 NumEntries = 3647;

    extern const Entry SJIS_LEVEL1[NumEntries];
}
#endif //INC_CHARCODE_SJIS1_H__

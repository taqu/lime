/**
@file jis_level2_utf16.cpp
@author t-sakai
@date 2011/02/24 create

*/
#include "conv_charcode.h"
#include "jis_level2_utf16.h"

namespace charcode
{
const u16 jis_level2_utf16_table[NUM_JIS_LEVEL2_UTF16] =
#include "jis_level2_utf16.txt"
;
}

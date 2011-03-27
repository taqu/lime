/**
@file conv_charcode.cpp
@author t-sakai
@date 2011/02/24 create

*/
#include "conv_charcode.h"
#include "jis_level1_utf16.h"

#if !defined(NULL)
#define NULL (0)
#endif

namespace charcode
{
    namespace
    {
        u8 dummy[4];
    }

    //--------------------------------------------
    // Shift-JIS文字をJIS文字に変換
    u32 SJISToJIS(u8* jis, const u8* sjis)
    {
        LASSERT(sjis != NULL);

        if(jis == NULL){
            jis = dummy;
        }

        //ASCIIはそのまま返す
        if(!(sjis[0] & 0x80U)){
            jis[0] = sjis[0];
            return 1;
        }

        //半角カナもそのまま返す
        if(sjis[0]>=0xA1U && sjis[0]<=0xDFU){
            jis[0] = sjis[0];
            return 1;
        }


        jis[0] = (sjis[0]>=0xE0U)? (sjis[0] - 0x40U) : sjis[0];

        jis[1] = (sjis[1]>=0x80U)? (sjis[1] - 1) : sjis[1];

        if(jis[1]>=0x9EU){
            jis[0] = (jis[0] - 0x70U) << 1;
            jis[1] -= 0x7DU;
        }else{
            jis[0] = ((jis[0] - 0x70U) << 1) - 1;
            jis[1] -= 0x1FU;
        }
        return 2;
    }

    //--------------------------------------------
    // UTF-16BEをUTF-8へ変換
    u32 UTF16BEToUTF8(u8* utf8, u16 utf16)
    {
        if(utf8 == NULL){
            utf8 = dummy;
        }

        u32 size;
        if(!(utf16 & (~0x7FU))){
            size = 1;
        }else if(!(utf16 & (~0x07FFU))){
            size = 2;
        }else{
            size = 3;
        }
        if(utf8 == NULL){
            return size;
        }

        switch(size)
        {
        case 1:
            utf8[0] = utf16 & 0x7FU;
            break;

        case 2:
            utf8[0] = (utf16>>6 & 0x1FU) | 0xC0U;
            utf8[1] = (utf16 & 0x3FU) | 0x80U;
            break;

        case 3:
            utf8[0] = (utf16>>12 & 0x0FU) | 0xE0U;
            utf8[1] = (utf16>> 6 & 0x3FU) | 0x80U;
            utf8[2] = (utf16 & 0x3FU) | 0x80U;
            break;
        };
        return size;
    }

    //--------------------------------------------
    // JIS文字からUTF-8文字へ変換
    u32 JISToUTF8(u8* utf, const u8* jis)
    {
        LASSERT(jis != NULL);

        if(utf == NULL){
            utf = dummy;
        }

        //カナの範囲
        //---------------------------------
        if(jis[0]>=0xA1U && jis[0]<=0xBFU){
            utf[0] = 0xEFU;
            utf[1] = 0xBDU;
            utf[2] = jis[0];
            return 3;
        }

        if(jis[0]>=0xC0U && jis[0]<=0xDFU){
            utf[0] = 0xEFU;
            utf[1] = 0xBDU;
            utf[2] = jis[0] - 0x40U;
            return 3;
        }

        //JIS漢字コード、第１・２水準範囲外
        u16 jiscode = (jis[1] | jis[0] << 8);
        if(jiscode < 0x2121U || 0x7426U<jiscode){
            return 0;
        }

        u16 utf16code;

        //第１水準の範囲か
        if(0x5021U>jiscode){
            utf16code = jis_level1_utf16_table[jiscode - 0x2121U];
        }else{
            //第２水準は無視
#if 1
            return 0;
#else
            utf16code = jis_level2_utf16_table[jiscode - 0x5021U];
#endif
        }

        if(utf16code == 0){
            return 0;
        }

        return UTF16BEToUTF8(utf, utf16code);
    }


    //--------------------------------------------
    u32 strSJISToUTF8(u8* utf, const u8* sjis)
    {
        LASSERT(sjis != NULL);

        u8 jis[2];
        u32 size = 0;
        u32 byte;
        while(*sjis != '\0'){
            byte = SJISToJIS(jis, sjis);
            sjis += byte;

            if(byte<=1){
                if(utf){
                    utf[0] = jis[0];
                    utf += 1;
                }
            }else{
                byte = JISToUTF8(utf, jis);
                if(utf){
                    utf += byte;
                }
            }
            size += byte;
        }
        return size;
    }
}

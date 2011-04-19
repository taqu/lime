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
    // Shift-JISï∂éöÇJISï∂éöÇ…ïœä∑
    u32 SJISToJIS(u8* jis, const u8* sjis)
    {
        LASSERT(sjis != NULL);

        if(jis == NULL){
            jis = dummy;
        }

        //ASCIIÇÕÇªÇÃÇ‹Ç‹ï‘Ç∑
        if(!(sjis[0] & 0x80U)){
            jis[0] = sjis[0];
            return 1;
        }

        //îºäpÉJÉiÇÕÇªÇÃÇ‹Ç‹ï‘Ç∑
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
    // UTF-16BEÇUTF-8Ç÷ïœä∑
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
    // JISï∂éöÇ©ÇÁUTF-8ï∂éöÇ÷ïœä∑
    u32 JISToUTF8(u8* utf, const u8* jis)
    {
        LASSERT(jis != NULL);

        if(utf == NULL){
            utf = dummy;
        }

        //ÉJÉiÇÃîÕàÕ
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

        //JISäøéöÉRÅ[ÉhÅAëÊÇPÅEÇQêÖèÄîÕàÕäO
        u16 jiscode = (jis[1] | jis[0] << 8);
        if(jiscode < 0x2121U || 0x7426U<jiscode){
            return 0;
        }

        u16 utf16code;

        //ëÊÇPêÖèÄÇÃîÕàÕÇ©
        if(0x5021U>jiscode){
            utf16code = jis_level1_utf16_table[jiscode - 0x2121U];
        }else{
            //ëÊÇQêÖèÄÇÕñ≥éã
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
                //îºäpÉJÉiÇÃîÕàÕ
                if(0xA1U<=jis[0] && jis[0]<=0xDFU){
                    byte = 3;
                    if(utf){
                        utf[0] = 0xEFU;
                        if(jis[0]>=0xC0U){
                            utf[1] = 0xBEU;
                            utf[2] = jis[0] - 0x40U;
                        }else{
                            utf[1] = 0xBDU;
                            utf[2] = jis[0];
                        }
                        utf += 3;
                    }

                }else{
                    if(utf){
                        utf[0] = jis[0];
                        utf += 1;
                    }
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

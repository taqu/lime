#ifndef INC_CONV_CHARCODE_H__
#define INC_CONV_CHARCODE_H__
/**
@file conv_charcode.h
@author t-sakai
@date 2011/02/24 create

*/
#include <lcore/lcore.h>

namespace charcode
{
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::Char;


    //--------------------------------------------
    /**
    @brief Shift-JIS文字をJIS文字に変換
    @return 変換後バイト数
    @param jis ... 出力。サイズ２以上の配列
    @param sjis ... 入力

    入力がShift-JISであること。制御コード、ローマ字、半角カナは変換しない。
    Shift-JIS
     1バイト
     00-1F, 7F 制御コード
     20-7E JISローマ字(ASCII)
     A1-DF JISカナ(半角カナ)
     2バイト
     81-9F 40-7E JIS漢字
     E0-EF 80-FC JIS漢字
    */
    u32 SJISToJIS(u8* jis, const u8* sjis);

    //--------------------------------------------
    /**
    @brief UTF-16BEをUTF-8へ変換
    @return 変換後バイト数
    @param utf8 ... 出力。変換後UTF-8文字。最大３バイト
    @param utf16 ... UTF-16BE文字
    */
    u32 UTF16BEToUTF8(u8* utf8, u16 utf16);


    //--------------------------------------------
    /**
    @brief JIS文字からUTF-8文字へ変換
    @return 変換後バイト数
    @param utf ... 出力。最大３バイト
    @param jis ... 入力

    UTF-8 <- JIS
    1バイト
    00-1F, 7F 制御コード
    20-7E JISローマ字(ASCII)
    FF61 - FF9F カナ(半角カナ) <- 21-5F

    JIS漢字コード
    0x2121 - 0x4F53 JIS-X0208 第１水準
    0x5021 - 0x7426 JIS-X0208 第２水準

    ASCII は対応しない
    */
    u32 JISToUTF8(u8* utf, const u8* jis);


    u32 strSJISToUTF8(u8* utf, const u8* sjis);
}
#endif //INC_CONV_CHARCODE_H__

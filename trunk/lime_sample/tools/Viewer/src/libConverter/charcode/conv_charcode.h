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
    @brief Shift-JIS������JIS�����ɕϊ�
    @return �ϊ���o�C�g��
    @param jis ... �o�́B�T�C�Y�Q�ȏ�̔z��
    @param sjis ... ����

    ���͂�Shift-JIS�ł��邱�ƁB����R�[�h�A���[�}���A���p�J�i�͕ϊ����Ȃ��B
    Shift-JIS
     1�o�C�g
     00-1F, 7F ����R�[�h
     20-7E JIS���[�}��(ASCII)
     A1-DF JIS�J�i(���p�J�i)
     2�o�C�g
     81-9F 40-7E JIS����
     E0-EF 80-FC JIS����
    */
    u32 SJISToJIS(u8* jis, const u8* sjis);

    //--------------------------------------------
    /**
    @brief UTF-16BE��UTF-8�֕ϊ�
    @return �ϊ���o�C�g��
    @param utf8 ... �o�́B�ϊ���UTF-8�����B�ő�R�o�C�g
    @param utf16 ... UTF-16BE����
    */
    u32 UTF16BEToUTF8(u8* utf8, u16 utf16);


    //--------------------------------------------
    /**
    @brief JIS��������UTF-8�����֕ϊ�
    @return �ϊ���o�C�g��
    @param utf ... �o�́B�ő�R�o�C�g
    @param jis ... ����

    UTF-8 <- JIS
    1�o�C�g
    00-1F, 7F ����R�[�h
    20-7E JIS���[�}��(ASCII)
    FF61 - FF9F �J�i(���p�J�i) <- 21-5F

    JIS�����R�[�h
    0x2121 - 0x4F53 JIS-X0208 ��P����
    0x5021 - 0x7426 JIS-X0208 ��Q����

    ASCII �͑Ή����Ȃ�
    */
    u32 JISToUTF8(u8* utf, const u8* jis);


    u32 strSJISToUTF8(u8* utf, const u8* sjis);
}
#endif //INC_CONV_CHARCODE_H__

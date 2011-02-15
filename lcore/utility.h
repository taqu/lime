#ifndef INC_LCORE_UTILITY_H__
#define INC_LCORE_UTILITY_H__
/**
@file utility.h
@author t-sakai
@date 2011/02/05 create
*/
#include "lcore.h"

namespace lcore
{
    /**
    @brief ������當���T��
    @param src ... ����
    @param c ... �T������
    @param size ... ������
    */
    const Char* rFindChr(const Char* src, Char c, u32 size);


    /**
    @brief �p�X����f�B���N�g���p�X���o
    @return dst�̒����B�k���܂܂�
    @param dst ... �o�̓o�b�t�@�B�k�����݂ŏ\���ȃT�C�Y�����邱��
    @param path ... ��̓p�X
    @param length ... ��̓p�X�̒����B�k���܂܂�
    */
    u32 extractDirectoryPath(CHAR* dst, const CHAR* path, u32 length);
}
#endif //INC_LCORE_UTILITY_H__

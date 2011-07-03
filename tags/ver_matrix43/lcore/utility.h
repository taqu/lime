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
    @brief 後方から文字探索
    @param src ... 入力
    @param c ... 探索文字
    @param size ... 文字列長
    */
    const Char* rFindChr(const Char* src, Char c, u32 size);


    /**
    @brief パスからディレクトリパス抽出
    @return dstの長さ。ヌル含まず
    @param dst ... 出力バッファ。ヌル込みで十分なサイズがあること
    @param path ... 解析パス
    @param length ... 解析パスの長さ。ヌル含まず
    */
    u32 extractDirectoryPath(Char* dst, const Char* path, u32 length);
}
#endif //INC_LCORE_UTILITY_H__

/**
@file utlity.cpp
@author t-sakai
@date 2011/02/05
*/
#include "lcore.h"
#include "utility.h"

namespace lcore
{
    //-------------------------------------------------------------
    // 後方から文字探索
    const Char* rFindChr(const Char* src, Char c, u32 size)
    {
        LASSERT(src != NULL);
        src += (size-1);
        for(u32 i=0; i<size; ++i){
            if(*src == c){
                return src;
            }
            --src;
        }
        return NULL;
    }


    //-------------------------------------------------------------
    // パスからディレクトリパス抽出
    u32 extractDirectoryPath(CHAR* dst, const CHAR* path, u32 length)
    {
        if(length<=0){
            return 0;
        }

        s32 i = static_cast<s32>(length-1);
        for(; 0<=i; --i){
            if(path[i] == '/' || path[i] == '\\'){
                break;
            }
        }
        u32 dstSize = i+1;
        for(u32 j=0; j<dstSize; ++j){
            dst[j] = path[j];
        }
        dst[dstSize] = '\0';
        return dstSize;
    }
}

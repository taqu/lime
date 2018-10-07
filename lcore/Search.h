#ifndef INC_LCORE_SEARCH_H_
#define INC_LCORE_SEARCH_H_
/**
@file Search.h
@author t-sakai
@date 2011/03/02 create

*/

namespace lcore
{
    /**
    @brief ソート済み配列の2分探索
    @return 見つかった要素のインデックス、なければ-1
    @param U ... 検索対象型
    @param V ... 検索対象の値の型

    @param num
    @param elements
    @param value
    */
    template<class U, class V>
    s32 binarySearchIndex(s32 num, const U* elements, const V& value)
    {
        s32 mid;
        s32 left = 0;
        s32 right = num-1;
        while(left<=right){
            mid = (left+right) >> 1;
            if(elements[mid]<value){
                left = mid + 1;
            }else if(value<elements[mid]){
                right = mid - 1;
            }else{
                return mid;
            }
        }
        return -1;
    }

    /**
    @brief ソート済み配列の2分探索
    @return 見つかった要素のインデックス、なければ-1
    @param U ... 検索対象型
    @param V ... 検索対象の値の型
    @param CompFunc(const U& v0, const V& v1) ... 比較関数 v0<v1:負数, v0==v1:0, v0>v1:正数

    @param num
    @param elements
    @param value
    @param func
    */
    template<class U, class V, class CompFunc>
    s32 binarySearchIndex(s32 num, const U* elements, const V& value, CompFunc func)
    {
        s32 mid;
        s32 left = 0;
        s32 right = num-1;
        while(left<=right){
            mid = (left+right) >> 1;
            s32 ret = func(elements[mid], value);
            if(ret<0){
                left = mid + 1;
            }else if(0<ret){
                right = mid - 1;
            }else{
                return mid;
            }
        }
        return -1;
    }
}

#endif //INC_LCORE_SEARCH_H_

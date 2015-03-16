#ifndef INC_LCORE_SEARCH_H__
#define INC_LCORE_SEARCH_H__
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
    @param T ... インデックスの型
    @param U ... 検索対象型
    @param V ... 検索対象の値の型
    @param CompFunc(const T&, const V&) ... 比較関数 v0<v1:負数, v0==v1:0, v0>v1:正数

    @param num
    @param elements
    @param value
    @param func
    */
    template<class T, class U, class V, class CompFunc>
    T binarySearchIndex(T num, const U* elements, const V& value, CompFunc func)
    {
        T mid = 0;
        T left = 0;
        T right = num;
        while(left<right){
            mid = (left+right) >> 1;
            if(func(elements[mid], value)<=0){
                left = mid + 1;
            }else{
                right = mid;
            }
        }
        if(left <= 0) {
            return -1;
        }
        --left;
        return (0 == func(elements[left], value))? left : -1;
    }
}

#endif //INC_LCORE_SEARCH_H__

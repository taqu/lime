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
    @brief ���郁���o������l�ȉ��̗v�f�̃C���f�b�N�X����������
    @param T ... �����Ώی^
    @param U ... �C���f�b�N�X�̌^
    @param V ... �����Ώۂ̒l�̌^
    @param FuncGetValue ... T����V�𓾂�֐�

    @param num
    @param elements
    @param value
    @param getValue
    */
    template<class T, class U, class V, class FuncGetValue>
    V binarySearchLessThanEqualIndex(V num, const T* elements, U value, FuncGetValue getValue)
    {
        V mid = 0;
        V left = 0;
        V right = num;
        while(left<right){
            mid = (left+right) >> 1;
            if(getValue(elements[mid])<=value){
                left = mid + 1;
            }else{
                right = mid;
            }
        }
        return (left>0)? (left - 1) : left;
    }
}

#endif //INC_LCORE_SEARCH_H__

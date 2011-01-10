#ifndef INC_LCORE_PAIR_H__
#define INC_LCORE_PAIR_H__
/**
@file Pair.h
@author t-sakai
@date 2008/11/13 create
@data 2009/05/19 lcoreライブラリ用にnamespace変更
*/

namespace lcore
{
    template<class FirstType, class SecondType>
    struct Pair
    {
        typedef FirstType first_type;
        typedef SecondType second_type;

        Pair()
            :_first(first_type()),
            _second(second_type())
        {
        }

        Pair(const first_type& first, const second_type& second)
            :_first(first), _second(second)
        {
        }

        Pair(const Pair<first_type, second_type>& pair)
            :_first(pair._first),
            _second(pair._second)
        {

        }

        first_type _first;
        second_type _second;
    };
}

#endif //INC_LCORE_PAIR_H__

#ifndef INC_LCORE_BITARRAY_H__
#define INC_LCORE_BITARRAY_H__
/**
@file BitArray.h
@author t-sakai
@date 2016/07/25 create
*/
#include "lcore.h"

namespace lcore
{
    //----------------------------------------------------
    /**
    @brief 動的ビット配列
    */
    class BitArray
    {
    public:
        BitArray();
        explicit BitArray(u32 sizeInBits);
        ~BitArray();

        void clear();
        inline u32 sizeInBits() const;

        /**
        @brief フラグチェック
        @param index
        */
        bool check(u32 index) const;

        /**
        @brief フラグ立てる
        */
        void set(u32 index);

        /**
        @brief フラグ降ろす
        */
        void reset(u32 index);

        void add(u32 index, bool flag);

        void swap(BitArray& rhs);
    private:
        BitArray(const BitArray&);
        BitArray& operator=(const BitArray&);

        static const u32 UnitBytes = 16;
        static const u32 UnitBits = UnitBytes<<3;

        u32 size_;
        u32* bits_;
    };

    inline u32 BitArray::sizeInBits() const
    {
        return size_<<5;
    }
}
#endif //INC_LCORE_BITARRAY_H__

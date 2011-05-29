#ifndef INC_LCORE_BITSET_H__
#define INC_LCORE_BITSET_H__
/**
@file BitSet.h
@author t-sakai
@date 2010/04/09 create

*/
#include "lcore.h"

namespace lcore
{
    //----------------------------------------------------
    /**
    @brief ビットフラグセット
    */
    template<u32 SizeInByte>
    class BitSet
    {
    public:
    private:
    };

    //----------------------------------------------------
    /**
    @brief ビットフラグセット比較
    */
    template<u32 SizeInByte>
    inline bool compare(const BitSet<SizeInByte>& b0, const BitSet<SizeInByte>& b1)
    {
        return false;
    }



    //----------------------------------------------------
    /**
    @brief ビットフラグセット
    */
    template<>
    class BitSet<4>
    {
    public:
        typedef BitSet<4> this_type;

        BitSet()
            :bits_(0)
        {
        }

        ~BitSet()
        {
        }

        void clear()
        {
            bits_ = 0;
        }

        /**
        @brief フラグチェック
        @param flag
        */
        inline bool check(u32 flag) const
        {
            return ((bits_ & flag) != 0);
        }

        /**
        @brief フラグ立てる
        */
        inline void set(u32 flag)
        {
            bits_ |= flag;
        }

        /**
        @brief フラグ降ろす
        */
        inline void reset(u32 flag)
        {
            bits_ &= ~flag;
        }

        inline void set(const this_type& rhs)
        {
            bits_ = rhs.bits_;
        }
    private:
        friend bool compare(const BitSet<4>& b0, const BitSet<4>& b1);

        u32 bits_;
    };

    //----------------------------------------------------
    /**
    @brief ビットフラグセット比較
    */
    inline bool compare(const BitSet<4>& b0, const BitSet<4>& b1)
    {
        return (b0.bits_ == b1.bits_);
    }


    typedef BitSet<4> BitSet32;
}

#endif //INC_LCORE_BITSET_H__

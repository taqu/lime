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
        typedef BitSet<SizeInByte> this_type;

        BitSet()
        {
            clear();
        }

        ~BitSet()
        {
        }

        void clear()
        {
            for(u32 i=0; i<SizeInByte; ++i){
                bits_[i] = 0;
            }
        }

        /**
        @brief フラグチェック
        @param flag
        */
        inline bool check(u32 index) const
        {
            u8 bit = index & 0x07U;
            index = index >> 3;
            return ((bits_[index] & (0x01U<<bit)) != 0);
        }

        /**
        @brief フラグ立てる
        */
        inline void set(u32 index)
        {
            u8 bit = index & 0x07U;
            index = index >> 3;
            bits_[index] |= (0x01U<<bit);
        }

        /**
        @brief フラグ
        */
        inline void set(u32 index, bool flag)
        {
            if(flag){
                set(index);
            }else{
                reset(index);
            }
        }

        /**
        @brief フラグ降ろす
        */
        inline void reset(u32 index)
        {
            u8 bit = index & 0x07U;
            index = index >> 3;
            bits_[index] &= ~(0x01U<<bit);
        }

        inline u8 getByte(u32 index) const{ return bits_[index];}
    private:
        u8 bits_[SizeInByte];
    };

    //----------------------------------------------------
    /**
    @brief ビットフラグセット比較
    */
    template<u32 SizeInByte>
    inline bool compare(const BitSet<SizeInByte>& b0, const BitSet<SizeInByte>& b1)
    {
        for(u32 i=0; i<SizeInByte; ++i){
            if(b0.getByte(i) != b1.getByte(i)){
                return false;
            }
        }
        return true;
    }


    class BitSet32
    {
    public:
        BitSet32()
            :flags_(0)
        {}

        explicit BitSet32(u32 flags)
            :flags_(flags)
        {}

        ~BitSet32()
        {}

        inline void clear();
        inline bool check(u32 flag) const;
        inline void set(u32 flag);
        inline void reset(u32 flag);
    private:
        u32 flags_;
    };

    inline void BitSet32::clear()
    {
        flags_ = 0;
    }

    inline bool BitSet32::check(u32 flag) const
    {
        return 0 != (flags_&flag);
    }

    inline void BitSet32::set(u32 flag)
    {
        flags_ |= flag;
    }

    inline void BitSet32::reset(u32 flag)
    {
        flags_ &= ~flag;
    }
}
#endif //INC_LCORE_BITSET_H__

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
    @brief ビットフラグセット。取りあえずリトルエンディアン
    */
    template<u32 SizeInByte>
    class BitSet
    {
    public:
        typedef BitSet<SizeInByte> this_type;
        typedef u8 slot_type;
        static const u32 Byte = SizeInByte;
        static const u32 BitsInByte = 8;

        BitSet()
        {
            for(u32 i=0; i<SizeInByte; ++i){
                bits_[i] = 0x00U;
            }
        }

        /**
        @brief フラグチェック
        @param bit ... チェックするビット番号
        */
        inline bool check(u32 bit) const
        {
            u32 slot = (bit>>3);
            LASSERT(slot<Byte);

            bit &= 0x07U;
            u8 flagInByte = (0x01U << bit);
            return ((bits_[slot] & flagInByte) != 0);
        }

        /**
        @brief フラグ立てる
        */
        inline void set(u32 bit)
        {
            u32 slot = (bit>>3);
            LASSERT(slot<Byte);

            bit &= 0x07U;
            bits_[slot] |= (0x01U << bit);
        }

        /**
        @brief フラグ降ろす
        */
        inline void reset(u32 bit)
        {
            u32 slot = (bit>>3);
            LASSERT(slot<Byte);

            bit &= 0x07U;
            bits_[slot] &= ~(0x01U << bit);
        }

        const u8* getBuffer() const{ return bits_;}
        u8* getBuffer(){ return bits_;}

        inline void set(const BitSet& rhs)
        {
            for(u32 i=0; i<Byte; ++i){
                bits_[i] = rhs.bits_[i];
            }
        }
    private:
        template<u32 Size> friend bool compare(const BitSet<Size>& b0, const BitSet<Size>& b1);

        u8 bits_[Byte];
    };

    //----------------------------------------------------
    /**
    @brief ビットフラグセット比較
    */
    template<u32 Size>
    inline bool compare(const BitSet<Size>& b0, const BitSet<Size>& b1)
    {
        return (lcore::memcmp(b0.bits_, b1.bits_, Size) == 0);
    }

}

#endif //INC_LCORE_BITSET_H__

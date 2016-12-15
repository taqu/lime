/**
@file BitArray.cpp
@author t-sakai
@date 2016/07/25 create
*/
#include "BitArray.h"

namespace lcore
{
    BitArray::BitArray()
        :size_(0)
        ,bits_(NULL)
    {
    }

    BitArray::BitArray(u32 sizeInBits)
    {
        u32 sizeInBytes = (sizeInBits<UnitBits)? UnitBytes : ((sizeInBits>>3) + UnitBytes)&~(UnitBytes-1);
        size_ = sizeInBytes>>2;
        bits_ = static_cast<u32*>(LMALLOC(sizeInBytes));
        lcore::memset(bits_, 0, sizeInBytes);
    }

    BitArray::~BitArray()
    {
        LFREE(bits_);
    }

    void BitArray::clear()
    {
        lcore::memset(bits_, 0, size_*sizeof(u32));
    }

    bool BitArray::check(u32 index) const
    {
        u32 i = index>>5;
        LASSERT(0<=i && i<size_);
        u32 bit = 0x01U << (index&31);
        return 0 != (bit & bits_[i]);
    }

    void BitArray::set(u32 index)
    {
        u32 i = index>>5;
        LASSERT(0<=i && i<size_);
        u32 bit = 0x01U << (index&31);
        bits_[i] |= bit;
    }

    void BitArray::reset(u32 index)
    {
        u32 i = index>>5;
        LASSERT(0<=i && i<size_);
        u32 bit = 0x01U << (index-(i<<5));
        bits_[i] &= ~bit;
    }

    void BitArray::add(u32 index, bool flag)
    {
        u32 i = index>>5;
        if(size_<=i){
            u32 oldSizeInBytes = size_<<2;
            u32 sizeInBytes = i<<2;
            sizeInBytes = (sizeInBytes+UnitBytes)&~(UnitBytes-1);
            u8* bits = static_cast<u8*>(LMALLOC(sizeInBytes));
            lcore::memset(bits+oldSizeInBytes, 0, sizeInBytes-oldSizeInBytes);
            lcore::memcpy(bits, bits_, oldSizeInBytes);
            LFREE(bits_);
            size_ = sizeInBytes>>2;
            bits_ = reinterpret_cast<u32*>(bits);
        }
        u32 bit = 0x01U << (index&31);
        if(flag){
            bits_[i] |= bit;
        } else{
            bits_[i] &= ~bit;
        }
    }

    void BitArray::swap(BitArray& rhs)
    {
        lcore::swap(size_, rhs.size_);
        lcore::swap(bits_, rhs.bits_);
    }
}

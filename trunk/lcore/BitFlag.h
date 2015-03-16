#ifndef INC_LCORE_BITFLAG_H__
#define INC_LCORE_BITFLAG_H__
/**
@file BitFlag.h
@author t-sakai
@date 2015/01/09 create
*/
#include "lcore.h"

namespace lcore
{
    class BitFlag32
    {
    public:
        BitFlag32()
            :flags_(0)
        {}

        ~BitFlag32()
        {}

        inline void clear();
        inline bool check(u32 flag) const;
        inline void set(u32 flag);
        inline void reset(u32 flag);
    private:
        u32 flags_;
    };

    inline void BitFlag32::clear()
    {
        flags_ = 0;
    }

    inline bool BitFlag32::check(u32 flag) const
    {
        return 0 != (flags_&flag);
    }

    inline void BitFlag32::set(u32 flag)
    {
        flags_ |= flag;
    }

    inline void BitFlag32::reset(u32 flag)
    {
        flags_ &= ~flag;
    }
}
#endif //INC_LCORE_BITFLAG_H__

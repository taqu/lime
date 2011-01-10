#ifndef INC_LMATH_COLOR_H__
#define INC_LMATH_COLOR_H__
/**
@file Color.h
@author t-sakai
@date 2009/08/11 create
*/
#include "lmathcore.h"

namespace lmath
{
    class Color
    {
    public:
        Color()
        {
        }

        Color(u32 value)
            :value_(value)
        {
        }

        u8 getA() const
        {
            return (value_ >> 24) & 0xFFU;
        }

        u8 getR() const
        {
            return (value_ >> 16) & 0xFFU;
        }

        u8 getG() const
        {
            return (value_ >> 8) & 0xFFU;
        }

        u8 getB() const
        {
            return (value_ >> 0) & 0xFFU;
        }

        void set(u8 a, u8 r, u8 g, u8 b)
        {
            value_ = b;
            value_ |= (a << 24 | r << 16 | g << 8);
        }

        u32 get() const{ return value_;}
        void set(u32 value){ value_ = value;}

    private:
        u32 value_;
    };
}

#endif //INC_LMATH_COLOR_H__

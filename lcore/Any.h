#ifndef INC_LCORE_ANY_H_
#define INC_LCORE_ANY_H_
/**
@file Any.h
@author t-sakai
@date 2016/09/21 create
*/
#include "lcore.h"

namespace lcore
{
    class Any
    {
    public:
        static const s16 Type_None = -1;
        static const s16 Type_Int = 0;
        static const s16 Type_Float = 1;
        static const s16 Type_String = 2;

        Any();
        Any(const Any& rhs);
        explicit Any(s32 value);
        explicit Any(f32 value);
        explicit Any(const Char* value);

        ~Any();

        s16 getType() const{ return type_;}

        inline Any& operator=(const Any& rhs);
        Any& operator=(s32 value);
        Any& operator=(f32 value);
        Any& operator=(const Char* value);

        operator s32() const;
        operator f32() const;
        operator const Char*() const;

        void swap(Any& rhs);

        s32 length() const;
        void toString(s32 size, Char* str);
        void parseInt(Char* str);
        void parseFloat(Char* str);
    private:
        inline void destroy();

        union
        {
            s32 int_;
            f32 float_;
            Char* str_;
        } value_;
        s16 type_;
        s16 length_;
    };

    inline Any& Any::operator=(const Any& rhs)
    {
        Any tmp(rhs);
        swap(tmp);
        return *this;
    }
}
#endif //INC_LCORE_ANY_H_

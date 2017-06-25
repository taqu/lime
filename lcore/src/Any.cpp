/**
@file Any.cpp
@author t-sakai
@date 2016/09/21 create
*/
#include "Any.h"
#include <stdio.h>

namespace lcore
{
    Any::Any()
        :type_(Type_None)
        ,length_(0)
    {
        value_.str_ = NULL;
    }

    Any::Any(const Any& rhs)
        :type_(rhs.type_)
        ,length_(rhs.length_)
    {
        switch(type_)
        {
        case Type_String:
            {
                LASSERT(length_ == lcore::strlen_s32(rhs.value_.str_));
                s32 size = sizeof(Char)*(length_+1);
                value_.str_ = (Char*)LMALLOC(size);
                lcore::memcpy(value_.str_, rhs.value_.str_, size);
            }
            break;
        case Type_Int:
            value_.int_ = rhs.value_.int_;
            break;
        case Type_Float:
            value_.float_ = rhs.value_.float_;
            break;
        }
    }

    Any::Any(s32 value)
        :type_(Type_Int)
        ,length_(0)
    {
        value_.int_ = value;
    }

    Any::Any(f32 value)
        :type_(Type_Float)
        ,length_(0)
    {
        value_.float_ = value;
    }

    Any::Any(const Char* value)
        :type_(Type_Int)
    {
        this->operator=(value);
    }

    Any::~Any()
    {
        destroy();
        value_.int_ = 0;
        type_ = length_ = 0;
    }

    Any::operator s32() const
    {
        LASSERT(type_ == Type_Int);
        return value_.int_;
    }

    Any::operator f32() const
    {
        LASSERT(type_ == Type_Float);
        return value_.float_;
    }

    Any::operator const Char*() const
    {
        LASSERT(type_ == Type_String);
        return value_.str_;
    }

    void Any::swap(Any& rhs)
    {
        lcore::swap(value_.str_, rhs.value_.str_);
        lcore::swap(type_, rhs.type_);
        lcore::swap(length_, rhs.length_);
    }

    Any& Any::operator=(s32 value)
    {
        destroy();
        type_ = Type_Int;
        length_ = 0;
        value_.int_ = value;
        return *this;
    }

    Any& Any::operator=(f32 value)
    {
        destroy();
        type_ = Type_Float;
        length_ = 0;
        value_.float_ = value;
        return *this;
    }

    Any& Any::operator=(const Char* value)
    {
        destroy();
        type_ = Type_String;
        if(NULL == value){
            length_ = 0;
            value_.str_ = NULL;
        }else{
            length_ = static_cast<s16>(lcore::strlen(value));
            s32 size = sizeof(Char)*(length_+1);
            value_.str_ = (Char*)LMALLOC(size);
            lcore::memcpy(value_.str_, value, size);
        }
        return *this;
    }

    inline void Any::destroy()
    {
        switch(type_)
        {
        case Type_String:
            LFREE(value_.str_);
            break;
        }
    }

    s32 Any::length() const
    {
        return length_;
    }

    void Any::toString(s32 size, Char* str)
    {
        switch(type_)
        {
        case Type_Int:
#ifdef _MSC_VER
            _snprintf_s(str, size, _TRUNCATE, "%d", value_.int_);
#else
            snprintf(str, size, "%d", value_.int_);
#endif
            break;
        case Type_Float:
#ifdef _MSC_VER
            _snprintf_s(str, size, _TRUNCATE, "%f", value_.float_);
#else
            snprintf(str, size, "%f", value_.float_);
#endif
            break;
        case Type_String:
            if(NULL == value_.str_){
                str[0] = CharNull;
            }else{
                lcore::memcpy(str, value_.str_, sizeof(Char)*(length_+1));
            }
            break;
        }
    }

    void Any::parseInt(Char* str)
    {
        LASSERT(NULL != str);
        destroy();
        type_ = Type_Int;
        length_ = 0;
        value_.int_ = atol(str);
    }

    void Any::parseFloat(Char* str)
    {
        LASSERT(NULL != str);
        destroy();
        type_ = Type_Float;
        length_ = 0;
        value_.float_ = static_cast<f32>(atof(str));
    }
}

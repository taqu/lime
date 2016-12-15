/**
@file LString.cpp
@author t-sakai
@date 2015/09/15 create
*/
#include "LString.h"
#include <stdio.h>
#include <stdarg.h>

namespace lcore
{
    //--------------------------------------------
    //---
    //--- String
    //---
    //--------------------------------------------
    String String::empty_ = String();

    String::String()
        :capacity_(ExpandSize)
        ,length_(0)
    {
        buffer_.small_[0] = CharNull;
    }

    String::String(s32 capacity)
        :capacity_(capacity)
        ,length_(0)
    {
        LASSERT(0<=capacity_);
        if(ExpandSize<capacity_){
            capacity_ = getCapacity(capacity_);
            buffer_.elements_ = LNEW Char[capacity_];
            buffer_.elements_[0] = CharNull;
        } else{
            capacity_ = ExpandSize;
            buffer_.small_[0] = CharNull;
        }
    }

    String::String(const Char* str)
    {
        LASSERT(NULL != str);
        s32 length = strlen_s32(str);
        initBuffer(length);
        Char* buffer = getBuffer();
        lcore::memcpy(buffer, str, sizeof(Char)*length);
    }
    
    String::String(s32 length, const Char* str)
    {
        LASSERT(NULL != str);
        LASSERT(length<=strlen_s32(str));
        initBuffer(length);
        Char* buffer = getBuffer();
        lcore::memcpy(buffer, str, sizeof(Char)*length);

    }

    String::String(s32 length, const Char* str, FilterFunc filter)
    {
        LASSERT(NULL != str);
        LASSERT(length<=strlen_s32(str));
        LASSERT(NULL != filter);
        initBuffer(length);
        Char* buffer = getBuffer();
        for(s32 i=0; i<length; ++i){
            buffer[i] = static_cast<Char>( filter(str[i]) );
        }
    }

    String::String(const String& rhs)
    {
        s32 length = rhs.length();
        initBuffer(length);
        Char* buffer = getBuffer();
        lcore::memcpy(buffer, rhs.c_str(), sizeof(Char)*length);
    }

    String::~String()
    {
        if(ExpandSize<capacity_){
            LDELETE_ARRAY(buffer_.elements_);
        }
        capacity_ = 0;
    }

    void String::clear(s32 capacity)
    {
        LASSERT(0<=capacity);
        length_ = 0;
        if(capacity<=capacity_) {
            Char* buffer = getBuffer();
            buffer[0] = CharNull;
            return;
        }
        if(ExpandSize<capacity_) {
            LDELETE_ARRAY(buffer_.elements_);
        }
        if(capacity<=ExpandSize){
            capacity_ = ExpandSize;
            buffer_.small_[0] = CharNull;
            return;
        }

        capacity_ = getCapacity(capacity);
        buffer_.elements_ = LNEW Char[capacity_];
        buffer_.elements_[0] = CharNull;
    }

    void String::fill(s32 length, Char c)
    {
        LASSERT(0<=length);
        createBuffer(length);
        length_ = length;
        Char* buffer = getBuffer();
        for(s32 i=0; i<length_; ++i){
            buffer[i] = c;
        }
        buffer[length_] = CharNull;
    }

    void String::replace(Char match, Char c)
    {
        Char* buffer = getBuffer();
        for(s32 i=0; i<length(); ++i){
            if(match == buffer[i]){
                buffer[i] = c;
            }
        }
    }

    void String::append(Char c)
    {
        expandBuffer(length_+1);
        Char* buffer = getBuffer();
        buffer[length_] = c;
        ++length_;
        buffer[length_] = CharNull;
    }

    s32 String::pop_back()
    {
        if(length_<=0){
            return -1;
        }

        --length_;
        Char* buffer = getBuffer();
        s32 ret = buffer[length_];
        buffer[length_] = CharNull;
        return ret;
    }

    void String::append(s32 length, const Char* str)
    {
        LASSERT(0<=length);
        LASSERT(NULL != str);
        expandBuffer(length_ + length);
        Char* buffer = getBuffer();
        for(s32 i=0; i<length; ++i, ++length_){
            buffer[length_] = str[i];
        }
        buffer[length_] = CharNull;
    }

    bool String::operator==(const String& rhs) const
    {
        if(length_ != rhs.length_){
            return false;
        }
        const Char* b0 = getBuffer();
        const Char* b1 = rhs.getBuffer();
        return 0 == lcore::strncmp(b0, b1, length_);
    }

    bool String::operator!=(const String& rhs) const
    {
        if(length_ != rhs.length_){
            return true;
        }
        const Char* b0 = getBuffer();
        const Char* b1 = rhs.getBuffer();
        return 0 != strncmp(b0, b1, length_);
    }

    s32 String::printf(const Char* format, ...)
    {
        va_list args;
        va_start(args, format);
        s32 length = vscprintf(format, args);
        createBuffer(length);

        Char* buffer = getBuffer();
#ifdef _MSC_VER
        length_ = vsnprintf_s(buffer, capacity_, length, format, args);
#else
        length_ = vsnprintf(buffer, length, format, args);
#endif
        va_end(args);
        return length_;
    }

    void String::initBuffer(s32 length)
    {
        if(length<ExpandSize){
            capacity_ = ExpandSize;
            length_ = length;
            buffer_.small_[length_] = CharNull;
            return;
        }
        s32 capacity = length+1;
        capacity_ = getCapacity(capacity);
        length_ = length;
        buffer_.elements_ = LNEW Char[capacity_];
        buffer_.elements_[length_] = CharNull;
    }

    void String::createBuffer(s32 length)
    {
        s32 capacity = length+1;
        if(capacity<=capacity_){
            return;
        }
        if(capacity<=ExpandSize){
            capacity_ = ExpandSize;
            return;
        }
        if(ExpandSize<capacity_){
            LDELETE_ARRAY(buffer_.elements_);
        }

        capacity_ = getCapacity(capacity);
        buffer_.elements_ = LNEW Char[capacity_];
    }

    void String::expandBuffer(s32 length)
    {
        s32 capacity = length+1;
        if(capacity<=capacity_) {
            return;
        }
        if(capacity<=ExpandSize){
            capacity_ = ExpandSize;
            return;
        }
        capacity = getCapacity(capacity);
        Char* elements = LNEW Char[capacity];
        Char* buffer = getBuffer();
        lcore::memcpy(elements, buffer, sizeof(Char)*capacity_);
        if(ExpandSize<capacity_){
            LDELETE_ARRAY(buffer_.elements_);
        }

        capacity_ = capacity;
        buffer_.elements_ = elements;
    }

    String& String::operator=(const String& rhs)
    {
        assign(rhs.length(), rhs.c_str());
        return *this;
    }

    String& String::operator=(const Char* str)
    {
        LASSERT(NULL != str);
        s32 length = strlen_s32(str);
        assign(length, str);
        return *this;
    }

    void String::assign(s32 length, const Char* str)
    {
        LASSERT(NULL != str);
        LASSERT(length<=strlen_s32(str));
        if(length<=0){
            clear();
            return;
        }
        createBuffer(length);
        length_ = length;
        Char* buffer = getBuffer();
        lcore::memcpy(buffer, str, sizeof(Char)*length_);
        buffer[length_] = CharNull;
    }

    void String::assign(s32 length, const Char* str, FilterFunc filter)
    {
        LASSERT(NULL != str);
        LASSERT(length<=strlen_s32(str));
        LASSERT(NULL != filter);
        if(length<=0){
            clear();
            return;
        }
        createBuffer(length);
        length_ = length;
        Char* buffer = getBuffer();
        for(s32 i=0; i<length_; ++i){
            buffer[i] = static_cast<Char>( filter(str[i]) );
        }
        buffer[length_] = CharNull;
    }

    void String::swap(String& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(length_, rhs.length_);

        s32* s0 = reinterpret_cast<s32*>(&buffer_);
        s32* s1 = reinterpret_cast<s32*>(&rhs.buffer_);
        s32 count = sizeof(Buffer)/sizeof(s32);
        for(s32 i = 0; i<count; ++i) {
            lcore::swap(s0[i], s1[i]);
        }
    }

    s32 compare(const String& str0, const String& str1)
    {
        s32 length = maximum(str0.length(), str1.length());
        return lcore::strncmp(str0.c_str(), str1.c_str(), length);
    }

    //--------------------------------------------
    //---
    //--- StringBuilder
    //---
    //--------------------------------------------
    StringBuilder::StringBuilder()
        :capacity_(0)
        ,length_(0)
        ,elements_(NULL)
    {}

    StringBuilder::StringBuilder(s32 capacity)
        :capacity_(getCapacity(capacity))
        ,length_(0)
        ,elements_(NULL)
    {
        LASSERT(0<=capacity_);
        elements_ = LNEW Char[capacity_];
        elements_[0] = CharNull;
    }

    StringBuilder::~StringBuilder()
    {
        LDELETE_ARRAY(elements_);
    }

    void StringBuilder::clear(s32 capacity)
    {
        LASSERT(0<=capacity);
        if(capacity_<capacity) {
            expand(getCapacity(capacity));
        }
        length_ = 0;
        if(0<capacity_) {
            elements_[0] = CharNull;
        }
    }

    void StringBuilder::replace(Char match, Char c)
    {
        for(s32 i = 0; i<length_; ++i) {
            if(match == elements_[i]) {
                elements_[i] = c;
            }
        }
    }

    void StringBuilder::append(Char c)
    {
        if(capacity_<(length_+2)) {
            expand(capacity_+ExpandSize);
        }
        elements_[length_] = c;
        ++length_;
        elements_[length_] = CharNull;
        LASSERT(length_<capacity_);
    }

    void StringBuilder::append(s32 length, const Char* str)
    {
        LASSERT(0<=length);
        LASSERT(NULL != str);

        if(capacity_<(length_+length+1)) {
            expand( getCapacity(length_+length+1) );
        }
        for(s32 i = 0; i<length; ++i,++length_) {
            elements_[length_] = str[i];
        }
        elements_[length_] = CharNull;
        LASSERT(length_<capacity_);
    }

    s32 StringBuilder::printf(const Char* format, ...)
    {
        va_list args;
        va_start(args, format);
        s32 length = vscprintf(format, args);

        if(capacity_<(length_+length+1)){
            expand( getCapacity(length_+length+1) );
        }
#ifdef _MSC_VER
        length = vsnprintf_s(elements_+length_, capacity_ - length_, length, format, args);
#else
        length = vsnprintf(elements_+length_, length, format, args);
#endif
        if(0<length){
            length_ += length;
        }
        va_end(args);
        return length;
    }

    void StringBuilder::toString(String& string)
    {
        string.clear(length_+1);
        Char* buffer = string.getBuffer();
        for(s32 i=0; i<length_; ++i){
            buffer[i] = elements_[i];
        }
        buffer[length_] = CharNull;
    }

    void StringBuilder::swap(StringBuilder& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(length_, rhs.length_);
        lcore::swap(elements_, rhs.elements_);
    }

    void StringBuilder::expand(s32 capacity)
    {
        Char* elements = LNEW Char[capacity];
        lcore::memcpy(elements, elements_, sizeof(Char)*capacity_);
        LDELETE_ARRAY(elements_);
        capacity_ = capacity;
        elements_ = elements;
    }

}

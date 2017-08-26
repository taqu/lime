#ifndef INC_LCORE_LSTRING_H__
#define INC_LCORE_LSTRING_H__
/**
@file LString.h
@author t-sakai
@date 2009/09/10 create
*/
#include "lcore.h"
#include <stdio.h>
#include <stdarg.h>
#include "xxHash.h"

namespace lcore
{
    //---------------------------------------------
    //---
    //--- String
    //---
    //---------------------------------------------
    /// 固定長マルチバイト文字列
    template<s32 SIZE>
    class StaticString
    {
    public:
        static const s32 Size = SIZE;

        typedef StaticString<SIZE> this_type;
        static const s32 MAX_LENGTH = SIZE-1;

        StaticString()
            :size_(0)
        {
            LASSERT(0<MAX_LENGTH);
            buffer_[0] = CharNull;
        }

        StaticString(const this_type& rhs)
            :size_(rhs.size())
        {
            lcore::memcpy(buffer_, rhs.buffer_, SIZE);
        }

        StaticString(s32 length, const Char* str)
        {
            LASSERT(0<MAX_LENGTH);
            assign(length, str);
        }

        ~StaticString()
        {
        }

        const Char* c_str() const
        {
            return buffer_;
        }

        Char* c_str()
        {
            return buffer_;
        }

        /// 文字列長を返す
        s32 size() const
        {
            return size_;
        }

        void clear()
        {
            size_ = 0;
            buffer_[0] = CharNull;
        }

        void assign(s32 length, const Char* str);
        void assign(const Char* str);

        void swap(this_type& rhs);

        bool operator==(const this_type& rhs) const;
        bool operator!=(const this_type& rhs) const;

        bool operator==(const Char* str) const;
        bool operator!=(const Char* str) const;

        friend inline bool operator==(const Char* str, const this_type& rhs)
        {
            return rhs == str;
        }
        friend inline bool operator!=(const Char* str, const this_type& rhs)
        {
            return rhs != str;
        }

        this_type& operator=(const this_type& rhs);

        s32 append(s32 length, const Char* str);

        Char& operator[](s32 index)
        {
            LASSERT(0<=index && index<=size_);
            return buffer_[index];
        }

        const Char& operator[](s32 index) const
        {
            LASSERT(0<=index && index<=size_);
            return buffer_[index];
        }

        /// 書式付出力
        s32 print(const Char* format, ... );

    protected:
        s32 size_; //文字列長
        Char buffer_[SIZE];
    };

    //----------------------------------------------------------
    template<s32 SIZE>
    void StaticString<SIZE>::assign(s32 length, const Char* str)
    {
        LASSERT(NULL != str);
        s32 count = (SIZE<=length)? MAX_LENGTH : length;
        s32 i=0;
        for(; i<count; ++i){
            buffer_[i] = str[i];
            if(str[i] == CharNull){
                break;
            }
        }
        size_ = i;
        buffer_[i] = CharNull;
    }

    //----------------------------------------------------------
    template<s32 SIZE>
    void StaticString<SIZE>::assign(const Char* str)
    {
        LASSERT(NULL != str);
        s32 i=0;
        for(; i<MAX_LENGTH; ++i){
            buffer_[i] = str[i];
            if(str[i] == CharNull){
                break;
            }
        }
        size_ = i;
        buffer_[i] = CharNull;
    }

    //----------------------------------------------------------
    template<s32 SIZE>
    void StaticString<SIZE>::swap(this_type& rhs)
    {
        Char buffer[SIZE];
        lcore::memcpy(buffer, rhs.buffer_, rhs.size_ + 1);
        lcore::memcpy(rhs.buffer_, buffer_, size_ + 1);
        lcore::memcpy(buffer_, buffer, rhs.size_ + 1);

        lcore::swap(size_, rhs.size_);
    }

    //----------------------------------------------------------
    template<s32 SIZE>
    bool StaticString<SIZE>::operator==(const this_type& rhs) const
    {
        if(size_ != rhs.size()){
            return false;
        }
        return (lcore::strncmp(buffer_, rhs.buffer_, size_) == 0);
    }

    //----------------------------------------------------------
    template<s32 SIZE>
    bool StaticString<SIZE>::operator!=(const this_type& rhs) const
    {
        if(size_ != rhs.size()){
            return true;
        }
        return (lcore::strncmp(buffer_, rhs.buffer_, size_) != 0);
    }

    //----------------------------------------------------------
    template<s32 SIZE>
    bool StaticString<SIZE>::operator==(const Char* str) const
    {
        LASSERT(str != NULL);
        s32 size = lcore::strlen_s32(str);
        if(size != size_){
            return false;
        }
        return (0 == lcore::strncmp(buffer_, str, size_));
    }

    //----------------------------------------------------------
    template<s32 SIZE>
    bool StaticString<SIZE>::operator!=(const Char* str) const
    {
        LASSERT(str != NULL);
        s32 size = lcore::strlen_s32(str);
        if(size != size_){
            return true;
        }
        return (0 != lcore::strncmp(buffer_, str, size_));
    }

    //----------------------------------------------------------
    template<s32 SIZE>
    typename StaticString<SIZE>::this_type& StaticString<SIZE>::operator=(const this_type& rhs)
    {
        if(this != &rhs){
            lcore::memcpy(buffer_, rhs.buffer_, SIZE);
            size_ = rhs.size_;
        }
        return *this;
    }

    //----------------------------------------------------------
    template<s32 SIZE>
    s32 StaticString<SIZE>::append(s32 length, const Char* str)
    {
        s32 j=0;
        len += size_;
        if(len>MAX_LENGTH){
            len = MAX_LENGTH;
        }

        while(size_<len){
            buffer_[size_] = str[j];
            ++size_;
            ++j;
        }
        buffer_[size_] = CharNull;
        return j;
    }

    //----------------------------------------------------------
    template<s32 SIZE>
    s32 StaticString<SIZE>::print(const Char* format, ... )
    {
        LASSERT(format != NULL);

        va_list ap;
        va_start(ap, format);
#if defined(_WIN32) || defined(_WIN64)
        s32 ret = vsnprintf_s(buffer_, SIZE-1, format, ap);
#else
        s32 ret = vsnprintf(buffer_, SIZE-1, format, ap);
#endif
        va_end(ap);

        if(ret<0){
            size_ = 0;
        }else{
            size_ = ret;
        }
        buffer_[size_] = CharNull;

        return ret;
    }


    template<s32 SIZE>
    inline u32 calcHash(const StaticString<SIZE>& str)
    {
        return xxHash32(static_cast<const u8*>(str.c_str()), str.size());
    }

    //--------------------------------------------
    //---
    //--- String
    //---
    //--------------------------------------------
    class String
    {
    public:
        static const s32 ExpandSize = 8;
        static const u32 ExpandMask = ExpandSize-1;

        typedef s32(*FilterFunc)(s32 c);

        static const String& empty(){ return empty_;}

        String();
        explicit String(s32 capacity);
        explicit String(const Char* str);
        String(s32 length, const Char* str);
        String(s32 length, const Char* str, FilterFunc filter);
        String(const String& rhs);
        ~String();

        inline s32 capacity() const;
        inline s32 length() const;
        inline const Char* c_str() const;
        inline const Char& operator[](s32 index) const;
        inline Char& operator[](s32 index);

        inline void clear();
        void clear(s32 capacity);
        void fill(s32 length, Char c=CharNull);

        void replace(Char match, Char c);

        void append(Char c);
        s32 pop_back();
        inline void append(const Char* str);
        void append(s32 length, const Char* str);

        bool operator==(const String& rhs) const;
        bool operator!=(const String& rhs) const;

        s32 printf(const Char* format, ...);
        String& operator=(const String& rhs);
        String& operator=(const Char* str);
        inline void assign(const Char* str);
        void assign(s32 length, const Char* str);
        void assign(s32 length, const Char* str, FilterFunc filter);
        void swap(String& rhs);
    private:
        friend class StringBuilder;

        static inline s32 getCapacity(s32 capacity)
        {
            return (capacity+ExpandMask) & ~ExpandMask;
        }

        inline const Char* getBuffer() const
        {
            return (capacity_<=ExpandSize)? buffer_.small_ : buffer_.elements_;
        }
        inline Char* getBuffer()
        {
            return (capacity_<=ExpandSize)? buffer_.small_ : buffer_.elements_;
        }


        void initBuffer(s32 length);
        void createBuffer(s32 length);
        void expandBuffer(s32 length);

        static String empty_;

        s32 capacity_;
        s32 length_;

        union Buffer
        {
            Char small_[ExpandSize];
            Char* elements_;
        };
        Buffer buffer_;
    };

    inline s32 String::capacity() const
    {
        return capacity_;
    }

    inline s32 String::length() const
    {
        return length_;
    }

    inline const Char* String::c_str() const
    {
        return (ExpandSize<capacity_)? buffer_.elements_ : buffer_.small_;
    }

    inline const Char& String::operator[](s32 index) const
    {
        LASSERT(0<=index && index<length_);
        return getBuffer()[index];
    }

    inline Char& String::operator[](s32 index)
    {
        LASSERT(0<=index && index<length_);
        return getBuffer()[index];
    }

    inline void String::clear()
    {
        length_ = 0;
        Char* buffer = getBuffer();
        buffer[0] = CharNull;
    }

    inline void String::append(const Char* str)
    {
        append(lcore::strlen_s32(str), str);
    }

    inline void String::assign(const Char* str)
    {
        assign(lcore::strlen_s32(str), str);
    }

    s32 compare(const String& str0, const String& str1);

    inline void swap(String& str0, String& str1)
    {
        str0.swap(str1);
    }

    inline u32 calcHash(const String& str)
    {
        return xxHash32(reinterpret_cast<const u8*>(str.c_str()), str.length());
    }

    //--------------------------------------------
    //---
    //--- StringBuilder
    //---
    //--------------------------------------------
    class StringBuilder
    {
    public:
        static const s32 ExpandSize = 64;
        static const u32 ExpandMask = ExpandSize-1;

        StringBuilder();
        explicit StringBuilder(s32 capacity);
        ~StringBuilder();


        inline s32 capacity() const;
        inline s32 length() const;
        inline const Char& operator[](s32 index) const;
        inline Char& operator[](s32 index);
        inline void clear();

        void clear(s32 capacity);

        void replace(Char match, Char c);

        void append(Char c);
        void append(s32 length, const Char* str);

        s32 printf(const Char* format, ...);

        void toString(String& string);

        void swap(StringBuilder& rhs);
    private:
        StringBuilder(const StringBuilder&);
        StringBuilder& operator=(const StringBuilder&);

        static inline s32 getCapacity(s32 capacity)
        {
            return capacity<=0 ? ExpandSize : (capacity+ExpandMask)&~ExpandMask;
        }
        void expand(s32 capacity);

        s32 capacity_;
        s32 length_;
        Char* elements_;
    };

    inline s32 StringBuilder::capacity() const
    {
        return capacity_;
    }

    inline s32 StringBuilder::length() const
    {
        return length_;
    }

    inline const Char& StringBuilder::operator[](s32 index) const
    {
        LASSERT(0<=index && index<length_);
        return elements_[index];
    }

    inline Char& StringBuilder::operator[](s32 index)
    {
        LASSERT(0<=index && index<length_);
        return elements_[index];
    }

    inline void StringBuilder::clear()
    {
        length_ = 0;
    }
}

#endif //INC_LCORE_LSTRING_H__

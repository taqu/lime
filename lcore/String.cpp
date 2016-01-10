/**
@file String.cpp
@author t-sakai
@date 2015/09/15 create
*/
#include "String.h"
#include "Hash.h"

namespace lcore
{
    //---------------------------------------------
    //---
    //--- DynamicString
    //---
    //---------------------------------------------
    Char DynamicString::null_[4] = {'\0', '\0', '\0', '\0'};

    DynamicString::DynamicString()
        :length_(0)
        ,data_(null_)
    {
    }

    DynamicString::DynamicString(const DynamicString& rhs)
        :length_(rhs.length())
        ,data_(null_)
    {
        if(0 < length_){
            data_ = LIME_NEW Char[length_ + 1];
            lcore::strncpy(data_, length_+1, rhs.data_, length_);
            data_[length_] = '\0';
        }
    }

    DynamicString::DynamicString(const Char* str)
        :length_(0)
        ,data_(null_)
    {
        LASSERT(NULL != str);
        length_ = strlen(str);
        data_ = LIME_NEW Char[length_+1];
        lcore::strncpy(data_, length_+1, str, length_);
        data_[length_] = '\0';
    }

    DynamicString::DynamicString(const Char* begin, const Char* end)
        :length_(0)
        ,data_(null_)
    {
        LASSERT(NULL != begin);
        LASSERT(NULL != end);
        LASSERT(begin <= end);
        length_ = end - begin;
        if(0 < length_){
            data_ = LIME_NEW Char[length_ + 1];
            lcore::strncpy(data_, length_+1, begin, length_);
            data_[length_] = '\0';
        }
    }

    DynamicString::~DynamicString()
    {
        if(null_ != data_){
            LIME_DELETE_ARRAY(data_);
        }
    }

    void DynamicString::assign(const Char* str)
    {
        LASSERT(NULL != str);
        if(null_ != data_){
            LIME_DELETE_ARRAY(data_);
        }

        length_ = strlen(str);
        data_ = LIME_NEW Char[length_+1];
        lcore::strncpy(data_, length_+1, str, length_);
        data_[length_] = '\0';
    }

    void DynamicString::assign(const Char* begin, const Char* end)
    {
        LASSERT(NULL != begin);
        LASSERT(NULL != end);
        LASSERT(begin<=end);

        if(null_ != data_){
            LIME_DELETE_ARRAY(data_);
        }

        length_ = end - begin;
        data_ = LIME_NEW Char[length_+1];
        lcore::strncpy(data_, length_+1, begin, length_);
        data_[length_] = '\0';
    }

    void DynamicString::append(const Char* begin, const Char* end)
    {
        LASSERT(NULL != begin);
        LASSERT(NULL != end);
        LASSERT(begin<=end);
        s32 appl = end - begin;
        s32 length = length_ + appl;
        Char* data = LIME_NEW Char[length+1];
        lcore::strncpy(data, length+1, data_, length_);
        data[length_] = '\0';

        lcore::strncpy(data+length_, appl+1, begin, appl);
        data[length] = '\0';

        if(null_ != data_){
            LIME_DELETE_ARRAY(data_);
        }
        length_ = length;
        data_ = data;
    }

    void DynamicString::swap(DynamicString& rhs)
    {
        lcore::swap(length_, rhs.length_);
        lcore::swap(data_, rhs.data_);
    }

    bool DynamicString::operator==(const DynamicString& rhs) const
    {
        if(length_ != rhs.length_){
            return false;
        }
        return 0 == lcore::strncmp(data_, rhs.data_, length_);
    }

    bool DynamicString::operator!=(const DynamicString& rhs) const
    {
        if(length_ != rhs.length_){
            return true;
        }
        return 0 != lcore::strncmp(data_, rhs.data_, length_);
    }

    s32 DynamicString::compare(const Char* rhs) const
    {
        LASSERT(NULL != rhs);
        return lcore::strncmp(data_, rhs, length_);
    }

    DynamicString& DynamicString::operator=(const DynamicString& rhs)
    {
        DynamicString str(rhs);
        str.swap(*this);
        return *this;
    }

    lcore::ostream& DynamicString::write(lcore::ostream& os)
    {
        os.write(data_, length_);
        return os;
    }

    /// 書式付出力
    int DynamicString::print(const Char* format, ...)
    {
        LASSERT(format != NULL);
        static const s32 MaxBuffer = 128;
        Char buffer[MaxBuffer];

        va_list ap;
        va_start(ap, format);

#if defined(_WIN32) || defined(_WIN64)
        int ret = vsnprintf_s(buffer, MaxBuffer, format, ap);
#else
        int ret = vsnprintf(buffer, MaxBuffer, format, ap);
#endif
        if(ret<=0){
            assign(buffer, buffer);

        }else if(ret<MaxBuffer){
            assign(buffer, buffer+ret);

        }else{
            s32 bufferSize = MaxBuffer;
            Char* buff = NULL;
            do{
                LIME_DELETE_ARRAY(buff);
                bufferSize += MaxBuffer;
                buff = LIME_NEW Char[bufferSize];

#if defined(_WIN32) || defined(_WIN64)
                ret = vsnprintf_s(buff, bufferSize, bufferSize, format, ap);
#else
                ret = vsnprintf(buff, bufferSize, format, ap);
#endif
            }while(bufferSize<=ret);
            assign(buff, buff+ret);
            LIME_DELETE_ARRAY(buff);
        }

        va_end(ap);
        return ret;
    }

    u32 hasher<DynamicString>::calc(const DynamicString& t)
    {
        return calc_hash(reinterpret_cast<const u8*>(t.c_str()), t.length());
    }

    //---------------------------------------------
    //---
    //--- DynamicStringBuilder
    //---
    //---------------------------------------------
    DynamicStringBuilder::DynamicStringBuilder(s32 capacity)
        :capacity_(capacity)
        ,length_(0)
        ,data_(NULL)
    {
        LASSERT(0 <= capacity_);
        if(0 < capacity_){
            data_ = LIME_NEW Char[capacity_];
            data_[length_] = '\0';
        }
    }

    DynamicStringBuilder::~DynamicStringBuilder()
    {
        LIME_DELETE_ARRAY(data_);
    }

    void DynamicStringBuilder::push_back(Char c)
    {
        s32 length = length_+1;
        if(capacity_<=length){
            resize();
        }
        data_[length_] = c;
        length_ = length;
    }

    void DynamicStringBuilder::toString(DynamicString& str)
    {
        DynamicString tmp(data_, data_+length_);
        str.swap(tmp);
    }

    void DynamicStringBuilder::resize()
    {
        s32 capacity = (capacity_<=0)? 16 : (capacity_<<1);
        Char* data = LIME_NEW Char[capacity];
        lcore::memcpy(data, data_, sizeof(Char)*length_);
        data[length_] = '\0';
        LIME_DELETE_ARRAY(data_);
        capacity_ = capacity;
        data_ = data;
    }
}

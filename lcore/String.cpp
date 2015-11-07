/**
@file String.cpp
@author t-sakai
@date 2015/09/15 create
*/
#include "String.h"

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

    DynamicString::DynamicString(const Char* str)
        :length_(0)
        ,data_(null_)
    {
        LASSERT(NULL != str);
        length_ = strlen(str);
        data_ = LIME_NEW Char[length_+1];
        strcpy_s(data_, length_, str);
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
        strcpy_s(data_, length_+1, str);
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
}

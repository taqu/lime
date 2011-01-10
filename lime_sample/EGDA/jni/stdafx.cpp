#include "stdafx.h"

#include <stdio.h>
#include <stdarg.h>
#include <android/log.h>

namespace egda
{
    //-----------------------------------------
    //---
    //--- JNIByteArray
    //---
    //-----------------------------------------
    JNIByteArray::JNIByteArray(JNIEnv* env, jbyteArray& byteArray)
        :env_(env)
        ,byteArray_(byteArray)
        ,size_(0)
        ,data_(NULL)
    {
        LASSERT(env_ != NULL);
        size_ = static_cast<u32>( env_->GetArrayLength(byteArray) );
        data_ = reinterpret_cast<Char*>( env->GetByteArrayElements(byteArray, NULL) );
    }

    JNIByteArray::~JNIByteArray()
    {
        env_->ReleaseByteArrayElements(byteArray_, reinterpret_cast<jbyte*>(data_), 0);
    }
}

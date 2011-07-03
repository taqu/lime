#ifndef INC_STDAFX_H__
#define INC_STDAFX_H__
/**
@file stdafx.h
@author t-sakai
@date 2010/06/03 create
*/
#include <jni.h>
#include <android/log.h>
#include <lcore/lcore.h>
#include <lmath/lmath.h>
//#include <lgraphics/lgraphics.h>

namespace egda
{
    using lcore::Char;
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;

    using lcore::u8;
    using lcore::u16;
    using lcore::u32;

    using lcore::f32;
    using lcore::f64;


    //-----------------------------------------
    //---
    //--- JNIByteArray
    //---
    //-----------------------------------------
    class JNIByteArray
    {
    public:
        JNIByteArray(JNIEnv* env, jbyteArray& byteArray);
        ~JNIByteArray();

        u32 size() const{ return size_;}
        Char* getData(){ return data_;}

    private:
        JNIEnv* env_;
        jbyteArray& byteArray_;
        lcore::u32 size_;
        lcore::Char* data_;
    };
}
#endif //INC_STDAFX_H__

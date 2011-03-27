#ifndef INC_LGRAPHICSCORE_H__
#define INC_LGRAPHICSCORE_H__
/**
@file lgraphicscore.h
@author t-sakai
@date 2009/01/19 create
*/
#include <lcore/lcore.h>
#include <lcore/String.h>


//-------------------------------------
//--- Features
//#define LIME_USE_DUAL_QUATERNION_FOR_SKINNING (1)

#ifdef _WIN32
struct HWND__;
struct tagRECT;
#else
typedef void* HWND__;
#endif

#define SAFE_RELEASE(p) {if(p!=NULL){(p)->Release();(p)=NULL;}}
#define LSAFE_RELEASE(p) {if(p!=NULL){(p)->release();(p)=NULL;}}
#define LRELEASE(p) {(p)->release();(p)=NULL;}
#define LBUFFER_OFFSET(offset) ((char*)NULL + (offset))

namespace lgraphics
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    static const u32 LIME_MAX_PALLET_MATRICES = 256;

    static const u32 LIME_MAX_NAME_SIZE = (24 - 1);

    static const u32 LIME_MAX_ANIM_NODE = 512;

#if defined(LIME_GLES1) || defined(LIME_GLES2)
#define LIME_GL (1)
#endif

    static const u32 MAX_NAME_BUFFER_SIZE = 32;
    typedef lcore::String<MAX_NAME_BUFFER_SIZE> NameString;

    static const u32 INVALID_SAMPLER_INDEX = 0xFFFFFFFFU;

//----------------------------------------------------------
//---
//--- プラットフォーム、API依存のdefine
//---
//----------------------------------------------------------

#if defined(LIME_DX9)
//----------------------------------------------------------
#define LIME_INCLUDE_DX9

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

static const lgraphics::u8 LIME_MAX_SKINNING_MATRICES = (80);

#elif defined(LIME_DX11)
//----------------------------------------------------------
#define LIME_INCLUDE_DX11

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

static const lgraphics::u8 LIME_MAX_SKINNING_MATRICES = (80);

#elif defined(LIME_GLES1)
//----------------------------------------------------------
#define LIME_INCLUDE_GLES1

#define LIME_INITGL

#elif defined(LIME_GLES2)
//----------------------------------------------------------

#define LIME_INCLUDE_GLES2

static const lgraphics::u8 LIME_MAX_SKINNING_MATRICES = (36);

#if defined(ANDROID)

#else // NOT ANDROID

#if defined(LIME_GLES2_ATI)
#define LIME_INIT_GLES2

#define LIME_INCLUDE_EGL

#define LIME_INIT_EGL

#define LIME_GLES2_DLL_NAME "atioglxx.dll"
#else

#define LIME_INCLUDE_EGL

#endif //LIME_GLES2_ATI

#endif // NOT ANDROID

#endif //LIME_GLES2

}
#endif //INC_LGRAPHICSCORE_H__

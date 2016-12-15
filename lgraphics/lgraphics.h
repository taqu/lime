#ifndef INC_LGRAPHICS_H__
#define INC_LGRAPHICS_H__
/**
@file lgraphics.h
@author t-sakai
@date 2009/01/18 create
*/
#include <lcore/lcore.h>
#include <lcore/lstring.h>
#include "Config.h"

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct WindowHandle
{
    HWND hWnd_;
};

struct HWND__;
struct tagRECT;

#elif defined(LGFX_USE_X)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

typedef int(*WNDPROC)(XEvent& event);
struct WindowHandle
{
    Display* display_;
    ::Window window_;
};

typedef void* HWND__;

#endif

#define LDXSAFE_RELEASE(p) if(p!=NULL){(p)->Release();(p)=NULL;}
#define LSAFE_RELEASE(p) if(p!=NULL){(p)->release();(p)=NULL;}
#define LRELEASE(p) (p)->release();(p)=NULL
#define LBUFFER_OFFSET(offset) ((lcore::u8*)NULL + (offset))

//----------------------------------------------------------
//---
//--- API
//---
//----------------------------------------------------------
//----------------------------------------------------------
#if defined(LGFX_DX9)
#include <d3d9.h>
#include <d3dx9.h>
#include <dxgi.h>
#endif

//----------------------------------------------------------
#if defined(LGFX_DX11)
#include <dxgi.h>
#include <d3d11.h>
#endif

//----------------------------------------------------------
#if defined(LGFX_GLES1)

#if defined(LIME_INITGL)
#include "api/gles1/lgl.h"
#include "api/gles1/lglext.h"
#else
#include <GLES/gl.h>
#endif

#if defined(ANDROID)
#else
#include <GLES/egl.h>
#endif

#endif

//----------------------------------------------------------
#if defined(LGFX_GLES2)

#if defined(LIME_INCLUDE_EGL)
#include "api/gles2/legl.h"
#endif

#include "api/gles2/lgl2.h"
#include "api/gles2/lgl2ext.h"

#endif //LGFX_GLES2

namespace lgfx
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::s64;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::u64;
    using lcore::f32;
    using lcore::f64;

    using lcore::Char;

    using lcore::lsize_t;

    static const s32 LGFX_MAX_SKINNING_MATRICES = LGFX_CONFIG_MAX_SKINNING_MATRICES;
    static const s32 LGFX_MAX_RENDER_TARGETS = LGFX_CONFIG_MAX_RENDER_TARGETS;

    static const u32 MAX_VERTEX_STREAMS = 2; //最大頂点ストリーム数

#if defined(LGFX_GLES1) || defined(LGFX_GLES2)
#define LGFX_GL (1)
#endif

#if defined(LGFX_GL)
    typedef s32 HANDLE;
#elif defined(LGFX_DX9)
    typedef D3DXHANDLE HANDLE;
#endif

    static const u32 MAX_NAME_BUFFER_SIZE = 48;
    typedef lcore::StaticString<MAX_NAME_BUFFER_SIZE> NameString;

    static const u32 INVALID_SAMPLER_INDEX = 0xFFFFFFFFU;
    static const u32 INVALID_TEXTURE_ID = 0xFFFFFFFFU;
    static const u32 INVALID_TEXTURE_LOCATION = 0xFFFFFFFFU;

    static const u32 MAX_TEXTURES = 3;

    //-----------------------------------------------------------
    //Bufferの場合はバイト単位、テクスチャの場合はテクセル単位
    struct Box
    {
        Box(){}
        Box(u32 left, u32 top, u32 front, u32 right, u32 bottom, u32 back)
            :left_(left)
            ,top_(top)
            ,front_(front)
            ,right_(right)
            ,bottom_(bottom)
            ,back_(back)
        {}

        u32 left_;
        u32 top_;
        u32 front_;
        u32 right_;
        u32 bottom_;
        u32 back_;
    };

    //-----------------------------------------------------------

    typedef D3D11_VIEWPORT Viewport;

    //-----------------------------------------------------------
    struct MappedSubresource
    {
        void* data_;
        u32 rowPitch_;
        u32 depthPitch_;

        template<class T>
        T* getData(){ return static_cast<T*>(data_);}
    };

    struct MappedSubresourceTransientBuffer
    {
        void* offsetData_;
        u32 offsetInBytes_;
        u32 sizeInBytes_;

        template<class T>
        T* getData(){ return static_cast<T*>(offsetData_);}
    };

    //-----------------------------------------------------------
    template<class T>
    class ScopedCOMPtr
    {
    public:
        ScopedCOMPtr()
            :ptr_(NULL)
        {}

        ScopedCOMPtr(T* ptr)
            :ptr_(ptr)
        {}

        ~ScopedCOMPtr()
        {
            if(ptr_){
                ptr_->Release();
            }
        }

        const T* operator->() const
        {
            return ptr_;
        }

        T* operator->()
        {
            return ptr_;
        }

        const T& operator*() const
        {
            return *ptr_;
        }

        T& operator*()
        {
            return *ptr_;
        }

        T* get(){ return ptr_;}

    private:
        ScopedCOMPtr(const ScopedCOMPtr&);
        ScopedCOMPtr& operator=(const ScopedCOMPtr&);

        T* ptr_;
    };

    //-----------------------------------------------------------
    template<class T, u32 N>
    class ResourceArrayBase
    {
    public:
        typedef typename T::pointer_type pointer_type;
        typedef pointer_type const const_pointer_type;
        typedef const_pointer_type* pointer_array_type;

        pointer_array_type get(){ return holder_.values_; }
        operator pointer_array_type(){ return holder_.values_; }
        const_pointer_type& operator[](s32 i){ return holder_.values_[i]; }

        ResourceArrayBase()
        {}

        ResourceArrayBase(pointer_type v0)
            :holder_({v0})
        {}

        ResourceArrayBase(pointer_type v0, pointer_type v1)
            :holder_({v0, v1})
        {}

        ResourceArrayBase(pointer_type v0, pointer_type v1, pointer_type v2)
            :holder_({v0, v1, v2})
        {}

        ResourceArrayBase(
            pointer_type v0,
            pointer_type v1,
            pointer_type v2,
            pointer_type v3)
            :holder_({v0, v1, v2, v3})
        {}

        ResourceArrayBase(
            pointer_type v0,
            pointer_type v1,
            pointer_type v2,
            pointer_type v3,
            pointer_type v4)
            :holder_({v0, v1, v2, v3, v4})
        {}

        ResourceArrayBase(
            pointer_type v0,
            pointer_type v1,
            pointer_type v2,
            pointer_type v3,
            pointer_type v4,
            pointer_type v5)
            :holder_({v0, v1, v2, v3, v4, v5})
        {}

        ResourceArrayBase(
            pointer_type v0,
            pointer_type v1,
            pointer_type v2,
            pointer_type v3,
            pointer_type v4,
            pointer_type v5,
            pointer_type v6,
            pointer_type v7)
            :holder_({v0, v1, v2, v3, v4, v5, v6, v7})
        {}

    private:
        struct Holder
        {
            pointer_type values_[N];
        };
        Holder holder_;
    };
    template<class T, u32 N>
    class ResourceArray : public ResourceArrayBase<T,N>
    {
    public:

    };

    template<class T>
    class ResourceArray<T,1> : public ResourceArrayBase<T,1>
    {
    public:
        typedef ResourceArrayBase<T,1> base_type;

        ResourceArray()
        {}

        ResourceArray(pointer_type v0)
            :values_{v0}
        {}
    };

    template<class T>
    class ResourceArray<T,2> : public ResourceArrayBase<T,2>
    {
    public:
        typedef ResourceArrayBase<T,2> base_type;

        ResourceArray()
        {}

        ResourceArray(pointer_type v0, pointer_type v1)
            :base_type(v0, v1)
        {}
    };

    template<class T>
    class ResourceArray<T,3> : public ResourceArrayBase<T,3>
    {
    public:
        typedef ResourceArrayBase<T,3> base_type;

        ResourceArray()
        {}

        ResourceArray(pointer_type v0, pointer_type v1, pointer_type v2)
            :base_type(v0, v1, v2)
        {}
    };

    template<class T>
    class ResourceArray<T,4> : public ResourceArrayBase<T,4>
    {
    public:
        typedef ResourceArrayBase<T,4> base_type;

        ResourceArray()
        {}

        ResourceArray(
            pointer_type v0,
            pointer_type v1,
            pointer_type v2,
            pointer_type v3)
            :base_type(v0, v1, v2, v3)
        {}
    };

    template<class T>
    class ResourceArray<T,5> : public ResourceArrayBase<T,5>
    {
    public:
        typedef ResourceArrayBase<T,5> base_type;

        ResourceArray()
        {}

        ResourceArray(
            pointer_type v0,
            pointer_type v1,
            pointer_type v2,
            pointer_type v3,
            pointer_type v4)
            :base_type(v0, v1, v2, v3, v4)
        {}
    };

    template<class T>
    class ResourceArray<T,6> : public ResourceArrayBase<T,6>
    {
    public:
        typedef ResourceArrayBase<T,6> base_type;

        ResourceArray()
        {}

        ResourceArray(
            pointer_type v0,
            pointer_type v1,
            pointer_type v2,
            pointer_type v3,
            pointer_type v4,
            pointer_type v5)
            :base_type(v0, v1, v2, v3, v4, v5)
        {}
    };

    template<class T>
    class ResourceArray<T,7> : public ResourceArrayBase<T,7>
    {
    public:
        typedef ResourceArrayBase<T,7> base_type;

        ResourceArray()
        {}

        ResourceArray(
            pointer_type v0,
            pointer_type v1,
            pointer_type v2,
            pointer_type v3,
            pointer_type v4,
            pointer_type v5,
            pointer_type v6)
            :base_type(v0, v1, v2, v3, v4, v5, v6)
        {}
    };

    template<class T>
    class ResourceArray<T,8> : public ResourceArrayBase<T,8>
    {
    public:
        typedef ResourceArrayBase<T,8> base_type;

        ResourceArray()
        {}

        ResourceArray(
            pointer_type v0,
            pointer_type v1,
            pointer_type v2,
            pointer_type v3,
            pointer_type v4,
            pointer_type v5,
            pointer_type v6,
            pointer_type v7)
            :base_type(v0, v1, v2, v3, v4, v5, v6, v7)
        {}
    };

    //-----------------------------------------------------------
    template<class T>
    inline typename T::pointer_array_type resource_array_cast(T& t)
    {
        return t;
    }
}
#endif //INC_LGRAPHICS_H__

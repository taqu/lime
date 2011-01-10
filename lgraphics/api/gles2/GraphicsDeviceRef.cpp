/**
@file GraphicsDeviceRef.cpp
@author t-sakai
@date 2010/05/29 create
@date 2010/12/12 modify for gles2
*/
#include "GraphicsDeviceRef.h"
#include "InitParam.h"
#include "../../lgraphics.h"

namespace lgraphics
{
    void Descriptor::release()
    {
        if(--counter_ == 0){
            lgraphics::Graphics::getDevice().free(this);
        }
    }

    //-------------------------------------------------------------
    //---
    //--- GraphicsDeviceRef
    //---
    //-------------------------------------------------------------
    void GraphicsDeviceRef::checkError()
    {
#if defined(ANDROID)
#else
        EGLint err = eglGetError();
        if(err != EGL_SUCCESS){
            LASSERT(false);
        }
#endif
    }

    void GraphicsDeviceRef::setClearColor(u32 color)
    {
        f32 a = static_cast<f32>((color>>24)&0xFFU) * (1.0f/255.0f);
        f32 r = static_cast<f32>((color>>16)&0xFFU) * (1.0f/255.0f);
        f32 g = static_cast<f32>((color>>8)&0xFFU) * (1.0f/255.0f);
        f32 b = static_cast<f32>((color>>0)&0xFFU) * (1.0f/255.0f);
        glClearColor(r, g, b, a);
    }

    void GraphicsDeviceRef::setClearDepth(f32 depth)
    {
        glClearDepthf(depth);
    }

    void GraphicsDeviceRef::setClearStencil(u32 stencil)
    {
        glClearStencil(static_cast<GLint>(stencil));
    }

    void GraphicsDeviceRef::enableState()
    {
        //Direct3Dベースなので座標系は逆、カリングを逆にしておく
        glCullFace(GL_FRONT);

        //s32 numUnits = 0;
        //glGetIntegerv(GL_MAX_TEXTURE_UNITS, reinterpret_cast<GLint*>(&numUnits));
        //maxMultiTextures_ =  (maxMultiTextures_ < static_cast<u32>(numUnits))? maxMultiTextures_ : numUnits;
        //for(u32 i=0; i<maxMultiTextures_; ++i){
        //    glActiveTexture(GL_TEXTURE0 + i);
        //    glEnable(GL_TEXTURE_2D);
        //}
    }

#if defined(ANDROID)
    GraphicsDeviceRef::GraphicsDeviceRef()
        :state_(0)
        ,alphaTestRef_(DefaultAlphaTestRef)
        ,alphaTestFunc_(DefaultAlphaTestFunc)
        ,descAllocator_(128)
    {
    }

    GraphicsDeviceRef::~GraphicsDeviceRef()
    {
        terminate();
    }

    bool GraphicsDeviceRef::initialize(const InitParam& param)
    {
        initializeGLES2();

        setClearColor(0);
        setClearDepth(1.0f);
        setClearStencil(0);

        glViewport(0, 0, param.backBufferWidth_, param.backBufferHeight_);

        enableState();
        return true;
    }

    void GraphicsDeviceRef::terminate()
    {
        terminateGLES2();
    }


#else
    namespace
    {
        class DC
        {
        public:
            DC(HDC hdc, HWND hwnd)
                :hdc_(hdc)
                ,hwnd_(hwnd)
            {
            }

            ~DC()
            {
                if(hdc_ != NULL){
                    ReleaseDC(hwnd_, hdc_);
                    hdc_ = NULL;
                }
            }

            bool isNull() const{ return hdc_ == NULL;}

            HDC release()
            {
                HDC tmp = hdc_;
                hdc_ = NULL;
                return tmp;
            }

            HDC get(){ return hdc_;}

            HDC hdc_;
            HWND hwnd_;
        };

        class HEGLDisplay
        {
        public:
            HEGLDisplay(EGLDisplay display)
                :display_(display)
            {
            }

            ~HEGLDisplay()
            {
                if(display_ != EGL_NO_DISPLAY){
                    eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                    eglTerminate(display_);
                    display_ = EGL_NO_DISPLAY;
                }
            }

            bool isNoDisplay() const{ return display_ == EGL_NO_DISPLAY;}

            EGLDisplay release()
            {
                EGLDisplay tmp = display_;
                display_ = EGL_NO_DISPLAY;
                return tmp;
            }

            EGLDisplay get(){ return display_;}

            HEGLDisplay& operator=(const EGLDisplay rhs)
            {
                release();
                display_ = rhs;
                return *this;
            }

            EGLDisplay display_;
        };
    }

    GraphicsDeviceRef::GraphicsDeviceRef()
        :display_(EGL_NO_DISPLAY)
        ,context_(EGL_NO_CONTEXT)
        ,surface_(EGL_NO_SURFACE)
        ,window_(NULL)

        ,state_(0)
        ,alphaTestRef_(DefaultAlphaTestRef)
        ,alphaTestFunc_(DefaultAlphaTestFunc)

        ,descAllocator_(128)
    {
    }

    GraphicsDeviceRef::~GraphicsDeviceRef()
    {
        terminate();
    }

    bool GraphicsDeviceRef::initialize(const InitParam& param)
    {
        initializeEGL();

        window_ = param.windowHandle_;
        DC hdc(GetDC(window_), window_);
        if(hdc.isNull()){
            return false;
        }

        // ディスプレイ取得、取得できなければデフォルト
        HEGLDisplay display = eglGetDisplay((NativeDisplayType)hdc.get());
        if(display.isNoDisplay()){
            display = eglGetDisplay((NativeDisplayType) EGL_DEFAULT_DISPLAY);
        }

        // GL初期化
        s32 major, minor;
        if(!eglInitialize(display.get(), &major, &minor)){
            return false;
        }

        // サーフェス属性設定
        //------------------------------------
        static const s32 ATTR_SIZE = 64;
        EGLint configAttr[ATTR_SIZE];
        s32 i=0;
        switch(param.format_)
        {
        case Display_R5G6B5:
            {
                configAttr[i++] = EGL_RED_SIZE;
                configAttr[i++] = 5;
                configAttr[i++] = EGL_GREEN_SIZE;
                configAttr[i++] = 6;
                configAttr[i++] = EGL_BLUE_SIZE;
                configAttr[i++] = 5;
                configAttr[i++] = EGL_ALPHA_SIZE;
                configAttr[i++] = 0;
            }
            break;

        case Display_X8R8G8B8:
            {
                configAttr[i++] = EGL_BUFFER_SIZE;
                configAttr[i++] = 32;
                configAttr[i++] = EGL_RED_SIZE;
                configAttr[i++] = 8;
                configAttr[i++] = EGL_GREEN_SIZE;
                configAttr[i++] = 8;
                configAttr[i++] = EGL_BLUE_SIZE;
                configAttr[i++] = 8;
                configAttr[i++] = EGL_ALPHA_SIZE;
                configAttr[i++] = 0;
            }
            break;

        case Display_A2R10G10B10:
            {
                configAttr[i++] = EGL_RED_SIZE;
                configAttr[i++] = 10;
                configAttr[i++] = EGL_GREEN_SIZE;
                configAttr[i++] = 10;
                configAttr[i++] = EGL_BLUE_SIZE;
                configAttr[i++] = 10;
                configAttr[i++] = EGL_ALPHA_SIZE;
                configAttr[i++] = 2;
            }
            break;
        default:
            LASSERT(false);
            return false;
        }

        configAttr[i++] = EGL_DEPTH_SIZE;
        configAttr[i++] = param.depthSize_;
        configAttr[i++] = EGL_STENCIL_SIZE;
        configAttr[i++] = param.stencilSize_;

        //configAttr[i++] = EGL_MAX_SWAP_INTERVAL;
        //configAttr[i++] = param.interval_;
        //configAttr[i++] = EGL_MIN_SWAP_INTERVAL;
        //configAttr[i++] = param.interval_;

        configAttr[i++] = EGL_SURFACE_TYPE;
        configAttr[i++] = EGL_WINDOW_BIT;
        configAttr[i++] = EGL_NONE;

        EGLConfig config = NULL;
        s32 configs =0;
        if(!eglChooseConfig(display.get(), configAttr, &config, 1, &configs)){
            return false;
            
        }else if(configs<=0){
            return false;
        }

        // サーフェス作成
        surface_ = eglCreateWindowSurface(display.get(), config, window_, NULL);
        if(surface_ == EGL_NO_SURFACE){
            surface_ = eglCreateWindowSurface(display.get(), config, NULL, NULL);
        }

        // コンテキスト作成
        //EGLint apiVersion[] = { EGL_CONTEXT_CLIENT_VERSION, 1, EGL_NONE };
        context_ = eglCreateContext(display.get(), config, NULL, NULL);
        if(context_ == EGL_NO_CONTEXT){
            return false;
        }

        EGLDisplay disp = display.get();
        EGLBoolean ret = eglMakeCurrent(disp, surface_, surface_, context_);
        if(!ret){
            return false;
        }

        display_ = display.release();

        initializeGLES2();

        LASSERT(0<=param.interval_ && param.interval_ < PresentInterval_Four);
        ret = eglSwapInterval(display_, param.interval_);
        hdc.release();

        glViewport(0, 0, param.backBufferWidth_, param.backBufferHeight_);

        enableState();

#if defined(_DEBUG)
        dumpEGLExtensions("eglext.txt");
        dumpGLES2Extensions("gles2ext.txt");
#endif
        return true;
    }

    void GraphicsDeviceRef::terminate()
    {
        if(display_ != EGL_NO_DISPLAY){

            eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if(context_ != EGL_NO_CONTEXT){
                eglDestroyContext(display_, context_);
                context_ = EGL_NO_CONTEXT;
            }

            if(surface_ != EGL_NO_SURFACE){
                eglDestroySurface(display_, surface_);
                surface_ = EGL_NO_SURFACE;
            }

            eglTerminate(display_);
            display_ = EGL_NO_DISPLAY;
        }

        terminateGLES2();
        terminateEGL();
    }

#endif
}

/**
@file GraphicsDeviceRef.cpp
@author t-sakai
@date 2010/05/29 create
@date 2010/12/12 modify for gles2
*/
#include "GraphicsDeviceRef.h"
#include "InitParam.h"
#include "../../lgraphics.h"
#include "RenderStateRef.h"
#include "../SamplerState.h"

namespace lgraphics
{
    namespace
    {
        const Char* ExtensionNames[Ext_Num] =
        {
            "GL_OES_compressed_ETC1_RGB8_texture",
        };
    }

    void Descriptor::release()
    {
        if(--counter_ == 0){
            lgraphics::Graphics::getDevice().freeDesc(this);
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

#if defined(ANDROID)
    GraphicsDeviceRef::GraphicsDeviceRef()
    {
    }

    GraphicsDeviceRef::~GraphicsDeviceRef()
    {
        terminate();
    }

    bool GraphicsDeviceRef::initialize(const InitParam& param)
    {
        descAllocator_.initialize(InitialIDAllocatorSize); //IDアロケータ初期化

        initializeGLES2();

        glViewport(0, 0, param.backBufferWidth_, param.backBufferHeight_);

        initializeRenderState();
        initializeExtension();
        return true;
    }

    void GraphicsDeviceRef::terminate()
    {
        terminateGLES2();
        //lcore::Log("desc cout: %d", descAllocator_.getCount());

        descAllocator_.terminate(); //IDアロケータ終了
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
    {
    }

    GraphicsDeviceRef::~GraphicsDeviceRef()
    {
        terminate();
    }

    bool GraphicsDeviceRef::initialize(const InitParam& param)
    {
        descAllocator_.initialize(InitialIDAllocatorSize); //IDアロケータ初期化

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

        eglBindAPI(EGL_OPENGL_ES_API);

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

        configAttr[i++] = EGL_LEVEL;
        configAttr[i++] = 0;

        configAttr[i++] = EGL_SURFACE_TYPE;
        configAttr[i++] = EGL_WINDOW_BIT;

        configAttr[i++] = EGL_RENDERABLE_TYPE;
        configAttr[i++] = EGL_OPENGL_ES2_BIT;

        configAttr[i++] = EGL_NATIVE_RENDERABLE;
        configAttr[i++] = EGL_FALSE;
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
        EGLint apiVersion[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        context_ = eglCreateContext(display.get(), config, NULL, apiVersion);
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

        initializeRenderState();
        initializeExtension();
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

        lcore::Log("desc cout: %d", descAllocator_.getCount());
        descAllocator_.terminate(); //IDアロケータ終了
    }

#endif

    // Rendering State系
    //------------------------------------------------------------------------------------------

    void GraphicsDeviceRef::initializeRenderState()
    {
        setClearColor(0);
        setClearDepth(1.0f);
        setClearStencil(0);

        glDepthFunc(GL_LESS);
        glBlendEquation(GL_FUNC_ADD);

        state_.flags_[State::Flag_AlphaTest] = true;
        state_.alphaTestRef_ = 128;
        state_.alphaTestFunc_ = Cmp_Greater;

        state_.flags_[State::Flag_MultiSampleAlias] = false;

        glEnable(GL_DEPTH_TEST);
        state_.flags_[State::Flag_ZEnable] = true;

        glDepthMask(GL_TRUE);
        state_.flags_[State::Flag_ZWriteEnable] = true;

        //Direct3Dベースなので座標系は逆、カリングを逆にしておく
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);
        glFrontFace(CullMode_CCW);

        //state_.flags_[State::Flag_CullingEnable] = true;
        state_.cullMode_ = CullMode_CCW;

        glDisable(GL_BLEND);
        state_.flags_[State::Flag_AlphaBlendEnable] = false;

        glBlendFuncSeparate(Blend_SrcAlpha, Blend_InvSrcAlpha, Blend_SrcAlpha, Blend_DestAlpha);
        state_.alphaBlendSrc_ = Blend_SrcAlpha;
        state_.alphaBlendDst_ = Blend_InvSrcAlpha;
        state_.flush_ = false;

        //テクスチャサンプラ初期化
        for(u32 i=0; i<MAX_TEXTURES; ++i){
            state_.samplerStates_[i].id_ = INVALID_TEXTURE_ID;
        }
    }

    //---------------------------------------------------------------
    // 拡張チェック
    void GraphicsDeviceRef::initializeExtension()
    {
        extension_.clear();
        const Char* ext = reinterpret_cast<const Char*>( glGetString(GL_EXTENSIONS) );

        const Char* name = ext;
        u32 len = 0;
        while(*ext != '\0'){
            if(lcore::isSpace(*ext)){
                len = ext - name;
                for(u32 i=0; i<Ext_Num; ++i){
                    if(0 == lcore::strncmp(name, ExtensionNames[i], len)){
                        extension_.set(LCORE_BIT(i));
                        lcore::Log("%s", ExtensionNames[i]);
                    }
                }
                name = ext + 1;
            }
            ++ext;
        }
    }

    //---------------------------------------------------------------
    namespace
    {
        inline void lglSetDepthTest(bool enable)
        {
            if(enable){
                glEnable(GL_DEPTH_TEST);
            }else{
                glDisable(GL_DEPTH_TEST);
            }
        }

        inline void lglSetZWrite(bool enable)
        {
            if(enable){
                glDepthMask(GL_TRUE);
            }else{
                glDepthMask(GL_FALSE);
            }
        }

        //inline void lglSetCulling(bool enable)
        //{
        //    if(enable){
        //        glEnable(GL_CULL_FACE);
        //    }else{
        //        glDisable(GL_CULL_FACE);
        //    }
        //}

        inline void lglSetAlphaBlend(bool enable)
        {
            if(enable){
                glEnable(GL_BLEND);
            }else{
                glDisable(GL_BLEND);
            }
        }
    }

    void GraphicsDeviceRef::setAlphaTest(bool enable)
    {
        if(state_.flags_[State::Flag_AlphaTest] != enable){
            state_.flags_[State::Flag_AlphaTest] = enable;
        }
    }

    void GraphicsDeviceRef::setAlphaTestFunc(CmpFunc func)
    {
        if(state_.alphaTestFunc_ != func){
            state_.alphaTestFunc_ = func;
        }
    }

    void GraphicsDeviceRef::setAlphaTestRef(s32 ref)
    {
        if(state_.alphaTestRef_ != ref){
            state_.alphaTestRef_ = ref;
        }
    }

    //void GraphicsDeviceRef::setCullingEnable(bool enable)
    //{
    //    if(state_.flags_[State::Flag_CullingEnable] != enable){
    //        state_.flags_[State::Flag_CullingEnable] = enable;
    //        lglSetCulling(enable);
    //    }
    //}

    void GraphicsDeviceRef::setCullMode(CullMode mode)
    {
        if(state_.cullMode_ != mode){
            state_.cullMode_ = mode;

            if(CullMode_None == state_.cullMode_){
                glDisable(GL_CULL_FACE);
            }else{
                glEnable(GL_CULL_FACE);
                glFrontFace(state_.cullMode_);
            }
        }
    }

    void GraphicsDeviceRef::setMultiSampleAlias(bool enable)
    {
        if(state_.flags_[State::Flag_MultiSampleAlias] != enable){
            state_.flags_[State::Flag_MultiSampleAlias] = enable;
        }
    }

    void GraphicsDeviceRef::setZEnable(bool enable)
    {
        if(state_.flags_[State::Flag_ZEnable] != enable){
            state_.flags_[State::Flag_ZEnable] = enable;
            lglSetDepthTest(enable);
        }
    }

    void GraphicsDeviceRef::setZWriteEnable(bool enable)
    {
        if(state_.flags_[State::Flag_ZWriteEnable] != enable){
            state_.flags_[State::Flag_ZWriteEnable] = enable;
            lglSetZWrite(enable);
        }
    }

    void GraphicsDeviceRef::setAlphaBlendEnable(bool enable)
    {
        if(state_.flags_[State::Flag_AlphaBlendEnable] != enable){
            state_.flags_[State::Flag_AlphaBlendEnable] = enable;
            lglSetAlphaBlend(enable);
        }
    }

    void GraphicsDeviceRef::setAlphaBlend(BlendType src, BlendType dst)
    {
        bool change = false;
        if(state_.alphaBlendSrc_ != src){
            change = true;
            state_.alphaBlendSrc_ = src;
        }

        if(state_.alphaBlendDst_ != dst){
            change = true;
            state_.alphaBlendDst_ = dst;
        }

        if(change){
            glBlendFuncSeparate(state_.alphaBlendSrc_, state_.alphaBlendDst_, Blend_SrcAlpha, Blend_DestAlpha);
        }
    }

    void GraphicsDeviceRef::setRenderState(const RenderStateRef& rhs)
    {
        bool enable;

        enable = rhs.check(RenderStateRef::Bit_ZEnable);
        if( state_.flags_[State::Flag_ZEnable] != enable ){
            state_.flags_[State::Flag_ZEnable] = enable;
            lglSetDepthTest(enable);
        }

        enable = rhs.check(RenderStateRef::Bit_ZWriteEnable);
        if( state_.flags_[State::Flag_ZWriteEnable] != enable ){
            state_.flags_[State::Flag_ZWriteEnable] = enable;
            lglSetZWrite(enable);
        }


        setCullMode(rhs.getCullMode());

        enable = rhs.check(RenderStateRef::Bit_AlphaBlendEnable);
        if( state_.flags_[State::Flag_AlphaBlendEnable] != enable ){
            state_.flags_[State::Flag_AlphaBlendEnable] = enable;
            lglSetAlphaBlend(enable);
        }

        setAlphaBlend(rhs.getAlphaBlendSrc(), rhs.getAlphaBlendDst());
    }


    void GraphicsDeviceRef::clearActiveTextures()
    {
        for(u32 i=0; i<MAX_TEXTURES; ++i){
            state_.samplerStates_[i].id_ = INVALID_TEXTURE_ID;
        }
    }

    // テクスチャセット
    void GraphicsDeviceRef::setTexture(u32 index, u32 id, u32 location)
    {
        LASSERT(0<=index && index<MAX_TEXTURES);

        glActiveTexture(GL_TEXTURE0 + index);

        if(state_.samplerStates_[index].id_ != id){
            state_.samplerStates_[index].id_ = id;
            glBindTexture(GL_TEXTURE_2D, id);


            //生成時に一度設定すればよいらしい
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, samplerState.getAddressU());
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, samplerState.getAddressV());
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplerState.getMagFilter());
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplerState.getMinFilter());

        }
        glUniform1i( location, index );
    }
}

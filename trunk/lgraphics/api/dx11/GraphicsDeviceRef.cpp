/**
@file GraphicsDeviceRef.cpp
@author t-sakai
@date 2010/03/23 create
*/
#include "GraphicsDeviceRef.h"
#include "InitParam.h"
#include "BlendStateRef.h"

namespace lgraphics
{
    //--------------------------------------
    //---
    //--- DepthStencilStateRef
    //---
    //--------------------------------------
    DepthStencilStateRef::DepthStencilStateRef()
        :state_(NULL)
    {}

    DepthStencilStateRef::DepthStencilStateRef(const DepthStencilStateRef& rhs)
        :state_(rhs.state_)
    {
        if(state_ != NULL){
            state_->AddRef();
        }
    }

    DepthStencilStateRef::~DepthStencilStateRef()
    {
        destroy();
    }

    void DepthStencilStateRef::destroy()
    {
        SAFE_RELEASE(state_);
    }

    DepthStencilStateRef::DepthStencilStateRef(ID3D11DepthStencilState* state)
        :state_(state)
    {}

    bool DepthStencilStateRef::valid() const
    {
        return (NULL != state_);
    }

    DepthStencilStateRef& DepthStencilStateRef::operator=(const DepthStencilStateRef& rhs)
    {
        DepthStencilStateRef tmp(rhs);
        tmp.swap(*this);
        return *this;
    }

    void DepthStencilStateRef::swap(DepthStencilStateRef& rhs)
    {
        lcore::swap(state_, rhs.state_);
    }

    //--------------------------------------
    //---
    //--- GraphicsDeviceRef
    //---
    //--------------------------------------

    static const Char* GSModels[] =
    {
        "gs_4_0",
        "gs_4_1",
        "gs_5_0",
    };

    static const Char* VSModels[] =
    {
        "vs_4_0",
        "vs_4_1",
        "vs_5_0",
    };

    static const Char* PSModels[] =
    {
        "ps_4_0",
        "ps_4_1",
        "ps_5_0",
    };

    const f32 GraphicsDeviceRef::Zero[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    const f32 GraphicsDeviceRef::One[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    const u32 GraphicsDeviceRef::UIZero[4] = {0, 0, 0, 0};

    ID3D11ShaderResourceView* const GraphicsDeviceRef::NULLResources[GraphicsDeviceRef::MaxShaderResources] =
    { NULL, };

    ID3D11RenderTargetView* const GraphicsDeviceRef::NullTargets[MaxRenderTargets] =
    { NULL, };

    const Char* GraphicsDeviceRef::GSModel = GSModels[0];
    const Char* GraphicsDeviceRef::VSModel = VSModels[0];
    const Char* GraphicsDeviceRef::PSModel = PSModels[0];

    GraphicsDeviceRef::GraphicsDeviceRef()
        :flags_(0)
        ,clearDepth_(1.0f)
        ,clearStencil_(0)
        ,clearDepthStencilFlag_(ClearDepth)
        ,syncInterval_(0)
        ,featureLevel_(0)
        ,device_(NULL)
        ,context_(NULL)
        ,swapChain_(NULL)
        ,backBuffer_(NULL)
        ,renderTargetView_(NULL)
        ,depthStencil_(NULL)
        ,depthStencilView_(NULL)
        ,depthStencilShaderResourceView_(NULL)
#ifdef _DEBUG
        ,debug_(NULL)
#endif
    {
        for(u32 i=0; i<MaxRenderTargets; ++i){
            blendFactors_[i] = 1.0f;
        }

        for(s32 i=0; i<Rasterizer_Num; ++i){
            rasterizerStates_[i] = NULL;
        }

        for(s32 i=0; i<BlendState_Num; ++i){
            blendStates_[i] = NULL;
        }

        for(s32 i=0; i<DepthStencil_Num; ++i){
            depthStencilStates_[i] = NULL;
        }
    }

    GraphicsDeviceRef::~GraphicsDeviceRef()
    {
    }

    //---------------------------------------------------------
    bool GraphicsDeviceRef::initialize(const InitParam& initParam)
    {
        LASSERT(0<initParam.supportHardwareLevel_);

        flags_ = initParam.flags_;
        u32 deviceFlags = 0;
#ifdef _DEBUG
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        // スワップ設定
        //-----------------------------------------------
        DXGI_SWAP_CHAIN_DESC swapChainDesc;

        u32 width = initParam.backBufferWidth_;
        u32 height = initParam.backBufferHeight_;

        // バッファ設定
        //-------------------------
        // サイズ
        swapChainDesc.BufferDesc.Width = width;
        swapChainDesc.BufferDesc.Height = height;

        // リフレッシュレート
        swapChainDesc.BufferDesc.RefreshRate.Numerator = initParam.refreshRate_;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

        // バッファフォーマット
        swapChainDesc.BufferDesc.Format = (DXGI_FORMAT)initParam.format_;

        // スキャンライン描画順
        swapChainDesc.BufferDesc.ScanlineOrdering = (DXGI_MODE_SCANLINE_ORDER)DXGIScanlineOrder_Unspecified;

        // スケーリング
        swapChainDesc.BufferDesc.Scaling = (DXGI_MODE_SCALING)DXGIScaling_Unspecified;

        // サンプリング設定
        //-------------------------
        swapChainDesc.SampleDesc.Count = 1; // サンプリング数／ピクセル
        swapChainDesc.SampleDesc.Quality = 0; // 品質：0からID3D10Device::CheckMultiSampleQualityLevelsまで

        swapChainDesc.BufferUsage = DXGIUsage_RenderTargetOutput;
        swapChainDesc.BufferCount = 2; //フロントバッファを含むバッファ数
        swapChainDesc.OutputWindow = initParam.windowHandle_;
        swapChainDesc.Windowed =    initParam.windowed_;
        swapChainDesc.SwapEffect =  (DXGI_SWAP_EFFECT)initParam.swapEffect_;;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        D3D_FEATURE_LEVEL attemptFeatureLevel[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };

        D3D_FEATURE_LEVEL supportedLevel;

        // デバイスとスワップチェイン作成
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            NULL, //IDXGIAdapter。デフォルトを使用する場合NULL
            (D3D_DRIVER_TYPE)initParam.type_, //ドライバタイプ
            NULL, //HMODULE ドライバタイプがSOFTWAREの場合LoadLibraryでDLLをロードして渡す。それ以外のタイプではNULL
            deviceFlags, //フラグ
            attemptFeatureLevel, //FutureLevel配列
            initParam.supportHardwareLevel_, //FutureLevel配列要素数
            D3D11_SDK_VERSION,
            &swapChainDesc,
            &swapChain_,
            &device_,
            &supportedLevel,
            &context_);

        if(FAILED(hr)){
            lcore::Log("Cannot create Direct3D Device");
            if(0 == initParam.allowSoftwareDevice_){
                return false;
            }
            lcore::Log("Try to create Reference Device");

            //リファレンスデバイス作成
            hr = D3D11CreateDeviceAndSwapChain(
                NULL, //IDXGIAdapter。デフォルトを使用する場合NULL
                D3D_DRIVER_TYPE_REFERENCE, //ドライバタイプ
                NULL, //HMODULE ドライバタイプがSOFTWAREの場合LoadLibraryでDLLをロードして渡す。それ以外のタイプではNULL
                deviceFlags, //フラグ
                attemptFeatureLevel, //FutureLevel配列
                initParam.supportHardwareLevel_, //FutureLevel配列要素数
                D3D11_SDK_VERSION,
                &swapChainDesc,
                &swapChain_,
                &device_,
                &supportedLevel,
                &context_);

            if(FAILED(hr)){
                lcore::Log("Cannot create Reference Device");
                return false;
            }
        }
        featureLevel_ = supportedLevel;

#ifdef _DEBUG
        device_->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug_));
#endif

        switch(supportedLevel)
        {
        case D3D_FEATURE_LEVEL_10_0:
            VSModel = VSModels[0];
            PSModel = PSModels[0];
            break;

        case D3D_FEATURE_LEVEL_10_1:
            VSModel = VSModels[1];
            PSModel = PSModels[1];
            break;

        case D3D_FEATURE_LEVEL_11_0:
        default:
            VSModel = VSModels[2];
            PSModel = PSModels[2];
            break;
        }

        createBackBuffer(width, height, initParam.depthStencilFormat_, flags_);

        syncInterval_ = initParam.interval_;
        refreshRate_ = lcore::clamp(initParam.refreshRate_, MinRefreshRate, MaxRefreshRate);
        clearColor_[0] = clearColor_[1] = clearColor_[2] = clearColor_[3] = 1.0f;

        {//ラスタライザステート
            D3D11_RASTERIZER_DESC rasterizerDesc;
            rasterizerDesc.FillMode = D3D11_FILL_SOLID;
            rasterizerDesc.CullMode = D3D11_CULL_BACK;
            rasterizerDesc.FrontCounterClockwise = FALSE;
            rasterizerDesc.DepthBias = 0;
            rasterizerDesc.SlopeScaledDepthBias = 0.0f;
            rasterizerDesc.DepthBiasClamp = 0.0f;
            rasterizerDesc.DepthClipEnable = TRUE;
            rasterizerDesc.ScissorEnable = FALSE;
            rasterizerDesc.MultisampleEnable = FALSE;
            rasterizerDesc.AntialiasedLineEnable = FALSE;

            hr = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerStates_[Rasterizer_FillSolid]);
            if(FAILED(hr)){
                return false;
            }

            rasterizerDesc.CullMode = D3D11_CULL_NONE;
            hr = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerStates_[Rasterizer_FillSolidNoCull]);
            if(FAILED(hr)){
                return false;
            }

            rasterizerDesc.CullMode = D3D11_CULL_BACK;
            rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
            hr = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerStates_[Rasterizer_FillWireFrame]);
            if(FAILED(hr)){
                return false;
            }

            rasterizerDesc.CullMode = D3D11_CULL_NONE;
            rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
            hr = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerStates_[Rasterizer_FillWireFrameNoCull]);
            if(FAILED(hr)){
                return false;
            }

            rasterizerDesc.FillMode = D3D11_FILL_SOLID;

            //デプスバッファのフォーマットによってDepthBiasの扱いが異なる
            const f32 depthBias = 0.0001f;
            switch(initParam.shadowMapFormat_)
            {
            case lgraphics::Data_D16_UNorm:
                rasterizerDesc.DepthBias = static_cast<INT>(depthBias * 0xFFFF);
                break;

            case lgraphics::Data_D32_Float:
            case lgraphics::Data_D32_Float_S8X24_UInt:
                rasterizerDesc.DepthBias = static_cast<INT>(depthBias * 0x7FFFFF);
                break;

            default:
                rasterizerDesc.DepthBias = static_cast<INT>(depthBias * 0xFFFFFF);
                break;
            };

            rasterizerDesc.SlopeScaledDepthBias = 1.0f;
            rasterizerDesc.DepthBiasClamp = 0.02f;

            hr = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerStates_[Rasterizer_DepthMap]);
            if(FAILED(hr)){
                return false;
            }

            setRasterizerState(lgraphics::GraphicsDeviceRef::Rasterizer_FillSolid);
        }

        {//DepthStencilステート
            D3D11_DEPTH_STENCIL_DESC desc;
            lcore::memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));

            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(Cmp_Less);
            desc.StencilEnable = FALSE;
            device_->CreateDepthStencilState(&desc, &depthStencilStates_[DepthStencil_DEnableWEnable]);

            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            device_->CreateDepthStencilState(&desc, &depthStencilStates_[DepthStencil_DEnableWDisable]);

            desc.DepthEnable = FALSE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            device_->CreateDepthStencilState(&desc, &depthStencilStates_[DepthStencil_DDisableWEnable]);

            desc.DepthEnable = FALSE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            device_->CreateDepthStencilState(&desc, &depthStencilStates_[DepthStencil_DDisableWDisable]);

            setDepthStencilState(DepthStencil_DEnableWEnable);
        }

        {//ブレンドステート
            blendStates_[BlendState_NoAlpha] = lgraphics::BlendState::createRaw(
                FALSE,
                FALSE,
                lgraphics::Blend_One,
                lgraphics::Blend_Zero,
                lgraphics::BlendOp_Add,
                lgraphics::Blend_One,
                lgraphics::Blend_Zero,
                lgraphics::BlendOp_Add,
                lgraphics::ColorWrite_All);

            blendStates_[BlendState_Alpha] = lgraphics::BlendState::createRaw(
                FALSE,
                TRUE,
                lgraphics::Blend_SrcAlpha,
                lgraphics::Blend_InvSrcAlpha,
                lgraphics::BlendOp_Add,
                lgraphics::Blend_SrcAlpha,
                lgraphics::Blend_DestAlpha,
                lgraphics::BlendOp_Add,
                lgraphics::ColorWrite_All);

            blendStates_[BlendState_AlphaAdditive] = lgraphics::BlendState::createRaw(
                FALSE,
                TRUE,
                lgraphics::Blend_SrcAlpha,
                lgraphics::Blend_One,
                lgraphics::BlendOp_Add,
                lgraphics::Blend_SrcAlpha,
                lgraphics::Blend_One,
                lgraphics::BlendOp_Add,
                lgraphics::ColorWrite_All);
        }

        if(0 == initParam.windowed_){
            ShowCursor(FALSE);
        }

        return true;
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::terminate()
    {
        if(NULL != context_){
            context_->ClearState();
            context_->Flush();
        }

        context_->OMSetRenderTargets(0, NULL, NULL);

        for(s32 i=0; i<BlendState_Num; ++i){
            SAFE_RELEASE(blendStates_[i]);
        }

        for(s32 i=0; i<DepthStencil_Num; ++i){
            SAFE_RELEASE(depthStencilStates_[i]);
        }

        for(s32 i=0; i<Rasterizer_Num; ++i){
            SAFE_RELEASE(rasterizerStates_[i]);
        }

        SAFE_RELEASE(depthStencilShaderResourceView_);
        SAFE_RELEASE(renderTargetView_);
        SAFE_RELEASE(depthStencilView_);
        SAFE_RELEASE(backBuffer_);
        SAFE_RELEASE(depthStencil_);

        if(NULL != swapChain_){
            BOOL fullscreen = FALSE;
            IDXGIOutput* output = NULL;
            HRESULT hr = swapChain_->GetFullscreenState(&fullscreen, &output);
            if(SUCCEEDED(hr) && fullscreen){
                swapChain_->SetFullscreenState(FALSE, NULL);
                ShowCursor(TRUE);
            }
            SAFE_RELEASE(output);
            swapChain_->Release();
            swapChain_ = NULL;
        }


        SAFE_RELEASE(context_);

#ifdef _DEBUG
        if(NULL != debug_){
            debug_->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        }
        SAFE_RELEASE(debug_);
#endif
        SAFE_RELEASE(device_);
    }

    //---------------------------------------------------------
    bool GraphicsDeviceRef::createBackBuffer(u32 width, u32 height, u32 depthStencilFormat, u32 flag)
    {
        HRESULT hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer_);
        if(FAILED(hr)){
            return false;
        }

        hr = device_->CreateRenderTargetView(backBuffer_, NULL, &renderTargetView_);
        if(FAILED(hr)){
            return false;
        }

        if(depthStencilFormat != 0){
            DXGI_FORMAT depthFormat = (DXGI_FORMAT)depthStencilFormat;
            if(0 != (flag & InitParam::Flag_DepthStencilShaderResource)){
                switch(depthFormat)
                {
                case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                    depthFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
                    break;
                case DXGI_FORMAT_D32_FLOAT:
                    depthFormat = DXGI_FORMAT_R32_TYPELESS;
                    break;
                case DXGI_FORMAT_D24_UNORM_S8_UINT:
                    depthFormat = DXGI_FORMAT_R24G8_TYPELESS;
                    break;
                case DXGI_FORMAT_D16_UNORM:
                    depthFormat = DXGI_FORMAT_R16_TYPELESS;
                    break;
                default:
                    depthFormat = DXGI_FORMAT_R24G8_TYPELESS;
                    break;
                }
            }
            D3D11_TEXTURE2D_DESC desc;
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = depthFormat;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = (0 != (flag & InitParam::Flag_DepthStencilShaderResource))? D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_DEPTH_STENCIL;
            desc.CPUAccessFlags = CPUAccessFlag_None;
            desc.MiscFlags = ResourceMisc_None;
            hr = device_->CreateTexture2D(&desc, NULL, &depthStencil_);
            if(FAILED(hr)){
                return false;
            }

            D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
            lcore::memset(&viewDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
            viewDesc.Format = (DXGI_FORMAT)depthStencilFormat;
            viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            viewDesc.Texture2D.MipSlice = 0;

            hr = device_->CreateDepthStencilView(depthStencil_, &viewDesc, &depthStencilView_);
            if(FAILED(hr)){
                return false;
            }

            if(0 != (flag & InitParam::Flag_DepthStencilShaderResource)){
                D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc;

                DXGI_FORMAT format;
                switch((DXGI_FORMAT)depthStencilFormat)
                {
                case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                    format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
                    break;
                case DXGI_FORMAT_D32_FLOAT:
                    format = DXGI_FORMAT_R32_FLOAT;
                    break;
                case DXGI_FORMAT_D24_UNORM_S8_UINT:
                    format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                    break;
                case DXGI_FORMAT_D16_UNORM:
                    format = DXGI_FORMAT_R16_UNORM;
                    break;
                default:
                    format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                    break;
                }

                resourceDesc.Format = format;
                resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                resourceDesc.Texture2D.MipLevels = 1;
                resourceDesc.Texture2D.MostDetailedMip = 0;

                hr = device_->CreateShaderResourceView(depthStencil_, &resourceDesc, &depthStencilShaderResourceView_);
                if(FAILED(hr)){
                    return false;
                }
            }
        }

        context_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

        setDefaultViewport(0, 0, width, height);
        restoreDefaultViewport();
        return true;
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::setBlendState(BlendStateRef& state)
    {
        context_->OMSetBlendState(state.get(), blendFactors_, 0xFFFFFFFFU);
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::getRenderTargetDesc(u32& width, u32& height)
    {
        width = 1;
        height = 1;

        D3D11_TEXTURE2D_DESC desc;
        backBuffer_->GetDesc(&desc);

        width = desc.Width;
        height = desc.Height;
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::setViewport(u32 x, u32 y, u32 width, u32 height)
    {
        D3D11_VIEWPORT vp;
        vp.Width = static_cast<f32>(width);
        vp.Height = static_cast<f32>(height);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = static_cast<f32>(x);
        vp.TopLeftY = static_cast<f32>(y);
        context_->RSSetViewports(1, &vp );
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::setDefaultViewport(u32 x, u32 y, u32 width, u32 height)
    {
        viewportX_ = x;
        viewportY_ = y;
        viewportWidth_ = width;
        viewportHeight_ = height;
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::restoreDefaultViewport()
    {
        setViewport(viewportX_, viewportY_, viewportWidth_, viewportHeight_);
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::onSize(u32 /*width*/, u32 /*height*/)
    {
        if(NULL == swapChain_ || NULL == context_){
            return;
        }

        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        HRESULT hr = swapChain_->GetDesc(&swapChainDesc);
        if(FAILED(hr)){
            return;
        }

        u32 depthStencilFormat = 0;
        if(NULL != depthStencil_){
            D3D11_TEXTURE2D_DESC desc;
            depthStencil_->GetDesc(&desc);
            depthStencilFormat = desc.Format;
        }

        context_->OMSetRenderTargets(0, NULL, NULL);
        SAFE_RELEASE(renderTargetView_);
        SAFE_RELEASE(depthStencilShaderResourceView_);
        SAFE_RELEASE(depthStencilView_);
        SAFE_RELEASE(backBuffer_);
        SAFE_RELEASE(depthStencil_);

        swapChain_->ResizeBuffers(
            swapChainDesc.BufferCount,
            swapChainDesc.BufferDesc.Width,
            swapChainDesc.BufferDesc.Height,
            swapChainDesc.BufferDesc.Format,
            swapChainDesc.Flags);

        createBackBuffer(swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height, depthStencilFormat, flags_);
    }

    //---------------------------------------------------------
    bool GraphicsDeviceRef::isFullScreen()
    {
        BOOL mode = FALSE;
        swapChain_->GetFullscreenState(&mode, NULL);
        return (TRUE == mode);
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::changeScreenMode()
    {
        if(isFullScreen()){
            swapChain_->SetFullscreenState(FALSE, NULL);
            ShowCursor(TRUE);
        }else{
            swapChain_->SetFullscreenState(TRUE, NULL);
            ShowCursor(FALSE);
        }
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::restoreDefaultRenderTargets()
    {
        context_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);
        restoreDefaultViewport();
    }

    //---------------------------------------------------------
    DepthStencilStateRef GraphicsDeviceRef::createDepthStencilState(
        bool depthEnable,
        DepthWriteMask depthWriteMask,
        CmpFunc depthFunc,
        bool stencilEnable,
        u8 stencilReadMask,
        u8 stencilWriteMask,
        const DepthStencilStateRef::StencilOPDesc& frontFace,
        const DepthStencilStateRef::StencilOPDesc& backFace)
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        desc.DepthEnable = (depthEnable)? TRUE : FALSE;
        desc.DepthWriteMask = (D3D11_DEPTH_WRITE_MASK)depthWriteMask;
        desc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(depthFunc);
        desc.StencilEnable = (stencilEnable)? TRUE : FALSE;
        desc.StencilReadMask = stencilReadMask;
        desc.StencilWriteMask = stencilWriteMask;

        desc.FrontFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(frontFace.failOp_);
        desc.FrontFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(frontFace.depthFailOp_);
        desc.FrontFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(frontFace.passOp_);
        desc.FrontFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(frontFace.cmpFunc_);

        desc.BackFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(backFace.failOp_);
        desc.BackFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(backFace.depthFailOp_);
        desc.BackFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(backFace.passOp_);
        desc.BackFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(backFace.cmpFunc_);

        ID3D11DepthStencilState* state = NULL;
        device_->CreateDepthStencilState(&desc, &state);

        return DepthStencilStateRef(state);
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::setDepthStencilState(DepthStencilStateRef& state, u32 stencilRef)
    {
        LASSERT(state.valid());

        context_->OMSetDepthStencilState(state.state_, stencilRef);
    }

    //---------------------------------------------------------
    bool GraphicsDeviceRef::checkMultisampleQualityLevels(
            lgraphics::DataFormat format,
            u32 sampleCount,
            u32* qualityLevels)
    {
        LASSERT(sampleCount<=MaxMultiSampleCount);
        LASSERT(NULL != qualityLevels);
        HRESULT hr = device_->CheckMultisampleQualityLevels((DXGI_FORMAT)format, sampleCount, qualityLevels);
        return SUCCEEDED(hr);
    }
}

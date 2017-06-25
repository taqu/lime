/**
@file GraphicsDeviceRef.cpp
@author t-sakai
@date 2010/03/23 create
*/
#include "GraphicsDeviceRef.h"

#include "InitParam.h"
#include "BlendStateRef.h"
#include "AsynchronousRef.h"
#include "PredicateRef.h"

namespace lgfx
{
    //--------------------------------------
    //---
    //--- CommandListRef
    //---
    //--------------------------------------
    CommandListRef::CommandListRef()
        :commandList_(NULL)
    {
    }

    CommandListRef::CommandListRef(const CommandListRef& rhs)
        :commandList_(rhs.commandList_)
    {
        if(NULL != commandList_){
            commandList_->AddRef();
        }
    }

    CommandListRef::CommandListRef(pointer_type commandList)
        :commandList_(commandList)
    {
    }

    CommandListRef::~CommandListRef()
    {
        LDXSAFE_RELEASE(commandList_);
    }

    CommandListRef& CommandListRef::operator=(const CommandListRef& rhs)
    {
        CommandListRef(rhs).swap(*this);
        return *this;
    }

    void CommandListRef::swap(CommandListRef& rhs)
    {
        lcore::swap(commandList_, rhs.commandList_);
    }

    //--------------------------------------
    //---
    //--- ContextRef
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

    static const Char* CSModels[] =
    {
        "cs_4_0",
        "cs_4_1",
        "cs_5_0",
    };

    static const Char* DSModels[] =
    {
        "",
        "",
        "ds_5_0",
    };

    static const Char* HSModels[] =
    {
        "",
        "",
        "hs_5_0",
    };

    const f32 ContextRef::Zero[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    const f32 ContextRef::One[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    const u32 ContextRef::UIZero[4] = {0, 0, 0, 0};

    ID3D11ShaderResourceView* const ContextRef::NULLResources[GraphicsDeviceRef::MaxShaderResources] =
    { NULL };
    ID3D11SamplerState* const ContextRef::NULLSamplerStates[GraphicsDeviceRef::MaxSamplerStates] =
    { NULL };
    ID3D11Buffer* const ContextRef::NULLBuffers[GraphicsDeviceRef::MaxBuffers] =
    { NULL };
    ID3D11RenderTargetView* const ContextRef::NullTargets[LGFX_MAX_RENDER_TARGETS] =
    { NULL };

    const Char* ContextRef::GSModel = GSModels[0];
    const Char* ContextRef::VSModel = VSModels[0];
    const Char* ContextRef::PSModel = PSModels[0];
    const Char* ContextRef::CSModel = CSModels[0];
    const Char* ContextRef::DSModel = DSModels[0];
    const Char* ContextRef::HSModel = HSModels[0];

    ContextRef::ContextRef()
        :featureFlags_(FeatureFlag_None)
        ,context_(NULL)
        ,backBuffer_(NULL)
        ,renderTargetView_(NULL)
        ,depthStencil_(NULL)
        ,depthStencilView_(NULL)
        ,depthStencilShaderResourceView_(NULL)
    {
        viewport_.x_ = viewport_.y_ = viewport_.width_ = viewport_.height_ = 0.0f;
        viewport_.minDepth_ = 0.0f;
        viewport_.maxDepth_ = 1.0f;

        for(u32 i=0; i<LGFX_MAX_RENDER_TARGETS; ++i){
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

    ContextRef::ContextRef(const ContextRef& rhs)
        :featureFlags_(rhs.featureFlags_)
        ,viewport_(rhs.viewport_)
        ,context_(rhs.context_)
        ,backBuffer_(rhs.backBuffer_)
        ,renderTargetView_(rhs.renderTargetView_)
        ,depthStencil_(rhs.depthStencil_)
        ,depthStencilView_(rhs.depthStencilView_)
        ,depthStencilShaderResourceView_(rhs.depthStencilShaderResourceView_)
    {
        for(s32 i=0; i<Rasterizer_Num; ++i){
            rasterizerStates_[i] = rhs.rasterizerStates_[i];
        }

        for(u32 i=0; i<LGFX_MAX_RENDER_TARGETS; ++i){
            blendFactors_[i] = rhs.blendFactors_[i];
        }

        for(s32 i=0; i<DepthStencil_Num; ++i){
            depthStencilStates_[i] = rhs.depthStencilStates_[i];
        }

        for(s32 i=0; i<BlendState_Num; ++i){
            blendStates_[i] = rhs.blendStates_[i];
        }
        if(context_){
            context_->AddRef();
        }
        addRef();
    }

    ContextRef::ContextRef(ID3D11Device* device, ID3D11DeviceContext* context, const ContextRef& rhs)
        :featureFlags_(FeatureFlag_None)
        ,viewport_(rhs.viewport_)
        ,context_(context)
        ,backBuffer_(rhs.backBuffer_)
        ,renderTargetView_(rhs.renderTargetView_)
        ,depthStencil_(rhs.depthStencil_)
        ,depthStencilView_(rhs.depthStencilView_)
        ,depthStencilShaderResourceView_(rhs.depthStencilShaderResourceView_)
    {
        for(s32 i=0; i<Rasterizer_Num; ++i){
            rasterizerStates_[i] = rhs.rasterizerStates_[i];
        }

        for(u32 i=0; i<LGFX_MAX_RENDER_TARGETS; ++i){
            blendFactors_[i] = rhs.blendFactors_[i];
        }

        for(s32 i=0; i<DepthStencil_Num; ++i){
            depthStencilStates_[i] = rhs.depthStencilStates_[i];
        }

        for(s32 i=0; i<BlendState_Num; ++i){
            blendStates_[i] = rhs.blendStates_[i];
        }
        addRef();
        setFeatureFlags(device);
    }

    ContextRef::~ContextRef()
    {
        terminate();
    }

    //---------------------------------------------------------
    void ContextRef::addRef()
    {
        for(s32 i=0; i<Rasterizer_Num; ++i){
            if(rasterizerStates_[i]){
                rasterizerStates_[i]->AddRef();
            }
        }

        for(s32 i=0; i<DepthStencil_Num; ++i){
            if(depthStencilStates_[i]){
                depthStencilStates_[i]->AddRef();
            }
        }

        for(s32 i=0; i<BlendState_Num; ++i){
            if(blendStates_[i]){
                blendStates_[i]->AddRef();
            }
        }

        if(backBuffer_){
            backBuffer_->AddRef();
        }

        if(renderTargetView_){
            renderTargetView_->AddRef();
        }

        if(depthStencil_){
            depthStencil_->AddRef();
        }

        if(depthStencilView_){
            depthStencilView_->AddRef();
        }

        if(depthStencilShaderResourceView_){
            depthStencilShaderResourceView_->AddRef();
        }
    }

    //---------------------------------------------------------
    void ContextRef::terminate()
    {
        if(NULL != context_){
            context_->OMSetRenderTargets(0, NULL, NULL);
            context_->ClearState();
            context_->Flush();
        }

        LDXSAFE_RELEASE(depthStencilShaderResourceView_);
        LDXSAFE_RELEASE(renderTargetView_);
        LDXSAFE_RELEASE(depthStencilView_);
        LDXSAFE_RELEASE(backBuffer_);
        LDXSAFE_RELEASE(depthStencil_);

        for(s32 i=0; i<BlendState_Num; ++i){
            LDXSAFE_RELEASE(blendStates_[i]);
        }

        for(s32 i=0; i<DepthStencil_Num; ++i){
            LDXSAFE_RELEASE(depthStencilStates_[i]);
        }

        for(s32 i=0; i<Rasterizer_Num; ++i){
            LDXSAFE_RELEASE(rasterizerStates_[i]);
        }

        LDXSAFE_RELEASE(context_);
    }

    //---------------------------------------------------------
    void ContextRef::setViewport(f32 x, f32 y, f32 width, f32 height)
    {
        D3D11_VIEWPORT vp;
        vp.Width = width;
        vp.Height = height;
        vp.MinDepth = viewport_.minDepth_;
        vp.MaxDepth = viewport_.maxDepth_;
        vp.TopLeftX = x;
        vp.TopLeftY = y;
        context_->RSSetViewports(1, &vp );
    }

    //---------------------------------------------------------
    void ContextRef::setViewport(const Viewport& viewport)
    {
        D3D11_VIEWPORT vp;
        vp.TopLeftX = viewport.x_;
        vp.TopLeftY = viewport.y_;
        vp.Width = viewport.width_;
        vp.Height = viewport.height_;
        vp.MinDepth = viewport_.minDepth_;
        vp.MaxDepth = viewport_.maxDepth_;
        context_->RSSetViewports(1, &vp);
    }

    //---------------------------------------------------------
    void ContextRef::setDefaultViewport(const Viewport& viewport)
    {
        viewport_ = viewport;
    }

    //---------------------------------------------------------
    void ContextRef::restoreDefaultViewport()
    {
        D3D11_VIEWPORT vp;
        vp.TopLeftX = viewport_.x_;
        vp.TopLeftY = viewport_.y_;
        vp.Width = viewport_.width_;
        vp.Height = viewport_.height_;
        vp.MinDepth = viewport_.minDepth_;
        vp.MaxDepth = viewport_.maxDepth_;
        context_->RSSetViewports(1, &vp);
    }

    //---------------------------------------------------------
    void ContextRef::restoreDefaultRenderTargets()
    {
        context_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);
    }

    //---------------------------------------------------------
    void ContextRef::getRenderTargetDesc(u32& width, u32& height)
    {
        width = 1;
        height = 1;

        D3D11_TEXTURE2D_DESC desc;
        backBuffer_->GetDesc(&desc);

        width = desc.Width;
        height = desc.Height;
    }

    //---------------------------------------------------------
    void ContextRef::begin(AsynchronousRef& asynchronous)
    {
        context_->Begin(asynchronous.asynchronous_);
    }

    //---------------------------------------------------------
    void ContextRef::end(AsynchronousRef& asynchronous)
    {
        context_->End(asynchronous.asynchronous_);
    }

    //---------------------------------------------------------
    void ContextRef::setPredicate(PredicateRef& predicate, BOOL predicateValue)
    {
        context_->SetPredication((ID3D11Predicate*)predicate.asynchronous_, predicateValue);
    }

    //---------------------------------------------------------
    s32 ContextRef::getData(AsynchronousRef& asynchronous, void* data, u32 size, u32 flag)
    {
        HRESULT hr = context_->GetData(asynchronous.asynchronous_, data, size, flag);
        return hr;
    }

    //---------------------------------------------------------
    CommandListRef ContextRef::finishCommandList(s32 restoreState)
    {
        CommandListRef tmp;
        context_->FinishCommandList(restoreState, &tmp.commandList_);
        return tmp;
    }

    //---------------------------------------------------------
    void ContextRef::executeCommandList(CommandListRef& commandList, s32 restoreState)
    {
        LASSERT(commandList.valid());
        context_->ExecuteCommandList(commandList.commandList_, restoreState);
    }

    //---------------------------------------------------------
    void ContextRef::swap(ContextRef& rhs)
    {
        lcore::swap(featureFlags_, rhs.featureFlags_);
        lcore::swap(viewport_, rhs.viewport_);

        lcore::swap(context_, rhs.context_);

        for(s32 i=0; i<Rasterizer_Num; ++i){
            lcore::swap(rasterizerStates_[i], rhs.rasterizerStates_[i]);
        }

        for(u32 i=0; i<LGFX_MAX_RENDER_TARGETS; ++i){
            lcore::swap(blendFactors_[i], rhs.blendFactors_[i]);
        }

        for(s32 i=0; i<DepthStencil_Num; ++i){
            lcore::swap(depthStencilStates_[i], rhs.depthStencilStates_[i]);
        }

        for(s32 i=0; i<BlendState_Num; ++i){
            lcore::swap(blendStates_[i], rhs.blendStates_[i]);
        }

        lcore::swap(backBuffer_, rhs.backBuffer_);
        lcore::swap(renderTargetView_, rhs.renderTargetView_);
        lcore::swap(depthStencil_, rhs.depthStencil_);
        lcore::swap(depthStencilView_, rhs.depthStencilView_);
        lcore::swap(depthStencilShaderResourceView_, rhs.depthStencilShaderResourceView_);
    }

    void ContextRef::setFeatureFlags(ID3D11Device* device)
    {
        featureFlags_ = FeatureFlag_None;

        D3D11_DEVICE_CONTEXT_TYPE contextType = context_->GetType();

        if(D3D11_DEVICE_CONTEXT_DEFERRED == contextType){
            featureFlags_ |= FeatureFlag_Deffered;
            D3D11_FEATURE_DATA_THREADING threadingCaps = { FALSE, FALSE };
            HRESULT hr = device->CheckFeatureSupport( D3D11_FEATURE_THREADING, &threadingCaps, sizeof(threadingCaps) );
            if(SUCCEEDED(hr)){
                if(threadingCaps.DriverCommandLists){
                    featureFlags_ |= FeatureFlag_CommandList;
                }
            }
        }else{
            featureFlags_ |= FeatureFlag_Immediate;
        }
    }

    //--------------------------------------
    //---
    //--- DefferedContextRef
    //---
    //--------------------------------------
    DefferedContextRef::DefferedContextRef()
    {
    }

    DefferedContextRef::DefferedContextRef(const DefferedContextRef& rhs)
        :ContextRef(rhs)
    {
    }

    DefferedContextRef::DefferedContextRef(ID3D11Device* device, ID3D11DeviceContext* context, const ContextRef& rhs)
        :ContextRef(device, context, rhs)
    {
    }

    DefferedContextRef::~DefferedContextRef()
    {
        ContextRef::terminate();
    }

    DefferedContextRef& DefferedContextRef::operator=(const DefferedContextRef& rhs)
    {
        DefferedContextRef tmp(rhs);
        tmp.swap(*this);
        return *this;
    }

    void DefferedContextRef::swap(DefferedContextRef& rhs)
    {
        ContextRef::swap(rhs);
    }

    void DefferedContextRef::terminate()
    {
        ContextRef::terminate();
    }

    //--------------------------------------
    //---
    //--- GraphicsDeviceRef
    //---
    //--------------------------------------
    GraphicsDeviceRef::GraphicsDeviceRef()
        :flags_(0)
        ,syncInterval_(0)
        ,featureLevel_(0)
        ,maximumFrameLatency_(0)
        ,device_(NULL)
        ,swapChain_(NULL)
    {
    }

    GraphicsDeviceRef::~GraphicsDeviceRef()
    {
    }

    //---------------------------------------------------------
    bool GraphicsDeviceRef::initialize(const InitParam& initParam)
    {
        LASSERT(0<initParam.supportHardwareLevel_);

        flags_ = initParam.flags_;
        u32 deviceFlags = initParam.deviceFlags_;
#ifdef _DEBUG
        deviceFlags |= CreateDevice_Debug;
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
        LIME_DEBUGNAME(swapChain_, "System SwapCain");
        LIME_DEBUGNAME(device_, "System Device");
        LIME_DEBUGNAME(context_, "System Context");

        featureLevel_ = supportedLevel;

        switch(supportedLevel)
        {
        case D3D_FEATURE_LEVEL_10_0:
            VSModel = VSModels[0];
            GSModel = GSModels[0];
            PSModel = PSModels[0];
            CSModel = CSModels[0];
            DSModel = DSModels[0];
            HSModel = HSModels[0];
            break;

        case D3D_FEATURE_LEVEL_10_1:
            VSModel = VSModels[1];
            GSModel = GSModels[1];
            PSModel = PSModels[1];
            CSModel = CSModels[1];
            DSModel = DSModels[1];
            HSModel = HSModels[1];
            break;

        case D3D_FEATURE_LEVEL_11_0:
        default:
            VSModel = VSModels[2];
            GSModel = GSModels[2];
            PSModel = PSModels[2];
            CSModel = CSModels[2];
            DSModel = DSModels[2];
            HSModel = HSModels[2];
            break;
        }

        {
            IDXGIDevice1* dxgiDevice = NULL;
            hr = device_->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);
            if(SUCCEEDED(hr)){
                UINT maximumFrameLatency = lcore::minimum(16U, initParam.maximumFrameLatency_);
                dxgiDevice->SetMaximumFrameLatency(maximumFrameLatency);
                hr = dxgiDevice->GetMaximumFrameLatency(&maximumFrameLatency);
                maximumFrameLatency_ = (SUCCEEDED(hr))? maximumFrameLatency : 3U;
                dxgiDevice->Release();
            } else{
                maximumFrameLatency_ = 3U;
            }
        }

        createBackBuffer(width, height, initParam.depthStencilFormat_, flags_);

        syncInterval_ = initParam.interval_;
        refreshRate_ = lcore::clamp(initParam.refreshRate_, MinRefreshRate, MaxRefreshRate);

        {//ラスタライザステート
            D3D11_RASTERIZER_DESC rasterizerDesc;
            rasterizerDesc.FillMode = (D3D11_FILL_MODE)Fill_Solid;
            rasterizerDesc.CullMode = (D3D11_CULL_MODE)Cull_Back;
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

            rasterizerDesc.CullMode = (D3D11_CULL_MODE)Cull_None;
            hr = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerStates_[Rasterizer_FillSolidNoCull]);
            if(FAILED(hr)){
                return false;
            }

            rasterizerDesc.FillMode = (D3D11_FILL_MODE)Fill_WireFrame;
            rasterizerDesc.CullMode = (D3D11_CULL_MODE)Cull_Back;
            hr = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerStates_[Rasterizer_FillWireFrame]);
            if(FAILED(hr)){
                return false;
            }

            rasterizerDesc.FillMode = (D3D11_FILL_MODE)Fill_WireFrame;
            rasterizerDesc.CullMode = (D3D11_CULL_MODE)Cull_None;
            hr = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerStates_[Rasterizer_FillWireFrameNoCull]);
            if(FAILED(hr)){
                return false;
            }

            rasterizerDesc.FillMode = (D3D11_FILL_MODE)Fill_Solid;

            //デプスバッファのフォーマットによってDepthBiasの扱いが異なる
            const f32 depthBias = initParam.depthBias_;
            switch(initParam.shadowMapFormat_)
            {
            case lgfx::Data_D16_UNorm:
                rasterizerDesc.DepthBias = static_cast<INT>(depthBias * 0xFFFF);
                break;

            case lgfx::Data_D32_Float:
            case lgfx::Data_D32_Float_S8X24_UInt:
                rasterizerDesc.DepthBias = static_cast<INT>(depthBias * 0x7FFFFF);
                break;

            default:
                rasterizerDesc.DepthBias = static_cast<INT>(depthBias * 0xFFFFFF);
                break;
            };

            rasterizerDesc.SlopeScaledDepthBias = initParam.slopeScaledDepthBias_;
            rasterizerDesc.DepthBiasClamp = initParam.depthBiasClamp_;

            hr = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerStates_[Rasterizer_DepthMap]);
            if(FAILED(hr)){
                return false;
            }

            setRasterizerState(lgfx::GraphicsDeviceRef::Rasterizer_FillSolid);

#ifdef _DEBUG
            for(s32 i=0; i<Rasterizer_Num; ++i){
                LIME_DEBUGNAME(rasterizerStates_[i], "System Rasterize State");
            }
#endif
        }

        {//DepthStencilステート
            D3D11_DEPTH_STENCIL_DESC desc;
            lcore::memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));

            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(Cmp_Greater);
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

#ifdef _DEBUG
            for(s32 i=0; i<DepthStencil_Num; ++i){
                LIME_DEBUGNAME(depthStencilStates_[i], "System Depth Stencil State");
            }
#endif
        }

        {//ブレンドステート
            blendStates_[BlendState_NoAlpha] = lgfx::BlendState::createRaw(
                FALSE,
                FALSE,
                lgfx::Blend_One,
                lgfx::Blend_Zero,
                lgfx::BlendOp_Add,
                lgfx::Blend_One,
                lgfx::Blend_Zero,
                lgfx::BlendOp_Add,
                lgfx::ColorWrite_All);

            blendStates_[BlendState_Alpha] = lgfx::BlendState::createRaw(
                FALSE,
                TRUE,
                lgfx::Blend_SrcAlpha,
                lgfx::Blend_InvSrcAlpha,
                lgfx::BlendOp_Add,
                lgfx::Blend_SrcAlpha,
                lgfx::Blend_DestAlpha,
                lgfx::BlendOp_Add,
                lgfx::ColorWrite_All);

            blendStates_[BlendState_AlphaAdditive] = lgfx::BlendState::createRaw(
                FALSE,
                TRUE,
                lgfx::Blend_SrcAlpha,
                lgfx::Blend_One,
                lgfx::BlendOp_Add,
                lgfx::Blend_SrcAlpha,
                lgfx::Blend_DestAlpha,
                lgfx::BlendOp_Add,
                lgfx::ColorWrite_All);

            blendStates_[BlendState_Add] = lgfx::BlendState::createRaw(
                FALSE,
                TRUE,
                lgfx::Blend_One,
                lgfx::Blend_One,
                lgfx::BlendOp_Add,
                lgfx::Blend_One,
                lgfx::Blend_One,
                lgfx::BlendOp_Add,
                lgfx::ColorWrite_All);
#ifdef _DEBUG
            for(s32 i=0; i<BlendState_Num; ++i){
                LIME_DEBUGNAME(blendStates_[i], "System Blend State");
            }
#endif
        }

        setFeatureFlags(device_);

        if(0 == initParam.windowed_){
            ShowCursor(FALSE);
        }

        return true;
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::terminate()
    {
        ContextRef::terminate();

        if(NULL != swapChain_){
            BOOL fullscreen = FALSE;
            IDXGIOutput* output = NULL;
            HRESULT hr = swapChain_->GetFullscreenState(&fullscreen, &output);
            if(SUCCEEDED(hr) && fullscreen){
                swapChain_->SetFullscreenState(FALSE, NULL);
                ShowCursor(TRUE);
            }
            LDXSAFE_RELEASE(output);
            swapChain_->Release();
            swapChain_ = NULL;
        }
#ifdef _DEBUG
        ID3D11Debug* debug = NULL;
        device_->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug));
        if(NULL != debug){
            debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
            LDXSAFE_RELEASE(debug);
        }
#endif
        LDXSAFE_RELEASE(device_);
    }

    //---------------------------------------------------------
    bool GraphicsDeviceRef::createBackBuffer(u32 width, u32 height, u32 depthStencilFormat, u32 flag)
    {
        HRESULT hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer_);
        if(FAILED(hr)){
            return false;
        }
        LIME_DEBUGNAME(backBuffer_, "System Back Buffer");

        hr = device_->CreateRenderTargetView(backBuffer_, NULL, &renderTargetView_);
        if(FAILED(hr)){
            return false;
        }
        LIME_DEBUGNAME(renderTargetView_, "System Render Target View");
        lcore::Log("create render target view for backbuffer");

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
            LIME_DEBUGNAME(depthStencil_, "System Depth Stencil Texture");

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
                LIME_DEBUGNAME(depthStencilShaderResourceView_, "System Depth Stencil View");
            }
        }

        context_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

        Viewport viewport = {0.0f, 0.0f, static_cast<f32>(width), static_cast<f32>(height), 0.0f, 1.0f};
        setDefaultViewport(viewport);
        restoreDefaultViewport();
        return true;
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
        LDXSAFE_RELEASE(renderTargetView_);
        LDXSAFE_RELEASE(depthStencilShaderResourceView_);
        LDXSAFE_RELEASE(depthStencilView_);
        LDXSAFE_RELEASE(backBuffer_);
        LDXSAFE_RELEASE(depthStencil_);

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
    bool GraphicsDeviceRef::checkMultisampleQualityLevels(
            lgfx::DataFormat format,
            u32 sampleCount,
            u32* qualityLevels)
    {
        LASSERT(sampleCount<=MaxMultiSampleCount);
        LASSERT(NULL != qualityLevels);
        HRESULT hr = device_->CheckMultisampleQualityLevels((DXGI_FORMAT)format, sampleCount, qualityLevels);
        return SUCCEEDED(hr);
    }

    //---------------------------------------------------------
    DefferedContextRef GraphicsDeviceRef::createDefferedContext()
    {
        ID3D11DeviceContext* deffered;
        HRESULT hr = device_->CreateDeferredContext(0, &deffered);
        if(FAILED(hr)){
            return DefferedContextRef();
        }
        return DefferedContextRef(device_, deffered, *this);
    }


    //--------------------------------------
    //---
    //--- ShaderResourceView
    //---
    //--------------------------------------
    ID3D11ShaderResourceView* ShaderResourceView::resources_[8];

    //--------------------------------------
    //---
    //--- ShaderSamplerState
    //---
    //--------------------------------------
    ID3D11SamplerState* ShaderSamplerState::states_[8];
}

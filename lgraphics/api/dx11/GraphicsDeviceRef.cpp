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
    static const Char* GSModels[] =
    {
        "gs_4_0",
        "gs_5_0",
    };

    static const Char* VSModels[] =
    {
        "vs_4_0",
        "vs_5_0",
    };

    static const Char* PSModels[] =
    {
        "ps_4_0",
        "ps_5_0",
    };

    ID3D11ShaderResourceView* const GraphicsDeviceRef::NULLResources[GraphicsDeviceRef::MaxShaderResources] =
    { NULL, };

    ID3D11RenderTargetView* const GraphicsDeviceRef::NullTargets[MaxRenderTargets] =
    { NULL, };

    const Char* GraphicsDeviceRef::GSModel = GSModels[0];
    const Char* GraphicsDeviceRef::VSModel = VSModels[0];
    const Char* GraphicsDeviceRef::PSModel = PSModels[0];

    GraphicsDeviceRef::GraphicsDeviceRef()
        :clearDepth_(1.0f)
        ,clearStencil_(0)
        ,clearDepthStencilFlag_(ClearDepth)
        ,syncInterval_(0)
        ,featureLevel_(0)
        ,device_(NULL)
        ,context_(NULL)
        ,swapChain_(NULL)
        ,renderTargetView_(NULL)
        ,depthStencil_(NULL)
        ,depthStencilView_(NULL)
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
        swapChainDesc.Flags = 0;

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
            NULL, //HMODULE ドライバタイプがSOFTWAREの場合LoadLibraryでDLLをロードして渡す。
                  //それ以外のタイプではNULL
            deviceFlags, //フラグ
            attemptFeatureLevel, //FutureLevel配列
            3,                   //FutureLevel配列要素数
            D3D11_SDK_VERSION,
            &swapChainDesc,
            &swapChain_,
            &device_,
            &supportedLevel,
            &context_);

        if(FAILED(hr)){
            lcore::Log("Cannot create Direct3D Device");
            return false;
        }
        featureLevel_ = supportedLevel;

        //VSModel = (supportedLevel >= D3D_FEATURE_LEVEL_11_0)? VSModels[1] : VSModels[0];
        //PSModel = (supportedLevel >= D3D_FEATURE_LEVEL_11_0)? PSModels[1] : PSModels[0];

        ID3D11Texture2D* backBuffer = NULL;
        hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
        if(FAILED(hr)){
            return false;
        }

        hr = device_->CreateRenderTargetView(backBuffer, NULL, &renderTargetView_);
        backBuffer->Release();
        if(FAILED(hr)){
            return false;
        }

        if(initParam.depthStencilFormat_ != 0){
            D3D11_TEXTURE2D_DESC desc;
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = (DXGI_FORMAT)initParam.depthStencilFormat_;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            desc.CPUAccessFlags = CPUAccessFlag_None;
            desc.MiscFlags = ResourceMisc_None;
            hr = device_->CreateTexture2D(&desc, NULL, &depthStencil_);
            if(FAILED(hr)){
                return false;
            }

            D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
            lcore::memset(&viewDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
            viewDesc.Format = desc.Format;
            viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            viewDesc.Texture2D.MipSlice = 0;

            hr = device_->CreateDepthStencilView(depthStencil_, &viewDesc, &depthStencilView_);
            if(FAILED(hr)){
                return false;
            }
        }
        context_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

        setViewport(0, 0, initParam.backBufferWidth_, initParam.backBufferHeight_);

        syncInterval_ = initParam.interval_;
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

            rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

            hr = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerStates_[Rasterizer_FillWireFrame]);
            if(FAILED(hr)){
                return false;
            }

            rasterizerDesc.FillMode = D3D11_FILL_SOLID;
            rasterizerDesc.DepthBias = 5;
            rasterizerDesc.SlopeScaledDepthBias = 0.2f;
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

        return true;
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::terminate()
    {
        if(NULL != context_){
            context_->ClearState();
            context_->Flush();
        }

        for(s32 i=0; i<BlendState_Num; ++i){
            SAFE_RELEASE(blendStates_[i]);
        }

        for(s32 i=0; i<DepthStencil_Num; ++i){
            SAFE_RELEASE(depthStencilStates_[i]);
        }

        for(s32 i=0; i<Rasterizer_Num; ++i){
            SAFE_RELEASE(rasterizerStates_[i]);
        }

        SAFE_RELEASE(renderTargetView_);
        SAFE_RELEASE(depthStencilView_);
        SAFE_RELEASE(depthStencil_);

        SAFE_RELEASE(swapChain_);
        SAFE_RELEASE(context_);
        SAFE_RELEASE(device_);
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::getRenderTargetDesc(u32& width, u32& height)
    {
        width = 1;
        height = 1;

        ID3D11Texture2D* backBuffer = NULL;
        HRESULT hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
        if(FAILED(hr)){
            return;
        }

        D3D11_TEXTURE2D_DESC desc;
        backBuffer->GetDesc(&desc);
        backBuffer->Release();

        width = desc.Width;
        height = desc.Height;
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::setViewport(s32 x, s32 y, u32 width, u32 height)
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

}

/**
@file GraphicsDeviceRef.cpp
@author t-sakai
@date 2010/03/23 create
*/
#include "GraphicsDeviceRef.h"
#include "initParam.h"

namespace lgraphics
{
    GraphicsDeviceRef::GraphicsDeviceRef()
        :device_(NULL)
        ,context_(NULL)
        ,swapChain_(NULL)
    {
    }

    GraphicsDeviceRef::~GraphicsDeviceRef()
    {
    }

    //---------------------------------------------------------
    bool GraphicsDeviceRef::initialize(const InitParam& initParam)
    {
        // スワップ設定
        //-----------------------------------------------
        DXGI_SWAP_CHAIN_DESC swapChainDesc;

        // バッファ設定
        //-------------------------
        // サイズ
        swapChainDesc.BufferDesc.Width = initParam.backBufferWidth_;
        swapChainDesc.BufferDesc.Height = initParam.backBufferHeight_;

        // リフレッシュレート
        swapChainDesc.BufferDesc.RefreshRate.Numerator = initParam.refreshRate_;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

        // バッファフォーマット
        swapChainDesc.BufferDesc.Format = Data_B8G8R8A8_UNorm_SRGB;

        // スキャンライン描画順
        swapChainDesc.BufferDesc.ScanlineOrdering = ScanlineOrder_Unspecified;

        // スケーリング
        swapChainDesc.BufferDesc.Scaling = Scaling_Stretched;

        // サンプリング設定
        //-------------------------
        swapChainDesc.SampleDesc.Count = 1; // サンプリング数／ピクセル
        swapChainDesc.SampleDesc.Quality = 0; // 品質：0からID3D10Device::CheckMultiSampleQualityLevelsまで

        swapChainDesc.BufferUsage = Usage_RenderTargetOutput;
        swapChainDesc.BufferCount = 2; //フロントバッファを含むバッファ数
        swapChainDesc.OutputWindow = initParam.windowHandle_;
        swapChainDesc.Windowed =    initParam.windowed_;
        swapChainDesc.SwapEffect =  initParam.swapEffect_;;
        swapChainDesc.Flags = 0;

        D3D_FEATURE_LEVEL supportedLevel;

        // デバイスとスワップチェイン作成
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            NULL, //IDXGIAdapter。デフォルトを使用する場合NULL
            initParam.type_, //ドライバタイプ
            NULL, //HMODULE ドライバタイプがSOFTWAREの場合LoadLibraryでDLLをロードして渡す。
                  //それ以外のタイプではNULL
            0, //フラグ
            &(initParam.level_), //FutureLevel配列
            1,                   //FutureLevel配列要素数
            &swapChainDesc,
            &swapChain_,
            &device_,
            &supportedLevel,
            &context_);

        if(FAILED(hr)){
            LogError("Cannot create Direct3D Device");
            return false;
        }

            
        return true;
    }

    //---------------------------------------------------------
    void GraphicsDeviceRef::terminate()
    {
        DX_SAFE_RELEASE(swapChain_);
        DX_SAFE_RELEASE(context_);
        DX_SAFE_RELEASE(device_);
    }
}

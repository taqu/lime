/**
@file GraphicsDeviceRef.cpp
@author t-sakai
@date 2009/01/18 create
*/
#include <lmath/lmathcore.h>

#include "GraphicsDeviceRef.h"
#include "initParam.h"
#include "RenderStateRef.h"

using namespace lcore;

namespace lgraphics
{

namespace
{
    bool selectFormat(IDirect3D9* d3d9, D3DFORMAT& display, D3DFORMAT& back, s32 Windowed)
    {
        HRESULT hr;
        hr = d3d9->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, display, back, Windowed);

        if(SUCCEEDED(hr)){
            return true;
        }

        display = (display == D3DFMT_X8R8G8B8)? D3DFMT_R5G6B5 : D3DFMT_X8R8G8B8;
        back = (display == D3DFMT_X8R8G8B8)? D3DFMT_A8R8G8B8 : D3DFMT_R5G6B5;

        hr = d3d9->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, display, back, Windowed);

        if(SUCCEEDED(hr)){
            return true;
        }

        return false;
    }

    bool selectNearestSize(IDirect3D9* d3d9, u32& width, u32& height, u32 rate, D3DFORMAT display, bool checkRate, bool checkAspect, bool checkSize)
    {
        f32 aspect = static_cast<f32>(width)/height;

        u32 count = d3d9->GetAdapterModeCount(D3DADAPTER_DEFAULT, display);
        D3DDISPLAYMODE dispMode;

        for(u32 i=0; i<count; ++i){
            HRESULT hr = d3d9->EnumAdapterModes(D3DADAPTER_DEFAULT, display, i, &dispMode);
            if(FAILED(hr)){
                continue;
            }
            if(checkRate && dispMode.RefreshRate != rate){
                continue;
            }

            f32 tmp = static_cast<f32>(dispMode.Width)/dispMode.Height;
            if(checkAspect && !lmath::isEqual(tmp, aspect)){
                continue;
            }

            if(checkSize){
                if(dispMode.Width == width
                    && dispMode.Height == height)
                {
                    width = dispMode.Width;
                    height = dispMode.Height;
                    return true;
                }
            }else{
                width = dispMode.Width;
                height = dispMode.Height;
                return true;
            }
        }
        return false;
    }

    bool selectNearestSize(IDirect3D9* d3d9, u32& width, u32& height, u32 rate, D3DFORMAT display)
    {

        if(selectNearestSize(d3d9, width, height, rate, display, true, true, true)){
            return true;
        }

        if(selectNearestSize(d3d9, width, height, rate, display, true, true, false)){
            return true;
        }

        if(selectNearestSize(d3d9, width, height, rate, display, true, false, false)){
            return true;
        }

        if(selectNearestSize(d3d9, width, height, rate, display, false, false, false)){
            return true;
        }
        return false;
    }

    bool createD3DParam(
        D3DPRESENT_PARAMETERS& d3dpp,
        IDirect3D9* d3d,
        u32 backBufferWidth,
        u32 backBufferHeight,
        u32 displayFormat,
        u32 refreshRate,
        HWND__ *windowHandle,
        s32 windowed,
        PresentInterval interval,
        SwapEffect swapEffect)
    {
        D3DFORMAT dispFormat = (D3DFORMAT)displayFormat;
        D3DFORMAT backFormat = (displayFormat == Buffer_X8R8G8B8)? static_cast<D3DFORMAT>(Buffer_A8R8G8B8) : static_cast<D3DFORMAT>(Buffer_R5G6B5);

        if(false == selectFormat(d3d, dispFormat, backFormat, windowed)){
            return false;
        }

        u32 width = backBufferWidth;
        u32 height = backBufferHeight;
        if(false == selectNearestSize(d3d, width, height, refreshRate, dispFormat)){
            return false;
        }

        d3dpp.BackBufferWidth = width;
        d3dpp.BackBufferHeight = height;
        d3dpp.BackBufferFormat = backFormat;


        d3dpp.BackBufferCount = 1;

        d3dpp.MultiSampleType = static_cast<D3DMULTISAMPLE_TYPE>(MutiSample_None);
        d3dpp.MultiSampleQuality = 0;

        d3dpp.SwapEffect = static_cast<D3DSWAPEFFECT>(swapEffect);

        d3dpp.hDeviceWindow = windowHandle;
        d3dpp.Windowed = windowed;

        d3dpp.EnableAutoDepthStencil = TRUE;
        d3dpp.AutoDepthStencilFormat = static_cast<D3DFORMAT>(Buffer_D24S8);
        //d3dpp.AutoDepthStencilFormat = D3DFMT_D32F_LOCKABLE;

        d3dpp.Flags = 0;

        d3dpp.FullScreen_RefreshRateInHz = (d3dpp.Windowed)? 0 : refreshRate;
        d3dpp.PresentationInterval = interval;
        return true;
    }
}

    GraphicsDeviceRef::GraphicsDeviceRef()
        :d3d_(NULL)
        ,d3dDevice_(NULL)
        ,deviceType_(DriverType_Hardware)
        ,backBufferWidth_(1)
        ,backBufferHeight_(1)
        ,displayFormat_(Buffer_X8R8G8B8)
        ,refreshRate_(FULL_SCREEN_REFRESH_RATE)
        ,windowHandle_(NULL)
        ,windowed_(1)
        ,interval_(PresentInterval_One)
        ,swapEffect_(SwapEffect_Discard)
        ,_clearColor(0xFF808080U)
        ,_clearDepth(1.0f)
        ,_clearStencil(0)
    {

    }

    GraphicsDeviceRef::GraphicsDeviceRef(const GraphicsDeviceRef& rhs)
        :d3d_(rhs.d3d_)
        ,d3dDevice_(rhs.d3dDevice_)
        ,deviceType_(rhs.deviceType_)
        ,backBufferWidth_(rhs.backBufferWidth_)
        ,backBufferHeight_(rhs.backBufferHeight_)
        ,displayFormat_(rhs.displayFormat_)
        ,refreshRate_(rhs.refreshRate_)
        ,windowHandle_(rhs.windowHandle_)
        ,windowed_(rhs.windowed_)
        ,interval_(rhs.interval_)
        ,swapEffect_(rhs.swapEffect_)
        ,_clearColor(rhs._clearColor)
        ,_clearDepth(rhs._clearDepth)
        ,_clearStencil(rhs._clearStencil)
    {
        if(d3d_ != NULL){
            d3d_->AddRef();
        }

        if(d3dDevice_ != NULL){
            d3dDevice_->AddRef();
        }
    }

    GraphicsDeviceRef::~GraphicsDeviceRef()
    {
       terminate();
    }

    bool GraphicsDeviceRef::initialize(const InitParam& initParam)
    {
        d3d_ = Direct3DCreate9( D3D_SDK_VERSION );
        if(d3d_ == NULL){
            lcore::Log("Cannot create Direct3D9");
            return false;
        }
        deviceType_ = initParam.type_;
        backBufferWidth_ = initParam.backBufferWidth_;
        backBufferHeight_ = initParam.backBufferHeight_;
        displayFormat_ = initParam.displayFormat_;
        refreshRate_ = initParam.refreshRate_;
        windowHandle_ = initParam.windowHandle_;
        windowed_ = initParam.windowed_;
        interval_ = initParam.interval_;
        swapEffect_ = initParam.swapEffect_;

        //パラメータ設定
        D3DPRESENT_PARAMETERS d3dpp;

        bool ret = createD3DParam(
            d3dpp,
            d3d_,
            backBufferWidth_,
            backBufferHeight_,
            displayFormat_,
            refreshRate_,
            windowHandle_,
            windowed_,
            interval_,
            swapEffect_);
        if(!ret){
            return false;
        }

        //
        u32 adapter = D3DADAPTER_DEFAULT;

#ifdef _DEBUG
        // NVPerf用アダプタ選択
        HRESULT result;
        D3DADAPTER_IDENTIFIER9 identifier;
        for(u32 i=0; i<d3d_->GetAdapterCount(); ++i){
            result = d3d_->GetAdapterIdentifier(i, 0, &identifier);
            if(FAILED(result)){
                continue;
            }

            if(strstr(identifier.Description, "PerfHUD") != 0){
                adapter = i;
                deviceType_ = DriverType_Reference;
                break;
            }
        }
#endif

        HRESULT hr = d3d_->CreateDevice(
            adapter, (D3DDEVTYPE)deviceType_, windowHandle_,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &d3dpp, &d3dDevice_);

        if(FAILED(hr)){
            lcore::Log("Cannot create Direct3D Device");
            return false;
        }

        if(d3dpp.Windowed){
            D3DDISPLAYMODE dispMode;
            HRESULT hr = d3d_->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispMode);
            if(FAILED(hr)){
                refreshRate_ = FULL_SCREEN_REFRESH_RATE;
            }else{
                refreshRate_ = dispMode.RefreshRate;
            }
        }

        setDefaultRenderState();
        return true;
    }

    bool GraphicsDeviceRef::reset()
    {
        D3DPRESENT_PARAMETERS d3dpp;

        bool ret = createD3DParam(
            d3dpp,
            d3d_,
            backBufferWidth_,
            backBufferHeight_,
            displayFormat_,
            refreshRate_,
            windowHandle_,
            windowed_,
            interval_,
            swapEffect_);

        if(!ret){
            return false;
        }

        HRESULT hr = d3dDevice_->Reset(&d3dpp);
        if(FAILED(hr)){
            return false;
        }

        setDefaultRenderState();
        return true;
    }

    void GraphicsDeviceRef::terminate()
    {
        SAFE_RELEASE(d3dDevice_);
        SAFE_RELEASE(d3d_);
    }

    void GraphicsDeviceRef::setViewport(s32 x, s32 y, u32 width, u32 height)
    {
        D3DVIEWPORT9 vp;
        vp.X = x;
        vp.Y = y;
        vp.Width = width;
        vp.Height = height;
        vp.MinZ = 0.0f;
        vp.MaxZ = 1.0f;
        d3dDevice_->SetViewport(&vp);
    }

    void GraphicsDeviceRef::setDefaultRenderState()
    {
        setRenderState(D3DRS_CULLMODE, CullMode_CCW);
        setRenderState(D3DRS_ALPHAREF, Default_AlphaTestRef);
        setRenderState(D3DRS_ALPHAFUNC, Cmp_Greater);
        setRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        setRenderState(D3DRS_STENCILENABLE , FALSE);
        setRenderState(D3DRS_CLIPPING, TRUE);
        setRenderState(D3DRS_LIGHTING, FALSE);
        setRenderState(D3DRS_COLORVERTEX, FALSE);
        setRenderState(D3DRS_LOCALVIEWER, FALSE);
        setRenderState(D3DRS_NORMALIZENORMALS, FALSE);
    }
}

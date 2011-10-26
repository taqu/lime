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
}

    GraphicsDeviceRef::GraphicsDeviceRef()
        :d3d_(NULL)
        ,d3dDevice_(NULL)
        ,width_(0)
        ,height_(0)
        ,refreshRate_(FULL_SCREEN_REFRESH_RATE)
        ,_clearColor(0xFF808080U)
        ,_clearDepth(1.0f)
        ,_clearStencil(0)
    {

    }

    GraphicsDeviceRef::GraphicsDeviceRef(const GraphicsDeviceRef& rhs)
        :d3d_(rhs.d3d_)
        ,d3dDevice_(rhs.d3dDevice_)
        ,width_(rhs.width_)
        ,height_(rhs.height_)
        ,refreshRate_(rhs.refreshRate_)
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
            Debug_LogError("Cannot create Direct3D9");
            return false;
        }

        //パラメータ設定
        D3DPRESENT_PARAMETERS d3dpp;

        {
            D3DFORMAT dispFormat = (D3DFORMAT)initParam.displayFormat_;
            D3DFORMAT backFormat = (initParam.displayFormat_ == Buffer_X8R8G8B8)? static_cast<D3DFORMAT>(Buffer_A8R8G8B8) : static_cast<D3DFORMAT>(Buffer_R5G6B5);

            if(false == selectFormat(d3d_, dispFormat, backFormat, initParam.windowed_)){
                return false;
            }

            u32 width = initParam.backBufferWidth_;
            u32 height = initParam.backBufferHeight_;
            if(false == selectNearestSize(d3d_, width, height, initParam.refreshRate_, dispFormat)){
                return false;
            }

            d3dpp.BackBufferWidth = width;
            d3dpp.BackBufferHeight = height;
            d3dpp.BackBufferFormat = backFormat;


            d3dpp.BackBufferCount = 1;

            d3dpp.MultiSampleType = static_cast<D3DMULTISAMPLE_TYPE>(MutiSample_None);
            d3dpp.MultiSampleQuality = 0;

            d3dpp.SwapEffect = static_cast<D3DSWAPEFFECT>(initParam.swapEffect_);

            d3dpp.hDeviceWindow = initParam.windowHandle_;
            d3dpp.Windowed = initParam.windowed_;

            d3dpp.EnableAutoDepthStencil = TRUE;
            d3dpp.AutoDepthStencilFormat = static_cast<D3DFORMAT>(Buffer_D24S8);
            //d3dpp.AutoDepthStencilFormat = D3DFMT_D32F_LOCKABLE;

            d3dpp.Flags = 0;

            d3dpp.FullScreen_RefreshRateInHz = (d3dpp.Windowed)? 0 : initParam.refreshRate_;
            d3dpp.PresentationInterval = initParam.interval_;
        }

        HRESULT hr = d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, initParam.windowHandle_,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &d3dpp, &d3dDevice_);
        if(FAILED(hr)){
            Debug_LogError("Cannot create Direct3D Device");
            return false;
        }

        width_ = d3dpp.BackBufferWidth;
        height_ = d3dpp.BackBufferHeight;
        refreshRate_ = d3dpp.FullScreen_RefreshRateInHz;
        if(d3dpp.Windowed){
            D3DDISPLAYMODE dispMode;
            HRESULT hr = d3d_->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispMode);
            if(FAILED(hr)){
                refreshRate_ = FULL_SCREEN_REFRESH_RATE;
            }else{
                refreshRate_ = dispMode.RefreshRate;
            }
        }

        this->setRenderState(D3DRS_CULLMODE, CullMode_CCW);
        this->setRenderState(D3DRS_ALPHAREF, Default_AlphaTestRef);
        this->setRenderState(D3DRS_ALPHAFUNC, Cmp_Greater);

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

}

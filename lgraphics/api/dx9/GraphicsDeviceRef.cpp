/**
@file GraphicsDeviceRef.cpp
@author t-sakai
@date 2009/01/18 create
*/
#include "GraphicsDeviceRef.h"
#include "initParam.h"
#include "RenderStateRef.h"

using namespace lcore;

namespace lgraphics
{
    GraphicsDeviceRef::GraphicsDeviceRef()
        :d3d_(NULL),
        d3dDevice_(NULL),
        _clearColor(0xFF808080U),
        _clearDepth(1.0f),
        _clearStencil(0)
    {

    }

    GraphicsDeviceRef::GraphicsDeviceRef(const GraphicsDeviceRef& rhs)
        :d3d_(rhs.d3d_)
        ,d3dDevice_(rhs.d3dDevice_)
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
            d3dpp.BackBufferWidth = initParam.backBufferWidth_;
            d3dpp.BackBufferHeight = initParam.backBufferHeight_;

            d3dpp.BackBufferFormat = static_cast<D3DFORMAT>(Buffer_A8R8G8B8);

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

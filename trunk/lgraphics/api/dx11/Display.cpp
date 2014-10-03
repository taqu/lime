/**
@file Display.cpp
@author t-sakai
@date 2013/05/16 create
*/
#include "Display.h"
#include "../../lgraphicscore.h"
#include "../../lgraphicsAPIInclude.h"
#include "InitParam.h"

namespace lgraphics
{
namespace
{
    u32 calcDistance(u32 v0, u32 v1)
    {
        return (v0<v1)? (v1-v0) : (v0-v1);
    }

    f32 calcRefreshRate(const DXGI_RATIONAL& rational)
    {
        return static_cast<f32>(rational.Numerator)/rational.Denominator;
    }
}

    Display::Display()
        :factory_(NULL)
        ,adapter_(NULL)
        ,output_(NULL)
    {
    }

    Display::~Display()
    {
        destroy();
    }

    bool Display::create()
    {
        if(NULL == factory_){
            CreateDXGIFactory1(__uuidof(IDXGIFactory1), (VOID**)&factory_);
        }
        return (NULL != factory_) && setAdapterOutput();
    }

    void Display::destroy()
    {
        SAFE_RELEASE(output_);
        SAFE_RELEASE(adapter_);
        SAFE_RELEASE(factory_);
    }

    u32 Display::getAdapterCount()
    {
        u32 count = 0;
        IDXGIAdapter1 *adapter; 
        while(factory_->EnumAdapters1(count, &adapter) != DXGI_ERROR_NOT_FOUND){ 
            adapter->Release();
            ++count;
        }
        return count;
    }

    bool Display::getNearestConfig(InitParam& dst, const InitParam& src)
    {
        static const u32 NumMaxDescs = 128;
        DXGI_MODE_DESC descs[NumMaxDescs];

        InitParam request = src;
        u32 numModes = NumMaxDescs;
        HRESULT hr = output_->GetDisplayModeList((DXGI_FORMAT)request.format_, 0, &numModes, descs);
        if(FAILED(hr) || numModes<=0){
            return false;
        }

        //最も近いサイズ
        u32 dw = calcDistance(descs[0].Width, request.backBufferWidth_);
        u32 dh = calcDistance(descs[0].Height, request.backBufferHeight_);
        u32 d = dw + dh;
        u32 index = 0;
        for(u32 i=1; i<numModes; ++i){
            dw = calcDistance(descs[i].Width, request.backBufferWidth_);
            dh = calcDistance(descs[i].Height, request.backBufferHeight_);
            u32 td = dw + dh;
            if(td<d){
                d = td;
                index = i;
            }
        }

        //最も近いリフレッシュレート
        u32 width = descs[index].Width;
        u32 height = descs[index].Height;
        f32 requestRefreshRate = static_cast<f32>(request.refreshRate_);
        f32 refreshRate = calcRefreshRate(descs[index].RefreshRate);

        f32 fd = lcore::absolute(refreshRate - requestRefreshRate);
        for(u32 i=0; i<numModes; ++i){
            if(width != descs[i].Width || height != descs[i].Height){
                continue;
            }

            refreshRate = calcRefreshRate(descs[i].RefreshRate);
            f32 td = lcore::absolute(refreshRate - requestRefreshRate);
            if(td<fd){
                fd = td;
                index = i;
            }
        }

        dst = request;
        dst.backBufferWidth_ = width;
        dst.backBufferHeight_ = height;
        dst.refreshRate_ = static_cast<u32>(refreshRate);
        return true;
    }

    bool Display::setAdapterOutput()
    {
        SAFE_RELEASE(adapter_);
        for(u32 i=0; i<NumMaxAdapters; ++i){
            if(factory_->EnumAdapters1(i, &adapter_) != DXGI_ERROR_NOT_FOUND){
                break;
            }
        }
        if(NULL == adapter_){
            return false;
        }

        SAFE_RELEASE(output_);
        for(u32 i=0; i<NumMaxOutputs; ++i){
            if(adapter_->EnumOutputs(i, &output_) != DXGI_ERROR_NOT_FOUND){
                break;
            }
        }
        if(NULL == output_){
            return false;
        }
        return true;

    }
}

/**
@file Display.cpp
@author t-sakai
@date 2011/08/14
*/
#include "Display.h"

namespace config
{
    D3DFORMAT toD3DFormat[Buffer_Num] =
    {
        D3DFMT_A8R8G8B8,
        D3DFMT_X8R8G8B8,
        D3DFMT_R5G6B5,
        D3DFMT_X1R5G5B5,
        D3DFMT_A1R5G5B5,
        D3DFMT_A2R10G10B10,
    };

    Display::Display()
        :dx9_(NULL)
    {
    }

    Display::~Display()
    {
        release();
    }

    bool Display::create()
    {
        if(dx9_ == NULL){
            dx9_ = Direct3DCreate9(D3D_SDK_VERSION);
        }
        return (dx9_ != NULL);
    }

    void Display::release()
    {
        if(dx9_!=NULL){
            dx9_->Release();
            dx9_ = NULL;
        }
    }

    u32 Display::getAdapterCount()
    {
        CONF_ASSERT(dx9_ != NULL);
        return dx9_->GetAdapterCount();
    }

    u32 Display::getAdapterModeCount(u32 adapter, BufferFormat format)
    {
        CONF_ASSERT(dx9_ != NULL);
        CONF_ASSERT(0<=format && format<Buffer_Num);
        return dx9_->GetAdapterModeCount(adapter, toD3DFormat[format]);
    }

    bool Display::enumAdapterModes(u32 adapter, BufferFormat format, u32 mode, DisplayMode* modes)
    {
        CONF_ASSERT(dx9_ != NULL);
        CONF_ASSERT(0<=format && format<Buffer_Num);
        CONF_ASSERT(modes != NULL);
        HRESULT hr = dx9_->EnumAdapterModes(adapter, toD3DFormat[format], mode, (D3DDISPLAYMODE*)modes);
        return SUCCEEDED(hr);
    }

    bool Display::checkDeviceType(u32 adapter, BufferFormat format, BufferFormat backFormat, bool hal, s32 windowed)
    {
        CONF_ASSERT(dx9_ != NULL);
        CONF_ASSERT(0<=format && format<Buffer_Num);
        CONF_ASSERT(0<=backFormat && backFormat<Buffer_Num);

        D3DDEVTYPE devType = (hal)? D3DDEVTYPE_HAL : D3DDEVTYPE_SW;
 
        HRESULT hr = dx9_->CheckDeviceType(adapter, devType, toD3DFormat[format], toD3DFormat[backFormat], windowed);
        return SUCCEEDED(hr);
    }

    bool Display::getAdapterDisplayMode(u32 adapter, DisplayMode& mode)
    {
        HRESULT hr = dx9_->GetAdapterDisplayMode(adapter, (D3DDISPLAYMODE*)&mode);
        return SUCCEEDED(hr);
    }
}

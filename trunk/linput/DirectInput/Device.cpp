/**
@file Device.cpp
@author t-sakai
@date 2009/05/13
*/
#include "../linputAPIInclude.h"
#include "../linputcore.h"

#include "Device.h"

namespace linput
{
    Device::Device()
        :device_(NULL)
    {
    }

    Device::~Device()
    {
    }

    void Device::initialize(InitParam& param)
    {
        LASSERT(param.device_ != NULL);
        terminate();
        device_ = param.device_;
    }

    void Device::terminate()
    {
        if(device_!=NULL){
            device_->Unacquire();
            device_->Release();
            device_ = NULL;
        }
    }

    bool Device::setCooperateLevel(HWND__* hWnd)
    {
        LASSERT(hWnd != NULL);
        LASSERT(device_ != NULL);
        HRESULT hr = device_->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        return SUCCEEDED(hr);
    }

    bool Device::setDataFormat(DeviceType type)
    {
        LASSERT(device_ != NULL);
        LASSERT(0<=type && type<DevType_Num);
        const DIDATAFORMAT* DataFormat[DevType_Num] = {&c_dfDIKeyboard, &c_dfDIMouse, NULL};
        HRESULT hr = device_->SetDataFormat(DataFormat[type]);
        return SUCCEEDED(hr);
    }

    bool Device::setProperty()
    {
        LASSERT(device_ != NULL);
        DIPROPDWORD dipdw; 
        dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        dipdw.diph.dwObj = 0; 
        dipdw.diph.dwHow = DIPH_DEVICE; 

        dipdw.dwData = DIPROPAXISMODE_REL;
        HRESULT hr = device_->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
        return SUCCEEDED(hr);
    }
}

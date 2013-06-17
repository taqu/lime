/**
@file Device.cpp
@author t-sakai
@date 2009/05/13
*/
#include "../linputEnum.h"
#include "Device.h"

namespace linput
{
    Device::Device()
        :device_(NULL)
    {
    }

    Device::Device(IDirectInputDevice8* device)
        :device_(device)
    {
    }

    Device::~Device()
    {
        terminate();
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
        const DIDATAFORMAT* DataFormat[DevType_Num] = {&c_dfDIKeyboard, &c_dfDIMouse, &c_dfDIJoystick };
        HRESULT hr = device_->SetDataFormat(DataFormat[type]);
        return SUCCEEDED(hr);
    }
}

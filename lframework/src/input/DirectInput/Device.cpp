/**
@file Device.cpp
@author t-sakai
@date 2009/05/13
*/
#include "input/DirectInput/Device.h"

namespace linput
{
    Device::Device()
        :device_(NULL)
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

    bool Device::setCooperateLevel(WINDOW_HANDLE hWnd)
    {
        LASSERT(hWnd != NULL);
        LASSERT(device_ != NULL);
        HRESULT hr = device_->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
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

    bool Device::setEventNotification(HANDLE handle)
    {
        HRESULT hr = device_->SetEventNotification(handle);
        return SUCCEEDED(hr);
    }

    bool Device::acquire()
    {
        clear();
        return (NULL != device_)? SUCCEEDED(device_->Acquire()) : false;
    }

    bool Device::unacquire()
    {
        clear();
        return (NULL != device_)? SUCCEEDED(device_->Unacquire()) : false;
    }
}

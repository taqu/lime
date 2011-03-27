/**
@file DeviceBase.cpp
@author t-sakai
@date 2011/02/19
*/
#include "../DeviceBase.h"
namespace linput
{
    bool DeviceBase::acquire()
    {
        if(device_.valid()){
            HRESULT hr = device_->Acquire();
            return SUCCEEDED(hr);
        }
        return false;
    }

    void DeviceBase::unacquire()
    {
        if(device_.valid()){
            device_->Unacquire();
        }
    }
}

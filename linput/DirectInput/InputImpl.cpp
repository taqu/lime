/**
@file InputImpl.cpp
@author t-sakai
@date 2010/07/22 create

*/
#include "../linputcore.h"
#include "InputImpl.h"

namespace linput
{
    InputImpl::InputImpl()
        :dinput_(NULL)
    {
    }

    InputImpl::~InputImpl()
    {
        terminate();
    }

    bool InputImpl::initialize(HWND__* hWnd)
    {
        if(dinput_ != NULL){
            return true;
        }

        LASSERT(hWnd != NULL);
        HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
        LASSERT(hInstance != NULL);

        IDirectInput8 *dinput = NULL;
        HRESULT hr = DirectInput8Create(
            hInstance,
            DIRECTINPUT_VERSION, 
            IID_IDirectInput8,
            (void**)&dinput,
            NULL);

        dinput_ = dinput;
        return SUCCEEDED(hr);
    }

    void InputImpl::terminate()
    {
        SAFE_RELEASE(dinput_);
    }

    IDirectInputDevice8* InputImpl::createDevice(const _GUID& deviceID)
    {
        LASSERT(dinput_ != NULL);
        IDirectInputDevice8 *device = NULL;
        dinput_->CreateDevice(deviceID, &device, NULL);
        return device;
    }

}

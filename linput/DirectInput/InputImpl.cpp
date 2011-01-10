/**
@file InputImpl.cpp
@author t-sakai
@date 2010/07/22 create

*/
#include <lcore/smart_ptr/ScopedPtr.h>
#include "../linputAPIInclude.h"
#include "../linputcore.h"
#include "InputImpl.h"

namespace linput
{
    InputImpl::InputImpl()
        :impl_(NULL)
    {
    }

    InputImpl::~InputImpl()
    {
        terminate();
    }

    void InputImpl::initialize(HWND__* hWnd)
    {
        if(impl_ != NULL){
            return;
        }

        LASSERT(hWnd != NULL);
        HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
        LASSERT(hInstance != NULL);

        IDirectInput8 *dinput = NULL;
        HRESULT hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, 
            IID_IDirectInput8, (void**)&dinput, NULL);

        LASSERT(SUCCEEDED(hr));

        impl_ = dinput;
    }

    void InputImpl::terminate()
    {
        SAFE_RELEASE(impl_);
    }

    IDirectInputDevice8* InputImpl::createDevice(const _GUID& deviceID)
    {
        IDirectInputDevice8 *device = NULL;
        impl_->CreateDevice(deviceID, &device, NULL);
        return device;
    }

}

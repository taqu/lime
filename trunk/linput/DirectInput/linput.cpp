/**
@file linput.cpp
@author t-sakai
@date 2009/05/13
*/
#include <lcore/smart_ptr/ScopedPtr.h>
#include "../linputAPIInclude.h"
#include "../linput.h"

#include "../InitParam.h"

#include "../Keyboard.h"
#include "../Mouse.h"

namespace linput
{
    Input::Input()
        :keyboard_(NULL)
        ,mouse_(NULL)
    {
    }

    Input::~Input()
    {
        LIME_DELETE(keyboard_);
        LIME_DELETE(mouse_);
    }

    void Input::initialize(InitParam& param)
    {
        Input &instance = getInstance();

        if(instance.impl_.isInit()){
            return;
        }

        instance.impl_.initialize(param.hWnd_);

        Device::InitParam devParam;
        devParam.hWnd_ = param.hWnd_;

        devParam.device_ = instance.impl_.createDevice(GUID_SysKeyboard);

        instance.keyboard_ = LIME_NEW Keyboard;
        if(false == instance.keyboard_->initialize(devParam)){
            LIME_DELETE(instance.keyboard_);
        }

        devParam.device_ = instance.impl_.createDevice(GUID_SysMouse);

        instance.mouse_ = LIME_NEW Mouse;
        if(false == instance.mouse_->initialize(devParam)){
            LIME_DELETE(instance.mouse_);
        }
    }

    void Input::terminate()
    {
        Input &instance = getInstance();

        LIME_DELETE(instance.keyboard_);
        LIME_DELETE(instance.mouse_);
        instance.impl_.terminate();
    }

    Input& Input::getInstance()
    {
        static Input instance_;
        return instance_;
    }

    void Input::update()
    {
        keyboard_->poll();
        mouse_->poll();
    }
}

/**
@file Input.cpp
@author t-sakai
@date 2016/11/07 create
*/
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "input/DirectInput/Input.h"
#include "input/linputEnum.h"
#include "input/Keyboard.h"
#include "input/Mouse.h"
#include "input/Joystick.h"

namespace linput
{
    namespace
    {
        struct EnumContext
        {
            EnumContext(DirectInput* dinput, s32 number)
                :dinput_(dinput)
                ,number_(number)
                ,device_(NULL)
            {
            }

            DirectInput* dinput_;
            s32 number_;
            DirectInputDevice* device_;
            DIDEVCAPS caps_;

        };

        BOOL CALLBACK EnumJoySticks(const DIDEVICEINSTANCE* dev, LPVOID data)
        {
            EnumContext* context = (EnumContext*)data;

            //デバイス作成
            DirectInputDevice* device = NULL;
            HRESULT hr = context->dinput_->CreateDevice(dev->guidInstance, &device, NULL);
            if(FAILED(hr)){
                return DIENUM_CONTINUE;
            }

            //DIDEVCAPS
            context->caps_.dwSize = sizeof(DIDEVCAPS);
            hr = device->GetCapabilities(&context->caps_);
            if(FAILED(hr)){
                LINPUT_SAFE_RELEASE(device);
                return DIENUM_CONTINUE;
            }
            context->device_ = device;
            return DIENUM_STOP;
        }
    }


    Input::Input()
        :directInput_(NULL)
    {
        for(s32 i=0; i<DevType_Num; ++i){
            devices_[i] = NULL;
        }
    }

    Input::~Input()
    {
        terminate();
    }

    Error Input::initialize(const InitParam& param)
    {
        LASSERT(NULL != param.windowHandle_);

        terminate();

        HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(param.windowHandle_, GWLP_HINSTANCE);
        if(NULL == hInstance){
            return Error_Init;
        }
        HRESULT hr = DirectInput8Create(
            hInstance,
            DIRECTINPUT_VERSION, 
            IID_IDirectInput8,
            (void**)&directInput_,
            NULL);
        if(FAILED(hr)){
            return Error_Init;
        }

        DirectInputDevice* device = NULL;

        if(param.initDevices_[DevType_Keyboard]){
            device = createDevice(GUID_SysKeyboard);

            Keyboard* keyboard = LNEW Keyboard;
            if(keyboard->initialize(param.windowHandle_, device)){
                devices_[DevType_Keyboard] = keyboard;
            }else{
                LDELETE(keyboard);
            }
        }

        if(param.initDevices_[DevType_Mouse]){
            device = createDevice(GUID_SysMouse);

            Mouse* mouse = LNEW Mouse;
            if(mouse->initialize(param.windowHandle_, device)){
                devices_[DevType_Mouse] = mouse;
            }else{
                LDELETE(mouse);
            }
        }

        if(param.initDevices_[DevType_Joystick]){
            EnumContext context(directInput_, 0);

            hr = directInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoySticks, (LPVOID)&context, DIEDFL_ATTACHEDONLY );

            if(SUCCEEDED(hr) && context.device_ != NULL){
                Joystick* joystick = LNEW Joystick;
                if(joystick->initialize(param.windowHandle_, context.device_)){
                    devices_[DevType_Joystick] = joystick;
                }else{
                    LDELETE(joystick);
                }
            }
        }
        return Error_None;
    }

    void Input::terminate()
    {
        for(s32 i=0; i<DevType_Num; ++i){
            LDELETE(devices_[i]);
        }
        LINPUT_SAFE_RELEASE(directInput_);
    }

    void Input::update()
    {
        for(s32 i=0; i<DevType_Num; ++i){
            if(devices_[i]){
                devices_[i]->poll();
            }
        }
    }

    void Input::clear()
    {
        for(s32 i=0; i<DevType_Num; ++i){
            if(devices_[i]){
                devices_[i]->clear();
            }
        }
    }

    DirectInputDevice* Input::createDevice(const _GUID& deviceID)
    {
        LASSERT(directInput_ != NULL);
        DirectInputDevice* device = NULL;
        directInput_->CreateDevice(deviceID, &device, NULL);
        return device;
    }
}

/**
@file linput.cpp
@author t-sakai
@date 2009/05/13
*/
#include "../linput.h"
#include "../linputEnum.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Joystick.h"

namespace linput
{
    namespace
    {
        struct EnumContext
        {
            EnumContext(IDirectInput8* dinput, s32 number)
                :dinput_(dinput)
                ,number_(number)
                ,device_(NULL)
            {
            }

            IDirectInput8* dinput_;
            s32 number_;
            IDirectInputDevice8* device_;
            DIDEVCAPS caps_;

        };

        BOOL CALLBACK EnumJoySticks(const DIDEVICEINSTANCE* dev, LPVOID data)
        {
            EnumContext* context = (EnumContext*)data;

            //デバイス作成
            IDirectInputDevice8* device = NULL;
            HRESULT hr = context->dinput_->CreateDevice(dev->guidInstance, &device, NULL);
            if(FAILED(hr)){
                return DIENUM_CONTINUE;
            }

            //DIDEVCAPS
            context->caps_.dwSize = sizeof(DIDEVCAPS);
            hr = device->GetCapabilities(&context->caps_);
            if(FAILED(hr)){
                SAFE_RELEASE(device);
                return DIENUM_CONTINUE;
            }
            context->device_ = device;
            return DIENUM_STOP;
        }
    }

    Input::InitParam::InitParam()
        :hWnd_(NULL)
    {
        for(s32 i=0; i<DevType_Num; ++i){
            initDevices_[i] = false;
        }
    }


    Input::Input()
    {
        for(s32 i=0; i<DevType_Num; ++i){
            devices_[i] = NULL;
        }
    }

    Input::~Input()
    {
    }

    Input::Error Input::initialize(InitParam& param)
    {
        Input& instance = getInstance();
        InputImpl &impl = instance.impl_;

        if(false == impl.valid()){
            impl.initialize(param.hWnd_);
            if(false == impl.valid()){
                return Error_None;
            }
        }

        IDirectInputDevice8 *device = NULL;
        
        Error error = Error_None;
        if(param.initDevices_[DevType_Keyboard]){
            LIME_DELETE(instance.devices_[DevType_Joystick]);

            device = impl.createDevice(GUID_SysKeyboard);

            Keyboard* keyboard = LIME_NEW Keyboard;
            if(false == keyboard->initialize(param.hWnd_, device)){
                keyboard->terminate();
                error = Error_Init;
            }
            instance.devices_[DevType_Keyboard] = keyboard;
        }

        if(param.initDevices_[DevType_Mouse]){
            LIME_DELETE(instance.devices_[DevType_Joystick]);

            device = impl.createDevice(GUID_SysMouse);

            Mouse* mouse = LIME_NEW Mouse;
            if(false == mouse->initialize(param.hWnd_, device)){
                mouse->terminate();
                error = Error_Init;
            }
            instance.devices_[DevType_Mouse] = mouse;
        }

        if(param.initDevices_[DevType_Joystick]){
            LIME_DELETE(instance.devices_[DevType_Joystick]);

            EnumContext context(impl.get(), 0);

            HRESULT hr = impl.get()->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoySticks, (LPVOID)&context, DIEDFL_ATTACHEDONLY );

            if(SUCCEEDED(hr) && context.device_ != NULL){
                Joystick* joystick = LIME_NEW Joystick;
                if(false == joystick->initialize(param.hWnd_, context.device_)){
                    joystick->terminate();
                    error = Error_Init;
                }
                instance.devices_[DevType_Joystick] = joystick;
            }else{
                instance.devices_[DevType_Joystick] = LIME_NEW Joystick;
            }
        }
        return error;
    }

    void Input::terminate()
    {
        Input &instance = getInstance();

        for(s32 i=0; i<DevType_Num; ++i){
            LIME_DELETE(instance.devices_[i]);
        }

        instance.impl_.terminate();
    }

    Input& Input::getInstance()
    {
        static Input instance_;
        return instance_;
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
                devices_[i]->poll();
            }
        }
    }
}

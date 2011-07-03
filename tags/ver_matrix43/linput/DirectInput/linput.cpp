/**
@file linput.cpp
@author t-sakai
@date 2009/05/13
*/
#include "../linput.h"
#include "../linputEnum.h"

#include "Keyboard.h"
#include "Mouse.h"

namespace linput
{
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
            device = impl.createDevice(GUID_SysKeyboard);

            Keyboard* keyboard = LIME_NEW Keyboard;
            if(false == keyboard->initialize(param.hWnd_, device)){
                LIME_DELETE(keyboard);
                error = Error_Init;
            }else{
                LIME_DELETE(instance.devices_[DevType_Keyboard]);
                instance.devices_[DevType_Keyboard] = keyboard;
            }
        }

        if(param.initDevices_[DevType_Mouse]){
            device = impl.createDevice(GUID_SysMouse);

            Mouse* mouse = LIME_NEW Mouse;
            if(false == mouse->initialize(param.hWnd_, device)){
                LIME_DELETE(mouse);
                error = Error_Init;
            }else{
                LIME_DELETE(instance.devices_[DevType_Mouse]);
                instance.devices_[DevType_Mouse] = mouse;
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

    void Input::acquire()
    {
        for(s32 i=0; i<DevType_Num; ++i){
            if(devices_[i]){
                devices_[i]->acquire();
            }
        }
    }

    void Input::unacquire()
    {
        for(s32 i=0; i<DevType_Num; ++i){
            if(devices_[i]){
                devices_[i]->unacquire();
            }
        }
    }
}
